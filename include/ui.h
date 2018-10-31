#ifndef H_UI
#define H_UI

#include <vector>

#include "model.h"
#include "pipeline.h"
#include "descriptorset.h"


struct ui_element_t
{
    std::vector<ui_element_t*> children;
    float x = 0.0f;
    float y = 0.0f;
    float width = 1.0f;
    float height = 1.0f;
};

struct ui_t
{
    model_t model;
    ui_element_t *root = NULL;
    bool dirty = false;
    VkDevice device;
    VkPhysicalDevice physicalDevice;
    VkCommandPool commandPool;
    VkQueue graphicsQueue;
};

void ui_create(ui_t *ui, VkDevice device, VkPhysicalDevice physicalDevice, VkCommandPool commandPool, VkQueue graphicsQueue);
void ui_render(ui_t *ui, VkCommandBuffer commandBuffer, pipeline_t *pipeline, descriptor_set_t *descriptorSet);
void ui_update(ui_t *ui);
size_t ui_build(ui_t *ui, ui_element_t *element, size_t offset);
void ui_cleanup(ui_t *ui);

ui_element_t *ui_element_create(ui_t *ui, ui_element_t *parent);
void ui_element_size(ui_element_t *element, float width, float height);
void ui_element_move(ui_element_t *element, float x, float y);

size_t ui_element_count_children(ui_element_t *element);

#endif
