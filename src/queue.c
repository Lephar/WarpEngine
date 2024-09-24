#include "queue.h"
#include "helper.h"

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

