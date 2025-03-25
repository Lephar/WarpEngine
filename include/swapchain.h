#pragma once

#include "pch.h"

typedef struct image Image;

struct swapchain {
    uint32_t imageCount;
    VkSwapchainKHR swapchain;
    Image *images;
} typedef Swapchain;

extern Swapchain oldSwapchain;
extern Swapchain swapchain;

void createSwapchain();
void destroySwapchain();