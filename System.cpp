#include "Base.hpp"
#include "System.hpp"

VULKAN_HPP_DEFAULT_DISPATCH_LOADER_DYNAMIC_STORAGE

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

void System::createWindow() {
	SDL_InitSubSystem(SDL_INIT_TIMER | SDL_INIT_AUDIO | SDL_INIT_VIDEO);
	window = SDL_CreateWindow(title.c_str(), SDL_WINDOWPOS_UNDEFINED, SDL_WINDOWPOS_UNDEFINED,
		size.x, size.y, SDL_WINDOW_VULKAN);
	VULKAN_HPP_DEFAULT_DISPATCHER.init(
		static_cast<PFN_vkGetInstanceProcAddr>(SDL_Vulkan_GetVkGetInstanceProcAddr()));
	SDL_Vulkan_GetDrawableSize(window, &size.x, &size.y);
}

void System::createInstance() {
	std::vector<const char*> layers{};
	
	uint32_t extensionCount;
	SDL_Vulkan_GetInstanceExtensions(window, &extensionCount, nullptr);
	std::vector<const char*> extensions{ extensionCount };
	SDL_Vulkan_GetInstanceExtensions(window, &extensionCount, extensions.data());

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
		.pApplicationName = title.c_str(),
		.applicationVersion = VK_MAKE_API_VERSION(0, 0, 1, 0),
		.pEngineName = title.c_str(),
		.engineVersion = VK_MAKE_API_VERSION(0, 0, 1, 0),
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
}

void System::createSurface() {
	VkSurfaceKHR surfaceHandle;
	SDL_Vulkan_CreateSurface(window, instance, &surfaceHandle);
	surface = vk::SurfaceKHR{ surfaceHandle };
}

void System::createDevice() {
	vk::PhysicalDevice physicalDevice = instance.enumeratePhysicalDevices().front();
	physicalDevice.getQueueFamilyProperties();

	float queuePriority = 1.0f;

	uint32_t transferQueueFamilyIndex = 1;
	uint32_t graphicsQueueFamilyIndex = 0;

	vk::PhysicalDeviceFeatures deviceFeatures{};
	std::vector<const char*> deviceExtensions{ VK_KHR_SWAPCHAIN_EXTENSION_NAME };

	vk::DeviceQueueCreateInfo transferQueueInfo{
		.queueFamilyIndex = transferQueueFamilyIndex,
		.queueCount = 1,
		.pQueuePriorities = &queuePriority
	};

	vk::DeviceQueueCreateInfo graphicsQueueInfo{
		.queueFamilyIndex = graphicsQueueFamilyIndex,
		.queueCount = 1,
		.pQueuePriorities = &queuePriority
	};

	std::array<vk::DeviceQueueCreateInfo, 2> queueInfos{ transferQueueInfo, graphicsQueueInfo };

	vk::DeviceCreateInfo deviceInfo{
		.queueCreateInfoCount = static_cast<unsigned int>(queueInfos.size()),
		.pQueueCreateInfos = queueInfos.data(),
		.enabledExtensionCount = static_cast<unsigned int>(deviceExtensions.size()),
		.ppEnabledExtensionNames = deviceExtensions.data(),
		.pEnabledFeatures = &deviceFeatures
	};

	device = physicalDevice.createDevice(deviceInfo);
	VULKAN_HPP_DEFAULT_DISPATCHER.init(device);

	transferQueue = device.getQueue(transferQueueFamilyIndex, 0);
	graphicsQueue = device.getQueue(graphicsQueueFamilyIndex, 0);

	vk::CommandPoolCreateInfo transferCommandPoolInfo{
		.flags = vk::CommandPoolCreateFlagBits::eResetCommandBuffer,
		.queueFamilyIndex = transferQueueFamilyIndex
	};

	vk::CommandPoolCreateInfo graphicsCommandPoolInfo{
		.flags = vk::CommandPoolCreateFlagBits::eResetCommandBuffer,
		.queueFamilyIndex = graphicsQueueFamilyIndex
	};

	transferCommandPool = device.createCommandPool(transferCommandPoolInfo);
	graphicsCommandPool = device.createCommandPool(graphicsCommandPoolInfo);

	vk::CommandBufferAllocateInfo transferCommandBufferInfo{
		.commandPool = transferCommandPool,
		.level = vk::CommandBufferLevel::ePrimary,
		.commandBufferCount = 1
	};

	transferCommandBuffer = device.allocateCommandBuffers(transferCommandBufferInfo).front();
}

System::System(const char* title, int width, int height) : title(title), size(width, height) {
	createWindow();
	createInstance();
	createSurface();
	createDevice();
}

void System::mainLoop(void)
{
	while (true) {
		SDL_Event event;
		SDL_PollEvent(&event);

		if (event.type == SDL_QUIT)
			break;
	}
}

System::~System(void)
{
	device.destroyCommandPool(graphicsCommandPool);
	device.destroyCommandPool(transferCommandPool);
	device.destroy();

	instance.destroySurfaceKHR(surface);
#ifndef NDEBUG
	instance.destroyDebugUtilsMessengerEXT(messenger);
#endif //NDEBUG
	instance.destroy();

	SDL_DestroyWindow(window);
	SDL_Quit();
}
