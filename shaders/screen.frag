#version 450
#extension GL_ARB_separate_shader_objects : enable

layout (binding = 1) uniform sampler2D samplerAlbedo;
layout (binding = 2) uniform sampler2D samplerNormal;
layout (binding = 3) uniform sampler2D samplerPosition;
layout (binding = 4) uniform sampler2D samplerDepth;
layout (binding = 5) uniform sampler2DArray shadowDepth;
//layout (binding = 4) uniform sampler2D shadowCoord;

struct light {
    vec4 position;
    vec4 color;
    mat4 mvp;
};

layout (binding = 0) uniform light_uniform_buffer_object {
    light lights[16];
    int lightCount;
} lightUBO;

layout (location = 0) in vec2 inUV;

layout (location = 0) out vec4 outFragColor;

#define AMBIENT 0.2
#define SHADOW_FACTOR 0.25

const mat4 biasMat = mat4(
    0.5, 0.0, 0.0, 0.0,
    0.0, 0.5, 0.0, 0.0,
    0.0, 0.0, 0.5, 0.0,
    0.5, 0.5, 0.5, 1.0);

float textureProj(vec4 P, vec2 off, int layer)
{
    float shadow = 1.0;
    vec4 shadowCoord = P;
    shadowCoord.st = shadowCoord.st * 0.5 + 0.5;
    if (shadowCoord.z > -1.0 && shadowCoord.z < 1.0) 
    {
        float dist = texture(shadowDepth, vec3(shadowCoord.st + off, layer)).r;
        if (shadowCoord.w > 0.0 && dist < shadowCoord.z - 0.0015)
        {
            shadow = SHADOW_FACTOR;
        }
    }
    return shadow;
}

float filterPCF(vec4 sc, int layer)
{
    ivec2 texDim = textureSize(shadowDepth, 0).xy;
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
            shadowFactor += textureProj(sc, vec2(dx*x, dy*y), layer);
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
    //vec4 coord = texture(shadowCoord, inUV);
    //float shadowDepth = texture(shadowDepth, coord.xy).r;
    //float shadowDepth = texture(shadowDepth, inUV).r;
    vec3 shadedColor = vec3(0.0, 0.0, 0.0);
    for (int i = 0; i < lightUBO.lightCount; ++i)
    {
        float shade = max(dot(normal, normalize(lightUBO.lights[i].position.xyz)), 0.0);
        shadedColor += lightUBO.lights[i].color.xyz * shade;
    }

    //float shadow = textureProj(coord, vec2(0.0, 0.0));
    float shadow = 1.0;
    for (int i = 0; i < lightUBO.lightCount; ++i)
    {
        //float shade = max(dot(normal, normalize(lightUBO.lights[i].position.xyz)), AMBIENT);
        vec4 coord = lightUBO.lights[i].mvp * vec4(position, 1.0);
        shadow *= filterPCF(coord, i);
    }
    shadow = max(shadow, AMBIENT);
    shadedColor *= shadow;
    //shade = min(shadow, shade);
    //float shade = dot(normal, normalize(lightUBO.lightPositions[1]));
    //float shade = dot(normal, normalize(lightUBO.lights[1].position.xyz));

    //outFragColor = vec4(shadowDepth, 0.0, 0.0, 1.0);
    //outFragColor = vec4(shadow);
    //outFragColor = vec4(albedo * shade, 1.0);
    //outFragColor = vec4(albedo * shadow, 1.0);
    outFragColor = vec4(albedo * shadedColor, 1.0);
    //outFragColor = vec4(lightUBO.lights[0].color);
    //outFragColor = biasMat * lightUBO.shadowSpaces[0] * vec4(position, 1.0);
    //outFragColor = texture(shadowDepth, vec3((biasMat * lightUBO.shadowSpaces[0] * vec4(position, 1.0)).xy, 0));
    //outFragColor = texture(shadowDepth, vec3((lightUBO.shadowSpaces[0] * vec4(position, 1.0) * 0.25 +0.5).xy, 0));
    //outFragColor = vec4(texture(shadowDepth, vec3(inUV, 0)).r, texture(shadowDepth, vec3(inUV, 1)).r, 0.0, 1.0);
}
