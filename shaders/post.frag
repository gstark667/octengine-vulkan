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

vec3 toScreenSpace(vec3 position)
{
    vec4 result = postUBO.cameraMVP * vec4(position, 1.0);
    return vec3(result.xy / result.w * 0.5 + 0.5, result.z);
}

float calcDist(vec3 pos1, vec3 pos2)
{
    return distance(pos1, postUBO.cameraPos.xyz) - distance(pos2, postUBO.cameraPos.xyz);
}

vec3 SSR(vec3 pos, vec3 dir)
{
    float dist = 0.0;
    for (int i = 0; i < 25; ++i)
    {
        dist = calcDist(texture(samplerPosition, toScreenSpace(pos).xy).xyz, pos);
        if (dist > 0.0)
        {
            pos += dir;
        }
        else
        {
            dir *= 0.5;
            pos -= dir;
        }
    }
    vec2 uv = toScreenSpace(pos).xy;
    //if (uv.x > 1.0 || uv.x < 0.0 || uv.y > 1.0 || uv.y < 0.0 || dist > 0.1 || dist < -0.1)
    //    return vec3(0.0);
    return texture(samplerComposite, uv).rgb;
}

void main()
{
    vec3 color = texture(samplerComposite, inUV).rgb;
    vec3 bright = texture(samplerBright, inUV).rgb;
    vec3 normal = normalize(texture(samplerNormal, inUV).rgb);
    vec3 position = texture(samplerPosition, inUV).rgb;

    vec3 V = normalize(position - postUBO.cameraPos.xyz);
    vec3 R = reflect(V, normal);

    outFragColor = vec4(color + bright, 1.0);
    //outFragColor = vec4(SSR(position + R, R), 1.0);
}

