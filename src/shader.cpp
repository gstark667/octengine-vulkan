#include <vulkan/vulkan.h>

#include "shader.h"
#include "util.h"

#include <vector>
#include <stdexcept>


using namespace std;

void create_shader_module(VkDevice *device, VkShaderModule *shaderModule, vector<char> code)
{
    VkShaderModuleCreateInfo createInfo = {};
    createInfo.sType = VK_STRUCTURE_TYPE_SHADER_MODULE_CREATE_INFO;
    createInfo.codeSize = code.size();
    createInfo.pCode = reinterpret_cast<const uint32_t*>(code.data());

    if (vkCreateShaderModule(*device, &createInfo, nullptr, shaderModule) != VK_SUCCESS) {
        throw std::runtime_error("failed to create shader module!");
    }
}

void shader_create(shader_t *shader, VkDevice *device, std::string vertPath, std::string fragPath)
{
    vector<char> vertShaderCode = read_file(vertPath);
    vector<char> fragShaderCode = read_file(fragPath);

    create_shader_module(device, &shader->vertShaderModule, vertShaderCode);
    create_shader_module(device, &shader->fragShaderModule, fragShaderCode);
}
