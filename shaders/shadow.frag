#version 450
#extension GL_ARB_separate_shader_objects : enable

layout(location = 0) out float outFragDepth;

void main() {
    outFragDepth = gl_FragCoord.z;
}
