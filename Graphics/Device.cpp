module;

#include "Graphics.hpp"

module Engine:Graphics;

namespace Engine::Graphics {
	extern vk::Instance instance;

	unsigned int physicalDeviceIndex;
	vk::PhysicalDeviceProperties physicalDeviceProperties;
	vk::PhysicalDeviceFeatures physicalDeviceFeatures;
	vk::PhysicalDevice physicalDevice;

	unsigned int transferQueueFamilyIndex;
	vk::QueueFamilyProperties transferQueueFamilyProperties;
	vk::Queue transferQueue;
	vk::CommandPool transferCommandPool;
	vk::CommandBuffer transferCommandBuffer;

	unsigned int graphicsQueueFamilyIndex;
	vk::QueueFamilyProperties graphicsQueueFamilyProperties;
	vk::Queue graphicsQueue;
	vk::CommandPool graphicsCommandPool;

	vk::Device device;

	void selectPhysicalDevice() {
		// TODO: Better physical device selection
		physicalDeviceIndex = 1;

		physicalDevice = instance.enumeratePhysicalDevices().at(physicalDeviceIndex);
		physicalDeviceProperties = physicalDevice.getProperties();
	}

	void selectQueueFamilies() {
		// TODO: Better queue family selection
		transferQueueFamilyIndex = 1;
		graphicsQueueFamilyIndex = 0;

		std::vector<vk::QueueFamilyProperties> queueFamilyPropertiesList = physicalDevice.getQueueFamilyProperties();

		transferQueueFamilyProperties = queueFamilyPropertiesList.at(transferQueueFamilyIndex);
		graphicsQueueFamilyProperties = queueFamilyPropertiesList.at(graphicsQueueFamilyIndex);
	}

	void createLogicalDevice() {
		float queuePriority = 1.0f;

		vk::PhysicalDeviceFeatures deviceFeatures{};

		std::vector<const char*> deviceExtensions {
			VK_KHR_SWAPCHAIN_EXTENSION_NAME
		};

		vk::PhysicalDeviceDynamicRenderingFeatures dynamicRenderingFeatures {
			.dynamicRendering = true
		};

		std::array<vk::DeviceQueueCreateInfo, 2> queueInfos {
			vk::DeviceQueueCreateInfo {
				.queueFamilyIndex = transferQueueFamilyIndex,
				.queueCount = 1,
				.pQueuePriorities = &queuePriority
			},
			vk::DeviceQueueCreateInfo {
				.queueFamilyIndex = graphicsQueueFamilyIndex,
				.queueCount = 1,
				.pQueuePriorities = &queuePriority
			}
		};

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
	}

	void createQueueStructures() {
		transferQueue = device.getQueue(transferQueueFamilyIndex, 0);
		graphicsQueue = device.getQueue(graphicsQueueFamilyIndex, 0);

		vk::CommandPoolCreateInfo transferCommandPoolInfo {
			.flags = vk::CommandPoolCreateFlagBits::eResetCommandBuffer,
			.queueFamilyIndex = transferQueueFamilyIndex
		};
		
		vk::CommandPoolCreateInfo graphicsCommandPoolInfo {
			.flags = vk::CommandPoolCreateFlagBits::eResetCommandBuffer,
			.queueFamilyIndex = graphicsQueueFamilyIndex
		};

		transferCommandPool = device.createCommandPool(transferCommandPoolInfo);
		graphicsCommandPool = device.createCommandPool(graphicsCommandPoolInfo);

		vk::CommandBufferAllocateInfo transferCommandBufferInfo {
			.commandPool = transferCommandPool,
			.level = vk::CommandBufferLevel::ePrimary,
			.commandBufferCount = 1
		};

		transferCommandBuffer = device.allocateCommandBuffers(transferCommandBufferInfo).front();
	}

	void createDevice() {
		selectPhysicalDevice();
		selectQueueFamilies();
		createLogicalDevice();
		createQueueStructures();
	}

	void destroyDevice() {
		device.destroyCommandPool(graphicsCommandPool);
		device.destroyCommandPool(transferCommandPool);

		device.destroy();

		VULKAN_HPP_DEFAULT_DISPATCHER.init(instance);
	}
}