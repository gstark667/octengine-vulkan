#ifndef H_PIPELINE
#define H_PIPELINE

#include <vulkan/vulkan.h>

#include "texture.h" 
#include "model.h" 
#include "gameobject.h" 

#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>
#include <string>
#include <map>


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

    std::map<std::string, model_t> models;
    std::map<std::string, std::vector<gameobject_t*>> gameobjects;
    std::map<std::string, script_t*> scripts;
    std::map<std::string, std::vector<gameobject_t*>> tempGameobjects;

    VkDevice device;
    VkPhysicalDevice physicalDevice;

    VkDescriptorPool descriptorPool;
    VkDescriptorSet descriptorSet;
    VkDescriptorSetLayout descriptorSetLayout;

    VkBuffer uniformBuffer;
    VkDeviceMemory uniformBufferMemory;

    VkRenderPass renderPass;
    VkPipelineLayout layout;
    VkPipeline pipeline;

    bool isDirty = false;

    VkCommandPool commandPool;
    VkQueue graphicsQueue;
};

void pipeline_create(pipeline_t *pipeline, uint32_t width, uint32_t height, std::string vertShader, std::string fragShader, VkDevice device, VkPhysicalDevice physicalDevice, VkFormat colorFormat, VkFormat depthFormat, VkCommandPool commandPool, VkQueue graphicsQueue);
void pipeline_recreate(pipeline_t *pipeline, uint32_t width, uint32_t height, VkDevice device, VkFormat colorFormat, VkFormat depthFormat);

void pipeline_add_model(pipeline_t *pipeline, std::string modelPath);
gameobject_t *pipeline_add_gameobject(pipeline_t *pipeline, std::string modelPath);
void pipeline_add_script(pipeline_t *pipeline, gameobject_t *object, std::string scriptPath);
void pipeline_render(pipeline_t *pipeline, VkCommandBuffer commandBuffer);
void pipeline_update(pipeline_t *pipeline, float delta);

void pipeline_cleanup(pipeline_t *pipeline, VkDevice device);

void pipeline_on_cursor_pos(pipeline_t *pipeline, double x, double y);

#endif
