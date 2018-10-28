#include <vulkan/vulkan.h>

#include "texture.h"
#include "util.h"

#include <string.h>
#include <iostream>
#include <stdexcept>
#include <math.h>


texture_data_t texture_get_data(std::string path)
{
    texture_data_t output;


    FIBITMAP *img = FreeImage_Load(FreeImage_GetFileType(path.c_str(), 0), path.c_str());
    FIBITMAP *bitmap = FreeImage_ConvertTo32Bits(img);
    output.width = FreeImage_GetWidth(bitmap);
    output.height = FreeImage_GetHeight(bitmap);
    output.size = output.width * output.height * 4;
    BYTE *bitmapData = FreeImage_GetBits(bitmap);

    std::cout << "adding texture: " << path << ":" << output.width << ":" << output.height << std::endl;

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


void texture_add(texture_t *texture, VkDevice device, VkPhysicalDevice physicalDevice, VkCommandPool commandPool, VkQueue graphicsQueue, std::string path, bool load)
{
    path = "textures/" + path;
    if (texture->indicies.find(path) != texture->indicies.end())
        return;

    size_t index = texture->data.size();
    texture->indicies[path] = index;
    texture->data.push_back(texture_get_data(path));
    if (texture->data[index].width > texture->width)
        texture->width = texture->data[index].width;
    if (texture->data[index].height > texture->height)
        texture->height = texture->data[index].height;

    if (load)
        texture_load(texture, device, physicalDevice, commandPool, graphicsQueue);
}


size_t texture_get(texture_t *texture, VkDevice device, VkPhysicalDevice physicalDevice, VkCommandPool commandPool, VkQueue graphicsQueue, std::string path, bool load)
{
    if (texture->indicies.find("textures/" + path) == texture->indicies.end())
        texture_add(texture, device, physicalDevice, commandPool, graphicsQueue, path, load);

    return texture->indicies["textures/" + path];
}


void texture_load(texture_t *texture, VkDevice device, VkPhysicalDevice physicalDevice, VkCommandPool commandPool, VkQueue graphicsQueue)
{
    if (texture->image.image)
        texture_cleanup(texture, device);
    texture->needsUpdate = true;
        
    size_t size = 0;
    for (std::vector<texture_data_t>::iterator it = texture->data.begin(); it != texture->data.end(); ++it)
    {
        size = it->width * it->height * 4;
        break;
    }
    size *= texture->data.size();

    /* OSX has floor() in math.h */
    /* OSX has log2() in math.h  */
    texture->mipLevels = (uint32_t)(floor(log2(std::max(texture->width, texture->height)))) + 1;
    std::cout << "mip levels: " << texture->mipLevels << std::endl;

    VkBuffer stagingBuffer;
    VkDeviceMemory stagingBufferMemory;
    create_buffer(device, physicalDevice, size, VK_BUFFER_USAGE_TRANSFER_SRC_BIT, VK_MEMORY_PROPERTY_HOST_VISIBLE_BIT | VK_MEMORY_PROPERTY_HOST_COHERENT_BIT, &stagingBuffer, &stagingBufferMemory);

    size_t offset = 0;
    size_t layer = 0;
    BYTE *data;
    std::vector<VkBufferImageCopy> bufferCopyRegions;
    vkMapMemory(device, stagingBufferMemory, 0, size, 0, (void**)&data);
    for (std::vector<texture_data_t>::iterator it = texture->data.begin(); it != texture->data.end(); ++it)
    {
        VkBufferImageCopy bufferCopyRegion = {};
        bufferCopyRegion.imageSubresource.aspectMask = VK_IMAGE_ASPECT_COLOR_BIT;
        bufferCopyRegion.imageSubresource.mipLevel = 0;
        bufferCopyRegion.imageSubresource.baseArrayLayer = layer;
        bufferCopyRegion.imageSubresource.layerCount = 1;
        bufferCopyRegion.imageExtent.width = it->width;
        bufferCopyRegion.imageExtent.height = it->height;
        bufferCopyRegion.imageExtent.depth = 1;
        bufferCopyRegion.bufferOffset = offset;

        bufferCopyRegions.push_back(bufferCopyRegion);
        memcpy(data + offset, (void*)it->data.data(), it->data.size());
        offset += it->data.size();
        it->index = layer;
        ++layer;
    }
    vkUnmapMemory(device, stagingBufferMemory);

    texture->image.cube = texture->cube;
    image_create(&texture->image, device, physicalDevice, texture->width, texture->height, texture->data.size(), texture->mipLevels, VK_FORMAT_R8G8B8A8_UNORM, VK_IMAGE_TILING_OPTIMAL, VK_IMAGE_USAGE_TRANSFER_DST_BIT | VK_IMAGE_USAGE_TRANSFER_SRC_BIT | VK_IMAGE_USAGE_SAMPLED_BIT, VK_MEMORY_PROPERTY_DEVICE_LOCAL_BIT, VK_SAMPLE_COUNT_1_BIT);

    image_transition_layout(&texture->image, device, commandPool, graphicsQueue, VK_FORMAT_R8G8B8A8_UNORM, VK_IMAGE_LAYOUT_UNDEFINED, VK_IMAGE_LAYOUT_TRANSFER_DST_OPTIMAL);
    VkCommandBuffer commandBuffer = begin_single_time_commands(device, commandPool);
    vkCmdCopyBufferToImage(commandBuffer, stagingBuffer, texture->image.image, VK_IMAGE_LAYOUT_TRANSFER_DST_OPTIMAL, bufferCopyRegions.size(), bufferCopyRegions.data());

    end_single_time_commands(device, commandPool, graphicsQueue, commandBuffer);

    vkDestroyBuffer(device, stagingBuffer, nullptr);
    vkFreeMemory(device, stagingBufferMemory, nullptr);

    image_generate_mipmaps(&texture->image, device, physicalDevice, commandPool, graphicsQueue);
    image_create_view(&texture->image, device, VK_FORMAT_R8G8B8A8_UNORM, VK_IMAGE_ASPECT_COLOR_BIT, 0, true);

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
    samplerInfo.maxLod = (float)texture->mipLevels;

    if (vkCreateSampler(device, &samplerInfo, nullptr, &texture->sampler) != VK_SUCCESS) {
        throw std::runtime_error("failed to create texture sampler!");
    }
}

void texture_cleanup(texture_t *texture, VkDevice device)
{
    vkDestroySampler(device, texture->sampler, nullptr);
    image_cleanup(&texture->image, device);
}

