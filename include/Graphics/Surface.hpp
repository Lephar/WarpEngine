#pragma once

#include <vulkan/vulkan_raii.hpp>

namespace Graphics {
    class Surface {
    private:
        vk::raii::SurfaceKHR *surface;
    };
}