#include "surface.h"

#include "window.h"
#include "instance.h"
#include "physicalDevice.h"

#include "logger.h"

uint32_t surfaceInstanceExtensionCount;
char const **surfaceInstanceExtensions;

uint32_t surfaceDeviceExtensionCount;
char const **surfaceDeviceExtensions;

void *surfaceDeviceFeatures;

VkSurfaceKHR surface;

VkPresentModeKHR presentMode;
VkSurfaceFormatKHR surfaceFormat;

VkSurfaceCapabilitiesKHR surfaceCapabilities;

VkExtent2D surfaceExtent;

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
        VK_PRESENT_MODE_FIFO_LATEST_READY_EXT,
        VK_PRESENT_MODE_MAILBOX_KHR,
        VK_PRESENT_MODE_IMMEDIATE_KHR,
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

    debug("Present Mode Selected: %d", presentMode);

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

    debug("Surface Format Selected:");
    debug("\tFormat:      %d", surfaceFormat.format);
    debug("\tColor Space: %d", surfaceFormat.colorSpace);

    free(surfaceFormats);
}

void setSurfaceCapabilities() {
    VkSurfacePresentModeEXT surfacePresentMode = {
        .sType = VK_STRUCTURE_TYPE_SURFACE_PRESENT_MODE_EXT,
        .pNext = nullptr,
        .presentMode = presentMode
    };

    VkPhysicalDeviceSurfaceInfo2KHR surfaceInfo2 = {
        .sType = VK_STRUCTURE_TYPE_PHYSICAL_DEVICE_SURFACE_INFO_2_KHR,
        .pNext = &surfacePresentMode,
        .surface = surface
    };

    VkSurfacePresentModeCompatibilityEXT surfacePresentModeCompatibility = {
        .sType = VK_STRUCTURE_TYPE_SURFACE_PRESENT_MODE_COMPATIBILITY_EXT,
        .pNext = nullptr,
        .presentModeCount = 0,
        .pPresentModes = nullptr
    };

    VkSurfaceCapabilities2KHR surfaceCapabilities2 = {
        .sType = VK_STRUCTURE_TYPE_SURFACE_CAPABILITIES_2_KHR,
        .pNext = &surfacePresentModeCompatibility,
        .surfaceCapabilities = {}
    };

    vkGetPhysicalDeviceSurfaceCapabilities2KHR(physicalDevice, &surfaceInfo2, &surfaceCapabilities2);
    surfacePresentModeCompatibility.pPresentModes = malloc(surfacePresentModeCompatibility.presentModeCount * sizeof(VkPresentModeKHR));
    vkGetPhysicalDeviceSurfaceCapabilities2KHR(physicalDevice, &surfaceInfo2, &surfaceCapabilities2);

    // NOTICE: We don't care about the other present modes compatible with the current one
    assert(presentMode == surfacePresentModeCompatibility.pPresentModes[0]);
    free(surfacePresentModeCompatibility.pPresentModes);

    surfaceCapabilities = surfaceCapabilities2.surfaceCapabilities;
    debug("Surface Capabilities Set:");
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

    debug("\tExtent: %ux%u", surfaceExtent.width, surfaceExtent.height);
}

void setSurfaceDetails() {
    selectPresentMode();
    selectSurfaceFormat();
    setSurfaceCapabilities();
    setSurfaceExtent();
}

void destroySurface() {
    // NOTICE: I contributed this function to SDL :)
    SDL_Vulkan_DestroySurface(instance, surface, nullptr);
    debug("Surface destroyed");
}
