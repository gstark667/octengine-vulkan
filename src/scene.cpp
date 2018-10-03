#include "scene.h"

void scene_create(scene_t *scene, VkDevice device, VkPhysicalDevice physicalDevice, VkCommandPool commandPool, VkQueue graphicsQueue, uint32_t width, uint32_t height) {
    scene->device = device;
    scene->physicalDevice = physicalDevice;
    scene->commandPool = commandPool;
    scene->graphicsQueue = graphicsQueue;
    scene->width = width;
    scene->height = height;

    texture_add(&scene->textures, scene->device, scene->physicalDevice, scene->commandPool, scene->graphicsQueue, "default.png", false);
    texture_add(&scene->textures, scene->device, scene->physicalDevice, scene->commandPool, scene->graphicsQueue, "normal.png", false);
    texture_add(&scene->textures, scene->device, scene->physicalDevice, scene->commandPool, scene->graphicsQueue, "pbr.png", false);
    texture_load(&scene->textures, scene->device, scene->physicalDevice, scene->commandPool, scene->graphicsQueue);

    physics_world_init(&scene->world);
}

void scene_add_model(scene_t *scene, std::string modelPath)
{   
    scene->models[modelPath] = new model_t();
    model_load(scene->models[modelPath], modelPath);
    model_create_buffers(scene->models[modelPath], scene->device, scene->physicalDevice, scene->commandPool, scene->graphicsQueue);
}

gameobject_t *scene_add_gameobject(scene_t *scene)
{
    gameobject_t *temp = new gameobject_t();
    scene->tempGameobjects.insert(temp);
    return temp;
}

camera_t *scene_add_camera(scene_t *scene)
{
    camera_t *temp = new camera_t();
    temp->object = new gameobject_t();
    scene->tempGameobjects.insert(temp->object);
    temp->width = scene->width;
    temp->height = scene->height;
    std::cout << "adding camera: " << temp << std::endl;
    camera_resize(temp);
    return temp;
}

light_t *scene_add_light(scene_t *scene)
{
    scene->isDirty = true;
    light_t *temp = new light_t();
    temp->camera.object = new gameobject_t();
    scene->tempGameobjects.insert(temp->camera.object);
    return temp;
}

void scene_set_model(scene_t *scene, gameobject_t *object, std::string modelPath)
{
    scene->isDirty = true;
    if (scene->models.find(modelPath) == scene->models.end())
    {
       scene_add_model(scene, modelPath);
    }
    object->model = scene->models[modelPath];
}

void scene_set_texture(scene_t *scene, gameobject_t *object, std::string texturePath)
{
    object->textureIdx.x = texture_get(&scene->textures, scene->device, scene->physicalDevice, scene->commandPool, scene->graphicsQueue, texturePath, true);
}

void scene_set_normal(scene_t *scene, gameobject_t *object, std::string texturePath)
{
    object->textureIdx.y = texture_get(&scene->textures, scene->device, scene->physicalDevice, scene->commandPool, scene->graphicsQueue, texturePath, true);
}

void scene_set_pbr(scene_t *scene, gameobject_t *object, std::string texturePath)
{
    object->textureIdx.z = texture_get(&scene->textures, scene->device, scene->physicalDevice, scene->commandPool, scene->graphicsQueue, texturePath, true);
}


void scene_add_script(scene_t *scene, gameobject_t *object, std::string scriptPath)
{
    script_t *script = NULL;
    if (scene->scripts.find(scriptPath) == scene->scripts.end())
    {
        script = new script_t();
        script_create(script, scriptPath);
        script_setup(script, scene, object);
    }
    else
    {
        script = scene->scripts[scriptPath];
    }

    object->scripts.insert(script);
}

void scene_render(scene_t *scene, VkCommandBuffer commandBuffer, VkPipelineLayout pipelineLayout, VkPipeline graphicsPipeline, VkDescriptorSet descriptorSet)
{   
    scene->isDirty = false;
    for (std::map<std::string, model_t*>::iterator it = scene->models.begin(); it != scene->models.end(); ++it)
    {
        model_render(it->second, commandBuffer, pipelineLayout, graphicsPipeline, descriptorSet);
    }
}

void scene_update(scene_t *scene, float delta)
{   
    for (std::set<gameobject_t*>::iterator it = scene->tempGameobjects.begin(); it != scene->tempGameobjects.end(); ++it)
    {   
        scene->gameobjects.insert(*it);
    }
    scene->tempGameobjects.clear();
    
    for (std::set<gameobject_t*>::iterator it = scene->gameobjects.begin(); it != scene->gameobjects.end(); ++it)
    {   
        gameobject_update(*it, (void*)scene, delta);
    }
    
    physics_world_update(&scene->world, scene, delta);
    
    std::map<model_t*, std::vector<gameobject_t*>> modelUpdates;
    for (std::set<gameobject_t*>::iterator it = scene->gameobjects.begin(); it != scene->gameobjects.end(); ++it)
    {
        gameobject_update_global(*it);
        if ((*it)->model)
            modelUpdates[(*it)->model].push_back(*it);
    }
    
    for (std::map<model_t*, std::vector<gameobject_t*>>::iterator it = modelUpdates.begin(); it != modelUpdates.end(); ++it)
    {   
        model_update(it->first, delta);
        model_copy_instance_buffer(it->first, it->second, scene->device, scene->physicalDevice, scene->commandPool, scene->graphicsQueue);
    }
}

void scene_load(scene_t *scene, std::string path)
{
    script_create(&scene->script, path);
    script_setup(&scene->script, scene, NULL);
}

void scene_cleanup(scene_t *scene)
{
    for (std::map<std::string, model_t*>::iterator it = scene->models.begin(); it != scene->models.end(); ++it)
    {
        model_cleanup(it->second, scene->device);
    }

    physics_world_destroy(&scene->world);

    texture_cleanup(&scene->textures, scene->device);
}

void scene_on_cursor_pos(scene_t *scene, double x, double y)
{
    for (std::set<gameobject_t*>::iterator it = scene->gameobjects.begin(); it != scene->gameobjects.end(); ++it)
    {
        gameobject_on_cursor_pos(*it, (void*)scene, x, y);
    }
}

void scene_on_button_down(scene_t *scene, std::string buttonCode)
{
    for (std::set<gameobject_t*>::iterator it = scene->gameobjects.begin(); it != scene->gameobjects.end(); ++it)
    {
        gameobject_on_button_down(*it, (void*)scene, buttonCode);
    }
}

void scene_on_button_up(scene_t *scene, std::string buttonCode)
{
    for (std::set<gameobject_t*>::iterator it = scene->gameobjects.begin(); it != scene->gameobjects.end(); ++it)
    {
        gameobject_on_button_up(*it, (void*)scene, buttonCode);
    }
}

