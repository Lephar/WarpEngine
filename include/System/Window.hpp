#pragma once

#include <SDL2/SDL.h>
#include <vulkan/vulkan_core.h>
#include <vector>

namespace System {
    class Window {
    private:
        VkExtent2D extent;
        SDL_Window *window;

    public:
        Window(const char *title, int32_t width, int32_t height);

        VkExtent2D getExtent(void);
        std::vector<const char *> getExtensions(void);

        VkSurfaceKHR createSurface(VkInstance instance);

        void draw(void (*render)(void));

        ~Window(void);
    };
}
