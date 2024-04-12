#include <Graphics.hpp>

VULKAN_HPP_DEFAULT_DISPATCH_LOADER_DYNAMIC_STORAGE;

Graphics::Graphics(std::string title, uint32_t width, uint32_t height) {
	this->title = title;
	this->extent = VkExtent2D{width, height};

	createWindow();
	createInstance();
	createSurface();
	createDevice();
	createSwapchain();
	createFramebuffers();
	createBuffers();
}

void Graphics::createWindow() {
	SDL_Init(SDL_INIT_EVERYTHING);
	SDL_Vulkan_LoadLibrary(nullptr);
	SDL_LogSetAllPriority(SDL_LOG_PRIORITY_VERBOSE);

	window = SDL_CreateWindow(title.c_str(), SDL_WINDOWPOS_UNDEFINED, SDL_WINDOWPOS_UNDEFINED, extent.width, extent.height, SDL_WINDOW_VULKAN);
	SDL_Vulkan_GetDrawableSize(window, reinterpret_cast<int32_t *>(&extent.width), reinterpret_cast<int32_t *>(&extent.height));
}

void Graphics::createInstance() {
	loader = reinterpret_cast<PFN_vkGetInstanceProcAddr>(SDL_Vulkan_GetVkGetInstanceProcAddr());
	VULKAN_HPP_DEFAULT_DISPATCHER.init(loader);

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

void Graphics::createSurface() {
	VkSurfaceKHR surfaceHandle;
	SDL_Vulkan_CreateSurface(window, instance, &surfaceHandle);
	surface = vk::SurfaceKHR(surfaceHandle);
}

void Graphics::createDevice() {
	physicalDevice = pickPhysicalDevice();
	auto queueFamilyIndex = selectQueueFamily();

	memoryProperties = physicalDevice.getMemoryProperties();

	vk::PhysicalDeviceFeatures deviceFeatures{};

	std::vector<const char*> deviceExtensions {
		VK_KHR_SWAPCHAIN_EXTENSION_NAME,
		VK_EXT_SHADER_OBJECT_EXTENSION_NAME
	};

	vk::PhysicalDeviceDynamicRenderingFeatures dynamicRenderingFeatures {
		.dynamicRendering = true
	};

	float queuePriority = 1.0f;

	vk::DeviceQueueCreateInfo queueInfo {
		.queueFamilyIndex = queueFamilyIndex,
		.queueCount = 1,
		.pQueuePriorities = &queuePriority
	};

	vk::DeviceCreateInfo deviceInfo {
		.pNext = &dynamicRenderingFeatures,
		.queueCreateInfoCount = 1,
		.pQueueCreateInfos = &queueInfo,
		.enabledExtensionCount = static_cast<uint32_t>(deviceExtensions.size()),
		.ppEnabledExtensionNames = deviceExtensions.data(),
		.pEnabledFeatures = &deviceFeatures
	};

	device = physicalDevice.createDevice(deviceInfo);
	VULKAN_HPP_DEFAULT_DISPATCHER.init(device);

	queue = device.getQueue(queueFamilyIndex, 0);

	vk::CommandPoolCreateInfo commandPoolInfo {
		.flags = vk::CommandPoolCreateFlagBits::eResetCommandBuffer,
		.queueFamilyIndex = queueFamilyIndex
	};

	commandPool = device.createCommandPool(commandPoolInfo);

	vk::CommandBufferAllocateInfo commandBufferInfo {
		.commandPool = commandPool,
		.level = vk::CommandBufferLevel::ePrimary,
		.commandBufferCount = 1
	};

	mainCommandBuffer = device.allocateCommandBuffers(commandBufferInfo).front();
}

void Graphics::createSwapchain() {
	auto surfaceCapabilities = physicalDevice.getSurfaceCapabilitiesKHR(surface);
	auto surfaceFormat = selectSurfaceFormat();
	auto presentMode = selectPresentMode();

	swapchainSize = std::clamp(3u, surfaceCapabilities.minImageCount, surfaceCapabilities.maxImageCount);

	vk::SwapchainCreateInfoKHR swapchainInfo {
		.surface = surface,
		.minImageCount = swapchainSize,
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

	for(auto& swapchainImage : swapchainImages)
		transitionImageLayout(swapchainImage, vk::ImageAspectFlagBits::eColor, vk::ImageLayout::eUndefined, vk::ImageLayout::ePresentSrcKHR);
}

void Graphics::createFramebuffers() {
	framebufferCount = 2;

	sampleCount = vk::SampleCountFlagBits::e8;

	depthStencilFormat = vk::Format::eD32SfloatS8Uint;
	colorFormat = vk::Format::eR16G16B16A16Sfloat;

	mipCount = 16;

	auto temporaryImage = createImage(extent.width, extent.height, colorFormat, vk::ImageUsageFlagBits::eColorAttachment, vk::SampleCountFlagBits::e8, 1);
	auto memoryRequirements = device.getImageMemoryRequirements(temporaryImage);

	auto typeIndex = chooseMemoryType(memoryRequirements.memoryTypeBits, vk::MemoryPropertyFlagBits::eDeviceLocal);
	auto heapSize = memoryProperties.memoryHeaps[memoryProperties.memoryTypes[typeIndex].heapIndex].size;

	device.destroy(temporaryImage);

	imageMemory.offset = 0;
	imageMemory.size = heapSize / 2;
	imageMemory.memory = allocateMemory(imageMemory.size, typeIndex);

	for(auto framebufferIndex = 0u; framebufferIndex < framebufferCount; framebufferIndex++) {
		auto depthStencil = createImage(extent.width, extent.height, depthStencilFormat, vk::ImageUsageFlagBits::eDepthStencilAttachment, sampleCount, 1);
		auto color = createImage(extent.width, extent.height, colorFormat, vk::ImageUsageFlagBits::eTransientAttachment | vk::ImageUsageFlagBits::eColorAttachment, sampleCount, 1);
		auto resolve = createImage(extent.width, extent.height, colorFormat, vk::ImageUsageFlagBits::eColorAttachment | vk::ImageUsageFlagBits::eTransferSrc, vk::SampleCountFlagBits::e1, 1);

		bindImageMemory(depthStencil);
		bindImageMemory(color);
		bindImageMemory(resolve);

		transitionImageLayout(resolve, vk::ImageAspectFlagBits::eColor, vk::ImageLayout::eUndefined, vk::ImageLayout::eColorAttachmentOptimal);

		auto depthStencilView = createImageView(depthStencil, depthStencilFormat, vk::ImageAspectFlagBits::eDepth | vk::ImageAspectFlagBits::eStencil, 1);
		auto colorView = createImageView(color, colorFormat, vk::ImageAspectFlagBits::eColor, 1);
		auto resolveView = createImageView(resolve, colorFormat, vk::ImageAspectFlagBits::eColor, 1);

		Framebuffer frambuffer{
			.depthStencil = Image {
				.image = depthStencil,
				.view = depthStencilView
			},
			.color = Image {
				.image = color,
				.view = colorView
			},
			.resolve = Image {
				.image = resolve,
				.view = resolveView
			}
		};

		framebuffers.push_back(frambuffer);
	}
}

void Graphics::createBuffers() {
	auto temporaryHostBuffer = createBuffer(extent.width * extent.height * 4, vk::BufferUsageFlagBits::eTransferSrc | vk::BufferUsageFlagBits::eUniformBuffer);
	auto hostMemoryRequirements = device.getBufferMemoryRequirements(temporaryHostBuffer);

	auto hostTypeIndex = chooseMemoryType(hostMemoryRequirements.memoryTypeBits, vk::MemoryPropertyFlagBits::eHostVisible | vk::MemoryPropertyFlagBits::eHostCoherent);
	auto hostHeapSize = memoryProperties.memoryHeaps[memoryProperties.memoryTypes[hostTypeIndex].heapIndex].size;

	hostMemory.memory = allocateMemory(hostHeapSize / 2, hostTypeIndex);

	device.destroyBuffer(temporaryHostBuffer);

	auto temporaryDeviceBuffer = createBuffer(extent.width * extent.height * 4, vk::BufferUsageFlagBits::eTransferDst | vk::BufferUsageFlagBits::eIndexBuffer | vk::BufferUsageFlagBits::eVertexBuffer);
	auto deviceMemoryRequirements = device.getBufferMemoryRequirements(temporaryDeviceBuffer);

	auto deviceTypeIndex = chooseMemoryType(deviceMemoryRequirements.memoryTypeBits, vk::MemoryPropertyFlagBits::eDeviceLocal);
	auto deviceHeapSize = memoryProperties.memoryHeaps[memoryProperties.memoryTypes[hostTypeIndex].heapIndex].size;

	hostMemory.memory = allocateMemory(deviceHeapSize / 2, deviceTypeIndex);
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
	for(auto framebuffer : framebuffers) {
		device.destroyImageView(framebuffer.resolve.view);
		device.destroyImageView(framebuffer.color.view);
		device.destroyImageView(framebuffer.depthStencil.view);

		device.destroyImage(framebuffer.resolve.image);
		device.destroyImage(framebuffer.color.image);
		device.destroyImage(framebuffer.depthStencil.image);
	}

	device.freeMemory(imageMemory.memory);
	device.freeMemory(deviceMemory.memory);
	device.freeMemory(hostMemory.memory);

	device.destroySwapchainKHR(swapchain);

	device.destroyCommandPool(commandPool);

	VULKAN_HPP_DEFAULT_DISPATCHER.init(instance);

	device.destroy();

	instance.destroySurfaceKHR(surface);

#ifndef NDEBUG
	instance.destroyDebugUtilsMessengerEXT(messenger);
#endif // NDEBUG

	VULKAN_HPP_DEFAULT_DISPATCHER.init(loader);

	instance.destroy();

	SDL_DestroyWindow(window);
	SDL_Vulkan_UnloadLibrary();
	SDL_Quit();
}
