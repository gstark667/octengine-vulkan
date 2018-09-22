#ifndef H_FRAMEBUFFER
#define H_FRAMEBUFFER

#include <vulkan/vulkan.h>


struct frame_buffer_attachment_t
{
    VkImage image;
    VkDeviceMemory mem;
    VkImageView view;
    VkFormat format;
};

struct frame_buffer_t
{
    int32_t width, height;
    VkFramebuffer frameBuffer;		
    frame_buffer_attachment_t position, normal, albedo;
    frame_buffer_attachment_t depth;
    VkRenderPass renderPass;
};

#endif
