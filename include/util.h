#ifndef H_UTIL
#define H_UTIL

#include <vector>
#include <string>


std::vector<char> read_file(const std::string filename);

VkCommandBuffer begin_single_time_commands(VkDevice *device, VkCommandPool *commandPool);
void end_single_time_commands(VkDevice *device, VkCommandPool *commandPool, VkQueue *graphicsQueue, VkCommandBuffer commandBuffer);

uint32_t find_memory_type(VkPhysicalDevice *physicalDevice, uint32_t typeFilter, VkMemoryPropertyFlags properties);
void create_buffer(VkDevice *device, VkPhysicalDevice *physicalDevice, VkDeviceSize size, VkBufferUsageFlags usage, VkMemoryPropertyFlags properties, VkBuffer *buffer, VkDeviceMemory *bufferMemory);
void copy_buffer(VkDevice *device, VkCommandPool *commandPool, VkQueue *graphicsQueue, VkBuffer srcBuffer, VkBuffer dstBuffer, VkDeviceSize size);

#endif
