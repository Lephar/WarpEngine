#include "graphics.h"

#include <stdio.h>
#include <string.h>

VkInstance instance;
VkSurfaceKHR surface;
VkPhysicalDevice physicalDevice;
uint32_t queueFamilyIndex;

VkDebugUtilsMessengerEXT messenger;

VKAPI_ATTR VkBool32 VKAPI_CALL messageCallback(VkDebugUtilsMessageSeverityFlagBitsEXT severity,
											   VkDebugUtilsMessageTypeFlagsEXT type,
											   const VkDebugUtilsMessengerCallbackDataEXT *pCallbackData,
											   void *pUserData) {
	(void) type;
	(void) pUserData;

	if (severity >= VK_DEBUG_UTILS_MESSAGE_SEVERITY_WARNING_BIT_EXT)
		printf("%s\n", pCallbackData->pMessage);

	return VK_FALSE;
}

void createInstance(uint32_t contextExtensionCount, const char **contextExtensionNames, VkInstance *instanceHandle) {
	uint32_t layerCount = 1u, extensionCount = contextExtensionCount + 1;
	const char *layerNames[] = {"VK_LAYER_KHRONOS_validation"}, *extensionNames[extensionCount];
	memcpy(extensionNames, contextExtensionNames, (extensionCount - 1) * sizeof(const char *));
	extensionNames[extensionCount - 1] = VK_EXT_DEBUG_UTILS_EXTENSION_NAME;

	VkApplicationInfo applicationInfo = {
			.sType = VK_STRUCTURE_TYPE_APPLICATION_INFO,
			.pApplicationName = "Zero",
			.applicationVersion = VK_MAKE_VERSION(0, 1, 0),
			.pEngineName = "Zero Engine",
			.engineVersion = VK_MAKE_VERSION(0, 1, 0),
			.apiVersion = VK_API_VERSION_1_2
	};

	VkDebugUtilsMessengerCreateInfoEXT messengerInfo = {
			.sType = VK_STRUCTURE_TYPE_DEBUG_UTILS_MESSENGER_CREATE_INFO_EXT,
			.messageSeverity = VK_DEBUG_UTILS_MESSAGE_SEVERITY_VERBOSE_BIT_EXT |
							   VK_DEBUG_UTILS_MESSAGE_SEVERITY_INFO_BIT_EXT |
							   VK_DEBUG_UTILS_MESSAGE_SEVERITY_WARNING_BIT_EXT |
							   VK_DEBUG_UTILS_MESSAGE_SEVERITY_ERROR_BIT_EXT,
			.messageType = VK_DEBUG_UTILS_MESSAGE_TYPE_GENERAL_BIT_EXT |
						   VK_DEBUG_UTILS_MESSAGE_TYPE_VALIDATION_BIT_EXT |
						   VK_DEBUG_UTILS_MESSAGE_TYPE_PERFORMANCE_BIT_EXT,
			.pfnUserCallback = messageCallback
	};

	VkInstanceCreateInfo instanceInfo = {
			.sType = VK_STRUCTURE_TYPE_INSTANCE_CREATE_INFO,
			.pApplicationInfo = &applicationInfo,
			.enabledLayerCount = layerCount,
			.ppEnabledLayerNames = layerNames,
			.enabledExtensionCount = extensionCount,
			.ppEnabledExtensionNames = extensionNames,
			.pNext = &messengerInfo
	};

	vkCreateInstance(&instanceInfo, NULL, &instance);
	PFN_vkCreateDebugUtilsMessengerEXT createDebugUtilsMessenger = (PFN_vkCreateDebugUtilsMessengerEXT)
			vkGetInstanceProcAddr(instance, "vkCreateDebugUtilsMessengerEXT");
	createDebugUtilsMessenger(instance, &messengerInfo, NULL, &messenger);
	*instanceHandle = instance;
}

void bindSurface(VkSurfaceKHR surfaceHandle) {
	surface = surfaceHandle;
}

//TODO: Implement a better device selection
VkPhysicalDevice pickPhysicalDevice() {
	uint32_t physicalDeviceCount = 0u;
	vkEnumeratePhysicalDevices(instance, &physicalDeviceCount, NULL);
	VkPhysicalDevice physicalDevices[physicalDeviceCount];
	vkEnumeratePhysicalDevices(instance, &physicalDeviceCount, physicalDevices);

	for (uint32_t index = 0u; index <= physicalDeviceCount; index++) {
		VkPhysicalDeviceProperties physicalDeviceProperties;
		vkGetPhysicalDeviceProperties(physicalDevices[index], &physicalDeviceProperties);

		if (physicalDeviceProperties.deviceType == VK_PHYSICAL_DEVICE_TYPE_DISCRETE_GPU)
			return physicalDevices[index];
	}

	return physicalDevices[0];
}

//TODO: Add exclusive queue family support
uint32_t selectQueueFamily() {
	uint32_t queueFamilyPropertyCount = 0u;
	vkGetPhysicalDeviceQueueFamilyProperties(physicalDevice, &queueFamilyPropertyCount, NULL);
	VkQueueFamilyProperties queueFamilyProperties[queueFamilyPropertyCount];
	vkGetPhysicalDeviceQueueFamilyProperties(physicalDevice, &queueFamilyPropertyCount, queueFamilyProperties);

	for (uint32_t index = 0u; index < queueFamilyPropertyCount; index++) {
		VkBool32 presentSupport = VK_FALSE;
		vkGetPhysicalDeviceSurfaceSupportKHR(physicalDevice, index, surface, &presentSupport);
		VkQueueFlags graphicsSupport = queueFamilyProperties[index].queueFlags & VK_QUEUE_GRAPHICS_BIT;

		if (presentSupport && graphicsSupport)
			return index;
	}

	return UINT32_MAX;
}

void startRenderer(void) {
	physicalDevice = pickPhysicalDevice();
	queueFamilyIndex = selectQueueFamily();
}

void destroyRenderer(void) {
	vkDestroySurfaceKHR(instance, surface, NULL);
	PFN_vkDestroyDebugUtilsMessengerEXT destroyDebugUtilsMessenger = (PFN_vkDestroyDebugUtilsMessengerEXT)
			vkGetInstanceProcAddr(instance, "vkDestroyDebugUtilsMessengerEXT");
	destroyDebugUtilsMessenger(instance, messenger, NULL);
	vkDestroyInstance(instance, NULL);
}