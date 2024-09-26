#include "queue.h"
#include "helper.h"

extern VkDevice device;

uint32_t queueFamilyCount;
VkQueueFamilyProperties *queueFamilyProperties;

Queue graphicsQueue;
Queue computeQueue;
Queue transferQueue;

Queue *queues[] = {
    &graphicsQueue,
    & computeQueue,
    &transferQueue
};

uint32_t queueCount;
uint32_t distinctQueueFamilyCount;

uint32_t chooseQueueFamily(VkQueueFlags requiredFlags) {
    uint32_t mostSuitedScore = 0;
    uint32_t mostSuitedIndex = UINT32_MAX;

    for(uint32_t queueFamilyIndex = 0; queueFamilyIndex < queueFamilyCount; queueFamilyIndex++) {
        VkQueueFlags supportedFlags = queueFamilyProperties[queueFamilyIndex].queueFlags;
        VkQueueFlags satisfiedFlags = supportedFlags &  requiredFlags;
        VkQueueFlags redundantFlags = supportedFlags & ~requiredFlags;

        uint32_t queueFamilyScore = ~popcount(redundantFlags);

        if(requiredFlags == satisfiedFlags && mostSuitedScore < queueFamilyScore) {
            mostSuitedScore = queueFamilyScore;
            mostSuitedIndex = queueFamilyIndex;
        }
    }

    assert(mostSuitedIndex < queueFamilyCount);
    return mostSuitedIndex;
}

void generateQueueDetails() {
    queueCount = sizeof(queues) / sizeof(Queue*);
    debug("Queue count: %d", queueCount);

    graphicsQueue.queueFamilyIndex = chooseQueueFamily(VK_QUEUE_GRAPHICS_BIT);
    computeQueue .queueFamilyIndex = chooseQueueFamily(VK_QUEUE_COMPUTE_BIT );
    transferQueue.queueFamilyIndex = chooseQueueFamily(VK_QUEUE_TRANSFER_BIT);

    debug("Graphics queue family index: %d", graphicsQueue.queueFamilyIndex);
    debug("Compute  queue family index: %d", computeQueue .queueFamilyIndex);
    debug("Transfer queue family index: %d", transferQueue.queueFamilyIndex);

    distinctQueueFamilyCount = 1;
    
    for(uint32_t queueIndex = 1; queueIndex < queueCount; queueIndex++) {
        VkBool32 queueDistinct = VK_TRUE;
        uint32_t comparisonIndex = queueIndex - 1;

        while(1) {
            if(queues[queueIndex]->queueFamilyIndex == queues[comparisonIndex]->queueFamilyIndex) {
                queues[queueIndex]->queueInfoIndex = queues[comparisonIndex]->queueInfoIndex;
                queues[queueIndex]->queueIndex = queues[comparisonIndex]->queueIndex + 1;

                queueDistinct = VK_FALSE;
                break;
            }

            if(!comparisonIndex--){
                break;
            }
        }

        if(queueDistinct) {
            queues[queueIndex]->queueInfoIndex = distinctQueueFamilyCount++;
        }
    }

    debug("Graphics queue index: %d", graphicsQueue.queueIndex);
    debug("Compute  queue index: %d", computeQueue .queueIndex);
    debug("Transfer queue index: %d", transferQueue.queueIndex);

    debug("Graphics queue info index: %d", graphicsQueue.queueInfoIndex);
    debug("Compute  queue info index: %d", computeQueue .queueInfoIndex);
    debug("Transfer queue info index: %d", transferQueue.queueInfoIndex);

    debug("Distinct queue family count: %d", distinctQueueFamilyCount);
}

void retrieveQueues() {
    VkDeviceQueueInfo2 *queueInfos = malloc(queueCount * sizeof(VkDeviceQueueInfo2));
    
    for(uint32_t queueIndex = 0; queueIndex < queueCount; queueIndex++) {
        queueInfos[queueIndex].sType = VK_STRUCTURE_TYPE_DEVICE_QUEUE_INFO_2;
        queueInfos[queueIndex].pNext = NULL;
        queueInfos[queueIndex].flags = 0;
        queueInfos[queueIndex].queueFamilyIndex = queues[queueIndex]->queueFamilyIndex;
        queueInfos[queueIndex].queueIndex = queues[queueIndex]->queueIndex;
    }

    vkGetDeviceQueue2(device, &queueInfos[0], &graphicsQueue.queue);
    vkGetDeviceQueue2(device, &queueInfos[1], & computeQueue.queue);
    vkGetDeviceQueue2(device, &queueInfos[2], &transferQueue.queue);

    debug("%d device queues retrieved", queueCount);

    free(queueInfos);
}
