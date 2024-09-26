#include "surface.h"
#include "helper.h"

extern SDL_bool windowCreated;
SDL_bool surfaceCreated = SDL_FALSE;

extern SDL_Window *window;

extern VkInstance instance;
VkSurfaceKHR surface;

void createSurface() {
    assert(windowCreated && !surfaceCreated);

    SDL_Vulkan_CreateSurface(window, instance, &surface);

    surfaceCreated = SDL_TRUE;
    debug("Surface created");
}

void destroySurface() {
    assert(surfaceCreated);

    vkDestroySurfaceKHR(instance, surface, NULL);

    surfaceCreated = SDL_FALSE;
    debug("Surface destroyed");
}
