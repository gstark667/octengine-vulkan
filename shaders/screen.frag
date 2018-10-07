#version 450
#extension GL_ARB_separate_shader_objects : enable

layout (binding = 2) uniform sampler2DMS samplerAlbedo;
layout (binding = 3) uniform sampler2DMS samplerNormal;
layout (binding = 4) uniform sampler2DMS samplerPosition;
layout (binding = 5) uniform sampler2DMS samplerPBR;
layout (binding = 6) uniform sampler2DMS samplerDepth;
layout (binding = 7) uniform sampler2DArray shadowDepth;

struct light {
    vec4 position;
    vec4 direction;
    vec4 color;
    mat4 mvp;
};

layout (binding = 0) uniform light_uniform_buffer_object {
    light lights[16];
    vec4 cameraPos;
    int lightCount;
} lightUBO;

layout (binding = 1) uniform render_uniform_buffer_object {
    vec4 ambient;
    int sampleCount;
} renderUBO;

layout (location = 0) in vec2 inUV;

layout (location = 0) out vec4 outFragColor;

#define SHADOW_FACTOR 0.25
#define PI 3.14159

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
    for (int i = 0; i < renderUBO.sampleCount; i++)
    {
        vec4 val = texelFetch(tex, uv, i); 
        result += val;
    }    
    // Average resolved samples
    return result / float(renderUBO.sampleCount);
}


// Normal Distribution
float D_GGX(float dotNH, float roughness)
{
    float alpha = roughness * roughness;
    float alpha2 = alpha * alpha;
    float denom = dotNH * dotNH * (alpha2 - 1.0) + 1.0;
    return (alpha2)/(PI * denom*denom); 
}

// Geometric Shadowing
float G_SchlicksmithGGX(float dotNL, float dotNV, float roughness)
{
    float r = (roughness + 1.0);
    float k = (r*r) / 8.0;
    float GL = dotNL / (dotNL * (1.0 - k) + k);
    float GV = dotNV / (dotNV * (1.0 - k) + k);
    return GL * GV;
}

// Fresnel
vec3 F_Schlick(float cosTheta, float metallic, vec3 albedo)
{
    vec3 F0 = mix(vec3(0.04), albedo, metallic); // * material.specular
    vec3 F = F0 + (1.0 - F0) * pow(1.0 - cosTheta, 5.0); 
    return F;
}

// Specular BRDF
vec3 BRDF(vec3 L, vec3 V, vec3 N, vec3 albedo, float metallic, float roughness)
{
    // Precalculate vectors and dot products    
    vec3 H = normalize (V + L);
    float dotNV = clamp(dot(N, V), 0.0, 1.0);
    float dotNL = clamp(dot(N, L), 0.0, 1.0);
    float dotLH = clamp(dot(L, H), 0.0, 1.0);
    float dotNH = clamp(dot(N, H), 0.0, 1.0);

    // Light color fixed
    vec3 lightColor = vec3(1.0);

    vec3 color = vec3(0.0);

    if (dotNL > 0.0)
    {
        float rroughness = max(0.05, roughness);
        // D = Normal distribution (Distribution of the microfacets)
        float D = D_GGX(dotNH, roughness); 
        // G = Geometric shadowing term (Microfacets shadowing)
        float G = G_SchlicksmithGGX(dotNL, dotNV, roughness);
        // F = Fresnel factor (Reflectance depending on angle of incidence)
        vec3 F = F_Schlick(dotNV, metallic, albedo);

        vec3 spec = D * F * G / (4.0 * dotNL * dotNV);

        color += spec * dotNL * lightColor;
    }

    return color;
}

void main()
{
    ivec2 attDim = textureSize(samplerAlbedo);
    ivec2 UV = ivec2(inUV * attDim);

    vec3 albedo = resolve(samplerAlbedo, UV).rgb;
    vec3 normal = resolve(samplerNormal, UV).rgb;
    vec3 position = resolve(samplerPosition, UV).rgb;
    vec3 pbr = resolve(samplerPBR, UV).rgb;

    vec3 N = normalize(normal);
    vec3 V = normalize(lightUBO.cameraPos.xyz - position);

    vec3 shadedColor = vec3(0.0, 0.0, 0.0);
    for (int i = 0; i < lightUBO.lightCount; ++i)
    {
        vec3 L = normalize(lightUBO.lights[0].direction.xyz - position);
        float shade = max(dot(normal, normalize(lightUBO.lights[i].direction.xyz)), 0.0);
        float shadow = filterPCF(lightUBO.lights[i].mvp * vec4(position, 1.0), i);
        shade = max(shade * shadow, renderUBO.ambient.x);
        vec3 shadeColor = lightUBO.lights[i].color.xyz * shade;
        shadedColor += shadeColor;
        shadedColor += BRDF(L, V, N, shadeColor, pbr.r, pbr.g);
    }
    outFragColor = vec4(albedo * shadedColor, 1.0);
    outFragColor += vec4(albedo * renderUBO.ambient.xyz, 1.0);
    outFragColor += vec4(albedo * pbr.b, 1.0);
}

