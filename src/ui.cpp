#include "ui.h"
#include "font.h"

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
    ui->root->textureIdx = -1;

    font_create(&ui->font);
    ui->fontTexture.data.push_back(ui->font.textureData);
    ui->fontTexture.width = ui->font.textureData.width;
    ui->fontTexture.height = ui->font.textureData.height;
    texture_load(&ui->fontTexture, ui->device, ui->physicalDevice, ui->commandPool, ui->graphicsQueue);
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
    ui_build(ui, ui->root, 0, glm::vec2(0.0f), glm::vec2(1.0f));
    model_copy_instance_buffer(&ui->model, ui->device, ui->physicalDevice, ui->commandPool, ui->graphicsQueue);
}

size_t ui_build(ui_t *ui, ui_element_t *element, size_t offset, glm::vec2 pos, glm::vec2 scale)
{
    pos.x = element->x * scale.x + pos.x;
    pos.y = element->y * scale.y + pos.y;
    scale.x = element->width * scale.x;
    scale.y = element->height * scale.y;
    ui->model.instances[offset].pos = glm::vec3(pos.x, pos.y, 0.0f);
    ui->model.instances[offset].rot = glm::vec3(1.0f, 1.0f, 0.0f);
    ui->model.instances[offset].scale = glm::vec3(scale.x, scale.y, 1.0f);
    ui->model.instances[offset].textureIdx.x = element->textureIdx;

    glm::vec2 textPos(0.0f);
    size_t first = offset + 1;
    size_t rows = 0;
    glm::vec2 textScale(element->textScale * scale.y * 9.0f / 16.0f, element->textScale * scale.y);
    for (size_t i = 0; i < element->text.length(); ++i)
    {
        offset++;
        font_glyph_t glyph = ui->font.glyphs[(short)element->text.at(i)];
        ui->model.instances[offset].pos = glm::vec3(pos.x + textPos.x + (glyph.width + glyph.left) * textScale.x, pos.y + (glyph.height / 2.0f - glyph.top) * textScale.y, 0.0f);
        ui->model.instances[offset].scale = glm::vec3(textScale.x * glyph.width, textScale.y * glyph.height, 1.0f);
        ui->model.instances[offset].rot = glm::vec3(glyph.uvWidth, glyph.uvHeight, glyph.uvOffset);
        ui->model.instances[offset].textureIdx.x = 0;
        textPos.x += glyph.xAdv * textScale.x * 2.0f;
    }

    for (size_t i = first; i <= offset; ++i)
    {
        // center
        if (element->textAllign == 0)
            ui->model.instances[i].pos.x -= textPos.x / 2.0f;
        // left
        if (element->textAllign == -1)
            ui->model.instances[i].pos.x -= scale.x;
        // right
        if (element->textAllign == 1)
            ui->model.instances[i].pos.x += scale.x - textPos.x;
    }

    for (auto it = element->children.begin(); it != element->children.end(); ++it)
    {
        offset = ui_build(ui, *it, offset + 1, pos, scale);
    }
    return offset;
}

void ui_cleanup(ui_t *ui)
{
    model_cleanup(&ui->model, ui->device);
    texture_cleanup(&ui->fontTexture, ui->device);
}

ui_element_t *ui_element_create(ui_t *ui, ui_element_t *parent)
{
    ui->dirty = true;
    ui_element_t *element = new ui_element_t();
    element->width = parent->width;
    element->height = parent->height;
    element->x = parent->x;
    element->y = parent->y;
    element->textureIdx = 1;

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
    size_t output = 1 + element->text.length();
    for (auto it = element->children.begin(); it != element->children.end(); ++it)
    {
        output += ui_element_count_children(*it);
    }
    return output;
}

