#pragma once

#include "pch.h"

typedef struct image Image;

struct swapchain {
    uint32_t imageCount;
    VkSurfaceFormatKHR surfaceFormat;
    VkPresentModeKHR presentMode;
    VkSurfaceTransformFlagsKHR transform;
    VkSwapchainKHR swapchain;
    Image **images;
} typedef Swapchain;

extern Swapchain oldSwapchain;
extern Swapchain swapchain;

void createSwapchain();
void destroySwapchain();