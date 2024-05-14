#include "System/Window.hpp"

namespace System {
    std::vector<const char *> Window::getExtensions() {
        uint32_t extensionCount;
        SDL_Vulkan_GetInstanceExtensions(window, &extensionCount, nullptr);

        std::vector<const char *> extensions{ extensionCount };
        SDL_Vulkan_GetInstanceExtensions(window, &extensionCount, extensions.data());

        return extensions;
    }

    vk::SurfaceKHR Window::createSurface(vk::Instance &instance) {
        VkSurfaceKHR surface;
        SDL_Vulkan_CreateSurface(window, instance, &surface);

        return surface;
    }

    void Window::draw(void (*render)(void)) {
        while (true) {
            SDL_Event event;
            SDL_PollEvent(&event);

            if(event.type == SDL_QUIT || (event.type == SDL_KEYDOWN && event.key.keysym.sym == SDLK_ESCAPE))
                break;

            if(render)
                render();
        }
    }
}
