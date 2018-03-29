#include <vulkan/vulkan.h>

#include "texture.h"
#include "util.h"

#include <FreeImage.h>
#include <string.h>
#include <stdexcept>


void texture_load(texture_t *texture, VkDevice device, VkPhysicalDevice physicalDevice, VkCommandPool commandPool, VkQueue graphicsQueue, std::string path)
{
    FIBITMAP *img = FreeImage_Load(FreeImage_GetFileType(path.c_str(), 0), path.c_str());
    FIBITMAP *bitmap = FreeImage_ConvertTo32Bits(img);
    uint32_t textureWidth = FreeImage_GetWidth(bitmap);
    uint32_t textureHeight = FreeImage_GetHeight(bitmap);
    VkDeviceSize imageSize = textureWidth * textureHeight * 4;
    BYTE *bitmapData = FreeImage_GetBits(bitmap);

    #if FREEIMAGE_COLORORDER == FREEIMAGE_COLORORDER_BGR
    for (size_t i = 0; i < imageSize; i += 4)
    {
        BYTE tmp = bitmapData[i];
        bitmapData[i] = bitmapData[i + 2];
        bitmapData[i + 2] = tmp;
    }
    #endif

    VkBuffer stagingBuffer;
    VkDeviceMemory stagingBufferMemory;
    create_buffer(device, physicalDevice, imageSize, VK_BUFFER_USAGE_TRANSFER_SRC_BIT, VK_MEMORY_PROPERTY_HOST_VISIBLE_BIT | VK_MEMORY_PROPERTY_HOST_COHERENT_BIT, &stagingBuffer, &stagingBufferMemory);

    void *data;
    vkMapMemory(device, stagingBufferMemory, 0, imageSize, 0, &data);
    memcpy(data, (void*)FreeImage_GetBits(bitmap), static_cast<size_t>(imageSize));
    vkUnmapMemory(device, stagingBufferMemory);

    FreeImage_Unload(bitmap);

    image_create(&texture->image, device, physicalDevice, textureWidth, textureHeight, VK_FORMAT_R8G8B8A8_UNORM, VK_IMAGE_TILING_OPTIMAL, VK_IMAGE_USAGE_TRANSFER_DST_BIT | VK_IMAGE_USAGE_SAMPLED_BIT, VK_MEMORY_PROPERTY_DEVICE_LOCAL_BIT);

    image_transition_layout(&texture->image, device, commandPool, graphicsQueue, VK_FORMAT_R8G8B8A8_UNORM, VK_IMAGE_LAYOUT_UNDEFINED, VK_IMAGE_LAYOUT_TRANSFER_DST_OPTIMAL);
    image_copy_buffer(&texture->image, device, commandPool, graphicsQueue, stagingBuffer);
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
    samplerInfo.compareEnable = VK_FALSE;
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

