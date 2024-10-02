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

uint32_t chooseMemoryType(uint32_t filter, VkMemoryPropertyFlags flags)
{
	for(uint32_t index = 0; index < memoryProperties.memoryTypeCount; index++)
		if((filter & (1 << index)) && (memoryProperties.memoryTypes[index].propertyFlags & flags) == flags)
			return index;

	return 0;
}

void generateMemoryDetails() {

}

void allocateMemories() {

}

void freeMemories() {

}
