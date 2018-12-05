#version 450
#extension GL_ARB_separate_shader_objects : enable

layout (binding = 0) uniform sampler2D samplerComposite;
layout (binding = 1) uniform sampler2D samplerBright;

struct light {
    vec4 position;
    vec4 direction;
    vec4 color;
    ivec4 shadowIdx;
    mat4 mvp;
};

layout (location = 0) in vec2 inUV;
layout (location = 0) out vec4 outFragColor;

#define SHADOW_FACTOR 0.0
#define PI 3.14159

vec3 blur(vec2 uv, float spread)
{
    vec3 result = vec3(0.0);
    float angleSpread = spread * 0.8;
    result += texture(samplerBright, inUV + vec2(angleSpread, angleSpread)).rgb * 0.5;
    result += texture(samplerBright, inUV + vec2(spread, 0.0)).rgb * 0.75;
    result += texture(samplerBright, inUV + vec2(angleSpread, -angleSpread)).rgb * 0.5;
    result += texture(samplerBright, inUV + vec2(0.0, spread)).rgb * 0.75;
    result += texture(samplerBright, inUV).rgb;
    result += texture(samplerBright, inUV + vec2(0.0, -spread)).rgb * 0.75;
    result += texture(samplerBright, inUV + vec2(-angleSpread, angleSpread)).rgb * 0.5;
    result += texture(samplerBright, inUV + vec2(-spread, 0.0)).rgb;
    result += texture(samplerBright, inUV + vec2(-angleSpread, -angleSpread)).rgb * 0.5;
    return result/7.0;
}

void main()
{
    vec3 color = texture(samplerComposite, inUV).rgb;
    //vec3 bright = texture(samplerBright, inUV).rgb;
    vec3 bright = blur(inUV, 0.003);

    //if (brightness > 0.75)
    //    outFragColor = vec4(color + blurResult, 1.0);
    //else
        //outFragColor = vec4(0.0, 0.0, 0.0, 1.0);
    outFragColor = vec4(color + bright, 1.0);
    outFragColor = vec4(bright, 1.0);
}

