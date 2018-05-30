#ifndef H_PIPELINE
#define H_PIPELINE

#include "texture.h" 

#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>
#include <string>


struct uniform_buffer_object_t
{
    glm::mat4 model = glm::mat4(0.0f);
    glm::mat4 view = glm::mat4(0.0f);
    glm::mat4 proj = glm::mat4(0.0f);
    glm::mat4 bones[64];
};

struct pipeline_t
{
    texture_t texture;

    std::string vertShader, fragShader;

    VkDescriptorPool descriptorPool;
    VkDescriptorSet descriptorSet;
    VkDescriptorSetLayout descriptorSetLayout;

    VkBuffer uniformBuffer;
    VkDeviceMemory uniformBufferMemory;

    VkRenderPass renderPass;
    VkPipelineLayout layout;
    VkPipeline pipeline;
};

void pipeline_create(pipeline_t *pipeline, uint32_t width, uint32_t height, std::string vertShader, std::string fragShader, VkDevice device, VkPhysicalDevice physicalDevice, VkFormat colorFormat, VkFormat depthFormat);
void pipeline_recreate(pipeline_t *pipeline, uint32_t width, uint32_t height, VkDevice device, VkFormat colorFormat, VkFormat depthFormat);
void pipeline_cleanup(pipeline_t *pipeline, VkDevice device);

#endif
