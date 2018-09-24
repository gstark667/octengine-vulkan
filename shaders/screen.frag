#version 450
#extension GL_ARB_separate_shader_objects : enable

layout (binding = 0) uniform sampler2D samplerAlbedo;
layout (binding = 1) uniform sampler2D samplerNormal;
layout (binding = 2) uniform sampler2D samplerPosition;
layout (binding = 3) uniform sampler2D samplerDepth;

layout (location = 0) in vec2 inUV;

layout (location = 0) out vec4 outFragColor;

struct Light {
	vec4 position;
	vec3 color;
	float radius;
};

layout (binding = 4) uniform UBO 
{
	Light lights[6];
	vec4 viewPos;
} ubo;

void main() {
    vec3 albedo = texture(samplerAlbedo, inUV).rgb;
    vec3 normal = texture(samplerNormal, inUV).rgb;
    vec3 position = texture(samplerPosition, inUV).rgb;
    float shade = max(dot(normal, vec3(0.8, 0.8, 0.8)), 0.5);
    outFragColor = vec4(albedo * shade, 1.0);
}
