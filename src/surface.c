#include "surface.h"

#include "window.h"
#include "instance.h"
#include "physicalDevice.h"

#include "logger.h"

uint32_t surfaceInstanceExtensionCount = 0;
char const **surfaceInstanceExtensions = nullptr;

uint32_t surfaceDeviceExtensionCount = 0;
char const **surfaceDeviceExtensions = nullptr;

void *surfaceDeviceFeatures = nullptr;

VkSurfaceKHR surface;

VkPresentModeKHR presentMode;
VkSurfaceFormatKHR surfaceFormat;

VkSurfaceCapabilitiesKHR surfaceCapabilities;

VkExtent2D surfaceExtent;

char const **getSurfaceInstanceExtensions(uint32_t *count) {
    uint32_t requiredExtensionCount = 0;
    char const *const *requiredExtensions = SDL_Vulkan_GetInstanceExtensions(&requiredExtensionCount);

    char const *optionalExtensions[] = {
        VK_KHR_GET_SURFACE_CAPABILITIES_2_EXTENSION_NAME,
        VK_EXT_SURFACE_MAINTENANCE_1_EXTENSION_NAME,
        VK_EXT_SWAPCHAIN_COLOR_SPACE_EXTENSION_NAME,
    };

    const uint32_t optionalExtensionCount = sizeof(optionalExtensions) / sizeof(char const *);

    if(surfaceInstanceExtensionCount == 0) {
        surfaceInstanceExtensionCount = requiredExtensionCount + optionalExtensionCount;
        surfaceInstanceExtensions = malloc(surfaceInstanceExtensionCount * sizeof(char const *));

        memcpy(surfaceInstanceExtensions, requiredExtensions, requiredExtensionCount * sizeof(char const *));
        memcpy(surfaceInstanceExtensions + requiredExtensionCount, optionalExtensions, optionalExtensionCount * sizeof(char const *));
    }

    *count = surfaceInstanceExtensionCount;
    return surfaceInstanceExtensions;
}

void createSurface() {
    VkBool32 result = SDL_Vulkan_CreateSurface(window, instance, nullptr, &surface);
    assert(result);

    debug("Surface created");
}

VkBool32 getSurfaceSupport(VkPhysicalDevice physicalDeviceCandidate) {
    uint32_t queueFamilyCountCandidate = 0;
    vkGetPhysicalDeviceQueueFamilyProperties2(physicalDeviceCandidate, &queueFamilyCountCandidate, nullptr);

    for(uint32_t queueFamilyIndex = 0; queueFamilyIndex < queueFamilyCountCandidate; queueFamilyIndex++) {
        VkBool32 support = VK_FALSE;
        VkResult result = vkGetPhysicalDeviceSurfaceSupportKHR(physicalDeviceCandidate, queueFamilyIndex, surface, &support);
        assert(result == VK_SUCCESS);

        if(support) {
            return VK_TRUE;
        }
    }

    return VK_FALSE;
}

void selectPresentMode() {
    uint32_t presentModeCount;
    vkGetPhysicalDeviceSurfacePresentModesKHR(physicalDevice, surface, &presentModeCount, nullptr);

    VkPresentModeKHR *presentModes = malloc(presentModeCount * sizeof(VkPresentModeKHR));
    vkGetPhysicalDeviceSurfacePresentModesKHR(physicalDevice, surface, &presentModeCount, presentModes);

    VkPresentModeKHR preferredPresentModes[] = {
        VK_PRESENT_MODE_MAILBOX_KHR,
        VK_PRESENT_MODE_IMMEDIATE_KHR
    };

    uint32_t preferredPresentModeCount = sizeof(preferredPresentModes) / sizeof(VkPresentModeKHR);

    VkBool32 presentModeMatched = VK_FALSE;

    for(uint32_t preferredPresentModeIndex = 0; preferredPresentModeIndex < preferredPresentModeCount; preferredPresentModeIndex++) {
        VkPresentModeKHR preferredPresentMode = preferredPresentModes[preferredPresentModeIndex];

        for(uint32_t supportedPresentModeIndex = 0; supportedPresentModeIndex < presentModeCount; supportedPresentModeIndex++) {
            VkPresentModeKHR supportedPresentMode = presentModes[supportedPresentModeIndex];

            if(preferredPresentMode == supportedPresentMode) {
                presentMode = preferredPresentMode;
                presentModeMatched = VK_TRUE;
                break;
            }
        }

        if(presentModeMatched) {
            break;
        }
    }

    if(!presentModeMatched) {
        presentMode = presentModes[0];
    }

    free(presentModes);
}

void selectSurfaceFormat() {
    VkPhysicalDeviceSurfaceInfo2KHR surfaceInfo = {
        .sType = VK_STRUCTURE_TYPE_PHYSICAL_DEVICE_SURFACE_INFO_2_KHR,
        .pNext = nullptr,
        .surface = surface
    };

    uint32_t surfaceFormatCount;
    vkGetPhysicalDeviceSurfaceFormats2KHR(physicalDevice, &surfaceInfo, &surfaceFormatCount, nullptr);

    VkSurfaceFormat2KHR *surfaceFormats = malloc(surfaceFormatCount * sizeof(VkSurfaceFormat2KHR));

    for(uint32_t surfaceFormatIndex = 0; surfaceFormatIndex < surfaceFormatCount; surfaceFormatIndex++) {
        surfaceFormats[surfaceFormatIndex].sType = VK_STRUCTURE_TYPE_SURFACE_FORMAT_2_KHR;
        surfaceFormats[surfaceFormatIndex].pNext = nullptr;
        memset(&surfaceFormats[surfaceFormatIndex].surfaceFormat, 0, sizeof(VkSurfaceFormatKHR));
    }

    vkGetPhysicalDeviceSurfaceFormats2KHR(physicalDevice, &surfaceInfo, &surfaceFormatCount, surfaceFormats);

    VkSurfaceFormatKHR preferredSurfaceFormats[] = {
        {
            .format = VK_FORMAT_R8G8B8A8_SRGB,
            .colorSpace = VK_COLOR_SPACE_SRGB_NONLINEAR_KHR
        },
        {
            .format = VK_FORMAT_B8G8R8A8_SRGB,
            .colorSpace = VK_COLOR_SPACE_SRGB_NONLINEAR_KHR
        }
    };

    uint32_t preferredSurfaceFormatCount = sizeof(preferredSurfaceFormats) / sizeof(VkSurfaceFormatKHR);

    VkBool32 surfaceFormatMatched = VK_FALSE;

    for(uint32_t preferredSurfaceFormatIndex = 0; preferredSurfaceFormatIndex < preferredSurfaceFormatCount; preferredSurfaceFormatIndex++) {
        VkSurfaceFormatKHR preferredSurfaceFormat = preferredSurfaceFormats[preferredSurfaceFormatIndex];

        for(uint32_t supportedSurfaceFormatIndex = 0; supportedSurfaceFormatIndex < surfaceFormatCount; supportedSurfaceFormatIndex++) {
            VkSurfaceFormatKHR supportedSurfaceFormat = surfaceFormats[supportedSurfaceFormatIndex].surfaceFormat;

            if(preferredSurfaceFormat.format == supportedSurfaceFormat.format && preferredSurfaceFormat.colorSpace == supportedSurfaceFormat.colorSpace) {
                surfaceFormat = preferredSurfaceFormat;
                surfaceFormatMatched = VK_TRUE;
                break;
            }
        }

        if(surfaceFormatMatched) {
            break;
        }
    }

    if(!surfaceFormatMatched) {
        surfaceFormat = surfaceFormats[0].surfaceFormat;
    }

    free(surfaceFormats);
}

void setSurfaceCapabilities() {
    VkSurfacePresentModeEXT presentModeInfo = {
        .sType = VK_STRUCTURE_TYPE_SURFACE_PRESENT_MODE_EXT,
        .pNext = nullptr,
        .presentMode = presentMode
    };

    VkPhysicalDeviceSurfaceInfo2KHR surfaceInfo = {
        .sType = VK_STRUCTURE_TYPE_PHYSICAL_DEVICE_SURFACE_INFO_2_KHR,
        .pNext = &presentModeInfo,
        .surface = surface
    };

    // NOTICE: We don't care about the other present modes compatible with the current one
    VkSurfacePresentModeCompatibilityEXT presentModeCompatibilityInfo = {
        .sType = VK_STRUCTURE_TYPE_SURFACE_PRESENT_MODE_COMPATIBILITY_EXT,
        .pNext = nullptr,
        .presentModeCount = 0,
        .pPresentModes = nullptr
    };

    // TODO: Revisit whole thing
    VkSurfaceCapabilities2KHR surfaceCapabilitiesInfo = {
        .sType = VK_STRUCTURE_TYPE_SURFACE_CAPABILITIES_2_KHR,
        .pNext = &presentModeCompatibilityInfo,
        .surfaceCapabilities = {}
    };

    vkGetPhysicalDeviceSurfaceCapabilities2KHR(physicalDevice, &surfaceInfo, &surfaceCapabilitiesInfo);

    surfaceCapabilities = surfaceCapabilitiesInfo.surfaceCapabilities;
}

void setSurfaceExtent() {
    if(surfaceCapabilities.currentExtent.width == 0          || surfaceCapabilities.currentExtent.height == 0 ||
       surfaceCapabilities.currentExtent.width == 1          || surfaceCapabilities.currentExtent.height == 1 ||
       surfaceCapabilities.currentExtent.width == UINT32_MAX || surfaceCapabilities.currentExtent.height == UINT32_MAX) {
        surfaceExtent.width  = windowWidth;
        surfaceExtent.height = windowHeight;
    } else {
        surfaceExtent = surfaceCapabilities.currentExtent;
    }
}

void generateSurfaceDetails() {
    selectPresentMode();
    selectSurfaceFormat();
    setSurfaceCapabilities();
    setSurfaceExtent();
}

char const **getSurfaceDeviceExtensions(uint32_t *count) {
    char const *requiredExtensions[] = {
        VK_KHR_SWAPCHAIN_EXTENSION_NAME,
    };
    uint32_t requiredExtensionCount = sizeof(requiredExtensions) / sizeof(char const *);

    char const *optionalExtensions[] = {
        VK_EXT_SWAPCHAIN_MAINTENANCE_1_EXTENSION_NAME,
        VK_EXT_PRESENT_MODE_FIFO_LATEST_READY_EXTENSION_NAME,
    };

    const uint32_t optionalExtensionCount = sizeof(optionalExtensions) / sizeof(char const *);

    if(surfaceDeviceExtensionCount == 0) {
        surfaceDeviceExtensionCount = requiredExtensionCount + optionalExtensionCount;
        surfaceDeviceExtensions = malloc(surfaceDeviceExtensionCount * sizeof(char const *));

        memcpy(surfaceDeviceExtensions, requiredExtensions, requiredExtensionCount * sizeof(char const *));
        memcpy(surfaceDeviceExtensions + requiredExtensionCount, optionalExtensions, optionalExtensionCount * sizeof(char const *));
    }

    *count = surfaceDeviceExtensionCount;
    return surfaceDeviceExtensions;
}

void *getSurfaceDeviceFeatures() {
    if(surfaceDeviceFeatures == nullptr) {
        VkPhysicalDevicePresentModeFifoLatestReadyFeaturesEXT *presentModeFifoLatestReadyFeatures = malloc(sizeof(VkPhysicalDevicePresentModeFifoLatestReadyFeaturesEXT));
        presentModeFifoLatestReadyFeatures->sType = VK_STRUCTURE_TYPE_PHYSICAL_DEVICE_PRESENT_MODE_FIFO_LATEST_READY_FEATURES_EXT;
        presentModeFifoLatestReadyFeatures->pNext = nullptr;
        presentModeFifoLatestReadyFeatures->presentModeFifoLatestReady = VK_TRUE;

        VkPhysicalDeviceSwapchainMaintenance1FeaturesEXT *swapchainMaintenance1Features = malloc(sizeof(VkPhysicalDeviceSwapchainMaintenance1FeaturesEXT));
        swapchainMaintenance1Features->sType = VK_STRUCTURE_TYPE_PHYSICAL_DEVICE_SWAPCHAIN_MAINTENANCE_1_FEATURES_EXT;
        swapchainMaintenance1Features->pNext = presentModeFifoLatestReadyFeatures;
        swapchainMaintenance1Features->swapchainMaintenance1 = VK_TRUE;

        surfaceDeviceFeatures = swapchainMaintenance1Features;
    }

    return surfaceDeviceFeatures;
}

void destroySurface() {
    if(surfaceDeviceFeatures != nullptr) {
        VkPhysicalDeviceSwapchainMaintenance1FeaturesEXT *swapchainMaintenance1Features = surfaceDeviceFeatures;
        VkPhysicalDevicePresentModeFifoLatestReadyFeaturesEXT *presentModeFifoLatestReadyFeatures = swapchainMaintenance1Features->pNext;
        assert(presentModeFifoLatestReadyFeatures->pNext == nullptr);

        free(presentModeFifoLatestReadyFeatures);
        free(swapchainMaintenance1Features);
        surfaceDeviceFeatures = nullptr;
    }

    if(surfaceDeviceExtensionCount != 0) {
        surfaceDeviceExtensionCount = 0;
        free(surfaceDeviceExtensions);
    }

    // NOTICE: I contributed this function to SDL :)
    SDL_Vulkan_DestroySurface(instance, surface, nullptr);
    debug("Surface destroyed");

    if(surfaceInstanceExtensionCount != 0) {
        surfaceInstanceExtensionCount = 0;
        free(surfaceInstanceExtensions);
    }
}
