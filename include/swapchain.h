#pragma once

#include "pch.h"

typedef struct image Image;

struct swapchain {
    VkSwapchainKHR swapchain;

    uint32_t imageCount;
    Image *images;

    // NOTICE: There needs to be a spare because we dont know the index until we start the acquisition
    VkSemaphore  acquireSemaphore;
    VkSemaphore *acquireSemaphores;
    VkSemaphore *presentSemaphores;
} typedef Swapchain;

extern Swapchain oldSwapchain;
extern Swapchain swapchain;

void createSwapchain();
void destroySwapchain();