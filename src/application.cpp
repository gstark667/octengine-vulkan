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
    SDL_Init(SDL_INIT_VIDEO | SDL_INIT_JOYSTICK);
    app->window = SDL_CreateWindow(
        "octengine",
        SDL_WINDOWPOS_UNDEFINED,
        SDL_WINDOWPOS_UNDEFINED,
        app->windowWidth,
        app->windowHeight,
        (app->fullscreen ? SDL_WINDOW_FULLSCREEN | SDL_WINDOW_VULKAN : SDL_WINDOW_VULKAN)
    );

    // setup mouse
    SDL_SetWindowGrab(app->window, SDL_TRUE);
    SDL_SetRelativeMouseMode(SDL_TRUE);

    // setup joystick
    if (SDL_NumJoysticks() != 0)
    {
        app->gameController = SDL_JoystickOpen(0);
        if(app->gameController == NULL)
        {
             std::cout << "Unable to open joystick: " << SDL_GetError();
        }
    }
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

void application_get_usable_samples(application_t *app) {
    VkPhysicalDeviceProperties physicalDeviceProperties;
    vkGetPhysicalDeviceProperties(app->physicalDevice, &physicalDeviceProperties);

    VkSampleCountFlags counts = std::min(physicalDeviceProperties.limits.framebufferColorSampleCounts, physicalDeviceProperties.limits.framebufferDepthSampleCounts);
    if (counts & VK_SAMPLE_COUNT_2_BIT) { app->sampleCount = VK_SAMPLE_COUNT_2_BIT; }
    if (counts & VK_SAMPLE_COUNT_4_BIT) { app->sampleCount = VK_SAMPLE_COUNT_4_BIT; }
    if (counts & VK_SAMPLE_COUNT_8_BIT) { app->sampleCount = VK_SAMPLE_COUNT_8_BIT; }
    if (counts & VK_SAMPLE_COUNT_16_BIT) { app->sampleCount = VK_SAMPLE_COUNT_16_BIT; }
    if (counts & VK_SAMPLE_COUNT_32_BIT) { app->sampleCount = VK_SAMPLE_COUNT_32_BIT; }
    if (counts & VK_SAMPLE_COUNT_64_BIT) { app->sampleCount = VK_SAMPLE_COUNT_64_BIT; }
    app->sampleCount = VK_SAMPLE_COUNT_4_BIT;
    std::cout << "sample count: " << app->sampleCount << std::endl;
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

    application_get_usable_samples(app);
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
    deviceFeatures.samplerAnisotropy = VK_TRUE;
    deviceFeatures.sampleRateShading = VK_TRUE;

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
    colorAttachment.image.samples = VK_SAMPLE_COUNT_1_BIT;
    colorAttachment.usage = VK_IMAGE_USAGE_COLOR_ATTACHMENT_BIT;
    colorAttachment.format = app->swapChainImageFormat;
    colorAttachment.layout = VK_IMAGE_LAYOUT_COLOR_ATTACHMENT_OPTIMAL;
    colorAttachment.finalLayout = VK_IMAGE_LAYOUT_PRESENT_SRC_KHR;

    pipeline_attachment_t depthAttachment;
    pipeline_attachment_create(&depthAttachment, app->device, app->physicalDevice, app->swapChainExtent.width, app->swapChainExtent.height, VK_SAMPLE_COUNT_1_BIT, app->depthFormat, VK_IMAGE_USAGE_DEPTH_STENCIL_ATTACHMENT_BIT, app->commandPool, app->graphicsQueue, false);
    app->depthImage = depthAttachment.image;

    app->attachments.push_back(colorAttachment);
    app->attachments.push_back(depthAttachment);


    pipeline_attachment_create(&app->albedo, app->device, app->physicalDevice, app->swapChainExtent.width, app->swapChainExtent.height, app->sampleCount, VK_FORMAT_R8G8B8A8_UNORM, VK_IMAGE_USAGE_COLOR_ATTACHMENT_BIT, app->commandPool, app->graphicsQueue, false);

    pipeline_attachment_create(&app->normal, app->device, app->physicalDevice, app->swapChainExtent.width, app->swapChainExtent.height, app->sampleCount, VK_FORMAT_R16G16B16A16_SFLOAT, VK_IMAGE_USAGE_COLOR_ATTACHMENT_BIT, app->commandPool, app->graphicsQueue, false);

    pipeline_attachment_create(&app->position, app->device, app->physicalDevice, app->swapChainExtent.width, app->swapChainExtent.height, app->sampleCount, VK_FORMAT_R32G32B32A32_SFLOAT, VK_IMAGE_USAGE_COLOR_ATTACHMENT_BIT, app->commandPool, app->graphicsQueue, false);

    pipeline_attachment_create(&app->pbr, app->device, app->physicalDevice, app->swapChainExtent.width, app->swapChainExtent.height, app->sampleCount, VK_FORMAT_R16G16B16A16_SFLOAT, VK_IMAGE_USAGE_COLOR_ATTACHMENT_BIT, app->commandPool, app->graphicsQueue, false);

    pipeline_attachment_create(&app->offscreenDepthAttachment, app->device, app->physicalDevice, app->swapChainExtent.width, app->swapChainExtent.height, app->sampleCount, app->depthFormat, VK_IMAGE_USAGE_DEPTH_STENCIL_ATTACHMENT_BIT, app->commandPool, app->graphicsQueue, false);

    app->offscreenAttachments.push_back(app->albedo);
    app->offscreenAttachments.push_back(app->normal);
    app->offscreenAttachments.push_back(app->position);
    app->offscreenAttachments.push_back(app->pbr);
    app->offscreenAttachments.push_back(app->offscreenDepthAttachment);

    app->shadowImageArray = new image_t();
    app->shadowImageArray->forceArray = true;
    image_create(app->shadowImageArray, app->device, app->physicalDevice, app->shadowWidth, app->shadowHeight, 1, 1, app->depthFormat, VK_IMAGE_TILING_OPTIMAL, VK_IMAGE_USAGE_DEPTH_STENCIL_ATTACHMENT_BIT | VK_IMAGE_USAGE_SAMPLED_BIT, VK_MEMORY_PROPERTY_DEVICE_LOCAL_BIT, VK_SAMPLE_COUNT_1_BIT);
    image_create_view(app->shadowImageArray, app->device, app->depthFormat, VK_IMAGE_ASPECT_DEPTH_BIT, 0, true);
    /*pipeline_attachment_from_image(&app->shadowDepth1, app->device, VK_IMAGE_ASPECT_DEPTH_BIT, *app->shadowImageArray, 0, true);
    app->shadowAttachments1.push_back(app->shadowDepth1);

    pipeline_attachment_from_image(&app->shadowDepth2, app->device, VK_IMAGE_ASPECT_DEPTH_BIT, *app->shadowImageArray, 1, true);
    app->shadowAttachments2.push_back(app->shadowDepth2);*/
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

void application_draw_quad(application_t *app, VkCommandBuffer commandBuffer, VkPipeline pipeline)
{
}

// create command buffers
void application_create_command_buffers(application_t *app) {
    std::cout << "creating command buffers" << std::endl;
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
        app->pipeline.framebuffer = app->swapChainFramebuffers[i];
        pipeline_begin_render(&app->pipeline, app->commandBuffers[i]);
        model_render(&app->quad, app->commandBuffers[i], app->pipeline.layout, app->pipeline.pipeline, app->descriptorSet.descriptorSet);
        pipeline_end_render(&app->pipeline, app->commandBuffers[i]);
    }

    // create the offscreen command buffer
    allocInfo.commandBufferCount = 1;

    if (vkAllocateCommandBuffers(app->device, &allocInfo, &app->offscreenCommandBuffer) != VK_SUCCESS) {
        throw std::runtime_error("failed to allocate command buffers!");
    }

    pipeline_begin_render(&app->offscreenPipeline, app->offscreenCommandBuffer);
    scene_render(&app->scene, app->offscreenCommandBuffer, app->offscreenPipeline.layout, app->offscreenPipeline.pipeline, &app->offscreenDescriptorSet);
    pipeline_end_render(&app->offscreenPipeline, app->offscreenCommandBuffer);

    // create the shadow command buffers
    for (size_t idx = 0; idx < app->shadowPipelines.size(); ++idx)
    {
        std::cout << "creating shadow command buffer" << std::endl;
        allocInfo.commandBufferCount = 1;

        if (vkAllocateCommandBuffers(app->device, &allocInfo, app->shadowCommandBuffers[idx]) != VK_SUCCESS) {
            throw std::runtime_error("failed to allocate shadow command buffers!");
        }

        pipeline_begin_render(app->shadowPipelines[idx], *app->shadowCommandBuffers[idx]);
        scene_render(&app->scene, *app->shadowCommandBuffers[idx], app->shadowPipelines[idx]->layout, app->shadowPipelines[idx]->pipeline, app->shadowDescriptorSets[idx]);
        pipeline_end_render(app->shadowPipelines[idx], *app->shadowCommandBuffers[idx]);
    }
}

// create semaphores
void application_create_semaphores(application_t *app) {
    VkSemaphoreCreateInfo semaphoreInfo = {};
    semaphoreInfo.sType = VK_STRUCTURE_TYPE_SEMAPHORE_CREATE_INFO;

    if (vkCreateSemaphore(app->device, &semaphoreInfo, nullptr, &app->imageAvailableSemaphore) != VK_SUCCESS ||
        vkCreateSemaphore(app->device, &semaphoreInfo, nullptr, &app->offscreenSemaphore) != VK_SUCCESS ||
        vkCreateSemaphore(app->device, &semaphoreInfo, nullptr, &app->renderFinishedSemaphore) != VK_SUCCESS) {

        throw std::runtime_error("failed to create semaphores!");
    }
}

auto lastTime = std::chrono::high_resolution_clock::now();
auto startTime = std::chrono::high_resolution_clock::now();
auto initialTime = std::chrono::high_resolution_clock::now();
float total = 0.0f;

void application_add_shadow_pipelines(application_t *app)
{
    if (!app->shadowImageArray)
    {
        app->shadowImageArray = new image_t();
        app->shadowImageArray->forceArray = true;
        app->shadowImageArray->layers = 0;
    }

    while (app->shadowSemaphores.size() < app->scene.lights.size())
    {
        std::cout << "adding semaphore" << std::endl;
        VkSemaphoreCreateInfo semaphoreInfo = {};
        semaphoreInfo.sType = VK_STRUCTURE_TYPE_SEMAPHORE_CREATE_INFO;
        app->shadowSemaphores.push_back(new VkSemaphore());
        if (vkCreateSemaphore(app->device, &semaphoreInfo, nullptr, app->shadowSemaphores.back()) != VK_SUCCESS)
            throw std::runtime_error("unable to create shadow semaphore!");
    }

    if (app->shadowImageArray->layers < app->scene.lights.size())
    {
        image_cleanup(app->shadowImageArray, app->device);
        image_create(app->shadowImageArray, app->device, app->physicalDevice, app->shadowWidth, app->shadowHeight, app->scene.lights.size(), 1, app->depthFormat, VK_IMAGE_TILING_OPTIMAL, VK_IMAGE_USAGE_DEPTH_STENCIL_ATTACHMENT_BIT | VK_IMAGE_USAGE_SAMPLED_BIT, VK_MEMORY_PROPERTY_DEVICE_LOCAL_BIT, VK_SAMPLE_COUNT_1_BIT);
        image_create_view(app->shadowImageArray, app->device, app->depthFormat, VK_IMAGE_ASPECT_DEPTH_BIT, 0, true);
        descriptor_set_update_image(&app->descriptorSet, app->shadowImageArray, 7);
    }

    while (app->shadowPipelines.size() < app->scene.lights.size())
    {
        std::cout << "adding pipeline" << std::endl;
        size_t idx = app->shadowPipelines.size();
        app->shadowPipelines.push_back(new pipeline_t());
        app->shadowDescriptorSets.push_back(new descriptor_set_t());
        app->shadowCommandBuffers.push_back(new VkCommandBuffer());
        app->shadowAttachments.push_back({});

        pipeline_attachment_t attachment;
        pipeline_attachment_from_image(&attachment, app->device, VK_IMAGE_ASPECT_DEPTH_BIT, *app->shadowImageArray, idx, true);
        app->shadowAttachments[idx].push_back(attachment);

        descriptor_set_setup(app->shadowDescriptorSets[idx], app->device, app->physicalDevice);
        descriptor_set_add_buffer(app->shadowDescriptorSets[idx], sizeof(uniform_buffer_object_t), 0, true);
        descriptor_set_add_buffer(app->shadowDescriptorSets[idx], sizeof(bone_ubo_t), 1, true);
        descriptor_set_add_texture(app->shadowDescriptorSets[idx], &app->scene.textures, 2, false);
        descriptor_set_create(app->shadowDescriptorSets[idx]);
        app->shadowPipelines[idx]->cullBack = true;
        pipeline_create(app->shadowPipelines[idx], app->shadowDescriptorSets[idx], app->shadowWidth, app->shadowHeight, "shaders/shadow_vert.spv", "shaders/shadow_frag.spv", app->device, app->physicalDevice, VK_SAMPLE_COUNT_1_BIT, app->commandPool, app->graphicsQueue, app->shadowAttachments[idx], true, true);
    }
}

void application_update_uniforms(application_t *app)
{
    auto currentTime = std::chrono::high_resolution_clock::now();
    float delta = std::chrono::duration<float, std::chrono::seconds::period>(currentTime - lastTime).count();
    lastTime = std::chrono::high_resolution_clock::now();
    total += delta;

    scene_update(&app->scene, delta);

    if (app->shadowPipelines.size() < app->scene.lights.size())
    {
        application_add_shadow_pipelines(app);
    }

    app->lightUBO.lightCount = app->scene.lights.size();
    size_t i = 0;
    for (auto it = app->scene.lights.begin(); it != app->scene.lights.end(); ++it)
    {
        camera_update(&(*it)->camera);
        app->lightUBO.lights[i].position = glm::vec4((*it)->camera.object->globalPos, 1.0f);
        app->lightUBO.lights[i].direction = glm::vec4(gameobject_rotate_vector((*it)->camera.object, glm::vec3(0, 0, 1)), 1.0f);
        app->lightUBO.lights[i].color = glm::vec4((*it)->color, 1.0f);
        app->lightUBO.lights[i].mvp = (*it)->camera.proj * (*it)->camera.view;

        app->ubo.cameraMVP = app->lightUBO.lights[i].mvp;
        descriptor_set_update_buffer(app->shadowDescriptorSets[i], &app->ubo, 0);
        descriptor_set_update_buffer(app->shadowDescriptorSets[i], &app->scene.bones, 1);
        ++i;
    }

    if (app->scene.camera != NULL)
    {
        camera_update(app->scene.camera);
        app->ubo.cameraMVP = app->scene.camera->proj * app->scene.camera->view;
        app->lightUBO.cameraPos = glm::vec4(app->scene.camera->object->globalPos * 0.5f, 1.0f);
    }
}

void application_copy_uniforms(application_t *app)
{
    if (app->scene.textures.needsUpdate)
    {
        app->scene.textures.needsUpdate = false;
        descriptor_set_update_texture(&app->offscreenDescriptorSet, &app->scene.textures, 2);
    }

    descriptor_set_update_buffer(&app->descriptorSet, &app->lightUBO, 0);

    descriptor_set_update_buffer(&app->offscreenDescriptorSet, &app->ubo, 0);
    descriptor_set_update_buffer(&app->offscreenDescriptorSet, &app->scene.bones, 1);

    //app->ubo.cameraMVP = app->lightUBO.lights[1].mvp;
    //descriptor_set_update_buffer(&app->shadowDescriptorSet2, &app->ubo, 0);
}

// draw frame
void application_draw_frame(application_t *app) {
    uint32_t imageIndex;
    vkAcquireNextImageKHR(app->device, app->swapChain, std::numeric_limits<uint64_t>::max(), app->imageAvailableSemaphore, VK_NULL_HANDLE, &imageIndex);

    VkSubmitInfo submitInfo = {};
    submitInfo.sType = VK_STRUCTURE_TYPE_SUBMIT_INFO;

    // offscreen
    VkPipelineStageFlags waitStages[] = {VK_PIPELINE_STAGE_COLOR_ATTACHMENT_OUTPUT_BIT};
    submitInfo.waitSemaphoreCount = 1;
    submitInfo.pWaitSemaphores = &app->imageAvailableSemaphore;
    submitInfo.pWaitDstStageMask = waitStages;
    submitInfo.commandBufferCount = 1;
    submitInfo.pCommandBuffers = &app->offscreenCommandBuffer;

    VkSemaphore signalSemaphores[] = {app->offscreenSemaphore};
    submitInfo.signalSemaphoreCount = 1;
    submitInfo.pSignalSemaphores = signalSemaphores;

    if (vkQueueSubmit(app->graphicsQueue, 1, &submitInfo, VK_NULL_HANDLE) != VK_SUCCESS) {
        throw std::runtime_error("failed to submit draw command buffer!");
    }

    // shadows
    VkSemaphore *lastSemaphore = &app->offscreenSemaphore;
    for (size_t i = 0; i < app->scene.lights.size(); ++i)
    {
        submitInfo.waitSemaphoreCount = 1;
        submitInfo.pWaitSemaphores = lastSemaphore;
        submitInfo.pWaitDstStageMask = waitStages;
        submitInfo.commandBufferCount = 1;
        submitInfo.pCommandBuffers = app->shadowCommandBuffers[i];

        lastSemaphore = app->shadowSemaphores[i];
        signalSemaphores[0] = *app->shadowSemaphores[i];
        submitInfo.signalSemaphoreCount = 1;
        submitInfo.pSignalSemaphores = signalSemaphores;

        if (vkQueueSubmit(app->graphicsQueue, 1, &submitInfo, VK_NULL_HANDLE) != VK_SUCCESS) {
            throw std::runtime_error("failed to submit draw command buffer!");
        }
    }

    // onscreen
    submitInfo.waitSemaphoreCount = 1;
    submitInfo.pWaitSemaphores = lastSemaphore;
    submitInfo.pWaitDstStageMask = waitStages;
    submitInfo.commandBufferCount = 1;
    submitInfo.pCommandBuffers = &app->commandBuffers[imageIndex];

    signalSemaphores[0] = app->renderFinishedSemaphore;
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
    std::cout << "cleanup swap chain" << std::endl;
    image_cleanup(&app->depthImage, app->device);

    for (std::vector<pipeline_attachment_t>::iterator it = app->offscreenAttachments.begin(); it != app->offscreenAttachments.end(); ++it)
    {
        pipeline_attachment_cleanup(&(*it), app->device);
    }

    for (std::vector<pipeline_attachment_t>::iterator it = app->shadowAttachments1.begin(); it != app->shadowAttachments1.end(); ++it)
    {
        pipeline_attachment_cleanup(&(*it), app->device);
    }

    for (std::vector<pipeline_attachment_t>::iterator it = app->shadowAttachments2.begin(); it != app->shadowAttachments2.end(); ++it)
    {
        pipeline_attachment_cleanup(&(*it), app->device);
    }

    for (auto it = app->shadowAttachments.begin(); it != app->shadowAttachments.end(); ++it)
    {
        for (auto it2 = it->begin(); it2 != it->end(); ++it2)
        {
             pipeline_attachment_cleanup(&(*it2), app->device);
        }
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
    std::cout << "recreate swap chain" << std::endl;
    int width, height; 
    SDL_GetWindowSize(app->window, &width, &height);
    if (width == 0 || height == 0)
        return;

    vkDeviceWaitIdle(app->device);

    application_cleanup_swap_chain(app);

    application_create_swap_chain(app);
    application_create_image_views(app);
    pipeline_recreate(&app->pipeline, app->windowWidth, app->windowHeight, app->device);
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

    app->depthFormat = application_find_depth_format(app);
    application_create_depth_resources(app);

    scene_create(&app->scene, app->device, app->physicalDevice, app->commandPool, app->graphicsQueue, app->windowWidth, app->windowHeight);

    descriptor_set_setup(&app->descriptorSet, app->device, app->physicalDevice);
    descriptor_set_add_buffer(&app->descriptorSet, sizeof(light_ubo_t), 0, false);
    descriptor_set_add_buffer(&app->descriptorSet, sizeof(render_ubo_t), 1, false);
    descriptor_set_add_image(&app->descriptorSet, &app->albedo.image, 2, false, false, false);
    descriptor_set_add_image(&app->descriptorSet, &app->normal.image, 3, false, false, false);
    descriptor_set_add_image(&app->descriptorSet, &app->position.image, 4, false, false, false);
    descriptor_set_add_image(&app->descriptorSet, &app->pbr.image, 5, false, false, false);
    descriptor_set_add_image(&app->descriptorSet, &app->offscreenDepthAttachment.image, 6, false, false, false);
    descriptor_set_add_image(&app->descriptorSet, app->shadowImageArray, 7, false, false, true);
    descriptor_set_create(&app->descriptorSet);
    pipeline_create(&app->pipeline, &app->descriptorSet, app->windowWidth, app->windowHeight, "shaders/screen_vert.spv", "shaders/screen_frag.spv", app->device, app->physicalDevice, VK_SAMPLE_COUNT_1_BIT, app->commandPool, app->graphicsQueue, app->attachments, false, false);
    app->renderUBO.sampleCount = app->sampleCount;
    app->renderUBO.ambient = glm::vec4(0.2f, 0.2f, 0.2f, 1.0f);
    descriptor_set_update_buffer(&app->descriptorSet, &app->renderUBO, 1);

    descriptor_set_setup(&app->offscreenDescriptorSet, app->device, app->physicalDevice);
    descriptor_set_add_buffer(&app->offscreenDescriptorSet, sizeof(uniform_buffer_object_t), 0, true);
    descriptor_set_add_buffer(&app->offscreenDescriptorSet, sizeof(bone_ubo_t), 1, true);
    descriptor_set_add_texture(&app->offscreenDescriptorSet, &app->scene.textures, 2, false);
    descriptor_set_create(&app->offscreenDescriptorSet);
    pipeline_create(&app->offscreenPipeline, &app->offscreenDescriptorSet, app->windowWidth, app->windowHeight, "shaders/offscreen_vert.spv", "shaders/offscreen_frag.spv", app->device, app->physicalDevice, app->sampleCount, app->commandPool, app->graphicsQueue, app->offscreenAttachments, true, false);

    application_create_frame_buffers(app);
    application_update_uniforms(app);
    application_copy_uniforms(app);

    app->quad.instances.push_back({});
    model_load(&app->quad, "quad.dae");
    model_create_buffers(&app->quad, app->device, app->physicalDevice, app->commandPool, app->graphicsQueue);
    application_create_command_buffers(app);
    application_create_semaphores(app);
}

void application_main_loop(application_t *app) {
    scene_load(&app->scene, "scene_0.lua");
    bool running = true;
    while (running) {
        int x = 0;
        int y = 0;
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
            case SDL_KEYUP:
                if (event.key.repeat)
                    continue;
                settings_on_button(&app->settings, SDL_GetScancodeName(event.key.keysym.scancode), event.type == SDL_KEYDOWN);
                break;
            case SDL_JOYAXISMOTION:
                settings_on_axis(&app->settings, "JA_" + std::to_string(event.jaxis.axis), ((float)event.jaxis.value) / 32767.0);
                break;
            case SDL_JOYBUTTONDOWN:
            case SDL_JOYBUTTONUP:
                settings_on_button(&app->settings, "JB_" + std::to_string(event.jbutton.button), event.jbutton.state == SDL_PRESSED);
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
        for (auto it = app->settings.triggeredEvents.begin(); it != app->settings.triggeredEvents.end(); ++it)
        {
            scene_on_event(&app->scene, {*it, app->settings.events[*it]});
        }
        app->settings.triggeredEvents.clear();

        application_update_uniforms(app);
        application_copy_uniforms(app);

        if (app->scene.isDirty)
        {
            app->scene.isDirty = false;
            std::cout << "making render" << std::endl;
            application_create_command_buffers(app);
        }

        application_draw_frame(app);
    }
    vkDeviceWaitIdle(app->device);
}

void application_cleanup(application_t *app) {
    application_cleanup_swap_chain(app);

    pipeline_cleanup(&app->pipeline);
    pipeline_cleanup(&app->offscreenPipeline);
    for (auto it = app->shadowPipelines.begin(); it != app->shadowPipelines.end(); ++it)
    {
        pipeline_cleanup((*it));
        delete *it;
    }

    scene_cleanup(&app->scene);
    descriptor_set_cleanup(&app->descriptorSet);
    descriptor_set_cleanup(&app->offscreenDescriptorSet);
    for (auto it = app->shadowDescriptorSets.begin(); it != app->shadowDescriptorSets.end(); ++it)
    {
        descriptor_set_cleanup((*it));
        delete *it;
    }
    model_cleanup(&app->quad, app->device);
    if (app->shadowImageArray)
        image_cleanup(app->shadowImageArray, app->device);

    vkDestroySemaphore(app->device, app->renderFinishedSemaphore, nullptr);
    vkDestroySemaphore(app->device, app->offscreenSemaphore, nullptr);
    vkDestroySemaphore(app->device, app->imageAvailableSemaphore, nullptr);
    for (auto it = app->shadowSemaphores.begin(); it != app->shadowSemaphores.end(); ++it)
        vkDestroySemaphore(app->device, *(*it), nullptr);

    vkDestroyCommandPool(app->device, app->commandPool, nullptr);

    vkDestroyDevice(app->device, nullptr);
    DestroyDebugReportCallbackEXT(app->instance, app->callback, nullptr);
    vkDestroySurfaceKHR(app->instance, app->surface, nullptr);
    vkDestroyInstance(app->instance, nullptr);

    SDL_DestroyWindow(app->window);
    SDL_Quit();
}

void application_run(application_t *app) {
    application_init_window(app);
    settings_create(&app->settings);
    application_init_vulkan(app);
    application_main_loop(app);
    application_cleanup(app);
}

