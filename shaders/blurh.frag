#version 450
#extension GL_ARB_separate_shader_objects : enable

layout (binding = 0) uniform sampler2D samplerColor;

struct light {
    vec4 position;
    vec4 direction;
    vec4 color;
    ivec4 shadowIdx;
    mat4 mvp;
};

layout (location = 0) in vec2 inUV;
layout (location = 0) out vec4 outFragColor;

#define SHADOW_FACTOR 0.0
#define PI 3.14159

void main()
{
    float weight[5];
    weight[0] = 0.227027;
    weight[1] = 0.1945946;
    weight[2] = 0.1216216;
    weight[3] = 0.054054;
    weight[4] = 0.016216;

    vec2 tex_offset = 1.0 / textureSize(samplerColor, 0) * 0.1; // gets size of single texel
    vec3 result = texture(samplerColor, inUV).rgb * weight[0]; // current fragment's contribution
    for(int i = 1; i < 5; ++i)
    {
        result += texture(samplerColor, inUV + vec2(tex_offset.x * i, 0.0)).rgb * weight[i];
        result += texture(samplerColor, inUV - vec2(tex_offset.x * i, 0.0)).rgb * weight[i];
    }
    outFragColor = vec4(result, 1.0);
}

