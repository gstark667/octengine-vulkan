#ifndef H_SCENE
#define H_SCENE

#include <vulkan/vulkan.h>

#include "model.h"
#include "texture.h"
#include "gameobject.h"
#include "script.h"
#include "physics.h"
#include "audio.h"
#include "camera.h"
#include "light.h"
#include "settings.h"
#include "pipeline.h"
#include "descriptorset.h"
#include "ui.h"

#include <vector>
#include <map>
#include <string>
#include <set>

#include "export.h"


struct scene_t
{
    camera_t *camera = NULL;
    script_t script;
    ui_t ui;
    bone_ubo_t bones;

    VkDevice device;
    VkPhysicalDevice physicalDevice;
    VkCommandPool commandPool;
    VkQueue graphicsQueue;

    texture_t textures;
    std::vector<std::string> modelOrder;
    std::map<std::string, model_t*> models;
    std::map<std::string, script_t*> scripts;
    std::vector<light_t*> lights;
    std::set<camera_t*> cameras;
    std::set<gameobject_t*> gameobjects;
    std::set<gameobject_t*> tempGameobjects;

    physics_world_t world;
    audio_world_t audio;

    uint32_t width, height;
    bool isDirty = true;
    bool uiDirty = true;
};

extern "C" {
void EXPORT scene_create(scene_t *scene, VkDevice device, VkPhysicalDevice physicalDevice, VkCommandPool commandPool, VkQueue graphicsQueue, uint32_t width, uint32_t height);
void EXPORT scene_render(scene_t *scene, VkCommandBuffer commandBuffer, pipeline_t *pipeline, descriptor_set_t *descriptorSet, bool ui);
gameobject_t* EXPORT scene_add_gameobject(scene_t *scene);
camera_t* EXPORT scene_add_camera(scene_t *scene);
light_t* EXPORT scene_add_light(scene_t *scene);
int EXPORT scene_count_shadows(scene_t *scene);
void EXPORT scene_enable_shadow(scene_t *scene, light_t *light);
void EXPORT scene_set_model(scene_t *scene, gameobject_t *object, std::string modelPath);
void EXPORT scene_set_texture(scene_t *scene, gameobject_t *object, std::string texturePath);
void EXPORT scene_set_normal(scene_t *scene, gameobject_t *object, std::string texturePath);
void EXPORT scene_set_pbr(scene_t *scene, gameobject_t *object, std::string texturePath);
void EXPORT scene_add_script(scene_t *scene, gameobject_t *object, std::string scriptPath);
void EXPORT scene_resize(scene_t *scene, uint32_t width, uint32_t height);
void EXPORT scene_update(scene_t *scene, float delta);
void EXPORT scene_load(scene_t *scene, std::string path);
void EXPORT scene_cleanup(scene_t *scene);
void EXPORT scene_on_cursor_pos(scene_t *scene, double x, double y);
void EXPORT scene_on_button_down(scene_t *scene, std::string buttonCode);
void EXPORT scene_on_button_up(scene_t *scene, std::string buttonCode);
void EXPORT scene_on_event(scene_t *scene, event_t event);
}

#endif
