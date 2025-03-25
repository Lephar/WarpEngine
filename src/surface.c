#include "surface.h"

#include "window.h"
#include "instance.h"
#include "physicalDevice.h"
#include "queue.h"

#include "logger.h"

VkSurfaceKHR surface;

VkBool32 surfaceSupport;

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

    VkSurfaceFormatKHR  *surfaceFormats  = malloc(surfaceFormatCount * sizeof(VkSurfaceFormatKHR ));
    VkSurfaceFormat2KHR *surfaceFormats2 = malloc(surfaceFormatCount * sizeof(VkSurfaceFormat2KHR));

    for(uint32_t surfaceFormatIndex = 0; surfaceFormatIndex < surfaceFormatCount; surfaceFormatIndex++) {
        surfaceFormats2[surfaceFormatIndex].sType = VK_STRUCTURE_TYPE_SURFACE_FORMAT_2_KHR;
        surfaceFormats2[surfaceFormatIndex].pNext = NULL;
        memset(&surfaceFormats2[surfaceFormatIndex].surfaceFormat, 0, sizeof(VkSurfaceFormatKHR));
    }

    vkGetPhysicalDeviceSurfaceFormats2KHR(physicalDevice, &surfaceInfo, &surfaceFormatCount, surfaceFormats2);

    for(uint32_t surfaceFormatIndex = 0; surfaceFormatIndex < surfaceFormatCount; surfaceFormatIndex++) {
        surfaceFormats[surfaceFormatIndex] = surfaceFormats2[surfaceFormatIndex].surfaceFormat;
    }

    free(surfaceFormats2);

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
            VkSurfaceFormatKHR supportedSurfaceFormat = surfaceFormats[supportedSurfaceFormatIndex];

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
        surfaceFormat = surfaceFormats[0];
    }

    free(surfaceFormats);
}

void createSurface() {
    SDL_bool result = SDL_Vulkan_CreateSurface(window, instance, &surface);
    assert(result);

    debug("Surface created");

    vkGetPhysicalDeviceSurfaceSupportKHR(physicalDevice, graphicsQueue.queueFamilyIndex, surface, &surfaceSupport);
    assert(surfaceSupport);

    selectPresentMode();

    selectSurfaceFormat();

    VkPhysicalDeviceSurfaceInfo2KHR surfaceInfo = {
        .sType = VK_STRUCTURE_TYPE_PHYSICAL_DEVICE_SURFACE_INFO_2_KHR,
        .pNext = NULL,
        .surface = surface
    };

    VkSurfaceCapabilities2KHR surfaceCapabilities2 = {
        .sType = VK_STRUCTURE_TYPE_SURFACE_CAPABILITIES_2_KHR,
        .pNext = NULL,
        .surfaceCapabilities = {}
    };

    vkGetPhysicalDeviceSurfaceCapabilities2KHR(physicalDevice, &surfaceInfo, &surfaceCapabilities2);

    surfaceCapabilities = surfaceCapabilities2.surfaceCapabilities;

    if( surfaceCapabilities.currentExtent.width != 0          && surfaceCapabilities.currentExtent.height != 0 &&
        surfaceCapabilities.currentExtent.width != 1          && surfaceCapabilities.currentExtent.height != 1 &&
        surfaceCapabilities.currentExtent.width != UINT32_MAX && surfaceCapabilities.currentExtent.height != UINT32_MAX) {
        extent = surfaceCapabilities.currentExtent;
    }

    debug("Surface details set:");
    debug("\tSurface extent: %u %u", extent.width, extent.height);
}

void destroySurface() {
    vkDestroySurfaceKHR(instance, surface, NULL);

    debug("Surface destroyed");
}
