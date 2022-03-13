module;

#include "Graphics.hpp"

module Engine:Graphics;

import :System;

VULKAN_HPP_DEFAULT_DISPATCH_LOADER_DYNAMIC_STORAGE

namespace Engine::Graphics {
	vk::Instance instance;
#ifndef NDEBUG
	vk::DebugUtilsMessengerEXT messenger;
#endif //NDEBUG
	vk::SurfaceKHR surface;

	vk::Device device;
	Queue transferQueue;
	Queue graphicsQueue;

	Memory sharedMemory;
	Memory deviceMemory;

	Swapchain swapchain;

#ifndef NDEBUG
	VKAPI_ATTR VkBool32 VKAPI_CALL messageCallback(VkDebugUtilsMessageSeverityFlagBitsEXT severity,
		VkDebugUtilsMessageTypeFlagsEXT type, const VkDebugUtilsMessengerCallbackDataEXT* pCallbackData,
		void* pUserData) {

		switch (severity)
		{
		case VK_DEBUG_UTILS_MESSAGE_SEVERITY_VERBOSE_BIT_EXT:
		case VK_DEBUG_UTILS_MESSAGE_SEVERITY_INFO_BIT_EXT:
			break;
		case VK_DEBUG_UTILS_MESSAGE_SEVERITY_WARNING_BIT_EXT:
			std::cout << pCallbackData->pMessage << std::endl;
			break;
		case VK_DEBUG_UTILS_MESSAGE_SEVERITY_ERROR_BIT_EXT:
			std::cerr << pCallbackData->pMessage << std::endl;
			break;
		}

		return VK_FALSE;
	}
#endif //NDEBUG

	void createInstance() {
		VULKAN_HPP_DEFAULT_DISPATCHER.init(
			static_cast<PFN_vkGetInstanceProcAddr>(System::getLoader()));

		const char* title = System::getTitle();

		std::vector<const char*> layers;
		std::vector<const char*> extensions = System::getExtensions();

#ifndef NDEBUG
		layers.push_back("VK_LAYER_KHRONOS_validation");
		extensions.push_back(VK_EXT_DEBUG_UTILS_EXTENSION_NAME);

		vk::DebugUtilsMessengerCreateInfoEXT messengerInfo{
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
#endif //NDEBUG

		vk::ApplicationInfo applicationInfo{
			.pApplicationName = title,
			.applicationVersion = VK_MAKE_API_VERSION(0, 0, 0, 3),
			.pEngineName = title,
			.engineVersion = VK_MAKE_API_VERSION(0, 0, 0, 3),
			.apiVersion = VK_API_VERSION_1_3
		};

		vk::InstanceCreateInfo instanceInfo{
#ifndef NDEBUG
			.pNext = &messengerInfo,
#endif //NDEBUG
			.pApplicationInfo = &applicationInfo,
			.enabledLayerCount = static_cast<unsigned int>(layers.size()),
			.ppEnabledLayerNames = layers.data(),
			.enabledExtensionCount = static_cast<unsigned int>(extensions.size()),
			.ppEnabledExtensionNames = extensions.data()
		};

		instance = vk::createInstance(instanceInfo);
		VULKAN_HPP_DEFAULT_DISPATCHER.init(instance);

#ifndef NDEBUG
		messenger = instance.createDebugUtilsMessengerEXT(messengerInfo);
#endif //NDEBUG

		surface = vk::SurfaceKHR{ System::createSurface(instance) };
	}

	void createDevice() {
		// TODO: Better physical device selection
		vk::PhysicalDevice physicalDevice = instance.enumeratePhysicalDevices().front();
		physicalDevice.getQueueFamilyProperties();

		// TODO: Better queue family selection
		unsigned int transferQueueFamilyIndex = 1;
		unsigned int graphicsQueueFamilyIndex = 0;

		float queuePriority = 1.0f;

		vk::PhysicalDeviceFeatures deviceFeatures{};
		std::vector<const char*> deviceExtensions{ VK_KHR_SWAPCHAIN_EXTENSION_NAME };
		vk::PhysicalDeviceDynamicRenderingFeatures dynamicRenderingFeatures{
			.dynamicRendering = true
		};

		std::array<vk::DeviceQueueCreateInfo, 2> queueInfos{
			vk::DeviceQueueCreateInfo {
				.queueFamilyIndex = transferQueueFamilyIndex,
				.queueCount = 1,
				.pQueuePriorities = &queuePriority
			},
			vk::DeviceQueueCreateInfo {
				.queueFamilyIndex = graphicsQueueFamilyIndex,
				.queueCount = 1,
				.pQueuePriorities = &queuePriority
			}
		};

		vk::DeviceCreateInfo deviceInfo{
			.pNext = &dynamicRenderingFeatures,
			.queueCreateInfoCount = static_cast<unsigned int>(queueInfos.size()),
			.pQueueCreateInfos = queueInfos.data(),
			.enabledExtensionCount = static_cast<unsigned int>(deviceExtensions.size()),
			.ppEnabledExtensionNames = deviceExtensions.data(),
			.pEnabledFeatures = &deviceFeatures
		};

		device = physicalDevice.createDevice(deviceInfo);
		VULKAN_HPP_DEFAULT_DISPATCHER.init(device);

		transferQueue = Queue{ transferQueueFamilyIndex };
		graphicsQueue = Queue{ graphicsQueueFamilyIndex };
	}

	void allocateMemory() {
		unsigned int deviceMemoryIndex = 7;
		unsigned int sharedMemoryIndex = 10;

		deviceMemory = Memory{ deviceMemoryIndex, 134217728 };
		sharedMemory = Memory{ sharedMemoryIndex, 33554432 };
	}

	void createSwapchain() {
		unsigned int imageCount = 3;
		unsigned int activeImageCount = 2;

		swapchain = Swapchain{ imageCount, activeImageCount, deviceMemory };
	}

	void initialize() {
		createInstance();
		createDevice();
		allocateMemory();
		createSwapchain();
	}

	void terminate() {
		swapchain.destroy();

		sharedMemory.free();
		deviceMemory.free();

		graphicsQueue.destroy();
		transferQueue.destroy();

		device.destroy();

		instance.destroySurfaceKHR(surface);
#ifndef NDEBUG
		instance.destroyDebugUtilsMessengerEXT(messenger);
#endif //NDEBUG
		instance.destroy();
	}
}
