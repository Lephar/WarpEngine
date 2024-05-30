#include "Graphics/Device.hpp"
#include "Graphics/Queue.hpp"

#include <SDL_log.h>
#include <vulkan/vulkan_raii.hpp>

namespace Graphics {
    Device::Device(vk::raii::PhysicalDevice physicalDevice)	: physicalDevice(physicalDevice)
                                                            , properties(physicalDevice.getProperties())
                                                            , features(physicalDevice.getFeatures())
                                                            , memoryProperties(physicalDevice.getMemoryProperties())
                                                            , queueFamilyPropertiesList(physicalDevice.getQueueFamilyProperties())
                                                            {
        vk::PhysicalDeviceFeatures deviceFeatures {
        };

        std::vector<const char*> deviceExtensions {
            VK_KHR_SWAPCHAIN_EXTENSION_NAME,
            VK_EXT_SHADER_OBJECT_EXTENSION_NAME
        };

        vk::PhysicalDeviceDynamicRenderingFeatures dynamicRenderingFeatures {
            true
        };

        std::vector<float> queuePriorities{1.0f, 1.0f, 1.0f};

        uint32_t graphicsQueueFamilyIndex = Queue::chooseQueueFamily(queueFamilyPropertiesList, vk::QueueFlagBits::eGraphics);
        uint32_t computeQueueFamilyIndex  = Queue::chooseQueueFamily(queueFamilyPropertiesList, vk::QueueFlagBits::eCompute );
        uint32_t transferQueueFamilyIndex = Queue::chooseQueueFamily(queueFamilyPropertiesList, vk::QueueFlagBits::eTransfer);

        vk::DeviceQueueCreateInfo queueInfo {
            vk::DeviceQueueCreateFlags{},
            graphicsQueueFamilyIndex,
            1,
            queuePriorities.data()
        };

        std::vector<vk::DeviceQueueCreateInfo> queueInfos{queueInfo};

        if(computeQueueFamilyIndex == graphicsQueueFamilyIndex) {
            queueInfos.front().queueCount++;
        } else {
            queueInfo.queueFamilyIndex = computeQueueFamilyIndex;
            queueInfos.push_back(queueInfo);
        }

        if(transferQueueFamilyIndex == graphicsQueueFamilyIndex) {
            queueInfos.front().queueCount++;
        } else if(transferQueueFamilyIndex == computeQueueFamilyIndex) {
            queueInfos.back().queueCount++;
        } else {
            queueInfo.queueFamilyIndex = transferQueueFamilyIndex;
            queueInfos.push_back(queueInfo);
        }

        vk::DeviceCreateInfo deviceInfo {
            vk::DeviceCreateFlags{},
            static_cast<uint32_t>(queueInfos.size()),
            queueInfos.data(),
            0,
            nullptr,
            static_cast<uint32_t>(deviceExtensions.size()),
            deviceExtensions.data(),
            &deviceFeatures,
            &dynamicRenderingFeatures
        };

        device = new vk::raii::Device(physicalDevice, deviceInfo);
    }
}
