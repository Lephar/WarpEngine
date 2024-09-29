#include "memory.h"

VkPhysicalDeviceMemoryProperties memoryProperties;

Memory imageMemory;
Memory hostMemory;
Memory deviceMemory;

Memory *memoryReferences[] = {
    &imageMemory,
    &hostMemory,
    &deviceMemory
};

void generateMemoryDetails() {

}

void allocateMemories() {

}

void freeMemories() {

}
