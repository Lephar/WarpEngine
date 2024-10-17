#include "surface.h"

#include "helper.h"
#include "queue.h"

extern SDL_bool windowCreated;
SDL_bool surfaceCreated = SDL_FALSE;

extern SDL_Window *window;
extern VkExtent2D extent;

extern VkInstance instance;
VkSurfaceKHR surface;
extern VkPhysicalDevice physicalDevice;
extern Queue graphicsQueue;

VkBool32 surfaceSupport;
uint32_t surfaceFormatCount;
VkSurfaceFormatKHR *surfaceFormats;
uint32_t presentModeCount;
VkPresentModeKHR *presentModes;
VkSurfaceCapabilitiesKHR surfaceCapabilities;

void createSurface() {
    assert(windowCreated && !surfaceCreated);

    SDL_Vulkan_CreateSurface(window, instance, &surface);

    surfaceCreated = SDL_TRUE;
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
    extent = surfaceCapabilities.currentExtent;

    debug("Surface details set:");
    debug("\tSurface extent: %d %d", extent.width, extent.height);
    debug("\tSurface format count: %d", surfaceFormatCount);
    debug("\tPresent mode count: %d", presentModeCount);
}

void destroySurface() {
    assert(surfaceCreated);

    vkDestroySurfaceKHR(instance, surface, NULL);

    free(presentModes);
    free(surfaceFormats);

    surfaceCreated = SDL_FALSE;
    debug("Surface destroyed");
}
