#include "Graphics/Graphics.hpp"

//#include "Graphics/Device.hpp"
#include "Graphics/Memory.hpp"
#include "Graphics/Buffer.hpp"
#include "Graphics/Framebuffer.hpp"

VULKAN_HPP_DEFAULT_DISPATCH_LOADER_DYNAMIC_STORAGE;

namespace Graphics {
	PFN_vkGetInstanceProcAddr loader;

	vk::Instance instance;
#ifndef NDEBUG
	vk::DebugUtilsMessengerEXT messenger;
#endif // NDEBUG
	vk::SurfaceKHR surface;

//	DeviceInfo deviceInfo;
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

#ifndef NDEBUG
	SDL_LogPriority convertLogSeverity(VkDebugUtilsMessageSeverityFlagBitsEXT severity) {
		if(severity == VK_DEBUG_UTILS_MESSAGE_SEVERITY_ERROR_BIT_EXT)
			return SDL_LOG_PRIORITY_ERROR;
		else if(severity == VK_DEBUG_UTILS_MESSAGE_SEVERITY_WARNING_BIT_EXT)
			return SDL_LOG_PRIORITY_WARN;
		else if(severity == VK_DEBUG_UTILS_MESSAGE_SEVERITY_INFO_BIT_EXT)
			return SDL_LOG_PRIORITY_INFO;
		else if(severity == VK_DEBUG_UTILS_MESSAGE_SEVERITY_VERBOSE_BIT_EXT)
			return SDL_LOG_PRIORITY_VERBOSE;
		else
			return SDL_LOG_PRIORITY_DEBUG;
	}

	VKAPI_ATTR VkBool32 VKAPI_CALL messageCallback(VkDebugUtilsMessageSeverityFlagBitsEXT severity,
		VkDebugUtilsMessageTypeFlagsEXT type, const VkDebugUtilsMessengerCallbackDataEXT* pCallbackData,
		void* pUserData) {
		static_cast<void>(type);
		static_cast<void>(pUserData);

		SDL_LogMessage(SDL_LOG_CATEGORY_RENDER, convertLogSeverity(severity), "\tValidation Layers:\t%s\n", pCallbackData->pMessage);

		return VK_FALSE;
	}
#endif

	void initialize(const char *title, PFN_vkGetInstanceProcAddr loader, std::vector<const char *> extensions) {
		Graphics::loader = loader;
		VULKAN_HPP_DEFAULT_DISPATCHER.init(loader);

		std::vector<const char*> layers;

#ifndef NDEBUG
		layers.push_back("VK_LAYER_KHRONOS_validation");
		extensions.push_back(VK_EXT_DEBUG_UTILS_EXTENSION_NAME);
#endif // NDEBUG

#ifndef NDEBUG
		vk::DebugUtilsMessengerCreateInfoEXT messengerInfo {
			.messageSeverity =
				vk::DebugUtilsMessageSeverityFlagBitsEXT::eVerbose |
				vk::DebugUtilsMessageSeverityFlagBitsEXT::eInfo |
				vk::DebugUtilsMessageSeverityFlagBitsEXT::eWarning |
				vk::DebugUtilsMessageSeverityFlagBitsEXT::eError,
			.messageType =
				vk::DebugUtilsMessageTypeFlagBitsEXT::eGeneral |
				vk::DebugUtilsMessageTypeFlagBitsEXT::ePerformance |
				vk::DebugUtilsMessageTypeFlagBitsEXT::eValidation,
			.pfnUserCallback = messageCallback
		};
#endif // NDEBUG

		vk::ApplicationInfo applicationInfo {
			.pApplicationName = title,
			.applicationVersion = VK_MAKE_API_VERSION(0, 0, 0, 1),
			.pEngineName = title,
			.engineVersion = VK_MAKE_API_VERSION(0, 0, 0, 1),
			.apiVersion = VK_API_VERSION_1_3
		};

		vk::InstanceCreateInfo instanceInfo {
#ifndef NDEBUG
			.pNext = &messengerInfo,
#endif // NDEBUG
			.pApplicationInfo = &applicationInfo,
			.enabledLayerCount = static_cast<uint32_t>(layers.size()),
			.ppEnabledLayerNames = layers.data(),
			.enabledExtensionCount = static_cast<uint32_t>(extensions.size()),
			.ppEnabledExtensionNames = extensions.data()
		};

		instance = vk::createInstance(instanceInfo);
		VULKAN_HPP_DEFAULT_DISPATCHER.init(instance);

#ifndef NDEBUG
		messenger = instance.createDebugUtilsMessengerEXT(messengerInfo);
#endif // NDEBUG
	}

	vk::Instance getInstance(void) {
		return instance;
	}

	void registerSurface(vk::SurfaceKHR surface) {
		Graphics::surface = surface;
	}
}
