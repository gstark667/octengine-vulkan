#version 450
#extension GL_ARB_separate_shader_objects : enable

layout(binding = 0) uniform uniform_buffer_object {
    mat4 model;
    mat4 view;
    mat4 proj;
    mat4 shadowSpace;
    vec3 lightPos;
    mat4 bones[64];
} ubo;

layout(location = 0) in vec3 inPosition;
layout(location = 3) in vec4 inWeights;
layout(location = 4) in ivec4 inBones;

layout(location = 5) in vec3 instancePos;
layout(location = 6) in vec3 instanceRot;
layout(location = 7) in float instanceScale;

out gl_PerVertex
{
    vec4 gl_Position;
};

void main() 
{
    mat4 boneTransform = ubo.bones[inBones[0]] * inWeights[0];
    boneTransform     += ubo.bones[inBones[1]] * inWeights[1];
    boneTransform     += ubo.bones[inBones[2]] * inWeights[2];
    boneTransform     += ubo.bones[inBones[3]] * inWeights[3];

    mat3 mx, my, mz;
	
    // rotate around z
    float s = sin(instanceRot.z);
    float c = cos(instanceRot.z);
    mz[0] = vec3(c, s, 0.0);
    mz[1] = vec3(-s, c, 0.0);
    mz[2] = vec3(0.0, 0.0, 1.0);

    // rotate around y
    s = sin(-instanceRot.y);
    c = cos(-instanceRot.y);
    my[0] = vec3(c, 0.0, s);
    my[1] = vec3(0.0, 1.0, 0.0);
    my[2] = vec3(-s, 0.0, c);

    // rot around x
    s = sin(instanceRot.x);
    c = cos(instanceRot.x);	

    mx[0] = vec3(1.0, 0.0, 0.0);
    mx[1] = vec3(0.0, c, s);
    mx[2] = vec3(0.0, -s, c);

    mat3 rotMat = mz * my * mx;

    vec4 worldPos = mat4(rotMat) * vec4(inPosition * instanceScale, 1.0) + vec4(instancePos, 1.0);
    gl_Position = ubo.proj * ubo.view * mat4(rotMat) * boneTransform * vec4(inPosition, 1.0) + (ubo.proj * ubo.view * vec4(instancePos, 1.0));
    gl_Position = ubo.shadowSpace * worldPos;
}
