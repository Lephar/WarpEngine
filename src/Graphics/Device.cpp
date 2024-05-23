#include "Graphics/Device.hpp"
#include <limits>
#include <vulkan/vulkan_enums.hpp>
#include <vulkan/vulkan_raii.hpp>
#include <vulkan/vulkan_structs.hpp>

namespace Graphics {
    Device::Device(vk::raii::PhysicalDevice physicalDevice)	: physicalDevice(physicalDevice)
                                                            , properties(physicalDevice.getProperties())
                                                            , features(physicalDevice.getFeatures())
                                                            , memoryProperties(physicalDevice.getMemoryProperties())
                                                            , queueFamilyProperties(physicalDevice.getQueueFamilyProperties())
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

        float queuePriority = 1.0f;

        uint32_t graphicsQueueFamilyIndex = std::numeric_limits<uint32_t>::max();
        uint32_t computeQueueFamilyIndex  = std::numeric_limits<uint32_t>::max();
        uint32_t transferQueueFamilyIndex = std::numeric_limits<uint32_t>::max();

        for(size_t index = 0; index < queueFamilyProperties.size(); index++) {
            auto &queueFamily = queueFamilyProperties.at(index);

            if(graphicsQueueFamilyIndex  == std::numeric_limits<uint32_t>::max() &&
               queueFamily.queueFlags    &  vk::QueueFlagBits::eGraphics) {
                graphicsQueueFamilyIndex =  index;
            }

            if(computeQueueFamilyIndex   == std::numeric_limits<uint32_t>::max() &&
              (queueFamily.queueFlags    &  vk::QueueFlagBits::eCompute) &&
             !(queueFamily.queueFlags    &  vk::QueueFlagBits::eGraphics)) {
                computeQueueFamilyIndex  =  index;
            }

            if(transferQueueFamilyIndex  == std::numeric_limits<uint32_t>::max() &&
              (queueFamily.queueFlags    &  vk::QueueFlagBits::eTransfer) &&
             !(queueFamily.queueFlags    &  vk::QueueFlagBits::eCompute)  &&
             !(queueFamily.queueFlags    &  vk::QueueFlagBits::eGraphics)) {
                transferQueueFamilyIndex =  index;
            }
        }







        vk::DeviceQueueCreateInfo queueInfo {
            vk::DeviceQueueCreateFlags{},
            static_cast<uint32_t>(graphicsQueueFamilyIndex),
            1,
            &queuePriority
        };

        std::vector<vk::DeviceQueueCreateInfo> queueInfos{queueInfo};

        if(computeQueueFamilyIndex == graphicsQueueFamilyIndex) {
            queueInfos.back().queueCount++;
        } else {
            queueInfo.queueFamilyIndex = computeQueueFamilyIndex;
            queueInfos.push_back(queueInfo);
        }

        if(transferQueueFamilyIndex == computeQueueFamilyIndex) {
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
