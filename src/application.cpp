#include "application.h"

#include <chrono>


// queue family
bool queue_family_is_complete(queue_family_t *queueFamily)
{
    return queueFamily->graphicsFamily >= 0 && queueFamily->presentFamily >= 0;
}

// swapchain support
swapchain_support_t application_query_swap_chain_support(application_t *app, VkPhysicalDevice device) {
    swapchain_support_t details;

    vkGetPhysicalDeviceSurfaceCapabilitiesKHR(device, app->surface, &details.capabilities);

    uint32_t formatCount;
    vkGetPhysicalDeviceSurfaceFormatsKHR(device, app->surface, &formatCount, nullptr);

    if (formatCount != 0) {
        details.formats.resize(formatCount);
        vkGetPhysicalDeviceSurfaceFormatsKHR(device, app->surface, &formatCount, details.formats.data());
    }

    uint32_t presentModeCount;
    vkGetPhysicalDeviceSurfacePresentModesKHR(device, app->surface, &presentModeCount, nullptr);

    if (presentModeCount != 0) {
        details.presentModes.resize(presentModeCount);
        vkGetPhysicalDeviceSurfacePresentModesKHR(device, app->surface, &presentModeCount, details.presentModes.data());
    }

    return details;
}


void application_init_window(application_t *app) {
    SDL_Init(SDL_INIT_VIDEO);
    app->window = SDL_CreateWindow(
        "octengine",
        SDL_WINDOWPOS_UNDEFINED,
        SDL_WINDOWPOS_UNDEFINED,
        app->windowWidth,
        app->windowHeight,
        SDL_WINDOW_VULKAN
    );

    camera_resize(&app->scene.camera, app->windowWidth, app->windowHeight, 90.0f);

    SDL_SetWindowGrab(app->window, SDL_TRUE);
    SDL_SetRelativeMouseMode(SDL_TRUE);
}

// create instance
bool application_check_validation_layer_support(application_t *app) {
    uint32_t layerCount;
    vkEnumerateInstanceLayerProperties(&layerCount, nullptr);

    std::vector<VkLayerProperties> availableLayers(layerCount);
    vkEnumerateInstanceLayerProperties(&layerCount, availableLayers.data());

    for (const char* layerName: app->validationLayers) {
        bool layerFound = false;

        for (const auto& layerProperties: availableLayers) {
            if (strcmp(layerName, layerProperties.layerName) == 0) {
                layerFound = true;
                break;
            }
        }

        if (!layerFound) {
            return false;
        }
    }

    return true;
}

std::vector<const char*> application_get_required_extensions(application_t *app)
{
    uint32_t sdlExtensionCount = 0;
    const char** sdlExtensions;

    SDL_Vulkan_GetInstanceExtensions(app->window, &sdlExtensionCount, NULL);
    sdlExtensions = new const char *[sdlExtensionCount];
    SDL_Vulkan_GetInstanceExtensions(app->window, &sdlExtensionCount, sdlExtensions);

    std::vector<const char*> extensions(sdlExtensions, sdlExtensions + sdlExtensionCount);

    if (app->enableValidationLayers) {
        extensions.push_back(VK_EXT_DEBUG_REPORT_EXTENSION_NAME);
    }

    return extensions;
}

void application_create_instance(application_t *app) {
    if (app->enableValidationLayers && !application_check_validation_layer_support(app)) {
        throw std::runtime_error("validation layers requested, but not available!");
    }

    VkApplicationInfo appInfo = {};
    appInfo.sType = VK_STRUCTURE_TYPE_APPLICATION_INFO;
    appInfo.pApplicationName = app->name.c_str();
    appInfo.applicationVersion = VK_MAKE_VERSION(1, 0, 0);
    appInfo.pEngineName = "octengine";
    appInfo.engineVersion = VK_MAKE_VERSION(1, 0, 0);
    appInfo.apiVersion = VK_API_VERSION_1_0;

    VkInstanceCreateInfo createInfo = {};
    createInfo.sType = VK_STRUCTURE_TYPE_INSTANCE_CREATE_INFO;
    createInfo.pApplicationInfo = &appInfo;

    auto extensions = application_get_required_extensions(app);
    createInfo.enabledExtensionCount = static_cast<uint32_t>(extensions.size());
    createInfo.ppEnabledExtensionNames = extensions.data();

    if (app->enableValidationLayers) {
        createInfo.enabledLayerCount = static_cast<uint32_t>(app->validationLayers.size());
        createInfo.ppEnabledLayerNames = app->validationLayers.data();
    } else {
        createInfo.enabledLayerCount = 0;
    }

    if (vkCreateInstance(&createInfo, nullptr, &app->instance) != VK_SUCCESS) {
        throw std::runtime_error("failed to create instance!");
    }
}

// debug callback
VkResult CreateDebugReportCallbackEXT(VkInstance instance, const VkDebugReportCallbackCreateInfoEXT* pCreateInfo, const VkAllocationCallbacks* pAllocator, VkDebugReportCallbackEXT* pCallback)
{
    auto func = (PFN_vkCreateDebugReportCallbackEXT) vkGetInstanceProcAddr(instance, "vkCreateDebugReportCallbackEXT");
    if (func != nullptr)
    {
        return func(instance, pCreateInfo, pAllocator, pCallback);
    }
    else
    {
        return VK_ERROR_EXTENSION_NOT_PRESENT;
    }
}

void DestroyDebugReportCallbackEXT(VkInstance instance, VkDebugReportCallbackEXT callback, const VkAllocationCallbacks* pAllocator)
    {
    auto func = (PFN_vkDestroyDebugReportCallbackEXT) vkGetInstanceProcAddr(instance, "vkDestroyDebugReportCallbackEXT");
    if (func != nullptr)
    {
        func(instance, callback, pAllocator);
    }
}

static VKAPI_ATTR VkBool32 VKAPI_CALL debug_callback(VkDebugReportFlagsEXT flags, VkDebugReportObjectTypeEXT objType, uint64_t obj, size_t location, int32_t code, const char* layerPrefix, const char *msg, void *userData) {
    std::cerr << "validation layer: " << msg << std::endl;

    return VK_FALSE;
}

void application_setup_debug_callback(application_t *app) {
    if (!app->enableValidationLayers) return;

    VkDebugReportCallbackCreateInfoEXT createInfo = {};
    createInfo.sType = VK_STRUCTURE_TYPE_DEBUG_REPORT_CALLBACK_CREATE_INFO_EXT;
    createInfo.flags = VK_DEBUG_REPORT_ERROR_BIT_EXT | VK_DEBUG_REPORT_WARNING_BIT_EXT;
    createInfo.pfnCallback = debug_callback;

    if (CreateDebugReportCallbackEXT(app->instance, &createInfo, nullptr, &app->callback) != VK_SUCCESS) {
        throw std::runtime_error("failed to set up debug callback!");
    }
}

// create surface
void application_create_surface(application_t *app)
{
    SDL_Vulkan_CreateSurface(app->window, app->instance, &app->surface);
}

// pick physical device
queue_family_t application_find_queue_families(application_t *app, VkPhysicalDevice device) {
    queue_family_t indices;

    uint32_t queueFamilyCount = 0;
    vkGetPhysicalDeviceQueueFamilyProperties(device, &queueFamilyCount, nullptr);

    std::vector<VkQueueFamilyProperties> queueFamilies(queueFamilyCount);
    vkGetPhysicalDeviceQueueFamilyProperties(device, &queueFamilyCount, queueFamilies.data());

    int i = 0;
    for (const auto& queueFamily: queueFamilies) {
        if (queueFamily.queueCount > 0 && queueFamily.queueFlags & VK_QUEUE_GRAPHICS_BIT) {
            indices.graphicsFamily = i;
        }

        VkBool32 presentSupport = false;
        vkGetPhysicalDeviceSurfaceSupportKHR(device, i, app->surface, &presentSupport);

        if (queueFamily.queueCount > 0 && presentSupport) {
            indices.presentFamily = i;
        }

        if (queue_family_is_complete(&indices)) {
            break;
        }

        i++;
    }

    return indices;
}

bool application_check_device_extension_support(application_t *app, VkPhysicalDevice device) {
    uint32_t extensionCount;
    vkEnumerateDeviceExtensionProperties(device, nullptr, &extensionCount, nullptr);

    std::vector<VkExtensionProperties> availableExtensions(extensionCount);
    vkEnumerateDeviceExtensionProperties(device, nullptr, &extensionCount, availableExtensions.data());

    std::set<std::string> requiredExtensions(app->deviceExtensions.begin(), app->deviceExtensions.end());

    for (const auto& extension : availableExtensions) {
        requiredExtensions.erase(extension.extensionName);
    }

    return requiredExtensions.empty();
}

bool application_is_device_suitable(application_t *app, VkPhysicalDevice device) {
    queue_family_t indices = application_find_queue_families(app, device);

    bool extensionsSupported = application_check_device_extension_support(app, device);

    bool swapChainAdequate = false;
    if (extensionsSupported) {
        swapchain_support_t swapChainSupport = application_query_swap_chain_support(app, device);
        swapChainAdequate = !swapChainSupport.formats.empty() && !swapChainSupport.presentModes.empty();
    }

    return queue_family_is_complete(&indices) && extensionsSupported && swapChainAdequate;
}

void application_pick_physical_device(application_t *app) {
    uint32_t deviceCount = 0;
    vkEnumeratePhysicalDevices(app->instance, &deviceCount, nullptr);

    if (deviceCount == 0) {
        throw std::runtime_error("failed to find GPUs with Vulkan support!");
    }

    std::vector<VkPhysicalDevice> devices(deviceCount);
    vkEnumeratePhysicalDevices(app->instance, &deviceCount, devices.data());

    for (const auto& device : devices) {
        if (application_is_device_suitable(app, device)) {
            app->physicalDevice = device;
            break;
        }
    }

    if (app->physicalDevice == VK_NULL_HANDLE) {
        throw std::runtime_error("failed to find a suitable GPU!");
    }
}

// create logical device
void application_create_logical_device(application_t *app) {
    queue_family_t indices = application_find_queue_families(app, app->physicalDevice);

    std::vector<VkDeviceQueueCreateInfo> queueCreateInfos;
    std::set<int> uniqueQueueFamilies = {indices.graphicsFamily, indices.presentFamily};

    float queuePriority = 1.0f;
    for (int queueFamily : uniqueQueueFamilies) {
        VkDeviceQueueCreateInfo queueCreateInfo = {};
        queueCreateInfo.sType = VK_STRUCTURE_TYPE_DEVICE_QUEUE_CREATE_INFO;
        queueCreateInfo.queueFamilyIndex = queueFamily;
        queueCreateInfo.queueCount = 1;
        queueCreateInfo.pQueuePriorities = &queuePriority;
        queueCreateInfos.push_back(queueCreateInfo);
    }

    VkPhysicalDeviceFeatures deviceFeatures = {};

    VkDeviceCreateInfo createInfo = {};
    createInfo.sType = VK_STRUCTURE_TYPE_DEVICE_CREATE_INFO;

    createInfo.queueCreateInfoCount = static_cast<uint32_t>(queueCreateInfos.size());
    createInfo.pQueueCreateInfos = queueCreateInfos.data();

    createInfo.pEnabledFeatures = &deviceFeatures;

    createInfo.enabledExtensionCount = static_cast<uint32_t>(app->deviceExtensions.size());
    createInfo.ppEnabledExtensionNames = app->deviceExtensions.data();

    if (app->enableValidationLayers) {
        createInfo.enabledLayerCount = static_cast<uint32_t>(app->validationLayers.size());
        createInfo.ppEnabledLayerNames = app->validationLayers.data();
    } else {
        createInfo.enabledLayerCount = 0;
    }

    if (vkCreateDevice(app->physicalDevice, &createInfo, nullptr, &app->device) != VK_SUCCESS) {
        throw std::runtime_error("failed to create logical device!");
    }

    vkGetDeviceQueue(app->device, indices.graphicsFamily, 0, &app->graphicsQueue);
    vkGetDeviceQueue(app->device, indices.presentFamily, 0, &app->presentQueue);
}

// create swapchain
VkSurfaceFormatKHR application_choose_swap_surface_format(application_t * app, const std::vector<VkSurfaceFormatKHR>& availableFormats) {
    if (availableFormats.size() == 1 && availableFormats[0].format == VK_FORMAT_UNDEFINED) {
        return {VK_FORMAT_B8G8R8A8_UNORM, VK_COLOR_SPACE_SRGB_NONLINEAR_KHR};
    }

    for (const auto& availableFormat : availableFormats) {
        if (availableFormat.format == VK_FORMAT_B8G8R8A8_UNORM && availableFormat.colorSpace == VK_COLOR_SPACE_SRGB_NONLINEAR_KHR) {
            return availableFormat;
        }
    }

    return availableFormats[0];
}

VkPresentModeKHR application_choose_swap_present_mode(application_t *app, const std::vector<VkPresentModeKHR> availablePresentModes) {
    VkPresentModeKHR bestMode = VK_PRESENT_MODE_FIFO_KHR;

    for (const auto& availablePresentMode : availablePresentModes) {
        if (availablePresentMode == VK_PRESENT_MODE_MAILBOX_KHR) {
            return availablePresentMode;
        } else if (availablePresentMode == VK_PRESENT_MODE_IMMEDIATE_KHR) {
            bestMode = availablePresentMode;
        }
    }

    return bestMode;
}

VkExtent2D application_choose_swap_extent(application_t *app, const VkSurfaceCapabilitiesKHR& capabilities) {
    if (capabilities.currentExtent.width != std::numeric_limits<uint32_t>::max()) {
        return capabilities.currentExtent;
    } else {
        VkExtent2D actualExtent = {app->windowWidth, app->windowHeight};

        actualExtent.width = std::max(capabilities.minImageExtent.width, std::min(capabilities.maxImageExtent.width, actualExtent.width));
        actualExtent.height = std::max(capabilities.minImageExtent.height, std::min(capabilities.maxImageExtent.height, actualExtent.height));

        return actualExtent;
    }
}

void application_create_swap_chain(application_t *app) {
    swapchain_support_t swapChainSupport = application_query_swap_chain_support(app, app->physicalDevice);

    VkSurfaceFormatKHR surfaceFormat = application_choose_swap_surface_format(app, swapChainSupport.formats);
    VkPresentModeKHR presentMode = application_choose_swap_present_mode(app, swapChainSupport.presentModes);
    VkExtent2D extent = application_choose_swap_extent(app, swapChainSupport.capabilities);

    uint32_t imageCount = swapChainSupport.capabilities.minImageCount + 1;
    if (swapChainSupport.capabilities.maxImageCount > 0 && imageCount > swapChainSupport.capabilities.maxImageCount) {
        imageCount = swapChainSupport.capabilities.maxImageCount;
    }

    VkSwapchainCreateInfoKHR createInfo = {};
    createInfo.sType = VK_STRUCTURE_TYPE_SWAPCHAIN_CREATE_INFO_KHR;
    createInfo.surface = app->surface;

    createInfo.minImageCount = imageCount;
    createInfo.imageFormat = surfaceFormat.format;
    createInfo.imageColorSpace = surfaceFormat.colorSpace;
    createInfo.imageExtent = extent;
    createInfo.imageArrayLayers = 1;
    createInfo.imageUsage = VK_IMAGE_USAGE_COLOR_ATTACHMENT_BIT;

    queue_family_t indices = application_find_queue_families(app, app->physicalDevice);
    uint32_t queueFamilyIndices[] = {(uint32_t) indices.graphicsFamily, (uint32_t) indices.presentFamily};

    if (indices.graphicsFamily != indices.presentFamily) {
        createInfo.imageSharingMode = VK_SHARING_MODE_CONCURRENT;
        createInfo.queueFamilyIndexCount = 2;
        createInfo.pQueueFamilyIndices = queueFamilyIndices;
    } else {
        createInfo.imageSharingMode = VK_SHARING_MODE_EXCLUSIVE;
    }

    createInfo.preTransform = swapChainSupport.capabilities.currentTransform;
    createInfo.compositeAlpha = VK_COMPOSITE_ALPHA_OPAQUE_BIT_KHR;
    createInfo.presentMode = presentMode;
    createInfo.clipped = VK_TRUE;

    createInfo.oldSwapchain = VK_NULL_HANDLE;

    if (vkCreateSwapchainKHR(app->device, &createInfo, nullptr, &app->swapChain) != VK_SUCCESS) {
        throw std::runtime_error("failed to create swap chain!");
    }

    vkGetSwapchainImagesKHR(app->device, app->swapChain, &imageCount, nullptr);
    app->swapChainImages.resize(imageCount);
    vkGetSwapchainImagesKHR(app->device, app->swapChain, &imageCount, app->swapChainImages.data());

    app->swapChainImageFormat = surfaceFormat.format;
    app->swapChainExtent = extent;
}

// create image views
void application_create_image_views(application_t *app) {
    app->swapChainImageViews.resize(app->swapChainImages.size());

    for (size_t i = 0; i < app->swapChainImages.size(); i++) {
        VkImageViewCreateInfo createInfo = {};
        createInfo.sType = VK_STRUCTURE_TYPE_IMAGE_VIEW_CREATE_INFO;
        createInfo.image = app->swapChainImages[i];
        createInfo.viewType = VK_IMAGE_VIEW_TYPE_2D;
        createInfo.format = app->swapChainImageFormat;
        createInfo.components.r = VK_COMPONENT_SWIZZLE_IDENTITY;
        createInfo.components.g = VK_COMPONENT_SWIZZLE_IDENTITY;
        createInfo.components.b = VK_COMPONENT_SWIZZLE_IDENTITY;
        createInfo.components.a = VK_COMPONENT_SWIZZLE_IDENTITY;
        createInfo.subresourceRange.aspectMask = VK_IMAGE_ASPECT_COLOR_BIT;
        createInfo.subresourceRange.baseMipLevel = 0;
        createInfo.subresourceRange.levelCount = 1;
        createInfo.subresourceRange.baseArrayLayer = 0;
        createInfo.subresourceRange.layerCount = 1;

        if (vkCreateImageView(app->device, &createInfo, nullptr, &app->swapChainImageViews[i]) != VK_SUCCESS) {
            throw std::runtime_error("failed to create image views!");
        }
    }
}

// find depth format 
VkFormat application_find_supported_format(application_t *app, const std::vector<VkFormat> candidates, VkImageTiling tiling, VkFormatFeatureFlags features) {
    for (VkFormat format: candidates) {
        VkFormatProperties props;
        vkGetPhysicalDeviceFormatProperties(app->physicalDevice, format, &props);

        if (tiling == VK_IMAGE_TILING_LINEAR && (props.linearTilingFeatures & features) == features) {
            return format;
        } else if (tiling == VK_IMAGE_TILING_OPTIMAL && (props.optimalTilingFeatures & features) == features) {
            return format;
        }
    }
    throw std::runtime_error("failed to find supported format!");
}

VkFormat application_find_depth_format(application_t *app) {
    return application_find_supported_format(app,
        {VK_FORMAT_D32_SFLOAT, VK_FORMAT_D32_SFLOAT_S8_UINT, VK_FORMAT_D24_UNORM_S8_UINT},
        VK_IMAGE_TILING_OPTIMAL,
        VK_FORMAT_FEATURE_DEPTH_STENCIL_ATTACHMENT_BIT
    );
}

// create command pool
void application_create_command_pool(application_t *app) {
    queue_family_t queueFamilyIndices = application_find_queue_families(app, app->physicalDevice);

    VkCommandPoolCreateInfo poolInfo = {};
    poolInfo.sType = VK_STRUCTURE_TYPE_COMMAND_POOL_CREATE_INFO;
    poolInfo.queueFamilyIndex = queueFamilyIndices.graphicsFamily;

    if (vkCreateCommandPool(app->device, &poolInfo, nullptr, &app->commandPool) != VK_SUCCESS) {
        throw std::runtime_error("failed to create command pool!");
    }
}

// create depth resources
void application_create_depth_resources(application_t *app) {
    pipeline_attachment_t colorAttachment;
    colorAttachment.image.image = app->swapChainImages[0];
    colorAttachment.image.view = app->swapChainImageViews[0];
    colorAttachment.usage = VK_IMAGE_USAGE_COLOR_ATTACHMENT_BIT;
    colorAttachment.format = app->swapChainImageFormat;
    colorAttachment.layout = VK_IMAGE_LAYOUT_COLOR_ATTACHMENT_OPTIMAL;
    colorAttachment.finalLayout = VK_IMAGE_LAYOUT_PRESENT_SRC_KHR;

    pipeline_attachment_t depthAttachment;
    pipeline_attachment_create(&depthAttachment, app->device, app->physicalDevice, app->swapChainExtent.width, app->swapChainExtent.height, app->depthFormat, VK_IMAGE_USAGE_DEPTH_STENCIL_ATTACHMENT_BIT, app->commandPool, app->graphicsQueue);
    app->depthImage = depthAttachment.image;

    app->attachments.push_back(colorAttachment);
    app->attachments.push_back(depthAttachment);


    pipeline_attachment_t albedo;
    pipeline_attachment_create(&albedo, app->device, app->physicalDevice, app->swapChainExtent.width, app->swapChainExtent.height, VK_FORMAT_R8G8B8A8_UNORM, VK_IMAGE_USAGE_COLOR_ATTACHMENT_BIT, app->commandPool, app->graphicsQueue);

    pipeline_attachment_t normal;
    pipeline_attachment_create(&normal, app->device, app->physicalDevice, app->swapChainExtent.width, app->swapChainExtent.height, VK_FORMAT_R16G16B16A16_SFLOAT, VK_IMAGE_USAGE_COLOR_ATTACHMENT_BIT, app->commandPool, app->graphicsQueue);

    pipeline_attachment_t position;
    pipeline_attachment_create(&position, app->device, app->physicalDevice, app->swapChainExtent.width, app->swapChainExtent.height, VK_FORMAT_R16G16B16A16_SFLOAT, VK_IMAGE_USAGE_COLOR_ATTACHMENT_BIT, app->commandPool, app->graphicsQueue);

    pipeline_attachment_t offscreenDepthAttachment;
    pipeline_attachment_create(&offscreenDepthAttachment, app->device, app->physicalDevice, app->swapChainExtent.width, app->swapChainExtent.height, app->depthFormat, VK_IMAGE_USAGE_DEPTH_STENCIL_ATTACHMENT_BIT, app->commandPool, app->graphicsQueue);

    app->offscreenAttachments.push_back(albedo);
    app->offscreenAttachments.push_back(normal);
    app->offscreenAttachments.push_back(position);
    app->offscreenAttachments.push_back(offscreenDepthAttachment);
}

// create frame buffers
void application_create_frame_buffers(application_t *app) {
    app->swapChainFramebuffers.resize(app->swapChainImageViews.size());

    for (size_t i = 0; i < app->swapChainImageViews.size(); i++) {
        std::array<VkImageView, 2> views = {
            app->swapChainImageViews[i],
            app->depthImage.view
        };

        VkFramebufferCreateInfo framebufferInfo = {};
        framebufferInfo.sType = VK_STRUCTURE_TYPE_FRAMEBUFFER_CREATE_INFO;
        framebufferInfo.renderPass = app->pipeline.renderPass;
        framebufferInfo.attachmentCount = views.size();
        framebufferInfo.pAttachments = views.data();
        framebufferInfo.width = app->swapChainExtent.width;
        framebufferInfo.height = app->swapChainExtent.height;
        framebufferInfo.layers = 1;

        if (vkCreateFramebuffer(app->device, &framebufferInfo, nullptr, &app->swapChainFramebuffers[i]) != VK_SUCCESS) {
            throw std::runtime_error("failed to create framebuffer!");
        }
    }
}

// create command buffers
void application_create_command_buffers(application_t *app) {
    app->commandBuffers.resize(app->swapChainFramebuffers.size());

    VkCommandBufferAllocateInfo allocInfo = {};
    allocInfo.sType = VK_STRUCTURE_TYPE_COMMAND_BUFFER_ALLOCATE_INFO;
    allocInfo.commandPool = app->commandPool;
    allocInfo.level = VK_COMMAND_BUFFER_LEVEL_PRIMARY;
    allocInfo.commandBufferCount = (uint32_t) app->commandBuffers.size();

    if (vkAllocateCommandBuffers(app->device, &allocInfo, app->commandBuffers.data()) != VK_SUCCESS) {
        throw std::runtime_error("failed to allocate command buffers!");
    }

    for (size_t i = 0; i < app->commandBuffers.size(); i++) {
        VkCommandBufferBeginInfo beginInfo = {};
        beginInfo.sType = VK_STRUCTURE_TYPE_COMMAND_BUFFER_BEGIN_INFO;
        beginInfo.flags = VK_COMMAND_BUFFER_USAGE_SIMULTANEOUS_USE_BIT;

        vkBeginCommandBuffer(app->commandBuffers[i], &beginInfo);

        VkRenderPassBeginInfo renderPassInfo = {};
        renderPassInfo.sType = VK_STRUCTURE_TYPE_RENDER_PASS_BEGIN_INFO;
        renderPassInfo.renderPass = app->pipeline.renderPass;
        renderPassInfo.framebuffer = app->swapChainFramebuffers[i];
        renderPassInfo.renderArea.offset = {0, 0};
        renderPassInfo.renderArea.extent = app->swapChainExtent;

        std::array<VkClearValue, 2> clearValues = {};
        clearValues[0].color = {0.0f, 0.0f, 0.0f, 1.0f};
        clearValues[1].depthStencil = {1.0f, 0};

        renderPassInfo.clearValueCount = static_cast<uint32_t>(clearValues.size());
        renderPassInfo.pClearValues = clearValues.data();

        vkCmdBeginRenderPass(app->commandBuffers[i], &renderPassInfo, VK_SUBPASS_CONTENTS_INLINE);

        //pipeline_render(&app->pipeline, app->commandBuffers[i]);
        scene_render(&app->scene, app->commandBuffers[i], app->pipeline.layout, app->pipeline.pipeline, app->pipeline.descriptorSet);

        vkCmdEndRenderPass(app->commandBuffers[i]);

        if (vkEndCommandBuffer(app->commandBuffers[i]) != VK_SUCCESS) {
            throw std::runtime_error("failed to record command buffer!");
        }
    }
}

// create semaphores
void application_create_semaphores(application_t *app) {
    VkSemaphoreCreateInfo semaphoreInfo = {};
    semaphoreInfo.sType = VK_STRUCTURE_TYPE_SEMAPHORE_CREATE_INFO;

    if (vkCreateSemaphore(app->device, &semaphoreInfo, nullptr, &app->imageAvailableSemaphore) != VK_SUCCESS ||
        vkCreateSemaphore(app->device, &semaphoreInfo, nullptr, &app->renderFinishedSemaphore) != VK_SUCCESS) {

        throw std::runtime_error("failed to create semaphores!");
    }
}

auto lastTime = std::chrono::high_resolution_clock::now();
auto startTime = std::chrono::high_resolution_clock::now();
auto initialTime = std::chrono::high_resolution_clock::now();
float total = 0.0f;

void application_update_uniforms(application_t *app)
{
    auto currentTime = std::chrono::high_resolution_clock::now();
    float delta = std::chrono::duration<float, std::chrono::seconds::period>(currentTime - lastTime).count();
    lastTime = std::chrono::high_resolution_clock::now();
    total += delta;

    //pipeline_update(&app->pipeline, delta);
    scene_update(&app->scene, delta);

    camera_update(&app->scene.camera);
    app->ubo.view = app->scene.camera.view;
    app->ubo.proj = app->scene.camera.proj;
}

void application_copy_uniforms(application_t *app)
{
    void *data;
    vkMapMemory(app->device, app->pipeline.uniformBufferMemory, 0, sizeof(app->ubo), 0, &data);
    memcpy(data, &app->ubo, sizeof(app->ubo));
    vkUnmapMemory(app->device, app->pipeline.uniformBufferMemory);
}

// draw frame
void application_draw_frame(application_t *app) {
    uint32_t imageIndex;
    vkAcquireNextImageKHR(app->device, app->swapChain, std::numeric_limits<uint64_t>::max(), app->imageAvailableSemaphore, VK_NULL_HANDLE, &imageIndex);

    VkSubmitInfo submitInfo = {};
    submitInfo.sType = VK_STRUCTURE_TYPE_SUBMIT_INFO;

    VkSemaphore waitSemaphores[] = {app->imageAvailableSemaphore};
    VkPipelineStageFlags waitStages[] = {VK_PIPELINE_STAGE_COLOR_ATTACHMENT_OUTPUT_BIT};
    submitInfo.waitSemaphoreCount = 1;
    submitInfo.pWaitSemaphores = waitSemaphores;
    submitInfo.pWaitDstStageMask = waitStages;

    submitInfo.commandBufferCount = 1;
    submitInfo.pCommandBuffers = &app->commandBuffers[imageIndex];

    VkSemaphore signalSemaphores[] = {app->renderFinishedSemaphore};
    submitInfo.signalSemaphoreCount = 1;
    submitInfo.pSignalSemaphores = signalSemaphores;

    if (vkQueueSubmit(app->graphicsQueue, 1, &submitInfo, VK_NULL_HANDLE) != VK_SUCCESS) {
        throw std::runtime_error("failed to submit draw command buffer!");
    }

    VkPresentInfoKHR presentInfo = {};
    presentInfo.sType = VK_STRUCTURE_TYPE_PRESENT_INFO_KHR;

    presentInfo.waitSemaphoreCount = 1;
    presentInfo.pWaitSemaphores = signalSemaphores;

    VkSwapchainKHR swapChains[] = {app->swapChain};
    presentInfo.swapchainCount = 1;
    presentInfo.pSwapchains = swapChains;

    presentInfo.pImageIndices = &imageIndex;

    vkQueuePresentKHR(app->presentQueue, &presentInfo);

    vkQueueWaitIdle(app->presentQueue);
}

// cleanup swapchain
void application_cleanup_swap_chain(application_t *app) {
    image_cleanup(&app->depthImage, app->device);

    for (std::vector<pipeline_attachment_t>::iterator it = app->offscreenAttachments.begin(); it != app->offscreenAttachments.end(); ++it)
    {
        pipeline_attachment_destroy(&(*it), app->device);
    }

    for (auto framebuffer: app->swapChainFramebuffers) {
        vkDestroyFramebuffer(app->device, framebuffer, nullptr);
    }

    vkFreeCommandBuffers(app->device, app->commandPool, static_cast<uint32_t>(app->commandBuffers.size()), app->commandBuffers.data());

    for (auto imageView : app->swapChainImageViews) {
        vkDestroyImageView(app->device, imageView, nullptr);
    }

    vkDestroySwapchainKHR(app->device, app->swapChain, nullptr);
}

// recreate swapchain
void application_recreate_swap_chain(application_t *app) {
    int width, height; 
    SDL_GetWindowSize(app->window, &width, &height);
    if (width == 0 || height == 0)
        return;
    camera_resize(&app->scene.camera, width, height, 90.0f);

    vkDeviceWaitIdle(app->device);

    application_cleanup_swap_chain(app);

    application_create_swap_chain(app);
    application_create_image_views(app);
    pipeline_recreate(&app->pipeline, app->windowWidth, app->windowHeight, app->device, app->swapChainImageFormat, app->depthFormat);
    application_create_depth_resources(app);
    application_create_frame_buffers(app);
    application_create_command_buffers(app);
}

void application_init_vulkan(application_t *app) {
    application_create_instance(app);
    application_setup_debug_callback(app);
    application_create_surface(app);
    application_pick_physical_device(app);
    application_create_logical_device(app);
    application_create_swap_chain(app);
    application_create_image_views(app);

    application_create_command_pool(app);

    texture_add(&app->pipeline.texture, app->device, app->physicalDevice, app->commandPool, app->graphicsQueue, "default.png");
    texture_add(&app->pipeline.texture, app->device, app->physicalDevice, app->commandPool, app->graphicsQueue, "normal.png");
    app->depthFormat = application_find_depth_format(app);
    application_create_depth_resources(app);
    pipeline_create(&app->pipeline, app->windowWidth, app->windowHeight, "shaders/vert.spv", "shaders/frag.spv", app->device, app->physicalDevice, app->swapChainImageFormat, app->depthFormat, app->commandPool, app->graphicsQueue, app->attachments);
    scene_create(&app->scene, app->device, app->physicalDevice, app->commandPool, app->graphicsQueue);

    application_create_frame_buffers(app);
    application_update_uniforms(app);
    application_copy_uniforms(app);
    application_create_command_buffers(app);
    application_create_semaphores(app);
}

void application_main_loop(application_t *app) {
    //while (!glfwWindowShouldClose(app->window)) {
    scene_load(&app->scene, "scene_0.lua");
    bool running = true;
    while (running) {
        int x = 0;
        int y = 0;
        std::set<std::string> downs;
        std::set<std::string> ups;
        SDL_Event event;
        while (SDL_PollEvent(&event))
        {
            switch (event.type)
            {
            case SDL_MOUSEMOTION:
                x = event.motion.xrel;
                y = event.motion.yrel;
                break;
            case SDL_KEYDOWN:
                if (event.key.repeat)
                    continue;
                downs.insert(SDL_GetScancodeName(event.key.keysym.scancode));
                break;
            case SDL_KEYUP:
                if (event.key.repeat)
                    continue;
                ups.insert(SDL_GetScancodeName(event.key.keysym.scancode));
                break;
            case SDL_QUIT:
                running = false;
                break;
            default:
                break;
            }
        }

        if (x != 0 || y != 0)
            scene_on_cursor_pos(&app->scene, (double)x, (double)y);
            //pipeline_on_cursor_pos(&app->pipeline, (double)x, (double)y);

        for (auto it = downs.begin(); it != downs.end(); ++it)
            scene_on_button_down(&app->scene, *it);
            //pipeline_on_button_down(&app->pipeline, *it);
        for (auto it = ups.begin(); it != ups.end(); ++it)
            scene_on_button_up(&app->scene, *it);
            //pipeline_on_button_up(&app->pipeline, *it);

        application_update_uniforms(app);
        application_copy_uniforms(app);

        if (app->scene.isDirty)
        {
            std::cout << "making render" << std::endl;
            application_create_command_buffers(app);
        }

        application_draw_frame(app);
    }
    //script_destroy(&script);

    vkDeviceWaitIdle(app->device);
}

void application_cleanup(application_t *app) {
    application_cleanup_swap_chain(app);

    pipeline_cleanup(&app->pipeline, app->device);
    texture_cleanup(&app->pipeline.texture, app->device);
    scene_cleanup(&app->scene);
    //model_cleanup(&app->model, app->device);

    vkDestroySemaphore(app->device, app->renderFinishedSemaphore, nullptr);
    vkDestroySemaphore(app->device, app->imageAvailableSemaphore, nullptr);

    vkDestroyCommandPool(app->device, app->commandPool, nullptr);

    vkDestroyDevice(app->device, nullptr);
    DestroyDebugReportCallbackEXT(app->instance, app->callback, nullptr);
    vkDestroySurfaceKHR(app->instance, app->surface, nullptr);
    vkDestroyInstance(app->instance, nullptr);

    //glfwDestroyWindow(app->window);
    SDL_DestroyWindow(app->window);
    SDL_Quit();
}

void application_run(application_t *app) {
    application_init_window(app);
    application_init_vulkan(app);
    application_main_loop(app);
    application_cleanup(app);
}

