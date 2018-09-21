#ifndef H_TEXTURE
#define H_TEXTURE

#include "image.h"

#include <FreeImage.h>
#include <string>
#include <vector>
#include <map>


struct texture_data_t
{
    uint32_t width;
    uint32_t height;
    size_t size;
    size_t index;
    std::vector<BYTE> data;
};

struct texture_t
{
    uint32_t width;
    uint32_t height;
    image_t image;
    std::map<std::string, texture_data_t> data;
    VkSampler sampler;
};

bool texture_add(texture_t *texture, VkDevice device, VkPhysicalDevice physicalDevice, VkCommandPool commandPool, VkQueue graphicsQueue, std::string path);
void texture_load(texture_t *texture, VkDevice device, VkPhysicalDevice physicalDevice, VkCommandPool commandPool, VkQueue graphicsQueue);
void texture_cleanup(texture_t *texture, VkDevice device);

#endif
