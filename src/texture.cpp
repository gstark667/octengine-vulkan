#include <vulkan/vulkan.h>

#include "texture.h"
#include "util.h"

#include <string.h>
#include <iostream>
#include <stdexcept>


texture_data_t texture_get_data(std::string path)
{
    texture_data_t output;

    FIBITMAP *img = FreeImage_Load(FreeImage_GetFileType(path.c_str(), 0), path.c_str());
    FIBITMAP *bitmap = FreeImage_ConvertTo32Bits(img);
    output.width = FreeImage_GetWidth(bitmap);
    output.height = FreeImage_GetHeight(bitmap);
    output.size = output.width * output.height * 4;
    BYTE *bitmapData = FreeImage_GetBits(bitmap);

    #if FREEIMAGE_COLORORDER == FREEIMAGE_COLORORDER_BGR
    for (size_t i = 0; i < output.size; i += 4)
    {
        BYTE tmp = bitmapData[i];
        bitmapData[i] = bitmapData[i + 2];
        bitmapData[i + 2] = tmp;
    }
    #endif

    for (size_t i = 0; i < output.size; ++i)
    {
        output.data.push_back(bitmapData[i]);
    }

    FreeImage_Unload(img);
    FreeImage_Unload(bitmap);
    return output;
}


bool texture_add(texture_t *texture, VkDevice device, VkPhysicalDevice physicalDevice, VkCommandPool commandPool, VkQueue graphicsQueue, std::string path)
{
    if (texture->data.find(path) != texture->data.end())
        return false;

    std::cout << "loading texture: " << path << std::endl;

    texture->data[path] = texture_get_data(path);
    if (texture->data[path].width > texture->width)
        texture->width = texture->data[path].width;
    if (texture->data[path].height > texture->height)
        texture->height = texture->data[path].height;

    texture_load(texture, device, physicalDevice, commandPool, graphicsQueue);

    return true;
}


size_t texture_get(texture_t *texture, VkDevice device, VkPhysicalDevice physicalDevice, VkCommandPool commandPool, VkQueue graphicsQueue, std::string path)
{
    if (texture->data.find(path) == texture->data.end())
        texture_add(texture, device, physicalDevice, commandPool, graphicsQueue, path);

    return texture->data[path].index;
}


void texture_load(texture_t *texture, VkDevice device, VkPhysicalDevice physicalDevice, VkCommandPool commandPool, VkQueue graphicsQueue)
{
    if (texture->image.image)
        texture_cleanup(texture, device);
        
    size_t size = 0;
    for (std::map<std::string, texture_data_t>::iterator it = texture->data.begin(); it != texture->data.end(); ++it)
    {
        size += it->second.width * it->second.height * 4;
    }
    VkBuffer stagingBuffer;
    VkDeviceMemory stagingBufferMemory;
    create_buffer(device, physicalDevice, size, VK_BUFFER_USAGE_TRANSFER_SRC_BIT, VK_MEMORY_PROPERTY_HOST_VISIBLE_BIT | VK_MEMORY_PROPERTY_HOST_COHERENT_BIT, &stagingBuffer, &stagingBufferMemory);

    size_t offset = 0;
    size_t layer = 0;
    BYTE *data;
    std::vector<VkBufferImageCopy> bufferCopyRegions;
    vkMapMemory(device, stagingBufferMemory, 0, size, 0, (void**)&data);
    for (std::map<std::string, texture_data_t>::iterator it = texture->data.begin(); it != texture->data.end(); ++it)
    {
        VkBufferImageCopy bufferCopyRegion = {};
        bufferCopyRegion.imageSubresource.aspectMask = VK_IMAGE_ASPECT_COLOR_BIT;
        bufferCopyRegion.imageSubresource.mipLevel = 0;
        bufferCopyRegion.imageSubresource.baseArrayLayer = layer;
        bufferCopyRegion.imageSubresource.layerCount = 1;
        bufferCopyRegion.imageExtent.width = it->second.width;
        bufferCopyRegion.imageExtent.height = it->second.height;
        bufferCopyRegion.imageExtent.depth = 1;
        bufferCopyRegion.bufferOffset = offset;

        bufferCopyRegions.push_back(bufferCopyRegion);
        std::cout << "coyping: " << offset << ", " << it->second.data.size() << ", " << size << std::endl;
        memcpy(data + offset, (void*)it->second.data.data(), it->second.data.size());
        offset += it->second.data.size();
        it->second.index = layer;
        ++layer;
    }
    vkUnmapMemory(device, stagingBufferMemory);

    image_create(&texture->image, device, physicalDevice, texture->width, texture->height, texture->data.size(), VK_FORMAT_R8G8B8A8_UNORM, VK_IMAGE_TILING_OPTIMAL, VK_IMAGE_USAGE_TRANSFER_DST_BIT | VK_IMAGE_USAGE_SAMPLED_BIT, VK_MEMORY_PROPERTY_DEVICE_LOCAL_BIT);

    image_transition_layout(&texture->image, device, commandPool, graphicsQueue, VK_FORMAT_R8G8B8A8_UNORM, VK_IMAGE_LAYOUT_UNDEFINED, VK_IMAGE_LAYOUT_TRANSFER_DST_OPTIMAL);
    VkCommandBuffer commandBuffer = begin_single_time_commands(device, commandPool);
    vkCmdCopyBufferToImage(commandBuffer, stagingBuffer, texture->image.image, VK_IMAGE_LAYOUT_TRANSFER_DST_OPTIMAL, bufferCopyRegions.size(), bufferCopyRegions.data());

    end_single_time_commands(device, commandPool, graphicsQueue, commandBuffer);
    

    image_transition_layout(&texture->image, device, commandPool, graphicsQueue, VK_FORMAT_R8G8B8A8_UNORM, VK_IMAGE_LAYOUT_TRANSFER_DST_OPTIMAL, VK_IMAGE_LAYOUT_SHADER_READ_ONLY_OPTIMAL);

    vkDestroyBuffer(device, stagingBuffer, nullptr);
    vkFreeMemory(device, stagingBufferMemory, nullptr);

    image_create_view(&texture->image, device, VK_FORMAT_R8G8B8A8_UNORM, VK_IMAGE_ASPECT_COLOR_BIT);

    VkSamplerCreateInfo samplerInfo = {};
    samplerInfo.sType = VK_STRUCTURE_TYPE_SAMPLER_CREATE_INFO;
    samplerInfo.magFilter = VK_FILTER_LINEAR;
    samplerInfo.minFilter = VK_FILTER_LINEAR;
    samplerInfo.addressModeU = VK_SAMPLER_ADDRESS_MODE_REPEAT;
    samplerInfo.addressModeV = VK_SAMPLER_ADDRESS_MODE_REPEAT;
    samplerInfo.addressModeW = VK_SAMPLER_ADDRESS_MODE_REPEAT;
    samplerInfo.anisotropyEnable = VK_TRUE;
    samplerInfo.maxAnisotropy = 16;
    samplerInfo.borderColor = VK_BORDER_COLOR_INT_OPAQUE_BLACK;
    samplerInfo.unnormalizedCoordinates = VK_FALSE;
    samplerInfo.compareEnable = VK_TRUE;
    samplerInfo.compareOp = VK_COMPARE_OP_ALWAYS;
    samplerInfo.mipmapMode = VK_SAMPLER_MIPMAP_MODE_LINEAR;
    samplerInfo.mipLodBias = 0.0f;
    samplerInfo.minLod = 0.0f;
    samplerInfo.maxLod = 0.0f;

    if (vkCreateSampler(device, &samplerInfo, nullptr, &texture->sampler) != VK_SUCCESS) {
        throw std::runtime_error("failed to create texture sampler!");
    }
}

void texture_cleanup(texture_t *texture, VkDevice device)
{
    vkDestroySampler(device, texture->sampler, nullptr);
    image_cleanup(&texture->image, device);
}

