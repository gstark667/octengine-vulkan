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

float weight[5] = float[5](0.227027, 0.1945946, 0.1216216, 0.054054, 0.016216);

#define SHADOW_FACTOR 0.0
#define PI 3.14159

void main()
{

    vec2 tex_offset = 1.0 / textureSize(samplerColor, 0); // gets size of single texel
    vec3 result = texture(samplerColor, inUV).rgb * weight[0]; // current fragment's contribution
    for(int i = 1; i < 5; ++i)
    {
        result += texture(samplerColor, inUV + vec2(0.0, tex_offset.y * i)).rgb * weight[i] * 1.5;
        result += texture(samplerColor, inUV - vec2(0.0, tex_offset.y * i)).rgb * weight[i] * 1.5;
    }
    outFragColor = vec4(result, 1.0);
}

