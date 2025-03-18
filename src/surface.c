#include "surface.h"

#include "window.h"
#include "instance.h"
#include "device.h"

#include "helper.h"
#include "queue.h"

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

    vkGetPhysicalDeviceSurfaceFormatsKHR(physicalDevice, surface, &surfaceFormatCount, NULL);
    surfaceFormats = malloc(surfaceFormatCount * sizeof(VkSurfaceFormatKHR));
    vkGetPhysicalDeviceSurfaceFormatsKHR(physicalDevice, surface, &surfaceFormatCount, surfaceFormats);

    vkGetPhysicalDeviceSurfacePresentModesKHR(physicalDevice, surface, &presentModeCount, NULL);
    presentModes = malloc(presentModeCount * sizeof(VkPresentModeKHR));
    vkGetPhysicalDeviceSurfacePresentModesKHR(physicalDevice, surface, &presentModeCount, presentModes);

    vkGetPhysicalDeviceSurfaceCapabilitiesKHR(physicalDevice, surface, &surfaceCapabilities);

    if(surfaceCapabilities.currentExtent.width != UINT32_MAX && surfaceCapabilities.currentExtent.height != UINT32_MAX) {
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
