#include "Graphics/Renderer.hpp"

VULKAN_HPP_DEFAULT_DISPATCH_LOADER_DYNAMIC_STORAGE;

void Renderer::initialize(Window *window) {
	this->window = window;

	createInstance();
	createSurface();
	createDevice();
	createSwapchain();
	createFramebuffers();
	createBuffers();
}

void Renderer::createInstance() {
	auto title = window->getTitle();

	loader = window->getLoader();
	VULKAN_HPP_DEFAULT_DISPATCHER.init(loader);

	std::vector<const char*> layers;
	std::vector<const char*> extensions = window->getExtensions();

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
		.pApplicationName = title,
		.applicationVersion = VK_MAKE_API_VERSION(0, 0, 0, 1),
		.pEngineName = title,
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

void Renderer::createSurface() {
	surface = window->createSurface(instance);
}

void Renderer::createDevice() {
	auto discreteDeviceFound = false;
	auto physicalDevices = instance.enumeratePhysicalDevices();

	std::vector<DeviceInfo> deviceInfoList{physicalDevices.size()};

	for(auto deviceIndex = 0u; deviceIndex < physicalDevices.size(); deviceIndex++)
		deviceInfoList.at(deviceIndex).populate(physicalDevices.at(deviceIndex), surface);

	for(auto deviceIndex = 0u; deviceIndex < physicalDevices.size(); deviceIndex++) {
		auto &deviceCandidate = physicalDevices.at(deviceIndex);
		auto &candidateInfo = deviceInfoList.at(deviceIndex);

		if (candidateInfo.deviceProperties.deviceType == vk::PhysicalDeviceType::eDiscreteGpu) {
			physicalDevice = deviceCandidate;
			deviceInfo = candidateInfo;
			discreteDeviceFound = true;

			break;
		}
	}

	if(!discreteDeviceFound) {
		physicalDevice = physicalDevices.front();
		deviceInfo = deviceInfoList.front();
	}

	auto queueFamilyIndex = std::numeric_limits<uint32_t>::max();

	for(queueFamilyIndex = 0; queueFamilyIndex < deviceInfo.queueFamilyProperties.size(); queueFamilyIndex++) {
		auto &queueFamily = deviceInfo.queueFamilyProperties.at(queueFamilyIndex);
		auto &queueFamilySurfaceSupport = deviceInfo.queueFamilySurfaceSupport.at(queueFamilyIndex);

		if(queueFamilySurfaceSupport &&
			(queueFamily.queueFlags & vk::QueueFlagBits::eGraphics) &&
			(queueFamily.queueFlags & vk::QueueFlagBits::eTransfer)) {
			break;
		}
	}

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
}

void Renderer::createSwapchain() {
	auto extent = window->getExtent();

	auto surfaceCapabilities = deviceInfo.surfaceCapabilities;

	auto surfaceFormatChoosen = false;
	auto supportedSurfaceFormats = deviceInfo.surfaceFormats;
	auto surfaceFormat = supportedSurfaceFormats.front();


}

void Renderer::createFramebuffers() {
	framebufferCount = 2;

	sampleCount = vk::SampleCountFlagBits::e8;

	depthStencilFormat = vk::Format::eD32SfloatS8Uint;
	colorFormat = vk::Format::eR16G16B16A16Sfloat;

	mipCount = 16;

	auto temporaryImage = createImage(extent.width, extent.height, colorFormat, vk::ImageUsageFlagBits::eColorAttachment, sampleCount, 1);
	auto memoryRequirements = device.getImageMemoryRequirements(temporaryImage);

	auto typeIndex = chooseMemoryType(memoryRequirements.memoryTypeBits, vk::MemoryPropertyFlagBits::eDeviceLocal);
	auto heapSize = memoryProperties.memoryHeaps[memoryProperties.memoryTypes[typeIndex].heapIndex].size;

	device.destroy(temporaryImage);

	imageMemory.offset = 0;
	imageMemory.size = std::clamp(1UL << 30, heapSize / 8, heapSize / 2);
	imageMemory.memory = allocateMemory(imageMemory.size, typeIndex);
}

void Renderer::createBuffers() {
	auto temporaryHostBuffer = createBuffer(extent.width * extent.height * 4, vk::BufferUsageFlagBits::eTransferSrc | vk::BufferUsageFlagBits::eUniformBuffer);
	auto hostMemoryRequirements = device.getBufferMemoryRequirements(temporaryHostBuffer);

	auto hostTypeIndex = chooseMemoryType(hostMemoryRequirements.memoryTypeBits, vk::MemoryPropertyFlagBits::eHostVisible | vk::MemoryPropertyFlagBits::eHostCoherent);
	auto hostHeapSize = memoryProperties.memoryHeaps[memoryProperties.memoryTypes[hostTypeIndex].heapIndex].size;

	auto temporaryDeviceBuffer = createBuffer(extent.width * extent.height * 4, vk::BufferUsageFlagBits::eTransferDst | vk::BufferUsageFlagBits::eIndexBuffer | vk::BufferUsageFlagBits::eVertexBuffer);
	auto deviceMemoryRequirements = device.getBufferMemoryRequirements(temporaryDeviceBuffer);

	auto deviceTypeIndex = chooseMemoryType(deviceMemoryRequirements.memoryTypeBits, vk::MemoryPropertyFlagBits::eDeviceLocal);
	auto deviceHeapSize = memoryProperties.memoryHeaps[memoryProperties.memoryTypes[deviceTypeIndex].heapIndex].size;

	hostMemory.offset = 0;
	hostMemory.size = std::clamp(1UL << 30, hostHeapSize / 8, hostHeapSize / 2);
	hostMemory.memory = allocateMemory(hostMemory.size, hostTypeIndex);

	deviceMemory.offset = 0;
	deviceMemory.size = std::clamp(1UL << 30, deviceHeapSize / 8, deviceHeapSize / 2);
	deviceMemory.memory = allocateMemory(deviceMemory.size, deviceTypeIndex);

	device.destroyBuffer(temporaryHostBuffer);
	device.destroyBuffer(temporaryDeviceBuffer);

	vk::DeviceSize uniformBufferSize = 3 * sizeof(glm::mat4) * framebufferCount;
	vk::DeviceSize elementBufferSize = 3 * sizeof(glm::vec3) * std::numeric_limits<unsigned short>::max();

	uniformBuffer = createBuffer(uniformBufferSize, vk::BufferUsageFlagBits::eUniformBuffer);
	stagingBuffer = createBuffer(elementBufferSize, vk::BufferUsageFlagBits::eTransferSrc);
	elementBuffer = createBuffer(elementBufferSize, vk::BufferUsageFlagBits::eTransferDst | vk::BufferUsageFlagBits::eIndexBuffer | vk::BufferUsageFlagBits::eVertexBuffer);
}

Graphics::~Graphics() {
	device.destroyBuffer(elementBuffer.buffer);
	device.destroyBuffer(stagingBuffer.buffer);
	device.destroyBuffer(uniformBuffer.buffer);

	for(auto framebuffer : framebuffers) {
		device.destroyImageView(framebuffer.resolve.view);
		device.destroyImageView(framebuffer.color.view);
		device.destroyImageView(framebuffer.depthStencil.view);

		device.destroyImage(framebuffer.resolve.image);
		device.destroyImage(framebuffer.color.image);
		device.destroyImage(framebuffer.depthStencil.image);
	}

	device.freeMemory(deviceMemory.memory);
	device.freeMemory(hostMemory.memory);
	device.freeMemory(imageMemory.memory);

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
}
