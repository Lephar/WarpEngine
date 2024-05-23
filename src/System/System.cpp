#include "System/System.hpp"
#include "System/Window.hpp"

#include <SDL2/SDL_vulkan.h>
#include <algorithm>

namespace System {
    std::vector<Window *> windows;

    Window *initialize(const char *title, int32_t width, int32_t height) {
        SDL_Init(SDL_INIT_EVERYTHING);
        SDL_Vulkan_LoadLibrary(nullptr);
        SDL_LogSetAllPriority(SDL_LOG_PRIORITY_VERBOSE);

        return createWindow(title, width, height);
    }

    Window *createWindow(const char *title, int32_t width, int32_t height) {
        Window *window = new Window(title, width, height);
        windows.push_back(window);

        return window;
    }

    PFN_vkGetInstanceProcAddr getLoader(void) {
        return reinterpret_cast<PFN_vkGetInstanceProcAddr>(SDL_Vulkan_GetVkGetInstanceProcAddr());
    }

    Window *getWindow(size_t index) {
        return index >= 0 && index < windows.size() ? windows.at(index) : nullptr;
    }

    std::vector<const char *> getExtensions(size_t index) {
        return index >= 0 && index < windows.size() ? windows.at(index)->getExtensions() : std::vector<const char *>();
    }

    void destroyWindow(Window *window) {
        auto position = std::find(windows.begin(), windows.end(), window);

        if(position != windows.end()) {
            delete *position;
            windows.erase(position);
        }
    }

    void destroyWindow(size_t index) {
        if(index < windows.size()) {
            delete windows.at(index);
            windows.erase(windows.begin() + index);
        }
    }

    void quit(void) {
        while(!windows.empty()) {
            delete windows.back();
            windows.pop_back();
        }

        SDL_Vulkan_UnloadLibrary();
        SDL_Quit();
    }
}
