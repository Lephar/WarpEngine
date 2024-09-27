#include "queue.h"
#include "helper.h"

extern VkDevice device;

uint32_t queueFamilyCount;
VkQueueFamilyProperties *queueFamilyProperties;

Queue graphicsQueue = {.requiredFlags = VK_QUEUE_GRAPHICS_BIT};
Queue  computeQueue = {.requiredFlags = VK_QUEUE_COMPUTE_BIT };
Queue transferQueue = {.requiredFlags = VK_QUEUE_TRANSFER_BIT};

Queue *queueReferences[] = {
    &graphicsQueue,
    & computeQueue,
    &transferQueue
};

uint32_t queueCount;
uint32_t distinctQueueFamilyCount;

void generateQueueDetails() {
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
        debug("Device queue %d retrieved", queueIndex);
    }

    free(queueInfos);
}

void createCommandStructures() {

}
