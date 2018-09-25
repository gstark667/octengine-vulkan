#version 450
#extension GL_ARB_separate_shader_objects : enable

layout (binding = 0) uniform sampler2D samplerAlbedo;
layout (binding = 1) uniform sampler2D samplerNormal;
layout (binding = 2) uniform sampler2D samplerPosition;
layout (binding = 3) uniform sampler2D samplerDepth;
layout (binding = 4) uniform sampler2D shadowCoord;
layout (binding = 5) uniform sampler2D shadowDepth;

layout (location = 0) in vec2 inUV;

layout (location = 0) out vec4 outFragColor;

float ambient = 0.1;

struct Light {
    vec4 position;
    vec3 color;
    float radius;
};

layout (binding = 4) uniform UBO 
{
    Light lights[6];
    vec4 viewPos;
} ubo;

float textureProj(vec4 P, vec2 off)
{
    float shadow = 1.0;
    vec4 shadowCoord = P / P.w;
    if ( shadowCoord.z > -1.0 && shadowCoord.z < 1.0 ) 
    {
        float dist = texture(shadowDepth, shadowCoord.st + off ).r;
        if ( shadowCoord.w > 0.0 && dist < shadowCoord.z - 0.0005 ) 
        {
            shadow = ambient;
        }
    }
    return shadow;
}

float filterPCF(vec4 sc)
{
    ivec2 texDim = textureSize(shadowDepth, 0);
    float scale = 1.0;
    float dx = scale * 1.0 / float(texDim.x);
    float dy = scale * 1.0 / float(texDim.y);

    float shadowFactor = 0.0;
    int count = 0;
    int range = 1;
    
    for (int x = -range; x <= range; x++)
    {
        for (int y = -range; y <= range; y++)
        {
            shadowFactor += textureProj(sc, vec2(dx*x, dy*y));
            count++;
        }
    
    }
    return shadowFactor / count;
}

void main()
{
    vec3 albedo = texture(samplerAlbedo, inUV).rgb;
    vec3 normal = texture(samplerNormal, inUV).rgb;
    vec3 position = texture(samplerPosition, inUV).rgb;
    vec4 coord = texture(shadowCoord, inUV);
    float shadowDepth = texture(shadowDepth, coord.xy).r;

    float shadow = textureProj(coord, vec2(0.0, 0.0));
    shadow = filterPCF(coord);
    float shade = max(dot(normal, vec3(0.8, 0.8, 0.8)), ambient);

    shade = min(shadow, shade);

    outFragColor = vec4(albedo * shade, 1.0);
}
