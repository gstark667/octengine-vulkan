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
    //glm::mat4 bones[64];
};

struct descriptor_set_t
{
    texture_t *texture;
    VkBuffer uniformBuffer = NULL;
    VkDeviceMemory uniformBufferMemory = NULL;
    VkDescriptorPool descriptorPool = NULL;
    VkDescriptorSet descriptorSet = NULL;
    VkDescriptorSetLayout descriptorSetLayout = NULL;
};

void descriptor_set_create(descriptor_set_t *descriptorSet, VkDevice device, VkPhysicalDevice physicalDevice);
void descriptor_set_cleanup(descriptor_set_t *descriptor_set, VkDevice device);

#endif
