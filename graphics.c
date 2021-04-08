#include "graphics.h"

#include <stdio.h>
#include <string.h>

VkInstance instance;
VkSurfaceKHR surface;

VkDebugUtilsMessengerEXT messenger;
PFN_vkCreateDebugUtilsMessengerEXT createDebugUtilsMessenger;
PFN_vkDestroyDebugUtilsMessengerEXT destroyDebugUtilsMessenger;

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

VkDebugUtilsMessengerCreateInfoEXT getMessengerInfo(void) {
	return (VkDebugUtilsMessengerCreateInfoEXT){
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
}

void createInstance(uint32_t contextExtensionCount, const char **contextExtensionNames, VkInstance *instanceHandle) {
	uint32_t layerCount = 1u, extensionCount = contextExtensionCount + 1;
	const char *layerNames[] = {"VK_LAYER_KHRONOS_validation"}, *extensionNames[extensionCount];
	memcpy(extensionNames, contextExtensionNames, (extensionCount - 1) * sizeof(const char *));
	extensionNames[extensionCount - 1] = VK_EXT_DEBUG_UTILS_EXTENSION_NAME;

	VkDebugUtilsMessengerCreateInfoEXT messengerInfo = getMessengerInfo();

	VkApplicationInfo applicationInfo = {
			.sType = VK_STRUCTURE_TYPE_APPLICATION_INFO,
			.pApplicationName = "Zero",
			.applicationVersion = VK_MAKE_VERSION(0, 1, 0),
			.pEngineName = "Zero Engine",
			.engineVersion = VK_MAKE_VERSION(0, 1, 0),
			.apiVersion = VK_API_VERSION_1_2
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
	*instanceHandle = instance;
}

void bindFunctions(PFN_vkCreateDebugUtilsMessengerEXT messengerCreator,
				   PFN_vkDestroyDebugUtilsMessengerEXT messengerDestroyer) {
	createDebugUtilsMessenger = messengerCreator;
	destroyDebugUtilsMessenger = messengerDestroyer;
}

void createMessenger(void) {
	VkDebugUtilsMessengerCreateInfoEXT messengerInfo = getMessengerInfo();
	createDebugUtilsMessenger(instance, &messengerInfo, NULL, &messenger);
}

void bindSurface(VkSurfaceKHR surfaceHandle) {
	surface = surfaceHandle;
}

void startRenderer(void) {

}

void destroyRenderer(void) {
	vkDestroySurfaceKHR(instance, surface, NULL);
	destroyDebugUtilsMessenger(instance, messenger, NULL);
	vkDestroyInstance(instance, NULL);
}