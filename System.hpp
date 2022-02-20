#pragma once

#include "Base.hpp"

class System
{
private:
	std::string title;
	glm::ivec2 size;
	SDL_Window *window;
	
	vk::Instance instance;
#ifndef NDEBUG
	vk::DebugUtilsMessengerEXT messenger;
#endif //NDEBUG
	vk::SurfaceKHR surface;

	vk::Device device;
	vk::Queue graphicsQueue;
	vk::Queue transferQueue;
	vk::CommandPool transferCommandPool;
	vk::CommandPool graphicsCommandPool;
	vk::CommandBuffer transferCommandBuffer;

	void createWindow(void);
	void createInstance(void);
	void createSurface(void);
	void createDevice(void);
public:
	System(const char* title, int width, int height);
	void mainLoop(void);
	~System(void);
};
