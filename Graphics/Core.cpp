module;

#include "Graphics.hpp"

module Engine:Graphics;

import :System;

namespace Engine::Graphics {
	PFN_vkGetInstanceProcAddr loader;

	vk::Instance instance;
#ifndef NDEBUG
	vk::DebugUtilsMessengerEXT messenger;
#endif //NDEBUG
	vk::SurfaceKHR surface;

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
		loader = static_cast<PFN_vkGetInstanceProcAddr>(System::getLoader());
		VULKAN_HPP_DEFAULT_DISPATCHER.init(loader);

		const char* title = System::getTitle();

		std::vector<const char*> layers;
		std::vector<const char*> extensions = System::getExtensions();

#ifndef NDEBUG
		layers.push_back("VK_LAYER_KHRONOS_validation");
		extensions.push_back(VK_EXT_DEBUG_UTILS_EXTENSION_NAME);
#endif //NDEBUG

#ifndef NDEBUG
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

		vk::ApplicationInfo applicationInfo {
			.pApplicationName = title,
			.applicationVersion = VK_MAKE_API_VERSION(0, 0, 0, 3),
			.pEngineName = title,
			.engineVersion = VK_MAKE_API_VERSION(0, 0, 0, 3),
			.apiVersion = VK_API_VERSION_1_3
		};

		vk::InstanceCreateInfo instanceInfo {
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
	}

#ifndef NDEBUG
	void createMessenger() {
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

		messenger = instance.createDebugUtilsMessengerEXT(messengerInfo);
	}
#endif //NDEBUG

	void createSurface() {
		VkSurfaceKHR surfaceHandle = System::createSurface(instance);
		surface = vk::SurfaceKHR{ surfaceHandle };
	}

	void createCore() {
		createInstance();
#ifndef NDEBUG
		createMessenger();
#endif //NDEBUG
		createSurface();
	}

	void destroyCore() {
		instance.destroySurfaceKHR(surface);
#ifndef NDEBUG
		instance.destroyDebugUtilsMessengerEXT(messenger);
#endif //NDEBUG
		instance.destroy();

		VULKAN_HPP_DEFAULT_DISPATCHER.init(loader);
	}
}