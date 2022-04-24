#include "Graphics.hpp"

namespace Engine::Graphics {
	extern vk::Instance instance;
	extern vk::SurfaceKHR surface;

	PhysicalDevice physicalDevice;

	vk::Device device;

	Queue transferQueue;
	Queue graphicsQueue;

	PhysicalDevice generatePhysicalDeviceDetails(vk::PhysicalDevice& physicalDeviceHandle) {
		PhysicalDevice physicaDevice{
			.physicalDevice = physicalDeviceHandle,
			.properties = physicalDeviceHandle.getProperties(),
			.features = physicalDeviceHandle.getFeatures(),
			.queueFamilyPropertiesList = physicalDeviceHandle.getQueueFamilyProperties(),
			.memoryProperties = physicalDeviceHandle.getMemoryProperties(),
			.surfaceCapabilities = physicalDeviceHandle.getSurfaceCapabilitiesKHR(surface),
			.surfaceFormats = physicalDeviceHandle.getSurfaceFormatsKHR(surface),
			.presentModes = physicalDeviceHandle.getSurfacePresentModesKHR(surface)
		};

		return physicaDevice;
	}

	void selectPhysicalDevice() {
		auto physicalDevices = instance.enumeratePhysicalDevices();
		bool physicalDeviceInitialized = false;

		// TODO: Implement a better physical device selection
		for (auto& physicalDeviceCandidate : physicalDevices) {
			auto physicalDeviceCandidateDetails = generatePhysicalDeviceDetails(physicalDeviceCandidate);

			if (physicalDeviceCandidateDetails.properties.deviceType == vk::PhysicalDeviceType::eDiscreteGpu) {
				physicalDevice = physicalDeviceCandidateDetails;
				physicalDeviceInitialized = true;

				break;
			}
		}

		if (!physicalDeviceInitialized) {
			auto& physicalDeviceHandle = physicalDevices.front();
			physicalDevice = generatePhysicalDeviceDetails(physicalDeviceHandle);
		}
	}

	bool isQueueFlagRedundant(vk::QueueFlags requiredFlags, vk::QueueFlags supportedFlags, vk::QueueFlagBits subjectFlag) {
		return (requiredFlags & subjectFlag) != subjectFlag && (supportedFlags & subjectFlag) == subjectFlag;
	}

	unsigned selectMostSuitableQueueFamily(vk::QueueFlags requiredFlags) {
		unsigned leastRedundancyScore = std::numeric_limits<unsigned>::max();
		unsigned mostSuitableQueueFamilyIndex = std::numeric_limits<unsigned>::max();

		for (unsigned queueFamilyIndex = 0; queueFamilyIndex < physicalDevice.queueFamilyPropertiesList.size(); queueFamilyIndex++) {
			auto& supportedFlags = physicalDevice.queueFamilyPropertiesList.at(queueFamilyIndex).queueFlags;
			bool suitable = (supportedFlags & requiredFlags) == requiredFlags;

			if (suitable) {
				unsigned redundancyScore = 0;

				if (isQueueFlagRedundant(requiredFlags, supportedFlags, vk::QueueFlagBits::eGraphics))
					redundancyScore += 5;
				if (isQueueFlagRedundant(requiredFlags, supportedFlags, vk::QueueFlagBits::eCompute))
					redundancyScore += 3;
				if (isQueueFlagRedundant(requiredFlags, supportedFlags, vk::QueueFlagBits::eTransfer))
					redundancyScore += 0; // Transfer bit can be required implicitly even if it's not listed
				if (isQueueFlagRedundant(requiredFlags, supportedFlags, vk::QueueFlagBits::eSparseBinding))
					redundancyScore += 1; // TODO: This is arbitrary, consider researching more
				if (isQueueFlagRedundant(requiredFlags, supportedFlags, vk::QueueFlagBits::eProtected))
					redundancyScore += 7; // TODO: This is arbitrary, consider researching more

				if (leastRedundancyScore > redundancyScore) {
					leastRedundancyScore = redundancyScore;
					mostSuitableQueueFamilyIndex = queueFamilyIndex;
				}
			}
		}

		// TODO: Check the value in case no queue family is suitable
		return mostSuitableQueueFamilyIndex;
	}

	void selectQueueFamilies() {
		transferQueue.familyIndex = selectMostSuitableQueueFamily(vk::QueueFlagBits::eTransfer);
		graphicsQueue.familyIndex = selectMostSuitableQueueFamily(vk::QueueFlagBits::eGraphics);
	}

	void createLogicalDevice() {
		float queuePriority = 1.0f;

		vk::PhysicalDeviceFeatures deviceFeatures{};

		std::vector<const char*> deviceExtensions{
			VK_KHR_SWAPCHAIN_EXTENSION_NAME
		};

		vk::PhysicalDeviceDynamicRenderingFeatures dynamicRenderingFeatures{
			.dynamicRendering = true
		};

		// TODO: Consider 2 queue families being same and queue count being 1
		std::array<vk::DeviceQueueCreateInfo, 2> queueInfos{
			vk::DeviceQueueCreateInfo {
				.queueFamilyIndex = transferQueue.familyIndex,
				.queueCount = 1,
				.pQueuePriorities = &queuePriority
			},
			vk::DeviceQueueCreateInfo {
				.queueFamilyIndex = graphicsQueue.familyIndex,
				.queueCount = 1,
				.pQueuePriorities = &queuePriority
			}
		};

		vk::DeviceCreateInfo deviceInfo{
			.pNext = &dynamicRenderingFeatures,
			.queueCreateInfoCount = static_cast<unsigned>(queueInfos.size()),
			.pQueueCreateInfos = queueInfos.data(),
			.enabledExtensionCount = static_cast<unsigned>(deviceExtensions.size()),
			.ppEnabledExtensionNames = deviceExtensions.data(),
			.pEnabledFeatures = &deviceFeatures
		};

		device = physicalDevice.physicalDevice.createDevice(deviceInfo);
		VULKAN_HPP_DEFAULT_DISPATCHER.init(device);
	}

	void initializeQueue(Queue& queue) {
		queue.queue = device.getQueue(queue.familyIndex, 0);

		vk::CommandPoolCreateInfo commandPoolInfo{
			.flags = vk::CommandPoolCreateFlagBits::eResetCommandBuffer,
			.queueFamilyIndex = queue.familyIndex
		};

		queue.commandPool = device.createCommandPool(commandPoolInfo);
	}

	std::vector<vk::CommandBuffer> allocateCommandBuffers(Queue& queue, unsigned commandBufferCount) {
		vk::CommandBufferAllocateInfo commandBufferInfo{
			.commandPool = queue.commandPool,
			.level = vk::CommandBufferLevel::ePrimary,
			.commandBufferCount = commandBufferCount
		};

		return device.allocateCommandBuffers(commandBufferInfo);
	}

	void createQueueStructures() {
		initializeQueue(transferQueue);
		initializeQueue(graphicsQueue);

		allocateCommandBuffers(transferQueue, 1);
	}

	void createDevice() {
		selectPhysicalDevice();
		selectQueueFamilies();
		createLogicalDevice();
		createQueueStructures();
	}

	void freeCommandBuffers(Queue& queue, std::vector<vk::CommandBuffer>& commandBuffers) {
		device.freeCommandBuffers(queue.commandPool, commandBuffers);
	}

	void destroyDevice() {
		device.destroyCommandPool(graphicsQueue.commandPool);
		device.destroyCommandPool(transferQueue.commandPool);

		device.destroy();

		VULKAN_HPP_DEFAULT_DISPATCHER.init(instance);
	}
}
