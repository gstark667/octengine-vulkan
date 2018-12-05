#version 450
#extension GL_ARB_separate_shader_objects : enable

layout (binding = 0) uniform sampler2D samplerComposite;
layout (binding = 1) uniform sampler2D samplerBright;

layout (location = 0) in vec2 inUV;
layout (location = 0) out vec4 outFragColor;

void main()
{
    vec3 color = texture(samplerComposite, inUV).rgb;
    vec3 bright = texture(samplerBright, inUV).rgb;
    outFragColor = vec4(color + bright, 1.0);
}

