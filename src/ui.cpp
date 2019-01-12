#include "ui.h"
#include "font.h"

#include <iostream>


void ui_create(ui_t *ui, VkDevice device, VkPhysicalDevice physicalDevice, VkCommandPool commandPool, VkQueue graphicsQueue)
{
    ui->device = device;
    ui->physicalDevice = physicalDevice;
    ui->commandPool = commandPool;
    ui->graphicsQueue = graphicsQueue;

    model_load(&ui->model, "quad_centered.dae");
    ui->model.instances.push_back({});
    model_create_buffers(&ui->model, device, physicalDevice, commandPool, graphicsQueue);

    model_load(&ui->textModel, "quad.dae");
    ui->textModel.instances.push_back({});
    model_create_buffers(&ui->textModel, device, physicalDevice, commandPool, graphicsQueue);

    ui->root = new ui_element_t();
    ui->root->textureIdx = -1;

    font_create(&ui->font);
    ui->fontTexture.data.push_back(ui->font.textureData);
    ui->fontTexture.smooth = false;
    texture_data_load(&ui->fontTexture, &ui->fontTexture.data[0], ui->device, ui->physicalDevice, ui->commandPool, ui->graphicsQueue, 0);
}

void ui_render(ui_t *ui, VkCommandBuffer commandBuffer, pipeline_t *pipeline, descriptor_set_t *descriptorSet)
{
    model_render(&ui->model, commandBuffer, pipeline, descriptorSet);
    model_render(&ui->textModel, commandBuffer, pipeline, descriptorSet);
}

bool ui_update(ui_t *ui)
{
    size_t count = ui_element_count_children(ui->root);
    bool output = ui->model.instances.size() != count;
    while (ui->model.instances.size() < count)
    {
        ui->model.instances.push_back({});
    }
    while (ui->model.instances.size() > count)
    {
        ui->model.instances.pop_back();
    }
    while (ui->textModel.instances.size() < count)
    {
        ui->textModel.instances.push_back({});
    }
    while (ui->textModel.instances.size() > count)
    {
        ui->textModel.instances.pop_back();
    }
    ui_build(ui, ui->root, 0, 0, glm::vec2(0.0f), glm::vec2(1.0f));
    model_copy_instance_buffer(&ui->model, ui->device, ui->physicalDevice, ui->commandPool, ui->graphicsQueue);
    model_copy_instance_buffer(&ui->textModel, ui->device, ui->physicalDevice, ui->commandPool, ui->graphicsQueue);
    return count;
}

float ui_quantize_x(ui_t *ui, float x)
{
    return (((int)(x / ui->pxWidth)) * ui->pxWidth);
}

float ui_quantize(ui_t *ui, float x)
{
    return ((int)(x / ui->pxWidth)) * ui->pxWidth;
}

size_t ui_build(ui_t *ui, ui_element_t *element, size_t offset, size_t textOffset, glm::vec2 pos, glm::vec2 scale)
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
    std::map<int, float> widths;
    size_t first = textOffset;
    size_t rows = 0;
    size_t rowStart = first;
    size_t lastWord = first;
    for (size_t i = 0; i < element->text.length(); ++i)
    {
        font_glyph_t glyph = ui->font.glyphs[(short)element->text.at(i)];
        // position the character
        if (element->text.at(i) != '\n')
        {
            glm::vec2 textScale(ui->pxWidth * glyph.width, ui->pxHeight * ui->font.height);
            ui->textModel.instances[textOffset].pos = glm::vec3(pos.x + textPos.x + (glyph.left * 2.0f * ui->pxWidth), pos.y - (glyph.top * 2.0f * ui->pxHeight) + textPos.y, 0.0f);
            ui->textModel.instances[textOffset].scale = glm::vec3(textScale.x * 2.0f, textScale.y * 2.0f, 1.0f);
            ui->textModel.instances[textOffset].rot = glm::vec3(glyph.uvWidth, 1.0f, glyph.uvOffset);
            ui->textModel.instances[textOffset].textureIdx.x = 0;
            textPos.x += glyph.xAdv * ui->pxWidth * 2.0f;
        }
        // make a new row of characters
        if (textPos.x > scale.x * 2.0f || element->text.at(i) == '\n')
        {
            float rowWidth = textPos.x;
            textPos.x = 0.0f;
            textPos.y += ui->pxHeight * ui->font.height * 2.0f;
            // move the last character
            if (element->text.at(i) != '\n')
            {
                ui->textModel.instances[textOffset].pos = glm::vec3(pos.x + textPos.x + (glyph.left * 2.0f * ui->pxWidth), pos.y - (glyph.top * 2.0f * ui->pxHeight) + textPos.y, 0.0f);
                textPos.x += glyph.xAdv * ui->pxWidth * 2.0f;
                rowWidth -= textPos.x;
            }
            // save the row width
            for (size_t j = rowStart; j < textOffset; ++j)
                widths[j] = rowWidth;
            rowStart = textOffset;
        }
        // save last row width
        if (i == element->text.length() - 1)
        {
            for (size_t j = rowStart; j <= textOffset; ++j)
                widths[j] = textPos.x;
        }
        // increment
        if (element->text.at(i) != '\n')
        {
            textOffset++;
        }
    }

    for (size_t i = first; i < textOffset; ++i)
    {
        // center
        if (element->textAllign == 0)
            ui->textModel.instances[i].pos.x -= ui_quantize(ui, widths[i] / 2.0f);
        // left
        if (element->textAllign == -1)
            ui->textModel.instances[i].pos.x -= ui_quantize(ui, scale.x);
        // right
        if (element->textAllign == 1)
            ui->textModel.instances[i].pos.x += ui_quantize(ui, scale.x - widths[i]);
        // vertical
        ui->textModel.instances[i].pos.y -= ui_quantize(ui, textPos.y / 2.0f - ui->pxHeight * ui->font.size * 0.5f);
    }

    for (auto it = element->children.begin(); it != element->children.end(); ++it)
    {
        offset = ui_build(ui, *it, offset + 1, textOffset, pos, scale);
    }
    return offset;
}

void ui_cleanup(ui_t *ui)
{
    model_cleanup(&ui->model, ui->device);
    model_cleanup(&ui->textModel, ui->device);
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

