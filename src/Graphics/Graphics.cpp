#include "Graphics/Graphics.hpp"

namespace Graphics {
    PFN_vkGetInstanceProcAddr loader;

	vk::Instance instance;
#ifndef NDEBUG
	vk::DebugUtilsMessengerEXT messenger;
#endif // NDEBUG
	vk::SurfaceKHR surface;

	DeviceInfo deviceInfo;
    vk::PhysicalDevice physicalDevice;
    vk::Device device;
	vk::Queue queue;
	vk::CommandPool commandPool;

	Memory hostMemory;
	Memory deviceMemory;
	Memory imageMemory;

	Buffer uniformBuffer;
	Buffer stagingBuffer;
	Buffer elementBuffer;

	uint32_t swapchainSize;
	vk::SwapchainKHR swapchain;
	std::vector<vk::Image> swapchainImages;

	vk::SampleCountFlagBits sampleCount;
	vk::Format depthStencilFormat;
	vk::Format colorFormat;
	uint32_t mipCount;
	std::vector<Framebuffer> framebuffers;
}