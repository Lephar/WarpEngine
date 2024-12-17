#pragma once

#include "zero.h"

#include "image.h"

struct swapchain {
    uint32_t imageCount;
    VkSurfaceFormatKHR surfaceFormat;
    VkPresentModeKHR presentMode;
    VkSurfaceTransformFlagsKHR transform;
    VkSwapchainKHR swapchain;
    Image *images;
} typedef Swapchain;

void createSwapchain();
void destroySwapchain();