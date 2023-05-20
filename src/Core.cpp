#include <Core.hpp>

#include <algorithm>

VULKAN_HPP_DEFAULT_DISPATCH_LOADER_DYNAMIC_STORAGE;

#ifndef NDEBUG
VKAPI_ATTR VkBool32 VKAPI_CALL messageCallback(VkDebugUtilsMessageSeverityFlagBitsEXT severity,
	VkDebugUtilsMessageTypeFlagsEXT type, const VkDebugUtilsMessengerCallbackDataEXT* pCallbackData,
	void* pUserData) {
	if(severity == VK_DEBUG_UTILS_MESSAGE_SEVERITY_VERBOSE_BIT_EXT)
		SDL_LogMessage(SDL_LOG_CATEGORY_RENDER, SDL_LOG_PRIORITY_VERBOSE, "%s\n", pCallbackData->pMessage);
	else if(severity == VK_DEBUG_UTILS_MESSAGE_SEVERITY_INFO_BIT_EXT)
		SDL_LogMessage(SDL_LOG_CATEGORY_RENDER, SDL_LOG_PRIORITY_INFO, "%s\n", pCallbackData->pMessage);
	else if(severity == VK_DEBUG_UTILS_MESSAGE_SEVERITY_WARNING_BIT_EXT)
		SDL_LogMessage(SDL_LOG_CATEGORY_RENDER, SDL_LOG_PRIORITY_WARN, "%s\n", pCallbackData->pMessage);
	else if(severity == VK_DEBUG_UTILS_MESSAGE_SEVERITY_ERROR_BIT_EXT)
		SDL_LogMessage(SDL_LOG_CATEGORY_RENDER, SDL_LOG_PRIORITY_ERROR, "%s\n", pCallbackData->pMessage);

	return VK_FALSE;
}
#endif // NDEBUG

Core::Core(const char* title, unsigned int width, unsigned int height) : title(title), extent{width, height} {
	createWindow();
	createInstance();
	createSurface();
	createDevice();
	createSwapchain();
}

void Core::createWindow() {
	SDL_Init(SDL_INIT_EVERYTHING);
	SDL_Vulkan_LoadLibrary(nullptr);
	SDL_LogSetAllPriority(SDL_LOG_PRIORITY_VERBOSE);

	window = SDL_CreateWindow(title.c_str(), SDL_WINDOWPOS_UNDEFINED, SDL_WINDOWPOS_UNDEFINED, extent.width, extent.height, SDL_WINDOW_VULKAN);
	SDL_Vulkan_GetDrawableSize(window, reinterpret_cast<int *>(&extent.width), reinterpret_cast<int *>(&extent.height));

	loader = reinterpret_cast<PFN_vkGetInstanceProcAddr>(SDL_Vulkan_GetVkGetInstanceProcAddr());
	VULKAN_HPP_DEFAULT_DISPATCHER.init(loader);
}

void Core::createInstance() {
	uint32_t extensionCount;
	SDL_Vulkan_GetInstanceExtensions(window, &extensionCount, nullptr);

	std::vector<const char*> layers;
	std::vector<const char*> extensions{ extensionCount };
	SDL_Vulkan_GetInstanceExtensions(window, &extensionCount, extensions.data());

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
		.pApplicationName = title.c_str(),
		.applicationVersion = VK_MAKE_API_VERSION(0, 0, 0, 1),
		.pEngineName = title.c_str(),
		.engineVersion = VK_MAKE_API_VERSION(0, 0, 0, 1),
		.apiVersion = VK_API_VERSION_1_3
	};

	vk::InstanceCreateInfo instanceInfo {
#ifndef NDEBUG
		.pNext = &messengerInfo,
#endif // NDEBUG
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
#endif // NDEBUG
}

void Core::createSurface() {
	VkSurfaceKHR surfaceHandle;
	SDL_Vulkan_CreateSurface(window, instance, &surfaceHandle);
	surface = surfaceHandle;
}

vk::PhysicalDevice Core::pickPhysicalDevice() {
	auto physicalDevices = instance.enumeratePhysicalDevices();

	for (auto& physicalDeviceCandidate : physicalDevices)
		if (physicalDeviceCandidate.getProperties().deviceType == vk::PhysicalDeviceType::eDiscreteGpu)
			return physicalDeviceCandidate;

	return physicalDevices.front();
}

void Core::selectQueueFamilies(unsigned int& transferQueueFamilyIndex, unsigned int& graphicsQueueFamilyIndex) {
	auto queueFamilies = physicalDevice.getQueueFamilyProperties();

	bool specializedTransferQueue = false;

	for(unsigned int queueFamilyIndex = 0; queueFamilyIndex < queueFamilies.size(); queueFamilyIndex++) {
		auto& queueFamily = queueFamilies.at(queueFamilyIndex);
		
		if((queueFamily.queueFlags & vk::QueueFlagBits::eTransfer) && !(queueFamily.queueFlags & vk::QueueFlagBits::eGraphics)) {
			transferQueueFamilyIndex = queueFamilyIndex;
			specializedTransferQueue = true;
			break;
		}
	}

	if(!specializedTransferQueue) {
		for(unsigned int queueFamilyIndex = 0; queueFamilyIndex < queueFamilies.size(); queueFamilyIndex++) {
			auto& queueFamily = queueFamilies.at(queueFamilyIndex);
			
			if(queueFamily.queueFlags & vk::QueueFlagBits::eTransfer) {
				transferQueueFamilyIndex = queueFamilyIndex;
				break;
			}
		}
	}

	for(unsigned int queueFamilyIndex = 0; queueFamilyIndex < queueFamilies.size(); queueFamilyIndex++) {
		auto& queueFamily = queueFamilies.at(queueFamilyIndex);

		if(queueFamily.queueFlags & vk::QueueFlagBits::eGraphics) {
			graphicsQueueFamilyIndex = queueFamilyIndex;
			break;
		}
	}
}

void Core::createDevice() {
	physicalDevice = pickPhysicalDevice();

	unsigned int transferQueueFamilyIndex = 0;
	unsigned int graphicsQueueFamilyIndex = 0;

	selectQueueFamilies(transferQueueFamilyIndex, graphicsQueueFamilyIndex);

	bool sharedQueue = transferQueueFamilyIndex == graphicsQueueFamilyIndex;

	vk::PhysicalDeviceFeatures deviceFeatures{};

	std::vector<const char*> deviceExtensions {
		VK_KHR_SWAPCHAIN_EXTENSION_NAME,
		VK_KHR_DEFERRED_HOST_OPERATIONS_EXTENSION_NAME,
		VK_KHR_ACCELERATION_STRUCTURE_EXTENSION_NAME,
		VK_KHR_RAY_TRACING_PIPELINE_EXTENSION_NAME,
		VK_KHR_RAY_TRACING_MAINTENANCE_1_EXTENSION_NAME
	};

	vk::PhysicalDeviceDynamicRenderingFeatures dynamicRenderingFeatures {
		.dynamicRendering = true
	};

	std::vector<vk::DeviceQueueCreateInfo> queueInfos;
	
	if(sharedQueue) {
		std::array<float, 2> queuePriorities{1.0f, 1.0f};
		
		vk::DeviceQueueCreateInfo queueInfo {
			.queueFamilyIndex = graphicsQueueFamilyIndex,
			.queueCount = 2,
			.pQueuePriorities = queuePriorities.data()
		};
		
		queueInfos.push_back(queueInfo);
	} else {
		float queuePriority = 1.0f;

		vk::DeviceQueueCreateInfo transferQueueInfo{
			.queueFamilyIndex = transferQueueFamilyIndex,
			.queueCount = 1,
			.pQueuePriorities = &queuePriority
		};

		queueInfos.push_back(transferQueueInfo);

		vk::DeviceQueueCreateInfo graphicsQueueInfo {
			.queueFamilyIndex = graphicsQueueFamilyIndex,
			.queueCount = 1,
			.pQueuePriorities = &queuePriority
		};

		queueInfos.push_back(graphicsQueueInfo);
	}

	vk::DeviceCreateInfo deviceInfo {
		.pNext = &dynamicRenderingFeatures,
		.queueCreateInfoCount = static_cast<unsigned int>(queueInfos.size()),
		.pQueueCreateInfos = queueInfos.data(),
		.enabledExtensionCount = static_cast<unsigned int>(deviceExtensions.size()),
		.ppEnabledExtensionNames = deviceExtensions.data(),
		.pEnabledFeatures = &deviceFeatures
	};

	device = physicalDevice.createDevice(deviceInfo);
	VULKAN_HPP_DEFAULT_DISPATCHER.init(device);

	transferQueue = device.getQueue(transferQueueFamilyIndex, 0);

	if(sharedQueue)
		graphicsQueue = device.getQueue(transferQueueFamilyIndex, 1);
	else
		graphicsQueue = device.getQueue(graphicsQueueFamilyIndex, 0);
}

void Core::createSwapchain() {
	auto surfaceCapabilities = physicalDevice.getSurfaceCapabilitiesKHR(surface);
	auto surfaceFormat = physicalDevice.getSurfaceFormatsKHR(surface).back();
	auto presentMode = physicalDevice.getSurfacePresentModesKHR(surface).back();

	vk::SwapchainCreateInfoKHR swapchainInfo {
		.surface = surface,
		.minImageCount = std::clamp(3u, surfaceCapabilities.minImageCount, surfaceCapabilities.maxImageCount),
		.imageFormat = surfaceFormat.format,
		.imageColorSpace = surfaceFormat.colorSpace,
		.imageExtent = extent,
		.imageArrayLayers = 1,
		.imageUsage = vk::ImageUsageFlagBits::eColorAttachment,
		.imageSharingMode = vk::SharingMode::eExclusive,
		.queueFamilyIndexCount = 0,
		.pQueueFamilyIndices = nullptr,
		.preTransform = surfaceCapabilities.currentTransform,
		.compositeAlpha = vk::CompositeAlphaFlagBitsKHR::eOpaque,
		.presentMode = presentMode,
		.clipped = true,
		.oldSwapchain = nullptr
	};

	swapchain = device.createSwapchainKHR(swapchainInfo);
	swapchainImages = device.getSwapchainImagesKHR(swapchain);
}

void Core::draw(void (*render)(void)) {
	while (true) {
		SDL_Event event;
		SDL_PollEvent(&event);

		if(event.type == SDL_QUIT || (event.type == SDL_KEYDOWN && event.key.keysym.sym == SDLK_ESCAPE))
			break;

		if(render)
			render();
	}
}

Core::~Core() {
	device.destroySwapchainKHR(swapchain);

	device.destroy();

	VULKAN_HPP_DEFAULT_DISPATCHER.init(instance);

	instance.destroySurfaceKHR(surface);
#ifndef NDEBUG
	instance.destroyDebugUtilsMessengerEXT(messenger);
#endif // NDEBUG
	instance.destroy();

	VULKAN_HPP_DEFAULT_DISPATCHER.init(loader);

	SDL_DestroyWindow(window);
	SDL_Vulkan_UnloadLibrary();
	SDL_Quit();
}
