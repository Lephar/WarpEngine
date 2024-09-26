#include "queue.h"
#include "helper.h"

extern VkDevice device;

uint32_t queueFamilyCount;
VkQueueFamilyProperties *queueFamilyProperties;

Queue graphicsQueue;
Queue computeQueue;
Queue transferQueue;

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

void retrieveQueues() {
    Queue queues[] = {
        graphicsQueue,
        computeQueue,
        transferQueue
    };

    uint32_t queueCount = sizeof(queues) / sizeof(Queue);

    VkDeviceQueueInfo2 *queueInfos = malloc(queueCount * sizeof(VkDeviceQueueInfo2));
    
    for(uint32_t queueIndex = 0; queueIndex < queueCount; queueIndex++) {
        queueInfos[queueIndex].sType = VK_STRUCTURE_TYPE_DEVICE_QUEUE_INFO_2;
        queueInfos[queueIndex].pNext = NULL;
        queueInfos[queueIndex].flags = 0;
        queueInfos[queueIndex].queueFamilyIndex = queues[queueIndex].queueFamilyIndex;
        queueInfos[queueIndex].queueIndex = queues[queueIndex].queueIndex;
    }

    vkGetDeviceQueue2(device, &queueInfos[0], &graphicsQueue.queue);
    vkGetDeviceQueue2(device, &queueInfos[1], & computeQueue.queue);
    vkGetDeviceQueue2(device, &queueInfos[2], &transferQueue.queue);

    debug("%d device queues retrieved", queueCount);

    free(queueInfos);
}

