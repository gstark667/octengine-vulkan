#version 450
#extension GL_ARB_separate_shader_objects : enable

layout (binding = 0) uniform sampler2D samplerComposite;
layout (binding = 1) uniform sampler2D samplerFont;
layout (binding = 2) uniform sampler samp;
layout (binding = 3) uniform texture2D textures[80];

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
        //outFragColor = texture(sampler2D(textures[textureIdx], samp), inUV).rgba;
        outFragColor = vec4(0.0, 0.0, 0.0, 1.0);
}
