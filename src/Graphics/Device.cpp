#include "Graphics/Device.hpp"

#include <vulkan/vulkan_raii.hpp>

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
        /*
        std::vector<float> queuePriorities{1.0f, 1.0f, 1.0f};

        std::optional<vk::DeviceQueueCreateInfo> graphicsQueueFamilyInfo;
        std::optional<vk::DeviceQueueCreateInfo> computeQueueFamilyInfo;
        std::optional<vk::DeviceQueueCreateInfo> transferQueueFamilyInfo;

        for(size_t index = 0; index < queueFamilyProperties.size(); index++) {
            auto &queueFamily = queueFamilyProperties.at(index);

            if(!graphicsQueueFamilyInfo.has_value()                     &&
               (queueFamily.queueFlags & vk::QueueFlagBits::eGraphics)) {
                graphicsQueueFamilyInfo.emplace(
                    vk::DeviceQueueCreateFlags{},
                    index,
                    1,
                    queuePriorities.data()
                );
            }

            if(!computeQueueFamilyInfo.has_value()                      &&
               (queueFamily.queueFlags & vk::QueueFlagBits::eCompute )  &&
              !(queueFamily.queueFlags & vk::QueueFlagBits::eGraphics)) {
                computeQueueFamilyInfo.emplace(
                    vk::DeviceQueueCreateFlags{},
                    index,
                    1,
                    queuePriorities.data()
                );
            }

            if(!transferQueueFamilyInfo.has_value() &&
               (queueFamily.queueFlags & vk::QueueFlagBits::eTransfer)  &&
              !(queueFamily.queueFlags & vk::QueueFlagBits::eCompute )  &&
              !(queueFamily.queueFlags & vk::QueueFlagBits::eGraphics)) {
                transferQueueFamilyInfo.emplace(
                    vk::DeviceQueueCreateFlags{},
                    index,
                    1,
                    queuePriorities.data()
                );
            }
        }

        if(!graphicsQueueFamilyInfo.has_value()) {
            throw std::runtime_error("System doesn't support graphics operations!");
        }

        if(!computeQueueFamilyInfo.has_value()) {
            graphicsQueueFamilyInfo->queueCount++;
        }

        if(!transferQueueFamilyInfo.has_value()) {
            if(computeQueueFamilyInfo.has_value()) {
                computeQueueFamilyInfo->queueCount++;
            } else {
                graphicsQueueFamilyInfo->queueCount++;
            }
        }

        std::vector<vk::DeviceQueueCreateInfo> queueInfos{graphicsQueueFamilyInfo.value()};

        if(computeQueueFamilyInfo.has_value()) {
            queueInfos.push_back(computeQueueFamilyInfo.value());
        }

        if(transferQueueFamilyInfo.has_value()) {
            queueInfos.push_back(transferQueueFamilyInfo.value());
        }
        */
        float queuePriority = 1.0f;

        vk::DeviceQueueCreateInfo graphicsQueueInfo(
            vk::DeviceQueueCreateFlags{},
            0,
            1,
            &queuePriority
        );

        vk::DeviceQueueCreateInfo computeQueueInfo(
            vk::DeviceQueueCreateFlags{},
            1,
            1,
            &queuePriority
        );

        vk::DeviceQueueCreateInfo transferQueueInfo(
            vk::DeviceQueueCreateFlags{},
            2,
            1,
            &queuePriority
        );

        std::vector<vk::DeviceQueueCreateInfo> queueInfos{graphicsQueueInfo, computeQueueInfo, transferQueueInfo};

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
