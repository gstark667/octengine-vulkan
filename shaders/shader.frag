#version 450
#extension GL_ARB_separate_shader_objects : enable

layout(binding = 1) uniform sampler2D texSampler;

layout(location = 9) in vec3 fragNormal;
layout(location = 10) in vec3 fragColor;
layout(location = 11) in vec2 fragTexCoord;
layout(location = 12) flat in int fragTexIdx;

layout(location = 0) out vec4 outColor;

void main() {
    vec3 normal = (texture(texSampler, fragTexCoord).rgb * 2 - 1);

    vec3 tangent = normalize(normal - fragNormal * dot(normal, fragNormal));
    vec3 bitangent = cross(tangent, fragNormal);
    mat3 TBN = mat3(tangent, bitangent, fragNormal);

    normal = TBN * normal;

    float shade = max(dot(normal, vec3(0.8, 0.8, 0.8)), 0.5);
    outColor = texture(texSampler, fragTexCoord) * shade;
}
