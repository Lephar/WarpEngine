#pragma once

#include <vulkan/vulkan_raii.hpp>
#include <vulkan/vulkan_structs.hpp>

namespace System {
    class Window;
}

namespace Graphics {
    class Instance;

    class Surface {
    private:
        vk::raii::SurfaceKHR surface;
        vk::Extent2D extent;
        
    public:
        Surface(vk::raii::SurfaceKHR surface, vk::Extent2D extent);
    };
}
