#ifndef H_DESCRIPTORSET
#define H_DESCRIPTORSET

#include "buffer.h"
#include "texture.h"

#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>


struct uniform_buffer_object_t
{
    glm::mat4 model = glm::mat4(0.0f);
    glm::mat4 view = glm::mat4(0.0f);
    glm::mat4 proj = glm::mat4(0.0f);
    glm::mat4 shadowSpace[2];
    glm::vec3 lightPos[2];
    //glm::mat4 bones[64];
};

struct descriptor_texture_t
{
    texture_t *texture;
    uint32_t binding;
    bool vertex;
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
void descriptor_set_create(descriptor_set_t *descriptorSet);
void descriptor_set_cleanup(descriptor_set_t *descriptor_set);

#endif
