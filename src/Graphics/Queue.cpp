#include "Graphics/Queue.hpp"

#include <bit>
#include <limits>

namespace Graphics {
    Queue::Queue(vk::raii::Device *device, uint32_t queueFamilyIndex, uint32_t queueIndex) :
        queueFamilyIndex(queueFamilyIndex),
        queueIndex(queueIndex),
        queue(device->getQueue(queueFamilyIndex, queueIndex)),
        commandPool(*device, vk::CommandPoolCreateInfo {
            vk::CommandPoolCreateFlagBits::eResetCommandBuffer,
            queueFamilyIndex
        }),
        commandBuffer(std::move(vk::raii::CommandBuffers {
            *device,
            vk::CommandBufferAllocateInfo {
                *commandPool,
                vk::CommandBufferLevel::ePrimary,
                1
            }
        }.front())) {
    }

    vk::raii::Queue Queue::operator*() {
        return queue;
    }

    vk::raii::Queue Queue::operator->() {
        return queue;
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
}