#ifndef H_APPLICATION
#define H_APPLICATION

#include <SDL2/SDL.h>
#include <SDL2/SDL_vulkan.h>
#include <vulkan/vulkan.h>
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>

#include <vector>
#include <stdexcept>
#include <iostream>
#include <set>
#include <array>

#include "util.h"
#include "shader.h"
#include "pipeline.h"
#include "model.h"
#include "image.h"
#include "texture.h"
#include "scene.h"
#include "script.h"
#include "descriptorset.h"


struct queue_family_t
{
    int graphicsFamily = -1;
    int presentFamily = -1;
};

struct swapchain_support_t
{
    VkSurfaceCapabilitiesKHR capabilities;
    std::vector<VkSurfaceFormatKHR> formats;
    std::vector<VkPresentModeKHR> presentModes;
};

struct application_t
{
    bool enableValidationLayers;
    const std::vector<const char*> validationLayers = {
        "VK_LAYER_LUNARG_standard_validation"
    };

    //GLFWwindow* window;
    uint32_t windowWidth;
    uint32_t windowHeight;

    SDL_Window *window;

    double mouseX = 0.0;
    double mouseY = 0.0;

    std::string name;

    VkInstance instance;
    VkDebugReportCallbackEXT callback;
    VkSurfaceKHR surface;

    const std::vector<const char*> deviceExtensions = {
        VK_KHR_SWAPCHAIN_EXTENSION_NAME
    };

    VkPhysicalDevice physicalDevice = VK_NULL_HANDLE;
    VkDevice device;

    VkQueue graphicsQueue;
    VkQueue presentQueue;

    VkSwapchainKHR swapChain;
    std::vector<VkImage> swapChainImages;
    VkFormat swapChainImageFormat;
    VkExtent2D swapChainExtent;
    std::vector<VkImageView> swapChainImageViews;
    std::vector<VkFramebuffer> swapChainFramebuffers;

    VkFormat depthFormat;
    image_t depthImage;

    pipeline_t pipeline;
    pipeline_t offscreenPipeline;
    pipeline_t shadowPipeline;

    model_t model;
    texture_t texture;
    scene_t scene;
    uniform_buffer_object_t ubo;

    VkCommandPool commandPool;
    std::vector<VkCommandBuffer> commandBuffers;
    VkCommandBuffer offscreenCommandBuffer;
    VkCommandBuffer shadowCommandBuffer;

    pipeline_attachment_t albedo, normal, position, offscreenDepthAttachment, shadowPosition, shadowDepth;
    std::vector<pipeline_attachment_t> attachments;
    std::vector<pipeline_attachment_t> offscreenAttachments;
    std::vector<pipeline_attachment_t> shadowAttachments;

    descriptor_set_t descriptorSet;
    descriptor_set_t offscreenDescriptorSet;
    descriptor_set_t shadowDescriptorSet;

    VkSemaphore imageAvailableSemaphore;
    VkSemaphore offscreenSemaphore;
    VkSemaphore shadowSemaphore;
    VkSemaphore renderFinishedSemaphore;

    model_t quad;
};

void application_recreate_swap_chain(application_t *app);
void application_run(application_t *app);

#endif
