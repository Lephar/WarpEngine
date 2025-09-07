#pragma once

#include "pch.h"

typedef struct image Image;

struct swapchain {
    VkSwapchainKHR swapchain;

    uint32_t imageCount;
    Image *images;
    VkSemaphore *presentSemaphores;
} typedef Swapchain;

extern Swapchain oldSwapchain;
extern Swapchain swapchain;

void createSwapchain();
void destroySwapchain();