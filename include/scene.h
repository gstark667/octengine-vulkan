#ifndef H_SCENE
#define H_SCENE

#include <vulkan/vulkan.h>

#include "model.h"
#include "texture.h"
#include "shader.h"

#include <vector>
#include <map>
#include <string>


struct instance_t
{
    std::string model;
    std::string texture;
    std::string shader;
};

struct scene_t
{
    VkDevice device;
    VkPhysicalDevice physicalDevice;
    VkCommandPool commandPool;
    VkQueue graphicsQueue;

    std::map<std::string, texture_t> textures;
    std::map<std::string, model_t> models;
    std::map<std::string, shader_t> shaders;
    std::vector<instance_t> instances;
};

void scene_create(scene_t *scene, VkDevice device, VkPhysicalDevice physicalDevice, VkCommandPool commandPool, VkQueue graphicsQueue);
void scene_add_model(scene_t *scene, std::string modelPath, std::string texturePath, std::string fragPath, std::string vertPath);
void scene_render(scene_t *scene, VkCommandBuffer commandBuffer, VkPipelineLayout pipelineLayout, VkPipeline graphicsPipeline, VkDescriptorSet descriptorSet);

#endif
