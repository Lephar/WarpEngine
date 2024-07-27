#pragma once

#include <vulkan/vulkan_core.h>
#include <vector>

namespace System {
    class Window;

    Window *initialize(const char *title, int32_t width, int32_t height);
    Window *createWindow(const char *title, int32_t width, int32_t height);

    PFN_vkGetInstanceProcAddr getLoader();
    Window *getWindow(size_t index);
    Window *getMainWindow();
    std::vector<const char *> getRequiredExtensions(size_t index = 0);

    void destroyWindow(Window *window);
    void destroyWindow(size_t index = 0);

    void quit();
}
