#include "buffer.h"
#include "util.h"

#include <cstring>
#include <iostream>


void buffer_create(buffer_t *buffer, VkDevice device, VkPhysicalDevice physicalDevice, VkBufferUsageFlags usage, VkMemoryPropertyFlags properties, size_t size)
{
    buffer->usage = usage;
    buffer->properties = properties;
    buffer->size = size;
    buffer->physicalDevice = physicalDevice;
    if (size == 0)
        size = 1;

    VkBufferCreateInfo bufferInfo = {};
    bufferInfo.sType = VK_STRUCTURE_TYPE_BUFFER_CREATE_INFO;
    bufferInfo.size = size;
    bufferInfo.usage = usage;
    bufferInfo.sharingMode = VK_SHARING_MODE_EXCLUSIVE;

    if (vkCreateBuffer(device, &bufferInfo, nullptr, &buffer->buffer) != VK_SUCCESS)
    {
        throw std::runtime_error("failed to create buffer!");
    }

    VkMemoryRequirements memRequirements;
    vkGetBufferMemoryRequirements(device, buffer->buffer, &memRequirements);
    VkMemoryAllocateInfo allocInfo = {};
    allocInfo.sType = VK_STRUCTURE_TYPE_MEMORY_ALLOCATE_INFO;
    allocInfo.allocationSize = memRequirements.size;
    allocInfo.memoryTypeIndex = find_memory_type(physicalDevice, memRequirements.memoryTypeBits, properties);

    if (vkAllocateMemory(device, &allocInfo, nullptr, &buffer->memory) != VK_SUCCESS) {
        throw std::runtime_error("failed to allocate buffer memory!");
    }

    vkBindBufferMemory(device, buffer->buffer, buffer->memory, 0);
}

void buffer_resize(buffer_t *buffer, VkDevice device, size_t size)
{
    buffer_destroy(buffer, device);
    buffer_create(buffer, device, buffer->physicalDevice, buffer->usage, buffer->properties, size);
    buffer->size = size;
}

void buffer_copy(buffer_t *buffer, VkDevice device, VkPhysicalDevice physicalDevice, VkCommandPool commandPool, VkQueue graphicsQueue, void *data, size_t size)
{
    if (buffer->size != size)
    {
        buffer_resize(buffer, device, size);
    }

    buffer_t stagingBuffer;
    buffer_create(&stagingBuffer, device, physicalDevice, VK_BUFFER_USAGE_TRANSFER_SRC_BIT, VK_MEMORY_PROPERTY_HOST_VISIBLE_BIT | VK_MEMORY_PROPERTY_HOST_COHERENT_BIT, size);

    VkCommandBuffer commandBuffer = begin_single_time_commands(device, commandPool);

    buffer_stage(buffer, &stagingBuffer, device, commandBuffer, data, size);
    buffer_inline_copy(buffer, &stagingBuffer, commandBuffer);

    end_single_time_commands(device, commandPool, graphicsQueue, commandBuffer);

    buffer_destroy(&stagingBuffer, device);
}

void buffer_stage(buffer_t *dst, buffer_t *src, VkDevice device, VkCommandBuffer commandBuffer, void *data, size_t size)
{
    if (dst->size != size)
    {
        buffer_resize(dst, device, size);
    }

    if (src->size != size)
    {
        buffer_resize(src, device, size);
    }

    void *tempData;
    vkMapMemory(device, src->memory, 0, size, 0, &tempData);
    memcpy(tempData, data, size);
    vkUnmapMemory(device, src->memory);
}

void buffer_inline_copy(buffer_t *dst, buffer_t *src, VkCommandBuffer commandBuffer)
{
    VkBufferCopy copyRegion = {};
    copyRegion.size = dst->size;
    vkCmdCopyBuffer(commandBuffer, src->buffer, dst->buffer, 1, &copyRegion);
}

void buffer_destroy(buffer_t *buffer, VkDevice device)
{
    buffer->size = 0;
    vkDestroyBuffer(device, buffer->buffer, nullptr);
    vkFreeMemory(device, buffer->memory, nullptr);
}

