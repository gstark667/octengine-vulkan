#ifndef H_IMAGE
#define H_IMAGE


struct image_t
{
    uint32_t width, height;
    uint32_t layers;
    VkImage image;
    VkDeviceMemory memory;
    VkImageView view;
};

void image_create(image_t *image, VkDevice device, VkPhysicalDevice physicalDevice, uint32_t width, uint32_t height, uint32_t layers, VkFormat format, VkImageTiling tiling, VkImageUsageFlags usage, VkMemoryPropertyFlags properties);
void image_create_view(image_t *image, VkDevice device, VkFormat format, VkImageAspectFlags aspectFlags);
void image_transition_layout(image_t *image, VkDevice device, VkCommandPool commandPool, VkQueue graphicsQueue, VkFormat format, VkImageLayout oldLayout, VkImageLayout newLayout);
void image_copy_buffer(image_t *image, VkDevice device, VkCommandPool commandPool, VkQueue graphicsQueue, VkBuffer buffer);
void image_cleanup(image_t *image, VkDevice device);

#endif
