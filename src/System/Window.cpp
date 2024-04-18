#include "System/Window.hpp"

void Window::initialize(std::string title, int width, int height) {
	this->title = title;
    
    SDL_Init(SDL_INIT_EVERYTHING);
	SDL_Vulkan_LoadLibrary(nullptr);
	SDL_LogSetAllPriority(SDL_LOG_PRIORITY_VERBOSE);

	window = SDL_CreateWindow(title.c_str(), SDL_WINDOWPOS_UNDEFINED, SDL_WINDOWPOS_UNDEFINED, width, height, SDL_WINDOW_VULKAN);
	SDL_Vulkan_GetDrawableSize(window, reinterpret_cast<int32_t *>(&extent.width), reinterpret_cast<int32_t *>(&extent.height));
}

const char *Window::getTitle() {
    return title.c_str();
}

PFN_vkGetInstanceProcAddr Window::getLoader() {
    return reinterpret_cast<PFN_vkGetInstanceProcAddr>(SDL_Vulkan_GetVkGetInstanceProcAddr());
}

std::vector<const char *> Window::getExtensions() {
    uint32_t extensionCount;
	SDL_Vulkan_GetInstanceExtensions(window, &extensionCount, nullptr);
    
    std::vector<const char *> extensions{ extensionCount };
	SDL_Vulkan_GetInstanceExtensions(window, &extensionCount, extensions.data());

    return extensions;
}

vk::Extent2D Window::getExtent() {
    return extent;
}

vk::SurfaceKHR Window::createSurface(vk::Instance &instance) {
    VkSurfaceKHR surface;
	SDL_Vulkan_CreateSurface(window, instance, &surface);
	return surface;
}