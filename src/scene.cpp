#include "scene.h"


void scene_create(scene_t *scene, VkDevice device, VkPhysicalDevice physicalDevice, VkCommandPool commandPool, VkQueue graphicsQueue)
{
    scene->device = device;
    scene->physicalDevice = physicalDevice;
    scene->commandPool = commandPool;
    scene->graphicsQueue = graphicsQueue;
}


model_instance_t *scene_add_model(scene_t *scene, std::string modelPath, std::string texturePath, std::string vertPath, std::string fragPath)
{
    model_instance_t *inst = NULL;
    if (scene->models.find(modelPath) == scene->models.end())
    {
        model_t model;
        model_load(&model, modelPath);
        model_create_buffers(&model, scene->device, scene->physicalDevice, scene->commandPool, scene->graphicsQueue);
        scene->models[modelPath] = model;
    }
    inst = model_create_instance(&scene->models[modelPath]);

    if (scene->textures.find(texturePath) == scene->textures.end())
    {
        texture_t texture;
        texture_load(&texture, scene->device, scene->physicalDevice, scene->commandPool, scene->graphicsQueue, texturePath);
        scene->textures[texturePath] = texture;
    }

    std::string shaderName = vertPath + "|" + fragPath;
    if (scene->shaders.find(shaderName) == scene->shaders.end())
    {
        shader_t shader;
        shader_create(&shader, scene->device, vertPath, fragPath);
        scene->shaders[shaderName] = shader;
    }

    instance_t instance;
    instance.model = modelPath;
    instance.texture = texturePath;
    instance.shader = shaderName;
    std::string instanceName = modelPath + "|" + texturePath + "|" + shaderName;
    scene->instances[instanceName].push_back(instance);
    return inst;
}


void scene_render(scene_t *scene, VkCommandBuffer commandBuffer, VkPipelineLayout pipelineLayout, VkPipeline graphicsPipeline, VkDescriptorSet descriptorSet)
{
    for (std::map<std::string, std::vector<instance_t>>::iterator it = scene->instances.begin(); it != scene->instances.end(); ++it)
    {
        for (std::vector<instance_t>::iterator it2 = it->second.begin(); it2 != it->second.end(); ++it2)
        {
            model_render(&scene->models[it2->model], commandBuffer, pipelineLayout, graphicsPipeline, descriptorSet);
        }
    }
}

void scene_update(scene_t *scene, float delta)
{
    for (std::map<std::string, model_t>::iterator it = scene->models.begin(); it != scene->models.end(); ++it)
    {
        model_copy_index_buffer(&it->second, scene->device, scene->physicalDevice, scene->commandPool, scene->graphicsQueue);
    }
}

