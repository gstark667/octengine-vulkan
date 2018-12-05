#version 450
#extension GL_ARB_separate_shader_objects : enable

layout (binding = 0) uniform sampler2DMS samplerCurrPosition;

struct light {
    vec4 position;
    vec4 direction;
    vec4 color;
    ivec4 shadowIdx;
    mat4 mvp;
};

layout (location = 0) in vec2 inUV;
layout (location = 0) out vec4 outFragColor;

vec4 resolve(sampler2DMS tex, ivec2 uv)
{
    vec4 result = vec4(0.0);
    for (int i = 0; i < 2; i++)
    {
        result += texelFetch(tex, uv, i);
    }
    return result/2.0;
}

void main()
{
    ivec2 attDim = textureSize(samplerCurrPosition);
    ivec2 UV = ivec2(inUV * attDim);

    outFragColor = resolve(samplerCurrPosition, UV);
}

