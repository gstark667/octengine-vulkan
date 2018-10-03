#version 450
#extension GL_ARB_separate_shader_objects : enable
#extension GL_GOOGLE_include_directive : enable

#include "rotate.vert"

layout(binding = 0) uniform uniform_buffer_object {
    mat4 cameraMVP;
    mat4 shadowSpace;
} ubo;

layout(binding = 1) uniform uniform_buffer_object_bones {
    mat4 mats[64];
} bones;

layout(location = 0) in vec3 inPosition;
layout(location = 2) in vec2 inTexCoord;
layout(location = 3) in vec4 inWeights;
layout(location = 4) in ivec4 inBones;

layout(location = 5) in vec3 instancePos;
layout(location = 6) in vec3 instanceRot;
layout(location = 7) in float instanceScale;
layout(location = 8) in int instanceTex;

out gl_PerVertex
{
    vec4 gl_Position;
};

void main() 
{
    mat4 boneTransform;
    if (inBones[0] == -1)
    {
        boneTransform = mat4(1.0);
    }
    else
    {
        boneTransform = mat4(0.0);
        boneTransform += bones.mats[inBones[0]] * inWeights[0];
        boneTransform += bones.mats[inBones[1]] * inWeights[1];
        boneTransform += bones.mats[inBones[2]] * inWeights[2];
        boneTransform += bones.mats[inBones[3]] * inWeights[3];
    }

    mat3 rotMat = rotate_euler(instanceRot);

    vec4 worldPos = mat4(rotMat) * boneTransform * vec4(inPosition * instanceScale, 1.0) + vec4(instancePos, 1.0);
    gl_Position = ubo.cameraMVP * worldPos;
}
