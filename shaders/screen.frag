#version 450
#extension GL_ARB_separate_shader_objects : enable

layout (binding = 1) uniform sampler2D samplerAlbedo;
layout (binding = 2) uniform sampler2D samplerNormal;
layout (binding = 3) uniform sampler2D samplerPosition;

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
    vec3 fragPos = texture(samplerPosition, inUV).rgb;
    outFragColor = vec4(fragPos, 1.0);
}
