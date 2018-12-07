#version 450
#extension GL_ARB_separate_shader_objects : enable
#extension GL_GOOGLE_include_directive : enable

#include "rotate"

layout(binding = 0) uniform uniform_buffer_object {
    mat4 cameraMVP;
    mat4 shadowSpace;
} ubo;

layout(binding = 1) uniform uniform_buffer_object_bones {
    mat4 mats[64];
} bones;

layout(location = 0) in vec3 inPosition;
layout(location = 1) in vec3 inNormal;
layout(location = 2) in vec2 inTexCoord;
layout(location = 3) in vec4 inWeights;
layout(location = 4) in ivec4 inBones;

layout(location = 5) in vec3 instancePos;
layout(location = 6) in vec3 instanceRot;
layout(location = 7) in vec3 instanceScale;
layout(location = 8) in ivec3 instanceTex;

layout (location = 0) out vec3 outNormal;
layout (location = 1) out vec2 outUV;
layout (location = 2) out vec3 outColor;
layout (location = 3) out vec4 outWorldPos;
layout (location = 4) out ivec3 outTexIdx;

out gl_PerVertex {
    vec4 gl_Position;
};


const mat4 biasMat = mat4( 
	0.5, 0.0, 0.0, 0.0,
	0.0, 0.5, 0.0, 0.0,
	0.0, 0.0, 1.0, 0.0,
        0.5, 0.5, 0.0, 1.0 );

void main() {
    mat4 scale = mat4(instanceScale.x, 0.0, 0.0, 0.0,
                      0.0, instanceScale.y, 0.0, 0.0,
                      0.0, 0.0, instanceScale.z, 0.0,
                      0.0, 0.0, 0.0, 1.0);

    mat4 boneTransform = rotate_bone(inBones, inWeights, bones.mats);
    mat3 rotMat = rotate_euler(instanceRot);

    vec4 worldPos = mat4(rotMat) * scale * boneTransform * vec4(inPosition, 1.0) + vec4(instancePos, 1.0);
    gl_Position = ubo.cameraMVP * worldPos;
    outNormal = rotMat * mat3(boneTransform) * inNormal;
    outUV = inTexCoord;
    outColor = vec3(1.0, 1.0, 1.0);
    outTexIdx = instanceTex;
    outWorldPos = vec4(worldPos.xyz * 0.5, 1.0);
}
