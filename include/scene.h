#ifndef H_SCENE
#define H_SCENE

#include <vulkan/vulkan.h>

#include "model.h"
#include "texture.h"
#include "gameobject.h"
#include "script.h"
#include "physics.h"
#include "camera.h"

#include <vector>
#include <map>
#include <string>
#include <set>


struct scene_t
{
    camera_t camera;
    script_t script;

    VkDevice device;
    VkPhysicalDevice physicalDevice;
    VkCommandPool commandPool;
    VkQueue graphicsQueue;

    texture_t textures;
    std::map<std::string, model_t*> models;
    std::map<std::string, script_t*> scripts;
    std::set<gameobject_t*> gameobjects;
    std::set<gameobject_t*> tempGameobjects;

    physics_world_t world;

    bool isDirty = true;
};

void scene_create(scene_t *scene, VkDevice device, VkPhysicalDevice physicalDevice, VkCommandPool commandPool, VkQueue graphicsQueue);
void scene_render(scene_t *scene, VkCommandBuffer commandBuffer, VkPipelineLayout pipelineLayout, VkPipeline graphicsPipeline, VkDescriptorSet descriptorSet);
gameobject_t *scene_add_gameobject(scene_t *scene);
void scene_set_model(scene_t *scene, gameobject_t *object, std::string modelPath);
void scene_set_texture(scene_t *scene, gameobject_t *object, std::string texturePath);
void scene_add_script(scene_t *scene, gameobject_t *object, std::string scriptPath);
void scene_update(scene_t *scene, float delta);
void scene_load(scene_t *scene, std::string path);
void scene_cleanup(scene_t *scene);
void scene_on_cursor_pos(scene_t *scene, double x, double y);
void scene_on_button_down(scene_t *scene, std::string buttonCode);
void scene_on_button_up(scene_t *scene, std::string buttonCode);

#endif
