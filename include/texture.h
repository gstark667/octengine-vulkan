#ifndef H_TEXTURE
#define H_TEXTURE

#include "image.h"

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
};

struct texture_t
{
    uint32_t width = 0;
    uint32_t height = 0;
    image_t image;
    std::map<std::string, texture_data_t> data;
    VkSampler sampler;
};

bool texture_add(texture_t *texture, VkDevice device, VkPhysicalDevice physicalDevice, VkCommandPool commandPool, VkQueue graphicsQueue, std::string path);
size_t texture_get(texture_t *texture, VkDevice device, VkPhysicalDevice physicalDevice, VkCommandPool commandPool, VkQueue graphicsQueue, std::string path);
void texture_load(texture_t *texture, VkDevice device, VkPhysicalDevice physicalDevice, VkCommandPool commandPool, VkQueue graphicsQueue);
void texture_cleanup(texture_t *texture, VkDevice device);

#endif
