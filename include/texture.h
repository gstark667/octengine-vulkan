#ifndef H_TEXTURE
#define H_TEXTURE

#include "image.h"

#include <string>


struct texture_t
{
    image_t image;
    VkSampler sampler;
};

void texture_load(texture_t *texture, VkDevice *device, VkPhysicalDevice *physicalDevice, VkCommandPool *commandPool, VkQueue *graphicsQueue, std::string path);
void texture_cleanup(texture_t *texture, VkDevice *device);

#endif
