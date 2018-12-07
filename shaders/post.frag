#version 450
#extension GL_ARB_separate_shader_objects : enable

layout (binding = 0) uniform sampler2D samplerComposite;
layout (binding = 1) uniform sampler2D samplerBright;
layout (binding = 2) uniform sampler2D samplerNormal;
layout (binding = 3) uniform sampler2D samplerPosition;

layout (location = 0) in vec2 inUV;
layout (location = 0) out vec4 outFragColor;

layout (binding = 4) uniform post_uniform_buffer_object {
    mat4 cameraMVP;
    vec4 cameraPos;
} postUBO;

vec2 toScreenSpace(vec3 position)
{
    vec4 result = postUBO.cameraMVP * vec4(position, 1.0);
    return result.st / result.w * 0.5 + 0.5;
}

vec3 SSR(vec3 pos, vec3 dir)
{
    for (int i = 0; i < 100; ++i)
    {
        float depth = texture(samplerPosition, toScreenSpace(pos)).z;
        if (depth < pos.z)
        {
            pos += dir;
        }
        else
        {
            dir *= 0.5;
            pos -= dir;
        }
    }
    return texture(samplerComposite, toScreenSpace(pos)).rgb;
}

void main()
{
    vec3 color = texture(samplerComposite, inUV).rgb;
    vec3 bright = texture(samplerBright, inUV).rgb;
    vec3 normal = texture(samplerNormal, inUV).rgb;
    vec3 position = texture(samplerPosition, inUV).rgb;

    vec3 R = reflect(position - postUBO.cameraPos.xyz, normal) * 0.1;

    outFragColor = vec4(color + bright, 1.0);
    outFragColor = vec4(R, 1.0);
    //outFragColor = vec4(texture(samplerComposite, toScreenSpace(position)).rgb, 1.0);
    //outFragColor = vec4(texture(samplerComposite, toScreenSpace(ref)).rgb, 1.0);
    outFragColor = vec4(SSR(position + R, R), 1.0);
}

