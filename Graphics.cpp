#include "Graphics.hpp"

// Whatever
vk::DynamicLoader loader;
VULKAN_HPP_DEFAULT_DISPATCH_LOADER_DYNAMIC_STORAGE

#ifndef NDEBUG
#include <iostream>

VKAPI_ATTR VkBool32 VKAPI_CALL messageCallback(VkDebugUtilsMessageSeverityFlagBitsEXT severity,
	VkDebugUtilsMessageTypeFlagsEXT type, const VkDebugUtilsMessengerCallbackDataEXT* pCallbackData,
	void* pUserData) {

	if (severity >= VK_DEBUG_UTILS_MESSAGE_SEVERITY_WARNING_BIT_EXT)
		std::cout << pCallbackData->pMessage << std::endl;

	return VK_FALSE;
}
#endif //NDEBUG

Graphics::Graphics(System system)
{
	VULKAN_HPP_DEFAULT_DISPATCHER.init(system.getInstanceProcAddr());

	std::vector<const char*> layers;
	std::vector<const char*> instanceExtensions = system.getInstanceExtensions();

#ifndef NDEBUG
	layers.push_back("VK_LAYER_KHRONOS_validation");
	instanceExtensions.push_back(VK_EXT_DEBUG_UTILS_EXTENSION_NAME);

	vk::DebugUtilsMessengerCreateInfoEXT messengerInfo{
		.messageSeverity = vk::DebugUtilsMessageSeverityFlagBitsEXT::eVerbose |
							vk::DebugUtilsMessageSeverityFlagBitsEXT::eInfo |
							vk::DebugUtilsMessageSeverityFlagBitsEXT::eWarning |
							vk::DebugUtilsMessageSeverityFlagBitsEXT::eError,
		.messageType = vk::DebugUtilsMessageTypeFlagBitsEXT::eGeneral |
						vk::DebugUtilsMessageTypeFlagBitsEXT::ePerformance |
						vk::DebugUtilsMessageTypeFlagBitsEXT::eValidation,
		.pfnUserCallback = messageCallback
	};
#endif //NDEBUG

	vk::ApplicationInfo applicationInfo{
		.pApplicationName = "Zero",
		.applicationVersion = VK_MAKE_API_VERSION(0, 0, 1, 0),
		.pEngineName = "Zero Engine",
		.engineVersion = VK_MAKE_API_VERSION(0, 0, 1, 0),
		.apiVersion = VK_API_VERSION_1_2
	};

	vk::InstanceCreateInfo instanceInfo{
#ifndef NDEBUG
		.pNext = &messengerInfo,
#endif //NDEBUG
		.pApplicationInfo = &applicationInfo,
		.enabledLayerCount = static_cast<unsigned int>(layers.size()),
		.ppEnabledLayerNames = layers.data(),
		.enabledExtensionCount = static_cast<unsigned int>(instanceExtensions.size()),
		.ppEnabledExtensionNames = instanceExtensions.data()
	};

	instance = vk::createInstance(instanceInfo);
	VULKAN_HPP_DEFAULT_DISPATCHER.init(instance);

#ifndef NDEBUG
	messenger = instance.createDebugUtilsMessengerEXT(messengerInfo);
#endif //NDEBUG
	surface = system.createSurface(instance);

	physicalDevice = instance.enumeratePhysicalDevices().front();
	physicalDevice.getQueueFamilyProperties();

	float queuePriority = 1.0f;
	unsigned int graphicsQueueFamilyIndex = 0;
	unsigned int transferQueueFamilyIndex = 1;

	vk::PhysicalDeviceFeatures deviceFeatures{};
	std::vector<const char*> deviceExtensions{
		VK_KHR_MAINTENANCE_4_EXTENSION_NAME,
		VK_KHR_SWAPCHAIN_EXTENSION_NAME,
		VK_EXT_MULTI_DRAW_EXTENSION_NAME,
		VK_KHR_DYNAMIC_RENDERING_EXTENSION_NAME
	};

	vk::DeviceQueueCreateInfo graphicsQueueInfo{
		.queueFamilyIndex = graphicsQueueFamilyIndex,
		.queueCount = 1,
		.pQueuePriorities = &queuePriority
	};

	vk::DeviceQueueCreateInfo transferQueueInfo{
		.queueFamilyIndex = transferQueueFamilyIndex,
		.queueCount = 1,
		.pQueuePriorities = &queuePriority
	};

	std::array<vk::DeviceQueueCreateInfo, 2> queueInfos{ graphicsQueueInfo, transferQueueInfo };

	vk::DeviceCreateInfo deviceInfo{
		.queueCreateInfoCount = static_cast<unsigned int>(queueInfos.size()),
		.pQueueCreateInfos = queueInfos.data(),
		.enabledExtensionCount = static_cast<unsigned int>(deviceExtensions.size()),
		.ppEnabledExtensionNames = deviceExtensions.data(),
		.pEnabledFeatures = &deviceFeatures
	};

	device = physicalDevice.createDevice(deviceInfo);
	VULKAN_HPP_DEFAULT_DISPATCHER.init(device);

	graphicsQueue = device.getQueue(graphicsQueueFamilyIndex, 0);
	transferQueue = device.getQueue(transferQueueFamilyIndex, 0);
}

Graphics::~Graphics(void) {
	device.destroy();
	instance.destroySurfaceKHR(surface);
#ifndef NDEBUG
	instance.destroyDebugUtilsMessengerEXT(messenger);
#endif //NDEBUG
	instance.destroy();
}
