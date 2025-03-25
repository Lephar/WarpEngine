#include "swapchain.h"

#include "window.h"
#include "surface.h"
#include "device.h"
#include "image.h"

#include "logger.h"

Swapchain oldSwapchain;
Swapchain swapchain;

void createSwapchain() {
    swapchain.imageCount = surfaceCapabilities.minImageCount + 1;
    swapchain.transform = surfaceCapabilities.currentTransform;

    swapchain.presentMode   = presentMode;
    swapchain.surfaceFormat = surfaceFormat;

    // TODO: Research the non-trivial use-case
    VkSwapchainPresentModesCreateInfoEXT presentModesInfo = {
        .sType = VK_STRUCTURE_TYPE_SWAPCHAIN_PRESENT_MODES_CREATE_INFO_EXT,
        .pNext = NULL,
        .presentModeCount = 1,
        .pPresentModes = &swapchain.presentMode
    };

    VkSwapchainCreateInfoKHR swapchainInfo = {
        .sType = VK_STRUCTURE_TYPE_SWAPCHAIN_CREATE_INFO_KHR,
        .pNext = &presentModesInfo,
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
    swapchain.images = malloc(swapchain.imageCount * sizeof(Image *));

    vkGetSwapchainImagesKHR(device, swapchain.swapchain, &swapchain.imageCount, handles);
    debug("Retrieved %u swapchain images", swapchain.imageCount);

    for(uint32_t imageIndex = 0; imageIndex < swapchain.imageCount; imageIndex++) {
        swapchain.images[imageIndex] = wrapImage(handles[imageIndex], extent.width, extent.height, 1, VK_SAMPLE_COUNT_1_BIT, swapchain.surfaceFormat.format, VK_IMAGE_USAGE_TRANSFER_DST_BIT, VK_IMAGE_ASPECT_COLOR_BIT);
        transitionImageLayout(swapchain.images[imageIndex], VK_IMAGE_LAYOUT_PRESENT_SRC_KHR);
        debug("Wrapped and transitioned swapchain image %u", imageIndex);
    }

    free(handles);
}

void destroySwapchain() {
    vkDestroySwapchainKHR(device, swapchain.swapchain, NULL);

    for(uint32_t imageIndex = 0; imageIndex < swapchain.imageCount; imageIndex++) {
        free(swapchain.images[imageIndex]);
    }

    free(swapchain.images);

    debug("Swapchain destroyed");
}
