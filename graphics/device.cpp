#include "device.hpp"

namespace zero::graphics {
	Device::Device(Instance &instance, Internals &internals, Details &details) {
		float queuePriority = 1.0f;
		std::vector<float> queuePriorities(details.threadCount, queuePriority);
		vk::PhysicalDeviceFeatures deviceFeatures{};
		std::vector<const char*> extensions{VK_KHR_SWAPCHAIN_EXTENSION_NAME};

		vk::CommandPoolCreateInfo transferPoolInfo{};
		transferPoolInfo.flags = vk::CommandPoolCreateFlagBits::eResetCommandBuffer;
		transferPoolInfo.queueFamilyIndex = internals.transferQueueFamily;

		vk::CommandPoolCreateInfo graphicsPoolInfo{};
		graphicsPoolInfo.flags = vk::CommandPoolCreateFlagBits::eResetCommandBuffer;
		graphicsPoolInfo.queueFamilyIndex = internals.graphicsQueueFamily;

		vk::DeviceQueueCreateInfo transferQueueInfo{};
		transferQueueInfo.queueFamilyIndex = internals.transferQueueFamily;
		transferQueueInfo.queueCount = 1;
		transferQueueInfo.pQueuePriorities = &queuePriority;

		vk::DeviceQueueCreateInfo graphicsQueueInfo{};
		graphicsQueueInfo.queueFamilyIndex = internals.graphicsQueueFamily;
		graphicsQueueInfo.queueCount = queuePriorities.size();
		graphicsQueueInfo.pQueuePriorities = queuePriorities.data();

		std::array<vk::DeviceQueueCreateInfo, 2> queueInfos{ transferQueueInfo, graphicsQueueInfo };

		vk::DeviceCreateInfo deviceInfo{};
		deviceInfo.queueCreateInfoCount = queueInfos.size();
		deviceInfo.pQueueCreateInfos = queueInfos.data();
		deviceInfo.pEnabledFeatures = &deviceFeatures;
		deviceInfo.enabledExtensionCount = extensions.size();
		deviceInfo.ppEnabledExtensionNames = extensions.data();

		device = internals.physicalDevice.createDevice(deviceInfo);
		transferQueue = device.getQueue(internals.transferQueueFamily, 0);
		transferCommandPool = device.createCommandPool(transferPoolInfo);

		for (int index = 0; index < details.threadCount; index++)
			graphicsQueues.emplace_back(device.getQueue(internals.graphicsQueueFamily, index));

		for (int index = 0; index < details.imageCount; index++)
			graphicsCommandPools.emplace_back(device.createCommandPool(graphicsPoolInfo));
	}
}