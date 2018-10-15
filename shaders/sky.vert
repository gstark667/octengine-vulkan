#version 450
#extension GL_ARB_separate_shader_objects : enable

layout(binding = 0) uniform uniform_buffer_object {
    mat4 mvp;
    mat4 shadowSpace;
} ubo;

layout(location = 0) in vec3 inPosition;

layout (location = 0) out vec3 outPosition;

out gl_PerVertex {
    vec4 gl_Position;
};

void main() {
    gl_Position = ubo.mvp * vec4(inPosition, 1.0);
    outPosition = inPosition;
}
