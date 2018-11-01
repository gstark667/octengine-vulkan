#version 450
#extension GL_ARB_separate_shader_objects : enable

layout(location = 0) in vec3 inPosition;
layout(location = 1) in vec3 inNormal;
layout(location = 2) in vec2 inTexCoord;
layout(location = 5) in vec3 instancePos;
layout(location = 6) in vec3 instanceOff;
layout(location = 7) in vec3 instanceScale;
layout(location = 8) in ivec3 instanceTex;

layout(location = 0) out vec2 outUV;
layout(location = 1) out int outTextureIdx;

out gl_PerVertex
{
    vec4 gl_Position;
};

void main() 
{
    outUV.x = inTexCoord.x * instanceOff.x + instanceOff.z;
    outUV.y = inTexCoord.y * instanceOff.y;
    outTextureIdx = instanceTex.x;
    mat4 scale = mat4(instanceScale.x * 2.0, 0.0, 0.0, 0.0,
                      0.0, instanceScale.y * 2.0, 0.0, 0.0,
                      0.0, 0.0, instanceScale.z * 2.0, 0.0,
                      0.0, 0.0, 0.0, 1.0);
    gl_Position = (vec4(inPosition, 1.0) * scale + vec4(instancePos * 2.0f, 1.0));
}
