#pragma once

#include "System.hpp"

namespace Window {
    void initialize(std::string title, int width, int height);

    PFN_vkGetInstanceProcAddr getLoader();
    std::vector<const char *> getExtensions();

    vk::SurfaceKHR createSurface(vk::Instance &instance);

    void draw(void (*render)(void));
}
