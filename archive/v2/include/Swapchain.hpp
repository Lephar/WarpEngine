#pragma once

#include "Graphics.hpp"
#include "Image.hpp"

namespace Graphics {
    struct Swapchain {
        size_t size;

        vk::SwapchainKHR swapchain;
        std::vector<Image> images;

        bool created;

        void create(DeviceInfo deviceInfo, vk::SurfaceKHR surface, vk::Extent2D extent);
        Image getImage(size_t index);
    };
}
