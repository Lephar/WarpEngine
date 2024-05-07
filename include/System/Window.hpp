#pragma once

#include "System.hpp"

namespace System {
    void initialize(std::string title, int width, int height);

    PFN_vkGetInstanceProcAddr getLoader();
    std::vector<const char *> getExtensions();

    vk::SurfaceKHR createSurface(vk::Instance &instance);

    void draw(void (*render)(void));
}


struct Window {


};
