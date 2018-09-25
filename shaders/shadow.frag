#version 450
#extension GL_ARB_separate_shader_objects : enable

layout(binding = 1) uniform sampler2DArray texSampler;

layout (location = 0) in vec2 inUV;
layout (location = 1) flat in int inTexIdx;

layout (location = 0) out vec4 outFragColor;

void main() {
    outFragColor = vec4(1.0, 1.0, 1.0, 1.0);
    outFragColor = vec4(texture(texSampler, vec3(inUV, inTexIdx)).rgb, 1.0);
}
