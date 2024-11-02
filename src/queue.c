#include "queue.h"
#include "helper.h"

extern VkDevice device;

uint32_t queueFamilyCount;
VkQueueFamilyProperties *queueFamilyProperties;

Queue graphicsQueue;
Queue  computeQueue;
Queue transferQueue;

Queue *queueReferences[] = {
    &graphicsQueue,
    & computeQueue,
    &transferQueue
};

uint32_t queueCount;
uint32_t distinctQueueFamilyCount;

void generateQueueDetails() {
    graphicsQueue.requiredFlags = VK_QUEUE_GRAPHICS_BIT;
     computeQueue.requiredFlags = VK_QUEUE_COMPUTE_BIT ;
    transferQueue.requiredFlags = VK_QUEUE_TRANSFER_BIT;

    queueCount = sizeof(queueReferences) / sizeof(Queue*);
    debug("Queue count: %d", queueCount);

    for(uint32_t queueIndex = 0; queueIndex < queueCount; queueIndex++) {
        uint32_t mostSuitedScore = 0;
        uint32_t mostSuitedIndex = UINT32_MAX;

        for(uint32_t queueFamilyIndex = 0; queueFamilyIndex < queueFamilyCount; queueFamilyIndex++) {
            VkQueueFlags supportedFlags = queueFamilyProperties[queueFamilyIndex].queueFlags;
            VkQueueFlags satisfiedFlags = supportedFlags &  queueReferences[queueIndex]->requiredFlags;
            VkQueueFlags redundantFlags = supportedFlags & ~queueReferences[queueIndex]->requiredFlags;

            uint32_t queueFamilyScore = ~popcount(redundantFlags);

            if(queueReferences[queueIndex]->requiredFlags == satisfiedFlags && mostSuitedScore < queueFamilyScore) {
                mostSuitedScore = queueFamilyScore;
                mostSuitedIndex = queueFamilyIndex;
            }
        }

        queueReferences[queueIndex]->queueInfoIndex = 0;
        queueReferences[queueIndex]->queueIndex = 0;
        queueReferences[queueIndex]->queueFamilyIndex = mostSuitedIndex;

        assert(queueReferences[queueIndex]->queueFamilyIndex < queueFamilyCount);
    }

    distinctQueueFamilyCount = 1;

    for(uint32_t queueIndex = 1; queueIndex < queueCount; queueIndex++) {
        VkBool32 queueDistinct = VK_TRUE;

        for(int32_t comparisonIndex = queueIndex - 1; comparisonIndex >= 0; comparisonIndex--) {
            if(queueReferences[queueIndex]->queueFamilyIndex == queueReferences[comparisonIndex]->queueFamilyIndex) {
                queueReferences[queueIndex]->queueInfoIndex = queueReferences[comparisonIndex]->queueInfoIndex;
                queueReferences[queueIndex]->queueIndex = queueReferences[comparisonIndex]->queueIndex + 1;

                if(queueReferences[queueIndex]->queueIndex >= queueFamilyProperties[queueReferences[queueIndex]->queueFamilyIndex].queueCount) {
                    queueReferences[queueIndex]->queueIndex = queueFamilyProperties[queueReferences[queueIndex]->queueFamilyIndex].queueCount - 1;
                }

                queueDistinct = VK_FALSE;
                break;
            }
        }

        if(queueDistinct) {
            queueReferences[queueIndex]->queueInfoIndex = distinctQueueFamilyCount++;
        }
    }

    debug("Distinct queue family count: %d", distinctQueueFamilyCount);

    for(uint32_t queueIndex = 0; queueIndex < queueCount; queueIndex++) {
        debug("Queue %d:", queueIndex);
        debug("\tQueue Family Index: %d", queueReferences[queueIndex]->queueFamilyIndex);
        debug("\tQueue Index:        %d", queueReferences[queueIndex]->queueIndex);
        debug("\tQueue Info Index:   %d", queueReferences[queueIndex]->queueInfoIndex);
    }
}

void getQueues() {
    for(uint32_t queueIndex = 0; queueIndex < queueCount; queueIndex++) {
        Queue *queueReference = queueReferences[queueIndex];

        vkGetDeviceQueue(device, queueReference->queueFamilyIndex, queueReference->queueIndex, &queueReference->queue);
        debug("Device queue %d retrieved", queueIndex);

        VkCommandPoolCreateInfo commandPoolInfo = {
            .sType = VK_STRUCTURE_TYPE_COMMAND_POOL_CREATE_INFO,
            .pNext = NULL,
            .flags = 0,
            .queueFamilyIndex = queueReference->queueFamilyIndex
        };

        vkCreateCommandPool(device, &commandPoolInfo, NULL, &queueReference->commandPool);
        debug("\tCommand pool created");

        VkCommandBufferAllocateInfo commandBufferInfo = {
            .sType = VK_STRUCTURE_TYPE_COMMAND_BUFFER_ALLOCATE_INFO,
            .pNext = NULL,
            .commandPool = queueReference->commandPool,
            .level = VK_COMMAND_BUFFER_LEVEL_PRIMARY,
            .commandBufferCount = 1
        };

        vkAllocateCommandBuffers(device, &commandBufferInfo, &queueReference->commandBuffer);
        debug("\tCommand buffer created");
    }
}

void clearQueues() {
    for(uint32_t queueIndex = 0; queueIndex < queueCount; queueIndex++) {
        vkQueueWaitIdle(queueReferences[queueIndex]->queue);
        vkDestroyCommandPool(device, queueReferences[queueIndex]->commandPool, NULL);
        debug("Command pool %d destroy", queueIndex);
    }
}
