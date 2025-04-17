#include "surface.h"

#include "window.h"
#include "instance.h"
#include "physicalDevice.h"
#include "queue.h"

#include "logger.h"

VkSurfaceKHR surface;

VkPresentModeKHR presentMode;
VkSurfaceFormatKHR surfaceFormat;

VkSurfaceCapabilitiesKHR surfaceCapabilities;

void selectPresentMode() {
    uint32_t presentModeCount;
    vkGetPhysicalDeviceSurfacePresentModesKHR(physicalDevice, surface, &presentModeCount, NULL);

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
        .pNext = NULL,
        .surface = surface
    };

    uint32_t surfaceFormatCount;
    vkGetPhysicalDeviceSurfaceFormats2KHR(physicalDevice, &surfaceInfo, &surfaceFormatCount, NULL);

    VkSurfaceFormat2KHR *surfaceFormats = malloc(surfaceFormatCount * sizeof(VkSurfaceFormat2KHR));

    for(uint32_t surfaceFormatIndex = 0; surfaceFormatIndex < surfaceFormatCount; surfaceFormatIndex++) {
        surfaceFormats[surfaceFormatIndex].sType = VK_STRUCTURE_TYPE_SURFACE_FORMAT_2_KHR;
        surfaceFormats[surfaceFormatIndex].pNext = NULL;
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

void generateSurfaceCapabilities() {
    VkSurfacePresentModeEXT presentModeInfo = {
        .sType = VK_STRUCTURE_TYPE_SURFACE_PRESENT_MODE_EXT,
        .pNext = NULL,
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
        .pNext = NULL,
        .presentModeCount = 0,
        .pPresentModes = NULL
    };

    VkSurfaceCapabilities2KHR surfaceCapabilitiesInfo = {
        .sType = VK_STRUCTURE_TYPE_SURFACE_CAPABILITIES_2_KHR,
        .pNext = &presentModeCompatibilityInfo,
        .surfaceCapabilities = {}
    };

    vkGetPhysicalDeviceSurfaceCapabilities2KHR(physicalDevice, &surfaceInfo, &surfaceCapabilitiesInfo);

    surfaceCapabilities = surfaceCapabilitiesInfo.surfaceCapabilities;
}

void createSurface() {
    bool result = SDL_Vulkan_CreateSurface(window, instance, NULL, &surface);
    assert(result);

    debug("Surface created");

    VkBool32 surfaceSupport;
    vkGetPhysicalDeviceSurfaceSupportKHR(physicalDevice, graphicsQueue.queueFamilyIndex, surface, &surfaceSupport);
    assert(surfaceSupport);

    selectPresentMode();

    selectSurfaceFormat();

    generateSurfaceCapabilities();

    if( surfaceCapabilities.currentExtent.width != 0          && surfaceCapabilities.currentExtent.height != 0 &&
        surfaceCapabilities.currentExtent.width != 1          && surfaceCapabilities.currentExtent.height != 1 &&
        surfaceCapabilities.currentExtent.width != UINT32_MAX && surfaceCapabilities.currentExtent.height != UINT32_MAX) {
        extent = surfaceCapabilities.currentExtent;
    }

    debug("Surface details set:");
    debug("\tSurface extent: %u %u", extent.width, extent.height);
}

void destroySurface() {
    SDL_Vulkan_DestroySurface(instance, surface, NULL);
    debug("Surface destroyed");
}
