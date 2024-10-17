#pragma once

#include "zero.h"

struct swapchain {
    uint32_t imageCount;
    VkSurfaceFormatKHR surfaceFormat;
    VkPresentModeKHR presentMode;
    VkSurfaceTransformFlagsKHR transform;
    VkSwapchainKHR swapchain;
    VkImage *images;
} typedef Swapchain;

void createSwapchain();
void destroySwapchain();