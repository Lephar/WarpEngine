#pragma once

#include "pch.h"

typedef struct image Image;

typedef struct swapchain {
    VkSwapchainKHR swapchain;

    uint32_t imageCount;
    Image *images;

    // NOTICE: There needs to be a spare because we don't know the index until we start the acquisition
    // TODO: When framebuffers has more images and swapchain images are out of order, we may need a fence
    VkSemaphore  acquireSemaphore;
    VkSemaphore *acquireSemaphores;
    VkSemaphore *presentSemaphores;
} Swapchain, *PSwapchain;

extern Swapchain oldSwapchain;
extern Swapchain swapchain;

void createSwapchain();
void destroySwapchain();