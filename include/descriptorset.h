#ifndef H_DESCRIPTORSET
#define H_DESCRIPTORSET

#include "buffer.h"
#include "texture.h"

#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>


struct uniform_buffer_object_t
{
    glm::mat4 cameraMVP = glm::mat4(0.0f);
    glm::mat4 shadowSpace = glm::mat4(0.0f);
    //glm::mat4 bones[64];
};

struct bone_ubo_t
{
    glm::mat4 bones[64];
};

struct light_instance_t
{
    glm::vec4 position = glm::vec4(0.0f);
    glm::vec4 color = glm::vec4(0.0f);
    glm::mat4 mvp = glm::mat4(1.0f);
};

struct light_ubo_t
{
    light_instance_t lights[16];
    glm::vec4 cameraPos = glm::vec4(0.0f);
    int lightCount = 0;
};

struct render_ubo_t
{
    glm::vec4 ambient = glm::vec4(0.2f);
    int sampleCount = 2;
};

struct descriptor_texture_t
{
    texture_t *texture;
    uint32_t binding;
    bool vertex;
    bool repeat;
    bool shadow;
    bool fromImage = false;
};

struct descriptor_buffer_t
{
    VkBuffer uniformBuffer = NULL;
    VkDeviceMemory uniformBufferMemory = NULL;
    size_t size;
    uint32_t binding;
    bool vertex;
};

struct descriptor_set_t
{
    std::vector<descriptor_texture_t> textures;
    std::vector<descriptor_buffer_t> buffers;

    VkDevice device = NULL;
    VkPhysicalDevice physicalDevice = NULL;

    VkDescriptorPool descriptorPool = NULL;
    VkDescriptorSet descriptorSet = NULL;
    VkDescriptorSetLayout descriptorSetLayout = NULL;
};

void descriptor_set_setup(descriptor_set_t *descriptorSet, VkDevice device, VkPhysicalDevice physicalDevice);
void descriptor_set_add_buffer(descriptor_set_t *descriptorSet, size_t size, uint32_t binding, bool vertex);
void descriptor_set_add_texture(descriptor_set_t *descriptorSet, texture_t *texture, uint32_t binding, bool vertex);
void descriptor_set_add_image(descriptor_set_t *descriptorSet, image_t *image, uint32_t binding, bool vertex, bool repeat, bool shadow);
void descriptor_set_update_buffer(descriptor_set_t *descriptorSet, void *data, uint32_t binding);
void descriptor_set_update_texture(descriptor_set_t *descriptorSet, texture_t *image, uint32_t binding);
void descriptor_set_update_image(descriptor_set_t *descriptorSet, image_t *image, uint32_t binding);
void descriptor_set_create(descriptor_set_t *descriptorSet);
void descriptor_set_cleanup(descriptor_set_t *descriptor_set);

#endif
