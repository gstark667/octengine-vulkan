#version 450
#extension GL_ARB_separate_shader_objects : enable

layout(binding = 1) uniform sampler2DArray texSampler;

layout(location = 9) in vec3 fragNormal;
layout(location = 10) in vec3 fragColor;
layout(location = 11) in vec2 fragTexCoord;
layout(location = 12) flat in int fragTexIdx;

layout(location = 0) out vec4 outColor;

void main() {
    float shade = max(dot(fragNormal, vec3(2, 2, 0)), 0.5);
    outColor = texture(texSampler, vec3(fragTexCoord, fragTexIdx)) * vec4(fragColor.x * shade, fragColor.y * shade, fragColor.z * shade, 1.0f);
}
