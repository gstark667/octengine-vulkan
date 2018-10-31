#version 450
#extension GL_ARB_separate_shader_objects : enable

layout(location = 0) in vec3 inPosition;
layout(location = 1) in vec3 inNormal;
layout(location = 2) in vec2 inTexCoord;
layout(location = 5) in vec3 instancePos;
layout(location = 7) in vec3 instanceScale;

layout(location = 0) out vec2 outUV;

out gl_PerVertex
{
    vec4 gl_Position;
};

void main() 
{
    outUV = inTexCoord;
    mat4 scale = mat4(instanceScale.x, 0.0, 0.0, 0.0,
                      0.0, instanceScale.y, 0.0, 0.0,
                      0.0, 0.0, instanceScale.z, 0.0,
                      0.0, 0.0, 0.0, 1.0);
    gl_Position = vec4(inPosition + instancePos, 1.0) * scale;
}
