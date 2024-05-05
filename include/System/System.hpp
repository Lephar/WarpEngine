#pragma once

#define VULKAN_HPP_NO_SETTERS
#define VULKAN_HPP_NO_SMART_HANDLE
#define VULKAN_HPP_NO_CONSTRUCTORS
#define VULKAN_HPP_NO_NODISCARD_WARNINGS
#define VULKAN_HPP_NO_SPACESHIP_OPERATOR

#define VULKAN_HPP_DISPATCH_LOADER_DYNAMIC 1

#include <SDL2/SDL.h>
#include <SDL2/SDL_vulkan.h>
#include <vulkan/vulkan.hpp>

namespace System {
    void initialize(std::string title, int width, int height);

    PFN_vkGetInstanceProcAddr getLoader();
    std::vector<const char *> getExtensions();

    vk::SurfaceKHR createSurface(vk::Instance &instance);

    void draw(void (*render)(void));
}
