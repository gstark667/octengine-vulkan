#include <vulkan/vulkan.h>

#include "texture.h"
#include "util.h"

#include <string.h>
#include <iostream>
#include <stdexcept>
#include <math.h>


using namespace std;

texture_data_t texture_get_data(texture_t *texture, string path, VkDevice device, VkPhysicalDevice physicalDevice, VkCommandPool commandPool, VkQueue graphicsQueue, size_t idx)
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

    texture_data_load(texture, &output, device, physicalDevice, commandPool, graphicsQueue, idx);

    return output;
}

void texture_data_load(texture_t *texture, texture_data_t *textureData, VkDevice device, VkPhysicalDevice physicalDevice, VkCommandPool commandPool, VkQueue graphicsQueue, size_t idx)
{
    uint32_t mipLevels = (uint32_t)(floor(log2(max(textureData->width, textureData->height)))) + 1;
    uint32_t size = textureData->width * textureData->height * 4;

    VkBuffer stagingBuffer;
    VkDeviceMemory stagingBufferMemory;
    create_buffer(device, physicalDevice, size, VK_BUFFER_USAGE_TRANSFER_SRC_BIT, VK_MEMORY_PROPERTY_HOST_VISIBLE_BIT | VK_MEMORY_PROPERTY_HOST_COHERENT_BIT, &stagingBuffer, &stagingBufferMemory);

    BYTE *data;
    vkMapMemory(device, stagingBufferMemory, 0, size, 0, (void**)&data);

    VkBufferImageCopy bufferCopyRegion = {};
    bufferCopyRegion.imageSubresource.aspectMask = VK_IMAGE_ASPECT_COLOR_BIT;
    bufferCopyRegion.imageSubresource.mipLevel = 0;
    bufferCopyRegion.imageSubresource.baseArrayLayer = texture->cube ? idx : 0;
    bufferCopyRegion.imageSubresource.layerCount = 1;
    bufferCopyRegion.imageExtent.width = textureData->width;
    bufferCopyRegion.imageExtent.height = textureData->height;
    bufferCopyRegion.imageExtent.depth = 1;

    memcpy(data, textureData->data.data(), size);

    vkUnmapMemory(device, stagingBufferMemory);

    VkImage image;
    if (!texture->cube || idx == 0)
    {
        textureData->image.cube = texture->cube;
        image_create(&textureData->image, device, physicalDevice, textureData->width, textureData->height, texture->cube ? 6 : 1, mipLevels, VK_FORMAT_R8G8B8A8_UNORM, VK_IMAGE_TILING_OPTIMAL, VK_IMAGE_USAGE_TRANSFER_DST_BIT | VK_IMAGE_USAGE_TRANSFER_SRC_BIT | VK_IMAGE_USAGE_SAMPLED_BIT, VK_MEMORY_PROPERTY_DEVICE_LOCAL_BIT, VK_SAMPLE_COUNT_1_BIT);

        image_transition_layout(&textureData->image, device, commandPool, graphicsQueue, VK_FORMAT_R8G8B8A8_UNORM, VK_IMAGE_LAYOUT_UNDEFINED, VK_IMAGE_LAYOUT_TRANSFER_DST_OPTIMAL);
        image = textureData->image.image;
    }
    else
    {
        image = texture->data[0].image.image;
    }

    VkCommandBuffer commandBuffer = begin_single_time_commands(device, commandPool);
    vkCmdCopyBufferToImage(commandBuffer, stagingBuffer, image, VK_IMAGE_LAYOUT_TRANSFER_DST_OPTIMAL, 1, &bufferCopyRegion);

    end_single_time_commands(device, commandPool, graphicsQueue, commandBuffer);

    vkDestroyBuffer(device, stagingBuffer, nullptr);
    vkFreeMemory(device, stagingBufferMemory, nullptr);

    if (!texture->cube || idx == 5)
    {
        image_t *image = texture->cube ? &texture->data[0].image : &textureData->image;
        image_generate_mipmaps(image, device, physicalDevice, commandPool, graphicsQueue);
        image_create_view(image, device, VK_FORMAT_R8G8B8A8_UNORM, VK_IMAGE_ASPECT_COLOR_BIT, 0, true);
    }

    if (idx == 0)
    {
        VkSamplerCreateInfo samplerInfo = {};
        samplerInfo.sType = VK_STRUCTURE_TYPE_SAMPLER_CREATE_INFO;
        samplerInfo.magFilter = texture->smooth ? VK_FILTER_LINEAR : VK_FILTER_NEAREST;
        samplerInfo.minFilter = texture->smooth ? VK_FILTER_LINEAR : VK_FILTER_NEAREST;
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
        samplerInfo.maxLod = mipLevels;

        if (vkCreateSampler(device, &samplerInfo, nullptr, &texture->sampler) != VK_SUCCESS) {
            throw runtime_error("failed to create texture sampler!");
        }
    }

    std::cout << "texture combined: " << texture->combined << std::endl;
    texture->combined = texture->cube || idx == 0;
}


void texture_add(texture_t *texture, VkDevice device, VkPhysicalDevice physicalDevice, VkCommandPool commandPool, VkQueue graphicsQueue, string path, bool load)
{
    path = "textures/" + path;
    if (texture->indicies.find(path) != texture->indicies.end())
        return;

    size_t index = texture->data.size();
    texture->indicies[path] = index;
    texture->data.push_back(texture_get_data(texture, path, device, physicalDevice, commandPool, graphicsQueue, index));
    texture->needsUpdate = true;
}


size_t texture_get(texture_t *texture, VkDevice device, VkPhysicalDevice physicalDevice, VkCommandPool commandPool, VkQueue graphicsQueue, string path, bool load)
{
    if (texture->indicies.find("textures/" + path) == texture->indicies.end())
        texture_add(texture, device, physicalDevice, commandPool, graphicsQueue, path, load);

    return texture->indicies["textures/" + path];
}


void texture_cleanup(texture_t *texture, VkDevice device)
{
    vkDestroySampler(device, texture->sampler, nullptr);
    for (auto it = texture->data.begin(); it != texture->data.end(); ++it)
    {
        image_cleanup(&it->image, device);
    }
}

