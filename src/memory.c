#include "memory.h"

#include "helper.h"
#include "buffer.h"
#include "image.h"

extern VkDevice device;

VkPhysicalDeviceMemoryProperties memoryProperties;

Memory deviceMemory;
Memory sharedMemory;

void *mappedSharedMemory;

VkDeviceSize alignMemory(Memory *memory, VkMemoryRequirements memoryRequirements) {
    VkDeviceSize bindOffset = (memory->offset + memoryRequirements.alignment - 1) / memoryRequirements.alignment * memoryRequirements.alignment;

    memory->offset = bindOffset + memoryRequirements.size;

    return bindOffset;
}

void allocateMemory(Memory *memory, uint32_t typeFilter, VkMemoryPropertyFlags properties, VkDeviceSize size) {
    memory->properties = properties;
    memory->typeIndex = UINT32_MAX;
    memory->offset = 0;
    memory->size = size;

    for(uint32_t memoryIndex = 0; memoryIndex < memoryProperties.memoryTypeCount; memoryIndex++) {
        if((typeFilter & (1 << memoryIndex)) && (memoryProperties.memoryTypes[memoryIndex].propertyFlags & properties) == properties) {
            memory->typeIndex = memoryIndex; // TODO: Implement an actual logic
            break;
        }
    }

    assert(memory->typeIndex < memoryProperties.memoryTypeCount);
    assert(memory->size < memoryProperties.memoryHeaps[memoryProperties.memoryTypes[memory->typeIndex].heapIndex].size); // TODO: Take used memory into account

    VkMemoryAllocateInfo memoryInfo = {
        .sType = VK_STRUCTURE_TYPE_MEMORY_ALLOCATE_INFO,
        .pNext = NULL,
        .allocationSize = memory->size,
        .memoryTypeIndex = memory->typeIndex
    };

    vkAllocateMemory(device, &memoryInfo, NULL, &memory->memory);
}

void *mapMemory(Memory *memory) {
    assert((memory->properties & VK_MEMORY_PROPERTY_HOST_VISIBLE_BIT) && memory->memory != VK_NULL_HANDLE);

    void *map;

    vkMapMemory(device, memory->memory, 0, VK_WHOLE_SIZE, 0, &map);

    return map;
}

void allocateMemories() {
    Image temporaryImage;
    VkMemoryRequirements imageMemoryRequirements;

    Buffer temporaryBuffer;
    VkMemoryRequirements bufferMemoryRequirements;

    uint32_t typeFilter;

    createImage(&temporaryImage, 800, 600, 1, VK_SAMPLE_COUNT_1_BIT, VK_FORMAT_R8G8B8A8_UNORM,
        VK_IMAGE_USAGE_COLOR_ATTACHMENT_BIT | VK_IMAGE_USAGE_TRANSFER_DST_BIT);
    vkGetImageMemoryRequirements(device, temporaryImage.image, &imageMemoryRequirements);

    createBuffer(&temporaryBuffer, VK_BUFFER_USAGE_INDEX_BUFFER_BIT | VK_BUFFER_USAGE_VERTEX_BUFFER_BIT | VK_BUFFER_USAGE_TRANSFER_DST_BIT, 1L << 20);
    vkGetBufferMemoryRequirements(device, temporaryBuffer.buffer, &bufferMemoryRequirements);

    typeFilter = imageMemoryRequirements.memoryTypeBits & bufferMemoryRequirements.memoryTypeBits;

    debug("Device local memory:");
    debug("\tSuitable type indices:\t%08u", binarize(typeFilter));

    allocateMemory(&deviceMemory, typeFilter, VK_MEMORY_PROPERTY_DEVICE_LOCAL_BIT, 512L << 20);
    destroyBuffer(&temporaryBuffer);
    destroyImage(&temporaryImage);

    debug("\tSelected type index:\t%u", deviceMemory.typeIndex);
    debug("\t%ld bytes allocated", deviceMemory.size);

    createBuffer(&temporaryBuffer, VK_BUFFER_USAGE_UNIFORM_BUFFER_BIT | VK_BUFFER_USAGE_TRANSFER_SRC_BIT, 1L << 20);
    vkGetBufferMemoryRequirements(device, temporaryBuffer.buffer, &bufferMemoryRequirements);

    typeFilter = bufferMemoryRequirements.memoryTypeBits;

    debug("Host visible memory:");
    debug("\tSuitable type indices:\t%08u", binarize(typeFilter));

    allocateMemory(&sharedMemory, typeFilter,
        VK_MEMORY_PROPERTY_DEVICE_LOCAL_BIT | VK_MEMORY_PROPERTY_HOST_VISIBLE_BIT | VK_MEMORY_PROPERTY_HOST_COHERENT_BIT, 128L << 20);
    destroyBuffer(&temporaryBuffer);

    debug("\tSelected type index:\t%u", sharedMemory.typeIndex);
    debug("\t%ld bytes allocated",  sharedMemory.size);

    mappedSharedMemory = mapMemory(&sharedMemory);
    debug("\tSuccessfully mapped");
}

void unmapMemory(Memory *memory) {
    vkUnmapMemory(device, memory->memory);
}

void freeMemory(Memory *memory) {
    vkFreeMemory(device, memory->memory, NULL);

    memory->properties = 0;
    memory->typeIndex = UINT32_MAX;
    memory->size = 0;
    memory->offset = 0;
}

void freeMemories() {
    unmapMemory(&sharedMemory);
    freeMemory(&sharedMemory);
    debug("Host visible memory freed");

    freeMemory(&deviceMemory);
    debug("Device local memory freed");
}
