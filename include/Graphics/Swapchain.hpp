#pragma once

#include "Graphics.hpp"
#include "Image.hpp"
#include <vulkan/vulkan_handles.hpp>

class Swapchain {
    Renderer *owner;

    size_t size;

    vk::SwapchainKHR swapchain;
	std::vector<Image> images;

    bool created;

    void create(Renderer *owner, size_t size, vk::Extent2D extent, DeviceInfo deviceInfo);
    
};
