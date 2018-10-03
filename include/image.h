#ifndef H_IMAGE
#define H_IMAGE


struct image_t
{
    uint32_t width, height;
    uint32_t layers = 1;
    uint32_t mipLevels = 0;
    bool forceArray = false;
    VkImage image = NULL;
    VkDeviceMemory memory = NULL;
    VkImageView view = NULL;
    VkFormat format;
    VkImageUsageFlags usage;
    VkSampleCountFlagBits samples;
};

void image_create(image_t *image, VkDevice device, VkPhysicalDevice physicalDevice, uint32_t width, uint32_t height, uint32_t layers, uint32_t mipLevels, VkFormat format, VkImageTiling tiling, VkImageUsageFlags usage, VkMemoryPropertyFlags properties, VkSampleCountFlagBits samples);
void image_create_view(image_t *image, VkDevice device, VkFormat format, VkImageAspectFlags aspectFlags, uint32_t layer, bool allLayers);
void image_transition_layout(image_t *image, VkDevice device, VkCommandPool commandPool, VkQueue graphicsQueue, VkFormat format, VkImageLayout oldLayout, VkImageLayout newLayout);
void image_generate_mipmaps(image_t *image, VkDevice device, VkPhysicalDevice physicalDevice, VkCommandPool commanPool, VkQueue graphicsQueue);
void image_copy_buffer(image_t *image, VkDevice device, VkCommandPool commandPool, VkQueue graphicsQueue, VkBuffer buffer);
void image_cleanup(image_t *image, VkDevice device);

#endif
