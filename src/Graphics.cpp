#include <Graphics.hpp>

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

Graphics::Graphics(const char* title, unsigned int width, unsigned int height) {
	window.title = title;
	window.extent = VkExtent2D{width, height};

	createWindow();
	createInstance();
	createSurface();
	createDevice();
	createSwapchain();
}

void Graphics::createWindow() {
	SDL_Init(SDL_INIT_EVERYTHING);
	SDL_Vulkan_LoadLibrary(nullptr);
	SDL_LogSetAllPriority(SDL_LOG_PRIORITY_VERBOSE);

	window.window = SDL_CreateWindow(window.title.c_str(), SDL_WINDOWPOS_UNDEFINED, SDL_WINDOWPOS_UNDEFINED, window.extent.width, window.extent.height, SDL_WINDOW_VULKAN);
	SDL_Vulkan_GetDrawableSize(window.window, reinterpret_cast<int *>(&window.extent.width), reinterpret_cast<int *>(&window.extent.height));
}

void Graphics::createInstance() {
	core.loader = reinterpret_cast<PFN_vkGetInstanceProcAddr>(SDL_Vulkan_GetVkGetInstanceProcAddr());
	VULKAN_HPP_DEFAULT_DISPATCHER.init(core.loader);
	
	uint32_t extensionCount;
	SDL_Vulkan_GetInstanceExtensions(window.window, &extensionCount, nullptr);

	std::vector<const char*> layers;
	std::vector<const char*> extensions{ extensionCount };
	SDL_Vulkan_GetInstanceExtensions(window.window, &extensionCount, extensions.data());

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
		.pApplicationName = window.title.c_str(),
		.applicationVersion = VK_MAKE_API_VERSION(0, 0, 0, 1),
		.pEngineName = window.title.c_str(),
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

	core.instance = vk::createInstance(instanceInfo);
	VULKAN_HPP_DEFAULT_DISPATCHER.init(core.instance);

#ifndef NDEBUG
	core.messenger = core.instance.createDebugUtilsMessengerEXT(messengerInfo);
#endif // NDEBUG
}

void Graphics::createSurface() {
	VkSurfaceKHR surfaceHandle;
	SDL_Vulkan_CreateSurface(window.window, core.instance, &surfaceHandle);
	core.surface = surfaceHandle;
}

vk::PhysicalDevice Graphics::pickPhysicalDevice() {
	auto physicalDevices = core.instance.enumeratePhysicalDevices();

	for (auto& physicalDeviceCandidate : physicalDevices)
		if (physicalDeviceCandidate.getProperties().deviceType == vk::PhysicalDeviceType::eDiscreteGpu)
			return physicalDeviceCandidate;

	return physicalDevices.front();
}

void Graphics::selectQueueFamilies(unsigned int& transferQueueFamilyIndex, unsigned int& graphicsQueueFamilyIndex) {
	auto queueFamilies = device.physicalDevice.getQueueFamilyProperties();

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

void Graphics::createDevice() {
	device.physicalDevice = pickPhysicalDevice();

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

	device.device = device.physicalDevice.createDevice(deviceInfo);
	VULKAN_HPP_DEFAULT_DISPATCHER.init(device.device);

	swapchain.transferQueue = device.device.getQueue(transferQueueFamilyIndex, 0);

	if(sharedQueue)
		swapchain.graphicsQueue = device.device.getQueue(transferQueueFamilyIndex, 1);
	else
		swapchain.graphicsQueue = device.device.getQueue(graphicsQueueFamilyIndex, 0);
}

void Graphics::createSwapchain() {
	auto surfaceCapabilities = device.physicalDevice.getSurfaceCapabilitiesKHR(core.surface);
	auto surfaceFormat = device.physicalDevice.getSurfaceFormatsKHR(core.surface).back();
	auto presentMode = device.physicalDevice.getSurfacePresentModesKHR(core.surface).back();

	vk::SwapchainCreateInfoKHR swapchainInfo {
		.surface = core.surface,
		.minImageCount = std::clamp(3u, surfaceCapabilities.minImageCount, surfaceCapabilities.maxImageCount),
		.imageFormat = surfaceFormat.format,
		.imageColorSpace = surfaceFormat.colorSpace,
		.imageExtent = window.extent,
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

	swapchain.swapchain = device.device.createSwapchainKHR(swapchainInfo);
	swapchain.swapchainImages = device.device.getSwapchainImagesKHR(swapchain.swapchain);
}

void Graphics::draw(void (*render)(void)) {
	while (true) {
		SDL_Event event;
		SDL_PollEvent(&event);

		if(event.type == SDL_QUIT || (event.type == SDL_KEYDOWN && event.key.keysym.sym == SDLK_ESCAPE))
			break;

		if(render)
			render();
	}
}

Graphics::~Graphics() {
	device.device.destroySwapchainKHR(swapchain.swapchain);

	device.device.destroy();

	VULKAN_HPP_DEFAULT_DISPATCHER.init(core.instance);

	core.instance.destroySurfaceKHR(core.surface);
#ifndef NDEBUG
	core.instance.destroyDebugUtilsMessengerEXT(core.messenger);
#endif // NDEBUG
	core.instance.destroy();

	VULKAN_HPP_DEFAULT_DISPATCHER.init(core.loader);

	SDL_DestroyWindow(window.window);
	SDL_Vulkan_UnloadLibrary();
	SDL_Quit();
}
