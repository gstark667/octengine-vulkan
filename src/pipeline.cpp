#include <vulkan/vulkan.h>

#include "pipeline.h"
#include "util.h"
#include "shader.h"
#include "model.h"

#include <iostream>


void pipeline_attachment_create(pipeline_attachment_t *attachment, VkDevice device, VkPhysicalDevice physicalDevice, uint32_t width, uint32_t height, VkSampleCountFlagBits samples, VkFormat format, VkImageUsageFlags usage, VkCommandPool commandPool, VkQueue graphicsQueue, bool shadow)
{
    attachment->format = format;
    attachment->usage = usage;

    VkImageAspectFlags aspectFlags;
    if (usage & VK_IMAGE_USAGE_DEPTH_STENCIL_ATTACHMENT_BIT)
    {
        attachment->layout = VK_IMAGE_LAYOUT_DEPTH_STENCIL_ATTACHMENT_OPTIMAL;
        attachment->finalLayout = shadow ? VK_IMAGE_LAYOUT_DEPTH_STENCIL_READ_ONLY_OPTIMAL : VK_IMAGE_LAYOUT_DEPTH_STENCIL_ATTACHMENT_OPTIMAL;
        aspectFlags = VK_IMAGE_ASPECT_DEPTH_BIT;
    }
    else
    {
        attachment->layout = VK_IMAGE_LAYOUT_COLOR_ATTACHMENT_OPTIMAL;
        attachment->finalLayout = VK_IMAGE_LAYOUT_SHADER_READ_ONLY_OPTIMAL;
        aspectFlags = VK_IMAGE_ASPECT_COLOR_BIT;
    }
    image_create(&attachment->image, device, physicalDevice, width, height, 1, 1, format, VK_IMAGE_TILING_OPTIMAL, usage | VK_IMAGE_USAGE_SAMPLED_BIT, VK_MEMORY_PROPERTY_DEVICE_LOCAL_BIT, samples);
    image_create_view(&attachment->image, device, format, aspectFlags, 0, false);
}

void pipeline_attachment_from_image(pipeline_attachment_t *attachment, VkDevice device, VkImageAspectFlags aspectFlags, image_t image, uint32_t layer, bool shadow)
{
    attachment->format = image.format;
    attachment->usage = image.usage;
    attachment->image = image;
    attachment->destroy = false;
    if (image.usage & VK_IMAGE_USAGE_DEPTH_STENCIL_ATTACHMENT_BIT)
    {
        attachment->layout = VK_IMAGE_LAYOUT_DEPTH_STENCIL_ATTACHMENT_OPTIMAL;
        attachment->finalLayout = shadow ?  VK_IMAGE_LAYOUT_DEPTH_STENCIL_READ_ONLY_OPTIMAL : VK_IMAGE_LAYOUT_DEPTH_STENCIL_ATTACHMENT_OPTIMAL;
    }
    else
    {
        attachment->layout = VK_IMAGE_LAYOUT_COLOR_ATTACHMENT_OPTIMAL;
        attachment->finalLayout = VK_IMAGE_LAYOUT_SHADER_READ_ONLY_OPTIMAL;
    }
    image_create_view(&attachment->image, device, attachment->format, aspectFlags, layer, false); 
}

std::vector<VkImageView> pipeline_attachment_views(std::vector<pipeline_attachment_t> attachments)
{
    std::vector<VkImageView> output;
    for (std::vector<pipeline_attachment_t>::iterator it = attachments.begin(); it != attachments.end(); ++it)
    {
        output.push_back(it->image.view);
    }
    return output;
}

void pipeline_attachment_cleanup(pipeline_attachment_t *attachment, VkDevice device)
{
    if (attachment->destroy)
        image_cleanup(&attachment->image, device);
    else
        vkDestroyImageView(device, attachment->image.view, nullptr);
}

void pipeline_create_render_pass(pipeline_t *pipeline)
{
    std::vector<VkAttachmentDescription> descriptions;
    std::vector<VkAttachmentReference> colorRefs;
    VkAttachmentReference depthRef;
    VkAttachmentReference resolveRef;
    bool resolve = false;
    for (std::vector<pipeline_attachment_t>::iterator it = pipeline->attachments.begin(); it != pipeline->attachments.end(); ++it)
    {
        VkAttachmentDescription attachment = {};
        attachment.format = it->format;
        attachment.samples = it->image.samples;
        attachment.loadOp = VK_ATTACHMENT_LOAD_OP_CLEAR;
        attachment.storeOp = VK_ATTACHMENT_STORE_OP_STORE;
        attachment.stencilLoadOp = VK_ATTACHMENT_LOAD_OP_DONT_CARE;
        attachment.stencilStoreOp = VK_ATTACHMENT_STORE_OP_DONT_CARE;
        attachment.initialLayout = VK_IMAGE_LAYOUT_UNDEFINED;
        attachment.finalLayout = it->finalLayout;

        VkAttachmentReference attachmentRef = {};
        attachmentRef.attachment = descriptions.size();
        attachmentRef.layout = it->layout;
        if (it->usage & VK_IMAGE_USAGE_DEPTH_STENCIL_ATTACHMENT_BIT)
        {
            depthRef = attachmentRef;
        }
        else
        {
            if (it->resolve)
            {
                resolve = true;
                resolveRef = attachmentRef;
            }
            else
            {
                colorRefs.push_back(attachmentRef);
            }
        }
        descriptions.push_back(attachment);
    }

    VkSubpassDescription subpass = {};
    subpass.pipelineBindPoint = VK_PIPELINE_BIND_POINT_GRAPHICS;
    subpass.colorAttachmentCount = colorRefs.size();
    subpass.pColorAttachments = colorRefs.data();
    subpass.pDepthStencilAttachment = &depthRef;
    if (resolve)
    {
        subpass.pResolveAttachments = &resolveRef;
    }

    std::vector<VkSubpassDependency> dependencies;
    if (pipeline->offscreen)
    {
        if (pipeline->shadow)
        {
            VkSubpassDependency dependency0, dependency1;
            dependency0.srcSubpass = VK_SUBPASS_EXTERNAL;
            dependency0.dstSubpass = 0;
            dependency0.srcStageMask = VK_PIPELINE_STAGE_BOTTOM_OF_PIPE_BIT;
            dependency0.dstStageMask = VK_PIPELINE_STAGE_LATE_FRAGMENT_TESTS_BIT;
            dependency0.srcAccessMask = 0;
            dependency0.dstAccessMask = VK_ACCESS_DEPTH_STENCIL_ATTACHMENT_READ_BIT | VK_ACCESS_DEPTH_STENCIL_ATTACHMENT_WRITE_BIT;
            dependency0.dependencyFlags = VK_DEPENDENCY_BY_REGION_BIT;
            dependencies.push_back(dependency0);

            dependency1.srcSubpass = 0;
            dependency1.dstSubpass = VK_SUBPASS_EXTERNAL;
            dependency1.srcStageMask = VK_PIPELINE_STAGE_LATE_FRAGMENT_TESTS_BIT;
            dependency1.dstStageMask = VK_PIPELINE_STAGE_FRAGMENT_SHADER_BIT;
            dependency1.srcAccessMask = VK_ACCESS_DEPTH_STENCIL_ATTACHMENT_READ_BIT | VK_ACCESS_DEPTH_STENCIL_ATTACHMENT_WRITE_BIT;
            dependency1.dstAccessMask = VK_ACCESS_SHADER_READ_BIT;
            dependency1.dependencyFlags = VK_DEPENDENCY_BY_REGION_BIT;
            dependencies.push_back(dependency1);
        }
        else
        {
            VkSubpassDependency dependency0, dependency1;
            dependency0.srcSubpass = VK_SUBPASS_EXTERNAL;
            dependency0.dstSubpass = 0;
            dependency0.srcStageMask = VK_PIPELINE_STAGE_BOTTOM_OF_PIPE_BIT;
            dependency0.dstStageMask = VK_PIPELINE_STAGE_COLOR_ATTACHMENT_OUTPUT_BIT;
            dependency0.srcAccessMask = VK_ACCESS_MEMORY_READ_BIT;
            dependency0.dstAccessMask = VK_ACCESS_COLOR_ATTACHMENT_READ_BIT | VK_ACCESS_COLOR_ATTACHMENT_WRITE_BIT;
            dependency0.dependencyFlags = VK_DEPENDENCY_BY_REGION_BIT;
            dependencies.push_back(dependency0);

            dependency1.srcSubpass = 0;                                                        
            dependency1.dstSubpass = VK_SUBPASS_EXTERNAL;                                
            dependency1.srcStageMask = VK_PIPELINE_STAGE_COLOR_ATTACHMENT_OUTPUT_BIT;
            dependency1.dstStageMask = VK_PIPELINE_STAGE_BOTTOM_OF_PIPE_BIT;
            dependency1.srcAccessMask = VK_ACCESS_COLOR_ATTACHMENT_READ_BIT | VK_ACCESS_COLOR_ATTACHMENT_WRITE_BIT;
            dependency1.dstAccessMask = VK_ACCESS_MEMORY_READ_BIT;
            dependency1.dependencyFlags = VK_DEPENDENCY_BY_REGION_BIT;
            dependencies.push_back(dependency1);
        }
    }
    else
    {
        VkSubpassDependency dependency = {};
        dependency.srcSubpass = VK_SUBPASS_EXTERNAL;
        dependency.dstSubpass = 0;
        dependency.srcStageMask = VK_PIPELINE_STAGE_COLOR_ATTACHMENT_OUTPUT_BIT;
        dependency.srcAccessMask = 0;
        dependency.dstStageMask = VK_PIPELINE_STAGE_COLOR_ATTACHMENT_OUTPUT_BIT;
        dependency.dstAccessMask = VK_ACCESS_COLOR_ATTACHMENT_READ_BIT | VK_ACCESS_COLOR_ATTACHMENT_WRITE_BIT;
        dependencies.push_back(dependency);
    }

    VkRenderPassCreateInfo renderPassInfo = {};
    renderPassInfo.sType = VK_STRUCTURE_TYPE_RENDER_PASS_CREATE_INFO;
    renderPassInfo.attachmentCount = descriptions.size();
    renderPassInfo.pAttachments = descriptions.data();
    renderPassInfo.subpassCount = 1;
    renderPassInfo.pSubpasses = &subpass;
    renderPassInfo.dependencyCount = dependencies.size();
    renderPassInfo.pDependencies = dependencies.data();

    if (vkCreateRenderPass(pipeline->device, &renderPassInfo, nullptr, &pipeline->renderPass) != VK_SUCCESS) {
        throw std::runtime_error("failed to create render pass!");
    }
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
    rasterizer.depthBiasEnable = VK_TRUE;
    rasterizer.rasterizerDiscardEnable = VK_FALSE;
    rasterizer.polygonMode = VK_POLYGON_MODE_FILL;
    rasterizer.lineWidth = 1.0f;
    if (pipeline->cullBack)
        rasterizer.cullMode = VK_CULL_MODE_BACK_BIT;
    else
        rasterizer.cullMode = VK_CULL_MODE_FRONT_BIT;
    //rasterizer.frontFace = VK_FRONT_FACE_CLOCKWISE;
    rasterizer.frontFace = VK_FRONT_FACE_COUNTER_CLOCKWISE;
    rasterizer.depthBiasEnable = VK_FALSE;

    VkPipelineMultisampleStateCreateInfo multisampling = {};
    multisampling.sType = VK_STRUCTURE_TYPE_PIPELINE_MULTISAMPLE_STATE_CREATE_INFO;
    if (pipeline->samples != VK_SAMPLE_COUNT_1_BIT)
    {
        multisampling.sampleShadingEnable = VK_TRUE;
        multisampling.minSampleShading = 0.5f;
    }
    else
    {
        multisampling.sampleShadingEnable = VK_FALSE;
    }
    multisampling.rasterizationSamples = pipeline->samples;

    std::vector<VkPipelineColorBlendAttachmentState> colorBlendAttachments;
    for (auto it = pipeline->attachments.begin(); it != pipeline->attachments.end(); ++it)
    {
        if (it->usage & VK_IMAGE_USAGE_DEPTH_STENCIL_ATTACHMENT_BIT || it->resolve)
            continue;
        VkPipelineColorBlendAttachmentState colorBlendAttachment = {};
        colorBlendAttachment.colorWriteMask = VK_COLOR_COMPONENT_R_BIT | VK_COLOR_COMPONENT_G_BIT | VK_COLOR_COMPONENT_B_BIT | VK_COLOR_COMPONENT_A_BIT;
        colorBlendAttachment.blendEnable = VK_TRUE;
        colorBlendAttachment.srcColorBlendFactor=VK_BLEND_FACTOR_SRC_ALPHA;
        colorBlendAttachment.dstColorBlendFactor=VK_BLEND_FACTOR_ONE_MINUS_SRC_ALPHA;
        colorBlendAttachment.colorBlendOp=VK_BLEND_OP_ADD;
        colorBlendAttachment.srcAlphaBlendFactor=VK_BLEND_FACTOR_ONE;
        colorBlendAttachment.dstAlphaBlendFactor=VK_BLEND_FACTOR_ZERO;
        colorBlendAttachment.alphaBlendOp=VK_BLEND_OP_ADD;
        colorBlendAttachments.push_back(colorBlendAttachment);
    }

    VkPipelineColorBlendStateCreateInfo colorBlending = {};
    colorBlending.sType = VK_STRUCTURE_TYPE_PIPELINE_COLOR_BLEND_STATE_CREATE_INFO;
    colorBlending.logicOpEnable = VK_FALSE;
    colorBlending.logicOp = VK_LOGIC_OP_COPY;
    colorBlending.attachmentCount = colorBlendAttachments.size();
    colorBlending.pAttachments = colorBlendAttachments.data();
    colorBlending.blendConstants[0] = 0.0f;
    colorBlending.blendConstants[1] = 0.0f;
    colorBlending.blendConstants[2] = 0.0f;
    colorBlending.blendConstants[3] = 0.0f;

    VkPipelineLayoutCreateInfo pipelineLayoutInfo = {};
    pipelineLayoutInfo.sType = VK_STRUCTURE_TYPE_PIPELINE_LAYOUT_CREATE_INFO;
    pipelineLayoutInfo.setLayoutCount = 1;
    pipelineLayoutInfo.pSetLayouts = &pipeline->descriptorSet->descriptorSetLayout;
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

void pipeline_create_framebuffer(pipeline_t *pipeline)
{
    std::vector<VkImageView> views = pipeline_attachment_views(pipeline->attachments);
    VkFramebufferCreateInfo fbufCreateInfo = {};
    fbufCreateInfo.sType = VK_STRUCTURE_TYPE_FRAMEBUFFER_CREATE_INFO;
    fbufCreateInfo.pNext = NULL;
    fbufCreateInfo.renderPass = pipeline->renderPass;
    fbufCreateInfo.pAttachments = views.data();
    fbufCreateInfo.attachmentCount = views.size();
    fbufCreateInfo.width = pipeline->width;
    fbufCreateInfo.height = pipeline->height;
    fbufCreateInfo.layers = 1;

    if (vkCreateFramebuffer(pipeline->device, &fbufCreateInfo, nullptr, &pipeline->framebuffer) != VK_SUCCESS)
    {
        throw std::runtime_error("failed to create framebuffer!");
    }
}

void pipeline_create(pipeline_t *pipeline, descriptor_set_t *descriptorSet, uint32_t width, uint32_t height, std::string vertShader, std::string fragShader, VkDevice device, VkPhysicalDevice physicalDevice, VkSampleCountFlagBits samples, VkCommandPool commandPool, VkQueue graphicsQueue, std::vector<pipeline_attachment_t> attachments, bool offscreen, bool shadow) {
    pipeline->device = device;
    pipeline->physicalDevice = physicalDevice;
    pipeline->samples = samples;
    pipeline->width = width;
    pipeline->height = height;
    pipeline->vertShader = vertShader;
    pipeline->fragShader = fragShader;
    pipeline->commandPool = commandPool;
    pipeline->graphicsQueue = graphicsQueue;
    pipeline->descriptorSet = descriptorSet;
    pipeline->attachments = attachments;
    pipeline->offscreen = offscreen;
    pipeline->shadow = shadow;
    pipeline_create_render_pass(pipeline);
    pipeline_create_graphics(pipeline, width, height, device);
    if (offscreen)
    {
        pipeline_create_framebuffer(pipeline);
    }
}

void pipeline_begin_render(pipeline_t *pipeline, VkCommandBuffer commandBuffer)
{
    VkCommandBufferBeginInfo beginInfo = {};
    beginInfo.sType = VK_STRUCTURE_TYPE_COMMAND_BUFFER_BEGIN_INFO;
    beginInfo.flags = VK_COMMAND_BUFFER_USAGE_SIMULTANEOUS_USE_BIT;

    vkBeginCommandBuffer(commandBuffer, &beginInfo);

    VkRenderPassBeginInfo renderPassInfo = {};
    renderPassInfo.sType = VK_STRUCTURE_TYPE_RENDER_PASS_BEGIN_INFO;
    renderPassInfo.renderPass = pipeline->renderPass;
    renderPassInfo.framebuffer = pipeline->framebuffer;
    renderPassInfo.renderArea.offset = {0, 0};
    renderPassInfo.renderArea.extent = {pipeline->width, pipeline->height};

    std::vector<VkClearValue> clearValues;
    for (auto it = pipeline->attachments.begin(); it != pipeline->attachments.end(); ++it)
    {
        VkClearValue clearValue;
        if (it->usage & VK_IMAGE_USAGE_DEPTH_STENCIL_ATTACHMENT_BIT)
            clearValue.depthStencil = {1.0f, 0};
        else
            clearValue.color = {0.0f, 0.0f, 0.0f, 1.0f};
        clearValues.push_back(clearValue);
    }

    renderPassInfo.clearValueCount = clearValues.size();
    renderPassInfo.pClearValues = clearValues.data();

    vkCmdBeginRenderPass(commandBuffer, &renderPassInfo, VK_SUBPASS_CONTENTS_INLINE);
};

void pipeline_end_render(pipeline_t *pipeline, VkCommandBuffer commandBuffer)
{
    vkCmdEndRenderPass(commandBuffer);

    if (vkEndCommandBuffer(commandBuffer) != VK_SUCCESS) {
        throw std::runtime_error("failed to record command buffer!");
    }
}

void pipeline_recreate(pipeline_t *pipeline, uint32_t width, uint32_t height, VkDevice device)
{
    vkDestroyPipeline(device, pipeline->pipeline, nullptr);
    vkDestroyPipelineLayout(device, pipeline->layout, nullptr);
    vkDestroyRenderPass(device, pipeline->renderPass, nullptr);

    pipeline_create_graphics(pipeline, width, height, device);
    pipeline_create_render_pass(pipeline);
}

void pipeline_cleanup(pipeline_t *pipeline)
{
    vkDestroyPipeline(pipeline->device, pipeline->pipeline, nullptr);
    vkDestroyPipelineLayout(pipeline->device, pipeline->layout, nullptr);
    vkDestroyRenderPass(pipeline->device, pipeline->renderPass, nullptr);
    if (pipeline->offscreen)
        vkDestroyFramebuffer(pipeline->device, pipeline->framebuffer, nullptr);
}

