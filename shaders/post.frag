#version 450
#extension GL_ARB_separate_shader_objects : enable

layout (binding = 0) uniform sampler2D samplerComposite;
layout (binding = 1) uniform sampler2D samplerBright;
layout (binding = 2) uniform sampler2D samplerCurrPosition;
//layout (binding = 3) uniform sampler2D samplerLastPosition;

struct light {
    vec4 position;
    vec4 direction;
    vec4 color;
    ivec4 shadowIdx;
    mat4 mvp;
};

layout (location = 0) in vec2 inUV;
layout (location = 0) out vec4 outFragColor;

void main()
{
    vec3 color = texture(samplerComposite, inUV).rgb;
    vec3 bright = texture(samplerBright, inUV).rgb;
    vec3 currPosition = texture(samplerCurrPosition, inUV).rgb;
    //vec3 lastPosition = texture(samplerLastPosition, inUV).rgb;

    outFragColor = vec4(color + bright, 1.0);
    //outFragColor = vec4(lastPosition - currPosition, 1.0);
    outFragColor = vec4(currPosition, 1.0);
}

