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
    uint32_t mipLevels = 4;
    bool needsUpdate = false;
    bool cube = false;
    image_t image;
    std::vector<texture_data_t> data;
    std::map<std::string, size_t> indicies;
    VkSampler sampler;
};

void texture_add(texture_t *texture, VkDevice device, VkPhysicalDevice physicalDevice, VkCommandPool commandPool, VkQueue graphicsQueue, std::string path, bool load);
size_t texture_get(texture_t *texture, VkDevice device, VkPhysicalDevice physicalDevice, VkCommandPool commandPool, VkQueue graphicsQueue, std::string path, bool load);
void texture_load(texture_t *texture, VkDevice device, VkPhysicalDevice physicalDevice, VkCommandPool commandPool, VkQueue graphicsQueue);
void texture_cleanup(texture_t *texture, VkDevice device);

#endif
