#ifndef H_TEXTURE
#define H_TEXTURE

#include "image.h"


struct texture_t
{
    image_t image;
    VkSampler textureSampler;
};

#endif
