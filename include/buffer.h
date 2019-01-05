#ifndef H_BUFFER
#define H_BUFFER

#include <vulkan/vulkan.h>
#include <fstream>

struct buffer_t
{
    VkBuffer buffer;
    VkDeviceMemory memory;
    VkBufferUsageFlags usage;
    VkMemoryPropertyFlags properties;
    VkPhysicalDevice physicalDevice;
    size_t size;
    size_t count;
};

void buffer_create(buffer_t *buffer, VkDevice device, VkPhysicalDevice physicalDevice, VkBufferUsageFlags usage, VkMemoryPropertyFlags properties, size_t size);
void buffer_resize(buffer_t *buffer, VkDevice device, size_t size);
void buffer_copy(buffer_t *buffer, VkDevice device, VkPhysicalDevice physicalDevice, VkCommandPool commandPool, VkQueue graphicsQueue, void *data, size_t size);
bool buffer_stage(buffer_t *dst, buffer_t *src, VkDevice device, VkCommandBuffer commandBuffer, void *data, size_t size);
void buffer_inline_copy(buffer_t *dst, buffer_t *src, VkCommandBuffer commandBuffer);
void buffer_destroy(buffer_t *buffer, VkDevice device);

#endif
