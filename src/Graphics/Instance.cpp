#include "Graphics/Instance.hpp"

#include <SDL2/SDL_log.h>

namespace Graphics {
#ifndef NDEBUG
    SDL_LogPriority convertLogSeverity(VkDebugUtilsMessageSeverityFlagBitsEXT severity) {
        if(severity == VK_DEBUG_UTILS_MESSAGE_SEVERITY_ERROR_BIT_EXT)
            return SDL_LOG_PRIORITY_ERROR;
        else if(severity == VK_DEBUG_UTILS_MESSAGE_SEVERITY_WARNING_BIT_EXT)
            return SDL_LOG_PRIORITY_WARN;
        else if(severity == VK_DEBUG_UTILS_MESSAGE_SEVERITY_INFO_BIT_EXT)
            return SDL_LOG_PRIORITY_INFO;
        else if(severity == VK_DEBUG_UTILS_MESSAGE_SEVERITY_VERBOSE_BIT_EXT)
            return SDL_LOG_PRIORITY_VERBOSE;
        else
            return SDL_LOG_PRIORITY_DEBUG;
    }

    VKAPI_ATTR VkBool32 VKAPI_CALL messageCallback(VkDebugUtilsMessageSeverityFlagBitsEXT severity,
                                                   VkDebugUtilsMessageTypeFlagsEXT type,
                                                   const VkDebugUtilsMessengerCallbackDataEXT* pCallbackData,
                                                   void* pUserData) {
        static_cast<void>(type);
        static_cast<void>(pUserData);

        SDL_LogMessage(SDL_LOG_CATEGORY_RENDER, convertLogSeverity(severity), "\tValidation Layers:\t%s\n", pCallbackData->pMessage);

        return VK_FALSE;
    }
#endif

    Instance::Instance(const char *title, std::vector<const char *> layers, std::vector<const char *> extensions) {
        context = new vk::raii::Context{};

#ifndef NDEBUG
        vk::DebugUtilsMessengerCreateInfoEXT messengerInfo {
            vk::DebugUtilsMessengerCreateFlagsEXT{},
            vk::DebugUtilsMessageSeverityFlagBitsEXT::eVerbose |
            vk::DebugUtilsMessageSeverityFlagBitsEXT::eInfo |
            vk::DebugUtilsMessageSeverityFlagBitsEXT::eWarning |
            vk::DebugUtilsMessageSeverityFlagBitsEXT::eError,
            vk::DebugUtilsMessageTypeFlagBitsEXT::eGeneral |
            vk::DebugUtilsMessageTypeFlagBitsEXT::ePerformance |
            vk::DebugUtilsMessageTypeFlagBitsEXT::eValidation,
            messageCallback,
            nullptr
        };
#endif // NDEBUG

        vk::ApplicationInfo applicationInfo {
            title,
            VK_MAKE_API_VERSION(0, 0, 0, 1),
            title,
            VK_MAKE_API_VERSION(0, 0, 0, 1),
            VK_API_VERSION_1_3
        };

        vk::InstanceCreateInfo instanceInfo {
            vk::InstanceCreateFlags{},
            &applicationInfo,
            static_cast<uint32_t>(layers.size()),
            layers.data(),
            static_cast<uint32_t>(extensions.size()),
            extensions.data(),
#ifndef NDEBUG
            &messengerInfo
#endif // NDEBUG
        };

        instance = new vk::raii::Instance{*context, instanceInfo};
#ifndef NDEBUG
        messenger = new vk::raii::DebugUtilsMessengerEXT{*instance, messengerInfo};
#endif // NDEBUG
    }

    Instance::~Instance() {
        delete messenger;
        delete instance;
        delete context;
    }
}
