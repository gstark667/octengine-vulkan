#version 450
#extension GL_ARB_separate_shader_objects : enable

layout (binding = 1) uniform sampler2DMS samplerAlbedo;
layout (binding = 2) uniform sampler2DMS samplerNormal;
layout (binding = 3) uniform sampler2DMS samplerPosition;
layout (binding = 4) uniform sampler2DMS samplerDepth;
layout (binding = 5) uniform sampler2DArray shadowDepth;

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

vec4 resolve(sampler2DMS tex, ivec2 uv)
{
    vec4 result = vec4(0.0);
    for (int i = 0; i < 2; i++)
    {
        vec4 val = texelFetch(tex, uv, i); 
        result += val;
    }    
    // Average resolved samples
    return result / 2.0;
}

void main()
{
    ivec2 attDim = textureSize(samplerAlbedo);
    ivec2 UV = ivec2(inUV * attDim);

    vec3 albedo = resolve(samplerAlbedo, UV).rgb;
    vec3 normal = resolve(samplerNormal, UV).rgb;
    vec3 position = resolve(samplerPosition, UV).rgb;
    vec3 shadedColor = vec3(0.0, 0.0, 0.0);
    for (int i = 0; i < lightUBO.lightCount; ++i)
    {
        float shade = max(dot(normal, normalize(lightUBO.lights[i].position.xyz)), 0.0);
        shade *= filterPCF(lightUBO.lights[i].mvp * vec4(position, 1.0), i);
        shade = max(shade, AMBIENT);
        shadedColor += lightUBO.lights[i].color.xyz * shade;
    }
    outFragColor = vec4(albedo * shadedColor, 1.0);
}

