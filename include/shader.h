#ifndef H_SHADER
#define H_SHADER

#include <string>


struct shader_t
{
    VkShaderModule vertShaderModule;
    VkShaderModule fragShaderModule;
};

void shader_create(shader_t *shader, VkDevice *device, std::string vertPath, std::string fragPath);

#endif
