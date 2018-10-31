#include "ui.h"

#include <iostream>


void ui_create(ui_t *ui, VkDevice device, VkPhysicalDevice physicalDevice, VkCommandPool commandPool, VkQueue graphicsQueue)
{
    ui->device = device;
    ui->physicalDevice = physicalDevice;
    ui->commandPool = commandPool;
    ui->graphicsQueue = graphicsQueue;
    std::cout << ui->device << std::endl;

    model_load(&ui->model, "quad.dae");
    ui->model.instances.push_back({});
    model_create_buffers(&ui->model, device, physicalDevice, commandPool, graphicsQueue);

    ui->root = new ui_element_t();
}

void ui_render(ui_t *ui, VkCommandBuffer commandBuffer, pipeline_t *pipeline, descriptor_set_t *descriptorSet)
{
    model_render(&ui->model, commandBuffer, pipeline, descriptorSet);
}

void ui_update(ui_t *ui)
{
    size_t count = ui_element_count_children(ui->root);
    while (ui->model.instances.size() < count)
    {
        ui->model.instances.push_back({});
    }
    ui_build(ui, ui->root, 0);
    model_copy_instance_buffer(&ui->model, ui->device, ui->physicalDevice, ui->commandPool, ui->graphicsQueue);
}

size_t ui_build(ui_t *ui, ui_element_t *element, size_t offset)
{
    ui->model.instances[offset].pos = glm::vec3(element->x / element->width, element->y / element->height, 0.0f);
    ui->model.instances[offset].scale = glm::vec3(element->width, element->height, 1.0f);

    for (auto it = element->children.begin(); it != element->children.end(); ++it)
    {
        offset = ui_build(ui, *it, offset + 1);
    }
    return offset;
}

void ui_cleanup(ui_t *ui)
{
    model_cleanup(&ui->model, ui->device);
}

ui_element_t *ui_element_create(ui_t *ui, ui_element_t *parent)
{
    ui->dirty = true;
    ui_element_t *element = new ui_element_t();
    element->width = parent->width;
    element->height = parent->height;
    element->x = parent->x;
    element->y = parent->y;

    parent->children.push_back(element);

    return element;
}

void ui_element_size(ui_element_t *element, float width, float height)
{
    element->width = width;
    element->height = height;
}

void ui_element_move(ui_element_t *element, float x, float y)
{
    element->x = x;
    element->y = y;
}

size_t ui_element_count_children(ui_element_t *element)
{
    size_t output = 1;
    for (auto it = element->children.begin(); it != element->children.end(); ++it)
    {
        output += ui_element_count_children(*it);
    }
    return output;
}

