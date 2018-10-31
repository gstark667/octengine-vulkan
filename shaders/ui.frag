#version 450
#extension GL_ARB_separate_shader_objects : enable

layout (binding = 0) uniform sampler2D samplerComposite;
layout (binding = 1) uniform sampler2D samplerFont;

layout (location = 0) in vec2 inUV;
layout (location = 1) flat in int textureIdx;

layout (location = 0) out vec4 outFragColor;

void main()
{
    if (textureIdx == -1)
        outFragColor = texture(samplerComposite, inUV).rgba;
    else if (textureIdx == 0)
        outFragColor = texture(samplerFont, inUV).rgba;
    else
        outFragColor = vec4(0.0, 0.0, 0.0, 1.0);
}
