#include "Graphics/Device.hpp"
#include "Graphics/Queue.hpp"

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

        uint32_t graphicsQueueIndex = 0;
        uint32_t computeQueueIndex  = 0;
        uint32_t transferQueueIndex = 0;

        vk::DeviceQueueCreateInfo queueInfo {
            vk::DeviceQueueCreateFlags{},
            graphicsQueueFamilyIndex,
            1,
            queuePriorities.data()
        };

        std::vector<vk::DeviceQueueCreateInfo> queueInfos{queueInfo};

        if(computeQueueFamilyIndex == graphicsQueueFamilyIndex) {
            computeQueueIndex = graphicsQueueIndex + 1;
            queueInfos.front().queueCount++;
        } else {
            queueInfo.queueFamilyIndex = computeQueueFamilyIndex;
            queueInfos.push_back(queueInfo);
        }

        if(transferQueueFamilyIndex == computeQueueFamilyIndex) {
            transferQueueIndex = computeQueueIndex + 1;
            queueInfos.back().queueCount++;
        } else if(transferQueueFamilyIndex == graphicsQueueFamilyIndex) {
            transferQueueIndex = graphicsQueueIndex + 1;
            queueInfos.front().queueCount++;
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

        graphicsQueue = new Queue(device, graphicsQueueFamilyIndex, graphicsQueueIndex);
        computeQueue  = new Queue(device, computeQueueFamilyIndex , computeQueueIndex );
        transferQueue = new Queue(device, transferQueueFamilyIndex, transferQueueIndex);
    }
}
