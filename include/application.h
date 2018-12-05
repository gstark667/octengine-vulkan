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
#include "settings.h"
#include "descriptorset.h"
#include "ui.h"

#include "export.h"


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

    bool fullscreen = false;
    uint32_t windowWidth;
    uint32_t windowHeight;

    uint32_t shadowHeight = 4096;
    uint32_t shadowWidth = 4096;

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
    VkSampleCountFlagBits sampleCount = VK_SAMPLE_COUNT_1_BIT;

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
    image_t resolveImage;

    pipeline_t pipeline;
    pipeline_t offscreenPipeline;
    pipeline_t skyPipeline;
    pipeline_t blurHPipeline;
    pipeline_t postPipeline;
    pipeline_t uiPipeline;

    model_t model;
    texture_t texture;
    texture_t skybox, illumination;
    scene_t scene;
    camera_t skyCam;
    uniform_buffer_object_t ubo, skyUbo;
    light_ubo_t lightUBO;
    render_ubo_t renderUBO;

    VkCommandPool commandPool;
    std::vector<VkCommandBuffer> commandBuffers;
    VkCommandBuffer commandBuffer;
    VkCommandBuffer offscreenCommandBuffer;
    VkCommandBuffer skyCommandBuffer;
    VkCommandBuffer blurHCommandBuffer;
    VkCommandBuffer postCommandBuffer;
    std::vector<VkCommandBuffer*> shadowCommandBuffers;

    pipeline_attachment_t colorAttachment, brightAttachment, depthAttachment, albedo, normal, position, pbr, offscreenDepthAttachment, shadowPosition, sky, blurH, blurV, postColor, uiColor, uiDepth;
    std::vector<pipeline_attachment_t*> attachments;
    std::vector<pipeline_attachment_t*> offscreenAttachments;
    std::vector<pipeline_attachment_t*> skyAttachments;
    std::vector<pipeline_attachment_t*> blurHAttachments;
    std::vector<pipeline_attachment_t*> blurVAttachments;
    std::vector<pipeline_attachment_t*> postAttachments;
    std::vector<pipeline_attachment_t*> uiAttachments;

    image_t *shadowImageArray = NULL;
    std::vector<pipeline_t*> shadowPipelines;
    std::vector<std::vector<pipeline_attachment_t*>> shadowAttachments;

    descriptor_set_t descriptorSet;
    descriptor_set_t offscreenDescriptorSet;
    descriptor_set_t skyDescriptorSet;
    descriptor_set_t blurHDescriptorSet;
    descriptor_set_t postDescriptorSet;
    descriptor_set_t uiDescriptorSet;
    std::vector<descriptor_set_t*> shadowDescriptorSets;

    VkSemaphore imageAvailableSemaphore;
    VkSemaphore offscreenSemaphore;
    VkSemaphore skySemaphore;
    VkSemaphore uiSemaphore;
    VkSemaphore blurHSemaphore;
    VkSemaphore postSemaphore;
    VkSemaphore renderFinishedSemaphore;
    std::vector<VkSemaphore*> shadowSemaphores;

    camera_t shadowCam;

    model_t quad, cube;

    settings_t settings;
    SDL_Joystick *gameController;
};

extern "C" {
void EXPORT application_recreate_swap_chain(application_t *app);
void EXPORT application_run(application_t *app);
}

#endif
