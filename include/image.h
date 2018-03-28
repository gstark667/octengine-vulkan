#ifndef H_IMAGE
#define H_IMAGE


struct image_t
{
    uint32_t width, height;
    VkImage image;
    VkDeviceMemory memory;
    VkImageView view;
};

void image_create(image_t *image, VkDevice *device, VkPhysicalDevice *physicalDevice, uint32_t width, uint32_t height, VkFormat format, VkImageTiling tiling, VkImageUsageFlags usage, VkMemoryPropertyFlags properties);
void image_create_view(image_t *image, VkDevice *device, VkFormat format, VkImageAspectFlags aspectFlags);
void image_transition_layout(image_t *image, VkDevice *device, VkCommandPool *commandPool, VkQueue *graphicsQueue, VkFormat format, VkImageLayout oldLayout, VkImageLayout newLayout);

#endif
