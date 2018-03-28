#version 450
#extension GL_ARB_separate_shader_objects : enable


layout(location = 0) in vec3 fragNormal;
layout(location = 1) in vec2 fragTexCoord;

layout(location = 0) out vec4 outColor;

void main() {
    float shade = max(dot(fragNormal, vec3(2, -2, 0)), 0.5);
    outColor = vec4(shade, shade, shade, 1.0f);
}
