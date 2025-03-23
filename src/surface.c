#include "surface.h"

#include "window.h"
#include "instance.h"
#include "physicalDevice.h"
#include "queue.h"

#include "logger.h"

VkSurfaceKHR surface;

VkBool32 surfaceSupport;

uint32_t surfaceFormatCount;
VkSurfaceFormatKHR *surfaceFormats;

uint32_t presentModeCount;
VkPresentModeKHR *presentModes;

VkSurfaceCapabilitiesKHR surfaceCapabilities;

void createSurface() {
    SDL_bool result = SDL_Vulkan_CreateSurface(window, instance, &surface);
    assert(result);

    debug("Surface created");

    vkGetPhysicalDeviceSurfaceSupportKHR(physicalDevice, graphicsQueue.queueFamilyIndex, surface, &surfaceSupport);
    assert(surfaceSupport);

    VkPhysicalDeviceSurfaceInfo2KHR surfaceInfo = {
        .sType = VK_STRUCTURE_TYPE_PHYSICAL_DEVICE_SURFACE_INFO_2_KHR,
        .pNext = NULL,
        .surface = surface
    };

    vkGetPhysicalDeviceSurfaceFormats2KHR(physicalDevice, &surfaceInfo, &surfaceFormatCount, NULL);

    VkSurfaceFormat2KHR *surfaceFormats2 = malloc(surfaceFormatCount * sizeof(VkSurfaceFormat2KHR));

    for(uint32_t surfaceFormatIndex = 0; surfaceFormatIndex < surfaceFormatCount; surfaceFormatIndex++) {
        surfaceFormats2[surfaceFormatIndex].sType = VK_STRUCTURE_TYPE_SURFACE_FORMAT_2_KHR;
        surfaceFormats2[surfaceFormatIndex].pNext = NULL;
        memset(&surfaceFormats2[surfaceFormatIndex].surfaceFormat, 0, sizeof(VkSurfaceFormatKHR));
    }

    vkGetPhysicalDeviceSurfaceFormats2KHR(physicalDevice, &surfaceInfo, &surfaceFormatCount, surfaceFormats2);

    surfaceFormats = malloc(surfaceFormatCount * sizeof(VkSurfaceFormatKHR));

    for(uint32_t surfaceFormatIndex = 0; surfaceFormatIndex < surfaceFormatCount; surfaceFormatIndex++) {
        surfaceFormats[surfaceFormatIndex] = surfaceFormats2[surfaceFormatIndex].surfaceFormat;
    }

    free(surfaceFormats2);

    vkGetPhysicalDeviceSurfacePresentModesKHR(physicalDevice, surface, &presentModeCount, NULL);
    presentModes = malloc(presentModeCount * sizeof(VkPresentModeKHR));
    vkGetPhysicalDeviceSurfacePresentModesKHR(physicalDevice, surface, &presentModeCount, presentModes);

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
    debug("\tSurface format count: %u", surfaceFormatCount);
    debug("\tPresent mode count: %u", presentModeCount);
}

void destroySurface() {
    vkDestroySurfaceKHR(instance, surface, NULL);

    free(presentModes);
    free(surfaceFormats);

    debug("Surface destroyed");
}
