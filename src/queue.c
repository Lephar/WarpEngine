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

        queueReferences[queueIndex]->queueFamilyIndex = mostSuitedIndex;
        assert(queueReferences[queueIndex]->queueFamilyIndex < queueFamilyCount);
    }

    distinctQueueFamilyCount = 1;

    for(uint32_t queueIndex = 1; queueIndex < queueCount; queueIndex++) {
        VkBool32 queueDistinct = VK_TRUE;
        uint32_t comparisonIndex = queueIndex - 1;

        while(1) {
            if(queueReferences[queueIndex]->queueFamilyIndex == queueReferences[comparisonIndex]->queueFamilyIndex) {
                queueReferences[queueIndex]->queueInfoIndex = queueReferences[comparisonIndex]->queueInfoIndex;
                queueReferences[queueIndex]->queueIndex = queueReferences[comparisonIndex]->queueIndex + 1;

                queueDistinct = VK_FALSE;
                break;
            }

            if(!comparisonIndex--) {
                break;
            }
        }

        if(queueDistinct) {
            queueReferences[queueIndex]->queueInfoIndex = distinctQueueFamilyCount++;
        }
    }

    for(uint32_t queueIndex = 0; queueIndex < queueCount; queueIndex++) {
        queueReferences[queueIndex]->queueFamilySelected = VK_TRUE;
    }

    debug("Distinct queue family count: %d", distinctQueueFamilyCount);

    for(uint32_t queueIndex = 0; queueIndex < queueCount; queueIndex++) {
        debug(  "Queue %d:", queueIndex);
        debug(  "\tQueue Family Index: %d", queueReferences[queueIndex]->queueFamilyIndex);
        debug(  "\tQueue Index:        %d", queueReferences[queueIndex]->queueIndex);
        debug(  "\tQueue Info Index:   %d", queueReferences[queueIndex]->queueInfoIndex);
    }
}

void retrieveQueues() {
    VkDeviceQueueInfo2 *queueInfos = malloc(queueCount * sizeof(VkDeviceQueueInfo2));

    for(uint32_t queueIndex = 0; queueIndex < queueCount; queueIndex++) {
        queueInfos[queueIndex].sType = VK_STRUCTURE_TYPE_DEVICE_QUEUE_INFO_2;
        queueInfos[queueIndex].pNext = NULL;
        queueInfos[queueIndex].flags = 0;
        queueInfos[queueIndex].queueFamilyIndex = queueReferences[queueIndex]->queueFamilyIndex;
        queueInfos[queueIndex].queueIndex = queueReferences[queueIndex]->queueIndex;

        vkGetDeviceQueue2(device, &queueInfos[queueIndex], &queueReferences[queueIndex]->queue);
        queueReferences[queueIndex]->queueRetrieved = VK_TRUE;
        debug("Device queue %d retrieved", queueIndex);
    }

    free(queueInfos);
}

void createCommandStructures() {
    for(uint32_t queueIndex = 0; queueIndex < queueCount; queueIndex++) {
        VkCommandPoolCreateInfo commandPoolInfo = {
            .sType = VK_STRUCTURE_TYPE_COMMAND_POOL_CREATE_INFO,
            .pNext = NULL,
            .flags = 0,
            .queueFamilyIndex = queueReferences[queueIndex]->queueFamilyIndex
        };

        vkCreateCommandPool(device, &commandPoolInfo, NULL, &queueReferences[queueIndex]->commandPool);
        queueReferences[queueIndex]->commandStructuresCreated = VK_TRUE;
        debug("Command pool %d created", queueIndex);
    }
}

void destroyCommandStructures() {
    for(uint32_t queueIndex = 0; queueIndex < queueCount; queueIndex++) {
        vkDestroyCommandPool(device, queueReferences[queueIndex]->commandPool, NULL);
        queueReferences[queueIndex]->commandStructuresCreated = VK_FALSE;
        debug("Command pool %d destroy", queueIndex);
    }
}
