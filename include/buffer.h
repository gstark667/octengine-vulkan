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
    size_t size;
    size_t count;
};

void buffer_create(buffer_t *buffer, VkDevice device, VkPhysicalDevice physicalDevice, VkBufferUsageFlags usage, VkMemoryPropertyFlags properties, size_t size);
void buffer_copy(buffer_t *buffer, VkDevice device, VkPhysicalDevice physicalDevice, VkCommandPool commandPool, VkQueue graphicsQueue, void *data, size_t size);
void buffer_destroy(buffer_t *buffer, VkDevice device);

#endif
