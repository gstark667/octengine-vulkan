#ifndef H_SCENE
#define H_SCENE

#include <vulkan/vulkan.h>

#include "model.h"
#include "texture.h"
#include "gameobject.h"
#include "script.h"
#include "physics.h"
#include "camera.h"
#include "light.h"
#include "settings.h"
#include "descriptorset.h"

#include <vector>
#include <map>
#include <string>
#include <set>


struct scene_t
{
    camera_t *camera = NULL;
    script_t script;

    bone_ubo_t bones;

    VkDevice device;
    VkPhysicalDevice physicalDevice;
    VkCommandPool commandPool;
    VkQueue graphicsQueue;

    texture_t textures;
    std::vector<std::string> modelOrder;
    std::map<std::string, model_t*> models;
    std::map<std::string, script_t*> scripts;
    std::set<light_t*> lights;
    std::set<camera_t*> cameras;
    std::set<gameobject_t*> gameobjects;
    std::set<gameobject_t*> tempGameobjects;

    physics_world_t world;

    uint32_t width, height;
    bool isDirty = true;
};

void scene_create(scene_t *scene, VkDevice device, VkPhysicalDevice physicalDevice, VkCommandPool commandPool, VkQueue graphicsQueue, uint32_t width, uint32_t height);
void scene_render(scene_t *scene, VkCommandBuffer commandBuffer, VkPipelineLayout pipelineLayout, VkPipeline graphicsPipeline, descriptor_set_t *descriptorSet);
gameobject_t *scene_add_gameobject(scene_t *scene);
camera_t *scene_add_camera(scene_t *scene);
light_t *scene_add_light(scene_t *scene);
void scene_set_model(scene_t *scene, gameobject_t *object, std::string modelPath);
void scene_set_texture(scene_t *scene, gameobject_t *object, std::string texturePath);
void scene_set_normal(scene_t *scene, gameobject_t *object, std::string texturePath);
void scene_set_pbr(scene_t *scene, gameobject_t *object, std::string texturePath);
void scene_add_script(scene_t *scene, gameobject_t *object, std::string scriptPath);
void scene_update(scene_t *scene, float delta);
void scene_load(scene_t *scene, std::string path);
void scene_cleanup(scene_t *scene);
void scene_on_cursor_pos(scene_t *scene, double x, double y);
void scene_on_button_down(scene_t *scene, std::string buttonCode);
void scene_on_button_up(scene_t *scene, std::string buttonCode);
void scene_on_event(scene_t *scene, event_t event);

#endif
