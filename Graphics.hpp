#pragma once

#include "System.hpp"

class Graphics
{
private:
	vk::Instance instance;
#ifndef NDEBUG
	vk::DebugUtilsMessengerEXT messenger;
#endif
	vk::SurfaceKHR surface;
	vk::PhysicalDevice physicalDevice;
	vk::Device device;
	vk::Queue graphicsQueue;
	vk::Queue transferQueue;
public:
	Graphics(System system);
	~Graphics(void);
};

