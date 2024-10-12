#include "memory.h"

extern VkDevice device;

VkPhysicalDeviceMemoryProperties memoryProperties;

Memory  imageMemory;
Memory deviceMemory;
Memory   hostMemory;

Memory *memoryReferences[] = {
    & imageMemory,
    &deviceMemory,
    &  hostMemory
};

Memory allocateMemory(uint32_t typeFilter, VkMemoryPropertyFlags requiredProperties, VkDeviceSize size) {
    Memory memory = {
        .allocated = VK_FALSE,
        .typeIndex = UINT32_MAX,
        .size = size,
        .offset = 0,
        .memory = {}
    };

    for(uint32_t memoryIndex = 0; memoryIndex < memoryProperties.memoryTypeCount; memoryIndex++) {
        if((typeFilter & (1 << memoryIndex)) && (memoryProperties.memoryTypes[memoryIndex].propertyFlags & requiredProperties) == requiredProperties) {
            memory.typeIndex = memoryIndex; // TODO: Implement an actual logic
            break;
        }
    }

    assert(memory.typeIndex < memoryProperties.memoryTypeCount);

    VkMemoryAllocateInfo memoryInfo = {
        .allocationSize = memory.size,
        .memoryTypeIndex = memory.typeIndex
    };

    vkAllocateMemory(device, &memoryInfo, NULL, &memory.memory);
    memory.allocated = VK_TRUE;

    return memory;
}

VkDeviceSize alignMemory(Memory *memory, VkMemoryRequirements memoryRequirements) {
    VkDeviceSize bindOffset = (memory->offset + memoryRequirements.alignment - 1) / memoryRequirements.alignment * memoryRequirements.alignment;

    memory->offset = bindOffset + memoryRequirements.size;

    return bindOffset;
}

void generateMemoryDetails() {
     imageMemory.requiredProperties = VK_MEMORY_PROPERTY_DEVICE_LOCAL_BIT ;
    deviceMemory.requiredProperties = VK_MEMORY_PROPERTY_DEVICE_LOCAL_BIT ;
      hostMemory.requiredProperties = VK_MEMORY_PROPERTY_HOST_VISIBLE_BIT |
                                      VK_MEMORY_PROPERTY_HOST_COHERENT_BIT;
}

void allocateMemories() {

}

void freeMemories() {

}
