#ifndef H_PIPELINE
#define H_PIPELINE

#include <vulkan/vulkan.h>

#include "texture.h" 
#include "descriptorset.h" 
#include "camera.h"

#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>
#include <string>
#include <map>
#include <vector>


struct pipeline_attachment_t
{
    image_t image;
    VkImageUsageFlagBits usage;
    VkFormat format;
    VkImageLayout layout;
    VkImageLayout finalLayout;
};

struct pipeline_t
{
    std::string vertShader, fragShader;

    VkDevice device;
    VkPhysicalDevice physicalDevice;

    uint32_t width, height;

    descriptor_set_t *descriptorSet;

    VkRenderPass renderPass;
    VkPipelineLayout layout;
    VkPipeline pipeline;

    VkCommandPool commandPool;
    VkQueue graphicsQueue;

    VkFramebuffer framebuffer = NULL;

    std::vector<pipeline_attachment_t> attachments;
    bool offscreen;
};

void pipeline_attachment_create(pipeline_attachment_t *attachment, VkDevice device, VkPhysicalDevice physicalDevice, uint32_t width, uint32_t height, VkFormat format, VkImageUsageFlagBits usage, VkCommandPool, VkQueue graphicsQueue);
std::vector<VkImageView> pipeline_attachment_views(std::vector<pipeline_attachment_t> attachments);
void pipeline_attachment_destroy(pipeline_attachment_t *attachment, VkDevice device);

void pipeline_create(pipeline_t *pipeline, descriptor_set_t *descriptorSet, uint32_t width, uint32_t height, std::string vertShader, std::string fragShader, VkDevice device, VkPhysicalDevice physicalDevice, VkCommandPool commandPool, VkQueue graphicsQueue, std::vector<pipeline_attachment_t> attachments, bool offscreen);
void pipeline_recreate(pipeline_t *pipeline, uint32_t width, uint32_t height, VkDevice device);
void pipeline_cleanup(pipeline_t *pipeline);

#endif
