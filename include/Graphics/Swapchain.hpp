#pragma once

#include <vulkan/vulkan_raii.hpp>

namespace Graphics {
    class Swapchain {
    private:
        vk::raii::SwapchainKHR *swapchain;
    public:
        Swapchain(vk::raii::Device *device, vk::raii::SurfaceKHR *surface);
    };
}
