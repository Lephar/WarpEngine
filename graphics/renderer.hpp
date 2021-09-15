#ifndef ZERO_CLIENT_GRAPHICS_RENDERER_HPP
#define ZERO_CLIENT_GRAPHICS_RENDERER_HPP

#include "../system/window.hpp"

namespace zero::graphics {
    class Renderer {
    private:
        vk::Instance &instance;
        vk::SurfaceKHR surface;
        vk::SwapchainKHR swapchain;

        void createSurface(xcb_connection_t *connection, xcb_window_t window);
    public:
        Renderer(vk::Instance &instance, vk::Device &device, xcb_connection_t *connection, xcb_window_t window,
                 system::Window &properties);
        void destroy();
    };
}

#endif //ZERO_CLIENT_GRAPHICS_RENDERER_HPP
