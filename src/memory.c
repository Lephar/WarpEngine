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

VkDeviceSize alignMemory(Memory *memory, VkMemoryRequirements memoryRequirements) {
	VkDeviceSize bindOffset = (memory->offset + memoryRequirements.alignment - 1) / memoryRequirements.alignment * memoryRequirements.alignment;

	memory->offset = bindOffset + memoryRequirements.size;

	return bindOffset;
}

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
