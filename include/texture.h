#ifndef H_TEXTURE
#define H_TEXTURE

#include "image.h"

#include <vulkan/vulkan.h>
#include <FreeImage.h>
#include <string>
#include <vector>
#include <map>


struct texture_data_t
{
    uint32_t width = 0;
    uint32_t height = 0;
    size_t size = 0;
    size_t index = 0;
    std::vector<BYTE> data;
    image_t image;
};

struct texture_t
{
    bool needsUpdate = false;
    bool cube = false;
    bool combined = true;
    bool smooth = true;
    std::vector<texture_data_t> data;
    std::map<std::string, size_t> indicies;
    VkSampler sampler;
};

void texture_data_load(texture_t *texture, texture_data_t *textureData, VkDevice device, VkPhysicalDevice physicalDevice, VkCommandPool commandPool, VkQueue graphicsQueue, size_t idx);
void texture_add(texture_t *texture, VkDevice device, VkPhysicalDevice physicalDevice, VkCommandPool commandPool, VkQueue graphicsQueue, std::string path, bool load);
size_t texture_get(texture_t *texture, VkDevice device, VkPhysicalDevice physicalDevice, VkCommandPool commandPool, VkQueue graphicsQueue, std::string path, bool load);
//void texture_load(texture_t *texture, VkDevice device, VkPhysicalDevice physicalDevice, VkCommandPool commandPool, VkQueue graphicsQueue);
void texture_cleanup(texture_t *texture, VkDevice device);

#endif
