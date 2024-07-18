#include "Graphics/Queue.hpp"

#include <bit>
#include <limits>

namespace Graphics {
    Queue::Queue(vk::raii::Device *device, uint32_t queueFamilyIndex, uint32_t queueIndex) : queueFamilyIndex{queueFamilyIndex},
                                                                                             queueIndex{queueIndex} {
        queue = new vk::raii::Queue{device->getQueue(queueFamilyIndex, queueIndex)};

        vk::CommandPoolCreateInfo commandPoolInfo {
            vk::CommandPoolCreateFlagBits::eResetCommandBuffer,
            queueFamilyIndex
        };

        commandPool = new vk::raii::CommandPool{*device, commandPoolInfo};

        vk::CommandBufferAllocateInfo commandBufferInfo {
            *commandPool,
            vk::CommandBufferLevel::ePrimary,
            1
        };

        vk::raii::CommandBuffers commandBuffers{*device, commandBufferInfo};

        commandBuffer = new vk::raii::CommandBuffer{std::move(commandBuffers.front())};
    }

    uint32_t Queue::chooseQueueFamily(std::vector<vk::QueueFamilyProperties> queueFamilyPropertiesList, vk::QueueFlags requiredFlags) {
        uint32_t mostSuitedScore = 0;
        uint32_t mostSuitedIndex = std::numeric_limits<uint32_t>::max();

        for(uint32_t queueFamilyIndex = 0; queueFamilyIndex < queueFamilyPropertiesList.size(); queueFamilyIndex++) {
            vk::QueueFlags supportedFlags = queueFamilyPropertiesList.at(queueFamilyIndex).queueFlags;
            vk::QueueFlags satisfiedFlags = supportedFlags &  requiredFlags;
            vk::QueueFlags redundantFlags = supportedFlags & ~requiredFlags;

            uint32_t queueFamilyScore = ~static_cast<uint32_t>(std::popcount(static_cast<uint32_t>(redundantFlags)));

            if(requiredFlags == satisfiedFlags && mostSuitedScore < queueFamilyScore) {
                mostSuitedScore = queueFamilyScore;
                mostSuitedIndex = queueFamilyIndex;
            }
        }

        return mostSuitedIndex;
    }

    Queue::~Queue() {
        delete commandBuffer;
        delete commandPool;
        delete queue;
    }
}