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

    VkSwapchainPresentModesCreateInfoEXT presentModesInfo = {
        .sType = VK_STRUCTURE_TYPE_SWAPCHAIN_PRESENT_MODES_CREATE_INFO_EXT,
        .pNext = NULL,
        .presentModeCount = 1,
        .pPresentModes = &presentMode
    };

    VkSwapchainCreateInfoKHR swapchainInfo = {
        .sType = VK_STRUCTURE_TYPE_SWAPCHAIN_CREATE_INFO_KHR,
        .pNext = &presentModesInfo,
        .flags = 0,
        .surface = surface,
        .minImageCount = swapchain.imageCount,
        .imageFormat = surfaceFormat.format,
        .imageColorSpace = surfaceFormat.colorSpace,
        .imageExtent = extent,
        .imageArrayLayers = 1,
        .imageUsage = VK_IMAGE_USAGE_TRANSFER_DST_BIT,
        .imageSharingMode = VK_SHARING_MODE_EXCLUSIVE,
        .queueFamilyIndexCount = 0,
        .pQueueFamilyIndices = NULL,
        .preTransform = surfaceCapabilities.currentTransform,
        .compositeAlpha = VK_COMPOSITE_ALPHA_OPAQUE_BIT_KHR,
        .presentMode = presentMode,
        .clipped = VK_TRUE,
        .oldSwapchain = NULL
    };

    vkCreateSwapchainKHR(device, &swapchainInfo, NULL, &swapchain.swapchain);

    vkGetSwapchainImagesKHR(device, swapchain.swapchain, &swapchain.imageCount, NULL);
    VkImage *handles = malloc(swapchain.imageCount * sizeof(VkImage));
    swapchain.images = malloc(swapchain.imageCount * sizeof(Image  ));
    swapchain.presentSemaphores = malloc(swapchain.imageCount * sizeof(VkSemaphore));

    vkGetSwapchainImagesKHR(device, swapchain.swapchain, &swapchain.imageCount, handles);
    debug("Swapchain created with %d images:", swapchain.imageCount);

    for(uint32_t imageIndex = 0; imageIndex < swapchain.imageCount; imageIndex++) {
        wrapImage(&swapchain.images[imageIndex], handles[imageIndex], extent.width, extent.height, 1, VK_SAMPLE_COUNT_1_BIT, surfaceFormat.format, VK_IMAGE_USAGE_TRANSFER_DST_BIT, VK_IMAGE_ASPECT_COLOR_BIT, VK_IMAGE_TILING_OPTIMAL);
        transitionImageLayout(&swapchain.images[imageIndex], VK_IMAGE_LAYOUT_PRESENT_SRC_KHR);

        VkSemaphoreCreateInfo semaphoreInfo = {
            .sType = VK_STRUCTURE_TYPE_SEMAPHORE_CREATE_INFO,
            .pNext = NULL,
            .flags = 0
        };

        vkCreateSemaphore(device, &semaphoreInfo, NULL, &swapchain.presentSemaphores[imageIndex]);

        debug("\tWrapped and transitioned image %u and created the presentation semaphore", imageIndex);
    }

    free(handles);
}

void destroySwapchain() {
    for(uint32_t imageIndex = 0; imageIndex < swapchain.imageCount; imageIndex++) {
        vkDestroySemaphore(device, swapchain.presentSemaphores[imageIndex], NULL);
    }

    vkDestroySwapchainKHR(device, swapchain.swapchain, NULL);

    free(swapchain.presentSemaphores);
    free(swapchain.images);

    debug("Swapchain destroyed");
}
