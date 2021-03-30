#include "base.h"
#include <stdio.h>
#include <string.h>

GLFWwindow *window;
VkInstance instance;
VkDebugUtilsMessengerEXT messenger;
VkSurfaceKHR surface;

struct dimensions {
	int32_t width;
	int32_t height;
} dimensions;

struct controls {
	uint8_t keyW;
	uint8_t keyS;
	uint8_t keyA;
	uint8_t keyD;
	uint8_t keyQ;
	uint8_t keyE;
	uint8_t keyR;
	uint8_t keyF;
	double mouseX;
	double mouseY;
} controls;

void mouseCallback(GLFWwindow *handle, double x, double y) {
	(void) handle;

	controls.mouseX = x;
	controls.mouseY = y;
}

void keyboardCallback(GLFWwindow *handle, int32_t key, int32_t scancode, int32_t action, int32_t mods) {
	(void) scancode;
	(void) mods;

	if (action == GLFW_RELEASE) {
		if (key == GLFW_KEY_W)
			controls.keyW = 0;
		else if (key == GLFW_KEY_S)
			controls.keyS = 0;
		else if (key == GLFW_KEY_A)
			controls.keyA = 0;
		else if (key == GLFW_KEY_D)
			controls.keyD = 0;
		else if (key == GLFW_KEY_Q)
			controls.keyQ = 0;
		else if (key == GLFW_KEY_E)
			controls.keyE = 0;
		else if (key == GLFW_KEY_R)
			controls.keyR = 0;
		else if (key == GLFW_KEY_F)
			controls.keyF = 0;
	} else if (action == GLFW_PRESS) {
		if (key == GLFW_KEY_W)
			controls.keyW = 1;
		else if (key == GLFW_KEY_S)
			controls.keyS = 1;
		else if (key == GLFW_KEY_A)
			controls.keyA = 1;
		else if (key == GLFW_KEY_D)
			controls.keyD = 1;
		else if (key == GLFW_KEY_Q)
			controls.keyQ = 1;
		else if (key == GLFW_KEY_E)
			controls.keyE = 1;
		else if (key == GLFW_KEY_R)
			controls.keyR = 1;
		else if (key == GLFW_KEY_F)
			controls.keyF = 1;
		else if (key == GLFW_KEY_ESCAPE)
			glfwSetWindowShouldClose(handle, 1);
	}
}

void resizeCallback(GLFWwindow *handle, int32_t width, int32_t height) {
	(void) handle;

	dimensions.width = width;
	dimensions.height = height;
}

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

void initializeBase(void) {
	glfwInit();
	glfwWindowHint(GLFW_CLIENT_API, GLFW_NO_API);
	window = glfwCreateWindow(1280, 720, "Zero", NULL, NULL);

	glfwSetKeyCallback(window, keyboardCallback);
	glfwSetCursorPosCallback(window, mouseCallback);
	glfwSetFramebufferSizeCallback(window, resizeCallback);
	glfwGetCursorPos(window, &controls.mouseX, &controls.mouseY);

	uint32_t layerCount = 1u, extensionCount = 0u;
	const char **extensions = glfwGetRequiredInstanceExtensions(&extensionCount);

	extensionCount += 1;
	const char *layerNames[] = {"VK_LAYER_KHRONOS_validation"}, *extensionNames[extensionCount];
	memcpy(extensionNames, extensions, (extensionCount - 1) * sizeof(const char *));
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
	PFN_vkCreateDebugUtilsMessengerEXT vkCreateDebugUtilsMessengerEXT = (PFN_vkCreateDebugUtilsMessengerEXT)
			glfwGetInstanceProcAddress(instance, "vkCreateDebugUtilsMessengerEXT");
	vkCreateDebugUtilsMessengerEXT(instance, &messengerInfo, NULL, &messenger);
	glfwCreateWindowSurface(instance, window, NULL, &surface);
}

void clearBase(void) {
	vkDestroySurfaceKHR(instance, surface, NULL);
	PFN_vkDestroyDebugUtilsMessengerEXT vkDestroyDebugUtilsMessengerEXT = (PFN_vkDestroyDebugUtilsMessengerEXT)
			glfwGetInstanceProcAddress(instance, "vkDestroyDebugUtilsMessengerEXT");
	vkDestroyDebugUtilsMessengerEXT(instance, messenger, NULL);
	vkDestroyInstance(instance, NULL);
	glfwDestroyWindow(window);
	glfwTerminate();
}
