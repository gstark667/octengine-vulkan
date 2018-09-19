#include <vulkan/vulkan.h>

#include "pipeline.h"
#include "util.h"
#include "shader.h"
#include "model.h"

#include <iostream>

void pipeline_create_render_pass(pipeline_t *pipeline, VkDevice device, VkFormat colorFormat, VkFormat depthFormat) {
    VkAttachmentDescription colorAttachment = {};
    colorAttachment.format = colorFormat;
    colorAttachment.samples = VK_SAMPLE_COUNT_1_BIT;
    colorAttachment.loadOp = VK_ATTACHMENT_LOAD_OP_CLEAR;
    colorAttachment.storeOp = VK_ATTACHMENT_STORE_OP_STORE;
    colorAttachment.stencilLoadOp = VK_ATTACHMENT_LOAD_OP_DONT_CARE;
    colorAttachment.stencilStoreOp = VK_ATTACHMENT_STORE_OP_DONT_CARE;
    colorAttachment.initialLayout = VK_IMAGE_LAYOUT_UNDEFINED;
    colorAttachment.finalLayout = VK_IMAGE_LAYOUT_PRESENT_SRC_KHR;

    VkAttachmentReference colorAttachmentRef = {};
    colorAttachmentRef.attachment = 0;
    colorAttachmentRef.layout = VK_IMAGE_LAYOUT_COLOR_ATTACHMENT_OPTIMAL;

    VkAttachmentDescription depthAttachment = {};
    depthAttachment.format = depthFormat;
    depthAttachment.samples = VK_SAMPLE_COUNT_1_BIT;
    depthAttachment.loadOp = VK_ATTACHMENT_LOAD_OP_CLEAR;
    depthAttachment.storeOp = VK_ATTACHMENT_STORE_OP_DONT_CARE;
    depthAttachment.stencilLoadOp = VK_ATTACHMENT_LOAD_OP_DONT_CARE;
    depthAttachment.stencilStoreOp = VK_ATTACHMENT_STORE_OP_DONT_CARE;
    depthAttachment.initialLayout = VK_IMAGE_LAYOUT_UNDEFINED;
    depthAttachment.finalLayout = VK_IMAGE_LAYOUT_DEPTH_STENCIL_ATTACHMENT_OPTIMAL;

    VkAttachmentReference depthAttachmentRef = {};
    depthAttachmentRef.attachment = 1;
    depthAttachmentRef.layout = VK_IMAGE_LAYOUT_DEPTH_STENCIL_ATTACHMENT_OPTIMAL;

    VkSubpassDescription subpass = {};
    subpass.pipelineBindPoint = VK_PIPELINE_BIND_POINT_GRAPHICS;
    subpass.colorAttachmentCount = 1;
    subpass.pColorAttachments = &colorAttachmentRef;
    subpass.pDepthStencilAttachment = &depthAttachmentRef;

    VkSubpassDependency dependency = {};
    dependency.srcSubpass = VK_SUBPASS_EXTERNAL;
    dependency.dstSubpass = 0;
    dependency.srcStageMask = VK_PIPELINE_STAGE_COLOR_ATTACHMENT_OUTPUT_BIT;
    dependency.srcAccessMask = 0;
    dependency.dstStageMask = VK_PIPELINE_STAGE_COLOR_ATTACHMENT_OUTPUT_BIT;
    dependency.dstAccessMask = VK_ACCESS_COLOR_ATTACHMENT_READ_BIT | VK_ACCESS_COLOR_ATTACHMENT_WRITE_BIT;

    std::array<VkAttachmentDescription, 2> attachments = {colorAttachment, depthAttachment};
    VkRenderPassCreateInfo renderPassInfo = {};
    renderPassInfo.sType = VK_STRUCTURE_TYPE_RENDER_PASS_CREATE_INFO;
    renderPassInfo.attachmentCount = static_cast<uint32_t>(attachments.size());
    renderPassInfo.pAttachments = attachments.data();
    renderPassInfo.subpassCount = 1;
    renderPassInfo.pSubpasses = &subpass;
    renderPassInfo.dependencyCount = 1;
    renderPassInfo.pDependencies = &dependency;

    if (vkCreateRenderPass(device, &renderPassInfo, nullptr, &pipeline->renderPass) != VK_SUCCESS) {
        throw std::runtime_error("failed to create render pass!");
    }
}

void pipeline_create_descriptor_set_layout(pipeline_t *pipeline, VkDevice device) {
    VkDescriptorSetLayoutBinding uboLayoutBinding = {};
    uboLayoutBinding.binding = 0;
    uboLayoutBinding.descriptorType = VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER;
    uboLayoutBinding.descriptorCount = 1;
    uboLayoutBinding.stageFlags = VK_SHADER_STAGE_VERTEX_BIT;
    uboLayoutBinding.pImmutableSamplers = nullptr;

    VkDescriptorSetLayoutBinding samplerLayoutBinding = {};
    samplerLayoutBinding.binding = 1;
    samplerLayoutBinding.descriptorCount = 1;
    samplerLayoutBinding.descriptorType = VK_DESCRIPTOR_TYPE_COMBINED_IMAGE_SAMPLER;
    samplerLayoutBinding.pImmutableSamplers = nullptr;
    samplerLayoutBinding.stageFlags = VK_SHADER_STAGE_FRAGMENT_BIT;

    std::array<VkDescriptorSetLayoutBinding, 2> bindings = {uboLayoutBinding, samplerLayoutBinding};
    VkDescriptorSetLayoutCreateInfo layoutInfo = {};
    layoutInfo.sType = VK_STRUCTURE_TYPE_DESCRIPTOR_SET_LAYOUT_CREATE_INFO;
    layoutInfo.bindingCount = static_cast<uint32_t>(bindings.size());
    layoutInfo.pBindings = bindings.data();

    if (vkCreateDescriptorSetLayout(device, &layoutInfo, nullptr, &pipeline->descriptorSetLayout) != VK_SUCCESS) {
        throw std::runtime_error("failed to create descriptor set layout!");
    }
}

void pipeline_create_descriptor_pool(pipeline_t *pipeline, VkDevice device) {
    std::array<VkDescriptorPoolSize, 2> poolSizes = {};
    poolSizes[0].type = VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER;
    poolSizes[0].descriptorCount = 1;
    poolSizes[1].type = VK_DESCRIPTOR_TYPE_COMBINED_IMAGE_SAMPLER;
    poolSizes[1].descriptorCount = 1;

    VkDescriptorPoolCreateInfo poolInfo = {};
    poolInfo.sType = VK_STRUCTURE_TYPE_DESCRIPTOR_POOL_CREATE_INFO;
    poolInfo.poolSizeCount = static_cast<uint32_t>(poolSizes.size());
    poolInfo.pPoolSizes = poolSizes.data();
    poolInfo.maxSets = 1;

    if (vkCreateDescriptorPool(device, &poolInfo, nullptr, &pipeline->descriptorPool) != VK_SUCCESS) {
        throw std::runtime_error("failed to create descriptor pool!");
    }
}

void pipeline_create_uniform_buffer(pipeline_t *pipeline, VkDevice device, VkPhysicalDevice physicalDevice) {
    VkDeviceSize bufferSize = sizeof(uniform_buffer_object_t);
    create_buffer(device, physicalDevice, bufferSize, VK_BUFFER_USAGE_UNIFORM_BUFFER_BIT, VK_MEMORY_PROPERTY_HOST_VISIBLE_BIT | VK_MEMORY_PROPERTY_HOST_COHERENT_BIT, &pipeline->uniformBuffer, &pipeline->uniformBufferMemory);
}

void pipeline_create_descriptor_set(pipeline_t *pipeline, VkDevice device) {
    VkDescriptorSetLayout layouts[] = {pipeline->descriptorSetLayout};
    VkDescriptorSetAllocateInfo allocInfo = {};
    allocInfo.sType = VK_STRUCTURE_TYPE_DESCRIPTOR_SET_ALLOCATE_INFO;
    allocInfo.descriptorPool = pipeline->descriptorPool;
    allocInfo.descriptorSetCount = 1;
    allocInfo.pSetLayouts = layouts;

    if (vkAllocateDescriptorSets(device, &allocInfo, &pipeline->descriptorSet) != VK_SUCCESS)
    {
        throw std::runtime_error("failed to allocate descriptor set!");
    }

    VkDescriptorBufferInfo bufferInfo = {};
    bufferInfo.buffer = pipeline->uniformBuffer;
    bufferInfo.offset = 0;
    bufferInfo.range = sizeof(uniform_buffer_object_t);

    VkDescriptorImageInfo imageInfo = {};
    imageInfo.imageLayout = VK_IMAGE_LAYOUT_SHADER_READ_ONLY_OPTIMAL;
    imageInfo.imageView = pipeline->texture.image.view;
    imageInfo.sampler = pipeline->texture.sampler;

    std::array<VkWriteDescriptorSet, 2> descriptorWrites = {};
    descriptorWrites[0].sType = VK_STRUCTURE_TYPE_WRITE_DESCRIPTOR_SET;
    descriptorWrites[0].dstSet = pipeline->descriptorSet;
    descriptorWrites[0].dstBinding = 0;
    descriptorWrites[0].dstArrayElement = 0;
    descriptorWrites[0].descriptorType = VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER;
    descriptorWrites[0].descriptorCount = 1;
    descriptorWrites[0].pBufferInfo = &bufferInfo;

    descriptorWrites[1].sType = VK_STRUCTURE_TYPE_WRITE_DESCRIPTOR_SET;
    descriptorWrites[1].dstSet = pipeline->descriptorSet;
    descriptorWrites[1].dstBinding = 1;
    descriptorWrites[1].dstArrayElement = 0;
    descriptorWrites[1].descriptorType = VK_DESCRIPTOR_TYPE_COMBINED_IMAGE_SAMPLER;
    descriptorWrites[1].descriptorCount = 1;
    descriptorWrites[1].pImageInfo = &imageInfo;

    vkUpdateDescriptorSets(device, static_cast<uint32_t>(descriptorWrites.size()), descriptorWrites.data(), 0, nullptr);
}

void pipeline_create_graphics(pipeline_t *pipeline, uint32_t width, uint32_t height, VkDevice device)
{
    shader_t shader;
    shader_create(&shader, device, pipeline->vertShader, pipeline->fragShader);

    VkPipelineShaderStageCreateInfo vertShaderStageInfo = {};
    vertShaderStageInfo.sType = VK_STRUCTURE_TYPE_PIPELINE_SHADER_STAGE_CREATE_INFO;
    vertShaderStageInfo.stage = VK_SHADER_STAGE_VERTEX_BIT;
    vertShaderStageInfo.module = shader.vertShaderModule;
    vertShaderStageInfo.pName = "main";

    VkPipelineShaderStageCreateInfo fragShaderStageInfo = {};
    fragShaderStageInfo.sType = VK_STRUCTURE_TYPE_PIPELINE_SHADER_STAGE_CREATE_INFO;
    fragShaderStageInfo.stage = VK_SHADER_STAGE_FRAGMENT_BIT;
    fragShaderStageInfo.module = shader.fragShaderModule;
    fragShaderStageInfo.pName = "main";

    VkPipelineShaderStageCreateInfo shaderStages[] = {vertShaderStageInfo, fragShaderStageInfo};

    std::array<VkVertexInputBindingDescription, 2> bindingDescriptions = {
        vertex_get_binding_description(),
        instance_get_binding_description()
    };
    auto attributeDescriptions = vertex_get_attribute_descriptions();

    VkPipelineVertexInputStateCreateInfo vertexInputInfo = {};
    vertexInputInfo.sType = VK_STRUCTURE_TYPE_PIPELINE_VERTEX_INPUT_STATE_CREATE_INFO;
    vertexInputInfo.vertexBindingDescriptionCount = 2;
    vertexInputInfo.vertexAttributeDescriptionCount = static_cast<uint32_t>(attributeDescriptions.size());
    vertexInputInfo.pVertexBindingDescriptions = bindingDescriptions.data();
    vertexInputInfo.pVertexAttributeDescriptions = attributeDescriptions.data();

    VkPipelineInputAssemblyStateCreateInfo inputAssembly = {};
    inputAssembly.sType = VK_STRUCTURE_TYPE_PIPELINE_INPUT_ASSEMBLY_STATE_CREATE_INFO;
    inputAssembly.topology = VK_PRIMITIVE_TOPOLOGY_TRIANGLE_LIST;
    inputAssembly.primitiveRestartEnable = VK_FALSE;

    VkViewport viewport = {};
    viewport.x = 0.0f;
    viewport.y = 0.0f;
    viewport.width = (float) width;
    viewport.height = (float) height;
    viewport.minDepth = 0.0f;
    viewport.maxDepth = 1.0f;

    VkExtent2D extent = {};
    extent.width = width;
    extent.height = height;

    VkRect2D scissor = {};
    scissor.offset = {0, 0};
    scissor.extent = extent;

    VkPipelineViewportStateCreateInfo viewportState = {};
    viewportState.sType = VK_STRUCTURE_TYPE_PIPELINE_VIEWPORT_STATE_CREATE_INFO;
    viewportState.viewportCount = 1;
    viewportState.pViewports = &viewport;
    viewportState.scissorCount = 1;
    viewportState.pScissors = &scissor;

    VkPipelineRasterizationStateCreateInfo rasterizer = {};
    rasterizer.sType = VK_STRUCTURE_TYPE_PIPELINE_RASTERIZATION_STATE_CREATE_INFO;
    rasterizer.depthClampEnable = VK_FALSE;
    rasterizer.rasterizerDiscardEnable = VK_FALSE;
    rasterizer.polygonMode = VK_POLYGON_MODE_FILL;
    rasterizer.lineWidth = 1.0f;
    rasterizer.cullMode = VK_CULL_MODE_BACK_BIT;
    //rasterizer.frontFace = VK_FRONT_FACE_CLOCKWISE;
    rasterizer.frontFace = VK_FRONT_FACE_COUNTER_CLOCKWISE;
    rasterizer.depthBiasEnable = VK_FALSE;

    VkPipelineMultisampleStateCreateInfo multisampling = {};
    multisampling.sType = VK_STRUCTURE_TYPE_PIPELINE_MULTISAMPLE_STATE_CREATE_INFO;
    multisampling.sampleShadingEnable = VK_FALSE;
    multisampling.rasterizationSamples = VK_SAMPLE_COUNT_1_BIT;

    VkPipelineColorBlendAttachmentState colorBlendAttachment = {};
    colorBlendAttachment.colorWriteMask = VK_COLOR_COMPONENT_R_BIT | VK_COLOR_COMPONENT_G_BIT | VK_COLOR_COMPONENT_B_BIT | VK_COLOR_COMPONENT_A_BIT;
    colorBlendAttachment.blendEnable = VK_FALSE;

    VkPipelineColorBlendStateCreateInfo colorBlending = {};
    colorBlending.sType = VK_STRUCTURE_TYPE_PIPELINE_COLOR_BLEND_STATE_CREATE_INFO;
    colorBlending.logicOpEnable = VK_FALSE;
    colorBlending.logicOp = VK_LOGIC_OP_COPY;
    colorBlending.attachmentCount = 1;
    colorBlending.pAttachments = &colorBlendAttachment;
    colorBlending.blendConstants[0] = 0.0f;
    colorBlending.blendConstants[1] = 0.0f;
    colorBlending.blendConstants[2] = 0.0f;
    colorBlending.blendConstants[3] = 0.0f;

    VkPipelineLayoutCreateInfo pipelineLayoutInfo = {};
    pipelineLayoutInfo.sType = VK_STRUCTURE_TYPE_PIPELINE_LAYOUT_CREATE_INFO;
    pipelineLayoutInfo.setLayoutCount = 1;
    pipelineLayoutInfo.pSetLayouts = &pipeline->descriptorSetLayout;
    //pipelineLayoutInfo.setLayoutCount = 0;
    pipelineLayoutInfo.pushConstantRangeCount = 0;

    if (vkCreatePipelineLayout(device, &pipelineLayoutInfo, nullptr, &pipeline->layout) != VK_SUCCESS) {
        throw std::runtime_error("failed to create pipeline layout!");
    }

    VkPipelineDepthStencilStateCreateInfo depthStencil = {};
    depthStencil.sType = VK_STRUCTURE_TYPE_PIPELINE_DEPTH_STENCIL_STATE_CREATE_INFO;
    depthStencil.depthTestEnable = VK_TRUE;
    depthStencil.depthWriteEnable = VK_TRUE;
    depthStencil.depthCompareOp = VK_COMPARE_OP_LESS;
    depthStencil.depthBoundsTestEnable = VK_FALSE;
    depthStencil.minDepthBounds = 0.0f; // Optional
    depthStencil.maxDepthBounds = 1.0f; // Optional
    depthStencil.stencilTestEnable = VK_FALSE;
    depthStencil.front = {}; // Optional
    depthStencil.back = {}; // Optional

    VkGraphicsPipelineCreateInfo pipelineInfo = {};
    pipelineInfo.sType = VK_STRUCTURE_TYPE_GRAPHICS_PIPELINE_CREATE_INFO;
    pipelineInfo.stageCount = 2;
    pipelineInfo.pStages = shaderStages;
    pipelineInfo.pVertexInputState = &vertexInputInfo;
    pipelineInfo.pInputAssemblyState = &inputAssembly;
    pipelineInfo.pViewportState = &viewportState;
    pipelineInfo.pRasterizationState = &rasterizer;
    pipelineInfo.pMultisampleState = &multisampling;
    pipelineInfo.pColorBlendState = &colorBlending;
    pipelineInfo.pDepthStencilState = &depthStencil;
    pipelineInfo.layout = pipeline->layout;
    pipelineInfo.renderPass = pipeline->renderPass;
    pipelineInfo.subpass = 0;
    pipelineInfo.basePipelineHandle = VK_NULL_HANDLE;

    if (vkCreateGraphicsPipelines(device, VK_NULL_HANDLE, 1, &pipelineInfo, nullptr, &pipeline->pipeline) != VK_SUCCESS) {
        throw std::runtime_error("failed to create graphics pipeline!");
    }

    vkDestroyShaderModule(device, shader.fragShaderModule, nullptr);
    vkDestroyShaderModule(device, shader.vertShaderModule, nullptr);
}

void pipeline_create(pipeline_t *pipeline, uint32_t width, uint32_t height, std::string vertShader, std::string fragShader, VkDevice device, VkPhysicalDevice physicalDevice, VkFormat colorFormat, VkFormat depthFormat, VkCommandPool commandPool, VkQueue graphicsQueue) {
    pipeline->device = device;
    pipeline->physicalDevice = physicalDevice;
    pipeline->vertShader = vertShader;
    pipeline->fragShader = fragShader;
    pipeline->commandPool = commandPool;
    pipeline->graphicsQueue = graphicsQueue;
    pipeline_create_render_pass(pipeline, device, colorFormat, depthFormat);
    pipeline_create_descriptor_set_layout(pipeline, device);
    pipeline_create_descriptor_pool(pipeline, device);
    pipeline_create_uniform_buffer(pipeline, device, physicalDevice);
    pipeline_create_descriptor_set(pipeline, device);
    pipeline_create_graphics(pipeline, width, height, device);

    physics_world_init(&pipeline->world);
}

void pipeline_recreate(pipeline_t *pipeline, uint32_t width, uint32_t height, VkDevice device, VkFormat colorFormat, VkFormat depthFormat)
{
    vkDestroyPipeline(device, pipeline->pipeline, nullptr);
    vkDestroyPipelineLayout(device, pipeline->layout, nullptr);
    vkDestroyRenderPass(device, pipeline->renderPass, nullptr);

    pipeline_create_graphics(pipeline, width, height, device);
    pipeline_create_render_pass(pipeline, device, colorFormat, depthFormat);
}

void pipeline_add_model(pipeline_t *pipeline, std::string modelPath)
{
    pipeline->models[modelPath] = {};
    model_load(&pipeline->models[modelPath], modelPath);
    model_create_buffers(&pipeline->models[modelPath], pipeline->device, pipeline->physicalDevice, pipeline->commandPool, pipeline->graphicsQueue);
}

gameobject_t *pipeline_add_gameobject(pipeline_t *pipeline, std::string modelPath)
{
    pipeline->isDirty = true;
    if (modelPath != "" && pipeline->models.find(modelPath) == pipeline->models.end())
        pipeline_add_model(pipeline, modelPath);

    gameobject_t *temp = new gameobject_t();
    pipeline->tempGameobjects[modelPath].push_back(temp);
    return temp;
}

void pipeline_add_script(pipeline_t *pipeline, gameobject_t *object, std::string scriptPath)
{
    script_t *script = NULL;
    if (pipeline->scripts.find(scriptPath) == pipeline->scripts.end())
    {
        script = new script_t();
        script_create(script, scriptPath);
        script_setup(script, pipeline, object);
    }
    else
    {
        script = pipeline->scripts[scriptPath];
    }

    object->scripts.insert(script);
}

void pipeline_render(pipeline_t *pipeline, VkCommandBuffer commandBuffer)
{
    pipeline->isDirty = false;
    for (std::map<std::string, model_t>::iterator it = pipeline->models.begin(); it != pipeline->models.end(); ++it)
    {
        model_render(&it->second, commandBuffer, pipeline->layout, pipeline->pipeline, pipeline->descriptorSet);
    }
}

void pipeline_update(pipeline_t *pipeline, float delta)
{
    for (std::map<std::string, std::vector<gameobject_t*>>::iterator it = pipeline->tempGameobjects.begin(); it != pipeline->tempGameobjects.end(); ++it)
    {
        for (std::vector<gameobject_t*>::iterator it2 = it->second.begin(); it2 != it->second.end(); ++it2)
        {
            pipeline->gameobjects[it->first].push_back(*it2);
        }
        it->second.clear();
    }
    pipeline->tempGameobjects.clear();

    for (std::map<std::string, std::vector<gameobject_t*>>::iterator it = pipeline->gameobjects.begin(); it != pipeline->gameobjects.end(); ++it)
    {
        for (std::vector<gameobject_t*>::iterator it2 = it->second.begin(); it2 != it->second.end(); ++it2)
        {
            gameobject_update(*it2, (void*)pipeline, delta);
        }
    }

    physics_world_update(&pipeline->world, delta);

    for (std::map<std::string, std::vector<gameobject_t*>>::iterator it = pipeline->gameobjects.begin(); it != pipeline->gameobjects.end(); ++it)
    {
        for (std::vector<gameobject_t*>::iterator it2 = it->second.begin(); it2 != it->second.end(); ++it2)
        {
            gameobject_update_global(*it2);
        }
    }

    for (std::map<std::string, model_t>::iterator it = pipeline->models.begin(); it != pipeline->models.end(); ++it)
    {
        model_update(&it->second, delta);
        model_copy_instance_buffer(&it->second, pipeline->gameobjects[it->first], pipeline->device, pipeline->physicalDevice, pipeline->commandPool, pipeline->graphicsQueue);
    }
}

void pipeline_load(pipeline_t *pipeline, std::string path)
{
    script_create(&pipeline->script, path);
    script_setup(&pipeline->script, pipeline, NULL);
}

void pipeline_cleanup(pipeline_t *pipeline, VkDevice device)
{
    for (std::map<std::string, model_t>::iterator it = pipeline->models.begin(); it != pipeline->models.end(); ++it)
    {
        model_cleanup(&it->second, device);
    }
    vkDestroyPipeline(device, pipeline->pipeline, nullptr);
    vkDestroyPipelineLayout(device, pipeline->layout, nullptr);
    vkDestroyRenderPass(device, pipeline->renderPass, nullptr);
    vkDestroyDescriptorPool(device, pipeline->descriptorPool, nullptr);
    vkDestroyDescriptorSetLayout(device, pipeline->descriptorSetLayout, nullptr);
    vkDestroyBuffer(device, pipeline->uniformBuffer, nullptr);
    vkFreeMemory(device, pipeline->uniformBufferMemory, nullptr);
    physics_world_destroy(&pipeline->world);
}

void pipeline_on_cursor_pos(pipeline_t *pipeline, double x, double y)
{

    for (std::map<std::string, std::vector<gameobject_t*>>::iterator it = pipeline->gameobjects.begin(); it != pipeline->gameobjects.end(); ++it)
    {
        for (std::vector<gameobject_t*>::iterator it2 = it->second.begin(); it2 != it->second.end(); ++it2)
        {
            for (std::set<script_t*>::iterator it3 = (*it2)->scripts.begin(); it3 != (*it2)->scripts.end(); ++it3)
            {
                script_on_cursor_pos(*it3, pipeline, *it2, x, y);
            }
        }
    }
}

void pipeline_on_button_down(pipeline_t *pipeline, std::string buttonCode)
{

    for (std::map<std::string, std::vector<gameobject_t*>>::iterator it = pipeline->gameobjects.begin(); it != pipeline->gameobjects.end(); ++it)
    {
        for (std::vector<gameobject_t*>::iterator it2 = it->second.begin(); it2 != it->second.end(); ++it2)
        {
            for (std::set<script_t*>::iterator it3 = (*it2)->scripts.begin(); it3 != (*it2)->scripts.end(); ++it3)
            {
                script_on_button_down(*it3, pipeline, *it2, buttonCode);
            }
        }
    }
}

void pipeline_on_button_up(pipeline_t *pipeline, std::string buttonCode)
{
    for (std::map<std::string, std::vector<gameobject_t*>>::iterator it = pipeline->gameobjects.begin(); it != pipeline->gameobjects.end(); ++it)
    {
        for (std::vector<gameobject_t*>::iterator it2 = it->second.begin(); it2 != it->second.end(); ++it2)
        {
            for (std::set<script_t*>::iterator it3 = (*it2)->scripts.begin(); it3 != (*it2)->scripts.end(); ++it3)
            {
                script_on_button_up(*it3, pipeline, *it2, buttonCode);
            }
        }
    }
}

