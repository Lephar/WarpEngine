#include "swapchain.h"

#include "helper.h"
#include "surface.h"
#include "device.h"
#include "image.h"

Swapchain oldSwapchain;
Swapchain swapchain;

void createSwapchain() {
    // TODO: Why segmentation fault when add an extra image?
    swapchain.imageCount = surfaceCapabilities.minImageCount + 1;
    swapchain.transform = surfaceCapabilities.currentTransform;

    VkSurfaceFormatKHR preferredSurfaceFormats[] = {
        {
            .format = VK_FORMAT_B8G8R8A8_SRGB,
            .colorSpace = VK_COLOR_SPACE_SRGB_NONLINEAR_KHR
        },
        {
            .format = VK_FORMAT_R8G8B8A8_SRGB,
            .colorSpace = VK_COLOR_SPACE_SRGB_NONLINEAR_KHR
        },
        {
            .format = VK_FORMAT_B8G8R8_SRGB,
            .colorSpace = VK_COLOR_SPACE_SRGB_NONLINEAR_KHR
        },
        {
            .format = VK_FORMAT_R8G8B8_SRGB,
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
                swapchain.surfaceFormat = preferredSurfaceFormat;
                surfaceFormatMatched = VK_TRUE;
                break;
            }
        }

        if(surfaceFormatMatched) {
            break;
        }
    }

    if(!surfaceFormatMatched) {
        swapchain.surfaceFormat = surfaceFormats[0];
    }

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
                swapchain.presentMode = preferredPresentMode;
                presentModeMatched = VK_TRUE;
                break;
            }
        }

        if(presentModeMatched) {
            break;
        }
    }

    if(!presentModeMatched) {
        swapchain.presentMode = presentModes[0];
    }

    VkSwapchainCreateInfoKHR swapchainInfo = {
        .sType = VK_STRUCTURE_TYPE_SWAPCHAIN_CREATE_INFO_KHR,
        .pNext = NULL,
        .flags = 0,
        .surface = surface,
        .minImageCount = swapchain.imageCount,
        .imageFormat = swapchain.surfaceFormat.format,
        .imageColorSpace = swapchain.surfaceFormat.colorSpace,
        .imageExtent = extent,
        .imageArrayLayers = 1,
        .imageUsage = VK_IMAGE_USAGE_TRANSFER_DST_BIT,
        .imageSharingMode = VK_SHARING_MODE_EXCLUSIVE,
        .queueFamilyIndexCount = 0,
        .pQueueFamilyIndices = NULL,
        .preTransform = swapchain.transform,
        .compositeAlpha = VK_COMPOSITE_ALPHA_OPAQUE_BIT_KHR,
        .presentMode = swapchain.presentMode,
        .clipped = VK_TRUE,
        .oldSwapchain = NULL
    };

    vkCreateSwapchainKHR(device, &swapchainInfo, NULL, &swapchain.swapchain);
    debug("Swapchain created");

    vkGetSwapchainImagesKHR(device, swapchain.swapchain, &swapchain.imageCount, NULL);
    VkImage *handles = malloc(swapchain.imageCount * sizeof(VkImage));
    swapchain.images = malloc(swapchain.imageCount * sizeof(Image));

    vkGetSwapchainImagesKHR(device, swapchain.swapchain, &swapchain.imageCount, handles);
    debug("Retrieved %u swapchain images", swapchain.imageCount);

    for(uint32_t imageIndex = 0; imageIndex < swapchain.imageCount; imageIndex++) {
        wrapImage(&swapchain.images[imageIndex], handles[imageIndex], extent.width, extent.height, 1, VK_SAMPLE_COUNT_1_BIT, swapchain.surfaceFormat.format, VK_IMAGE_USAGE_TRANSFER_DST_BIT, VK_IMAGE_ASPECT_COLOR_BIT);
        transitionImageLayout(&swapchain.images[imageIndex], VK_IMAGE_LAYOUT_PRESENT_SRC_KHR);
        debug("Wrapped and transitioned swapchain image %u", imageIndex);
    }

    free(handles);
}

void destroySwapchain() {
    vkDestroySwapchainKHR(device, swapchain.swapchain, NULL);
    free(swapchain.images);
    debug("Swapchain destroyed");
}
