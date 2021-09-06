#include "instance.hpp"

namespace zero::graphics {
#ifndef NDEBUG
	VKAPI_ATTR VkBool32 VKAPI_CALL messageCallback(VkDebugUtilsMessageSeverityFlagBitsEXT severity,
												   VkDebugUtilsMessageTypeFlagsEXT type,
												   const VkDebugUtilsMessengerCallbackDataEXT *pCallbackData,
												   void *pUserData) {
		static_cast<void>(type);
		static_cast<void>(pUserData);

		if (severity >= VK_DEBUG_UTILS_MESSAGE_SEVERITY_VERBOSE_BIT_EXT)
			std::cout << pCallbackData->pMessage << std::endl;

		return VK_FALSE;
	}

#endif //NDEBUG

	Instance::Instance(const char *application, const char *engine, uint32_t version) {
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
				.pApplicationName = application,
				.applicationVersion = version,
				.pEngineName = engine,
				.engineVersion = version,
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

	vk::Instance Instance::get() {
		return instance;
	}

	void Instance::destroy(void) {
		instance.destroyDebugUtilsMessengerEXT(messenger, nullptr, loader);
		instance.destroy();
	}
}
