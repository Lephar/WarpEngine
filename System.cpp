#include "System.hpp"

System::System(const char* title, int width, int height) : title(title), width(width), height(height) {
	SDL_InitSubSystem(SDL_INIT_TIMER | SDL_INIT_AUDIO | SDL_INIT_VIDEO);
	window = SDL_CreateWindow(title, SDL_WINDOWPOS_UNDEFINED, SDL_WINDOWPOS_UNDEFINED, width, height, SDL_WINDOW_VULKAN);
}

std::vector<const char*> System::getInstanceExtensions(void)
{
	unsigned int count;
	SDL_Vulkan_GetInstanceExtensions(window, &count, nullptr);
	
	std::vector<const char*> extensions(count);
	SDL_Vulkan_GetInstanceExtensions(window, &count, extensions.data());

	return extensions;
}

PFN_vkGetInstanceProcAddr System::getInstanceProcAddr(void) {
	return static_cast<PFN_vkGetInstanceProcAddr>(SDL_Vulkan_GetVkGetInstanceProcAddr());
}

vk::SurfaceKHR System::createSurface(vk::Instance instance)
{
	VkSurfaceKHR surface;
	SDL_Vulkan_CreateSurface(window, instance, &surface);

	return vk::SurfaceKHR(surface);
}

void System::mainLoop(void)
{
	while (true) {
		SDL_Event event;
		SDL_PollEvent(&event);

		if (event.type == SDL_QUIT)
			break;
	}
}

System::~System(void)
{
	title = nullptr;
	width = -1;
	height = -1;

	SDL_DestroyWindow(window);
	window = nullptr;

	SDL_Quit();
}
