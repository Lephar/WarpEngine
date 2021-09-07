#include "graphics.hpp"

namespace zero::graphics {
#ifndef NDEBUG
	VKAPI_ATTR VkBool32 VKAPI_CALL messageCallback(VkDebugUtilsMessageSeverityFlagBitsEXT severity,
												   VkDebugUtilsMessageTypeFlagsEXT type,
												   const VkDebugUtilsMessengerCallbackDataEXT *pCallbackData,
												   void *pUserData) {
		static_cast<void>(type);
		static_cast<void>(pUserData);

		if (severity >= VK_DEBUG_UTILS_MESSAGE_SEVERITY_WARNING_BIT_EXT)
			std::cout << pCallbackData->pMessage << std::endl;

		return VK_FALSE;
	}

#endif //NDEBUG

	void Graphics::createInstance(void) {
		std::vector<const char *> layers{};
		std::vector<const char *> extensions{VK_KHR_SURFACE_EXTENSION_NAME, VK_KHR_XCB_SURFACE_EXTENSION_NAME};

#ifndef NDEBUG
		layers.push_back("VK_LAYER_KHRONOS_validation");
		extensions.push_back(VK_EXT_DEBUG_UTILS_EXTENSION_NAME);

		vk::DebugUtilsMessengerCreateInfoEXT messengerInfo{
				.messageSeverity = vk::DebugUtilsMessageSeverityFlagBitsEXT::eVerbose |
								   vk::DebugUtilsMessageSeverityFlagBitsEXT::eInfo |
								   vk::DebugUtilsMessageSeverityFlagBitsEXT::eWarning |
								   vk::DebugUtilsMessageSeverityFlagBitsEXT::eError,
				.messageType = vk::DebugUtilsMessageTypeFlagBitsEXT::eGeneral |
							   vk::DebugUtilsMessageTypeFlagBitsEXT::ePerformance |
							   vk::DebugUtilsMessageTypeFlagBitsEXT::eValidation,
				.pfnUserCallback = messageCallback
		};
#endif //NDEBUG

		vk::ApplicationInfo applicationInfo{
				.pApplicationName = "Zero",
				.applicationVersion = VK_MAKE_API_VERSION(0, 0, 1, 0),
				.pEngineName = "Zero Engine",
				.engineVersion = VK_MAKE_API_VERSION(0, 0, 1, 0),
				.apiVersion = VK_API_VERSION_1_2
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
#ifndef NDEBUG
		loader = vk::DispatchLoaderDynamic{instance, vkGetInstanceProcAddr};
		messenger = instance.createDebugUtilsMessengerEXT(messengerInfo, nullptr, loader);
#endif //NDEBUG
	}

	// TODO: Better device selection
	void Graphics::selectPhysicalDevice(void) {
		auto physicalDevices = instance.enumeratePhysicalDevices();

		for (auto &iteratorDevice: physicalDevices) {
			auto properties = iteratorDevice.getProperties();

			if (properties.deviceType == vk::PhysicalDeviceType::eDiscreteGpu) {
				physicalDevice = iteratorDevice;
				break;
			}
		}
	}

	uint32_t Graphics::selectQueueFamily(vk::QueueFlags requiredFlags) {
		uint32_t maxScore = 0;
		uint32_t mostSuitable = std::numeric_limits<uint32_t>::max();

		auto queueFamilies = physicalDevice.getQueueFamilyProperties();

		for (uint32_t index = 0; index < queueFamilies.size(); index++) {
			auto &supportedFlags = queueFamilies.at(index).queueFlags;

			// See if all the required bits are present in the supported bits
			bool suitable = (supportedFlags & requiredFlags) == requiredFlags;

			if (!suitable)
				continue;

			// XOR two flags and then NOT the result to get the matching bits
			// Then get the count of 1's using GCC's built-in function to use it as the score
			uint32_t score = __builtin_popcount(static_cast<uint32_t>(~(supportedFlags ^ requiredFlags)));

			if (score > maxScore) {
				maxScore = score;
				mostSuitable = index;
			}
		}

		return mostSuitable;
	}

	void Graphics::selectQueueFamilies(void) {
		transferQueueFamily = selectQueueFamily(vk::QueueFlagBits::eTransfer);
		graphicsQueueFamily = selectQueueFamily(vk::QueueFlagBits::eTransfer | vk::QueueFlagBits::eCompute | vk::QueueFlagBits::eGraphics);
	}

	void Graphics::createDevice(void) {
		float queuePriority = 1.0f;
		vk::PhysicalDeviceFeatures deviceFeatures{};
		std::vector<const char*> extensions{VK_KHR_SWAPCHAIN_EXTENSION_NAME};

		vk::DeviceQueueCreateInfo transferQueueInfo{};
		transferQueueInfo.queueFamilyIndex = transferQueueFamily;
		transferQueueInfo.queueCount = 1;
		transferQueueInfo.pQueuePriorities = &queuePriority;

		vk::DeviceQueueCreateInfo graphicsQueueInfo{};
		graphicsQueueInfo.queueFamilyIndex = graphicsQueueFamily;
		graphicsQueueInfo.queueCount = 1;
		graphicsQueueInfo.pQueuePriorities = &queuePriority;

		std::array<vk::DeviceQueueCreateInfo, 2> queueInfos{transferQueueInfo, graphicsQueueInfo};

		vk::DeviceCreateInfo deviceInfo{};
		deviceInfo.queueCreateInfoCount = queueInfos.size();
		deviceInfo.pQueueCreateInfos = queueInfos.data();
		deviceInfo.pEnabledFeatures = &deviceFeatures;
		deviceInfo.enabledExtensionCount = extensions.size();
		deviceInfo.ppEnabledExtensionNames = extensions.data();

		device = physicalDevice.createDevice(deviceInfo);

		transferQueue = device.getQueue(transferQueueFamily, 0);
		graphicsQueue = device.getQueue(graphicsQueueFamily, 0);
	}

	Graphics::Graphics(void) {
		createInstance();
		selectPhysicalDevice();
		selectQueueFamilies();
		createDevice();
	}

	Graphics::~Graphics(void) {
		device.destroy();

		instance.destroyDebugUtilsMessengerEXT(messenger, nullptr, loader);
		instance.destroy();
	}
}