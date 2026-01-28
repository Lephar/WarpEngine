#include "memory.h"

#include "physicalDevice.h"
#include "device.h"
#include "buffer.h"
#include "image.h"

#include "numerics.h"
#include "logger.h"

Memory sharedMemory;
Memory contentMemory;
Memory frameMemory;

void *mappedSharedMemory;

VkDeviceSize alignMemory(Memory *memory, VkMemoryRequirements memoryRequirements) {
    VkDeviceSize bindOffset = align(memory->offset, memoryRequirements.alignment);

    memory->offset = bindOffset + memoryRequirements.size;

    return bindOffset;
}

void allocateMemory(Memory *memory, uint32_t typeFilter, VkMemoryPropertyFlags requiredProperties, VkDeviceSize size) {
    memory->properties = requiredProperties;
    memory->typeIndex = UINT32_MAX;
    memory->offset = 0;

    for(uint32_t memoryIndex = 0; memoryIndex < memoryProperties.memoryTypeCount; memoryIndex++) {
        if((typeFilter & (1 << memoryIndex)) && (memoryProperties.memoryTypes[memoryIndex].propertyFlags & requiredProperties) == requiredProperties) {
            memory->typeIndex = memoryIndex; // TODO: Implement an actual logic
            break;
        }
    }

    assert(memory->typeIndex < memoryProperties.memoryTypeCount);
    memory->size = ulmin(size, 3 * memoryProperties.memoryHeaps[memoryProperties.memoryTypes[memory->typeIndex].heapIndex].size / 4);

    while(true) {
        VkMemoryAllocateInfo memoryInfo = {
            .sType = VK_STRUCTURE_TYPE_MEMORY_ALLOCATE_INFO,
            .pNext = nullptr,
            .allocationSize = memory->size,
            .memoryTypeIndex = memory->typeIndex
        };

        VkResult result = vkAllocateMemory(device, &memoryInfo, nullptr, &memory->memory);
        assert(result == VK_SUCCESS || result == VK_ERROR_OUT_OF_HOST_MEMORY || result == VK_ERROR_OUT_OF_DEVICE_MEMORY);

        if(result == VK_SUCCESS) {
            break;
        }

        // result == VK_ERROR_OUT_OF_HOST_MEMORY || result == VK_ERROR_OUT_OF_DEVICE_MEMORY
        memory->size = 3 * memory->size / 4;
    }
}

void *mapMemory(Memory *memory) {
    assert(memory->memory != nullptr);
    assert(memory->properties & VK_MEMORY_PROPERTY_HOST_VISIBLE_BIT);

    void *map;

    vkMapMemory(device, memory->memory, 0, VK_WHOLE_SIZE, 0, &map);

    return map;
}

void allocateMemories() {
    Image *temporaryImage = createImage(800, 600, 1, VK_SAMPLE_COUNT_1_BIT, VK_FORMAT_R8G8B8A8_SRGB, VK_IMAGE_USAGE_COLOR_ATTACHMENT_BIT | VK_IMAGE_USAGE_TRANSFER_DST_BIT, VK_IMAGE_ASPECT_COLOR_BIT, VK_IMAGE_TILING_OPTIMAL);
    VkMemoryRequirements imageMemoryRequirements;
    vkGetImageMemoryRequirements(device, temporaryImage->image, &imageMemoryRequirements);

    // TODO: Syntax highlighting fails for %b but it compiles, contribute to clangd maybe?
    uint32_t typeFilter = imageMemoryRequirements.memoryTypeBits;
    destroyImage(temporaryImage);

    debug("Device local frame memory:");
    debug("\tSuitable type indices:\t%08b", typeFilter);

    allocateMemory(&frameMemory, typeFilter, VK_MEMORY_PROPERTY_DEVICE_LOCAL_BIT, 256L << 20L);

    debug("\tSelected type index:\t%u", frameMemory.typeIndex);
    debug("\t%ld bytes allocated", frameMemory.size);

    Buffer temporaryBuffer;
    createBuffer(&temporaryBuffer, VK_BUFFER_USAGE_INDEX_BUFFER_BIT | VK_BUFFER_USAGE_VERTEX_BUFFER_BIT | VK_BUFFER_USAGE_TRANSFER_DST_BIT, 1L << 20L);
    VkMemoryRequirements bufferMemoryRequirements;
    vkGetBufferMemoryRequirements(device, temporaryBuffer.buffer, &bufferMemoryRequirements);

    typeFilter &= bufferMemoryRequirements.memoryTypeBits;
    destroyBuffer(&temporaryBuffer);

    debug("Device local content memory:");
    debug("\tSuitable type indices:\t%08b", typeFilter);

    allocateMemory(&contentMemory, typeFilter, VK_MEMORY_PROPERTY_DEVICE_LOCAL_BIT, 2L << 30L);

    debug("\tSelected type index:\t%u", contentMemory.typeIndex);
    debug("\t%ld bytes allocated", contentMemory.size);

    createBuffer(&temporaryBuffer, VK_BUFFER_USAGE_UNIFORM_BUFFER_BIT | VK_BUFFER_USAGE_TRANSFER_SRC_BIT, 1L << 20L);
    vkGetBufferMemoryRequirements(device, temporaryBuffer.buffer, &bufferMemoryRequirements);

    typeFilter = bufferMemoryRequirements.memoryTypeBits;
    destroyBuffer(&temporaryBuffer);

    debug("Host visible shared memory:");
    debug("\tSuitable type indices:\t%08b", typeFilter);

    allocateMemory(&sharedMemory, typeFilter, VK_MEMORY_PROPERTY_DEVICE_LOCAL_BIT | VK_MEMORY_PROPERTY_HOST_VISIBLE_BIT | VK_MEMORY_PROPERTY_HOST_COHERENT_BIT, 256L << 20L);

    debug("\tSelected type index:\t%u", sharedMemory.typeIndex);
    debug("\t%ld bytes allocated",  sharedMemory.size);

    mappedSharedMemory = mapMemory(&sharedMemory);
    debug("\tSuccessfully mapped");
}

void unmapMemory(Memory *memory) {
    vkUnmapMemory(device, memory->memory);
}

void freeMemory(Memory *memory) {
    vkFreeMemory(device, memory->memory, nullptr);

    memory->properties = 0;
    memory->typeIndex = UINT32_MAX;
    memory->size = 0;
    memory->offset = 0;
}

void freeMemories() {
    unmapMemory(&sharedMemory);
    freeMemory(&sharedMemory);
    debug("Host visible shared memory freed");

    freeMemory(&contentMemory);
    debug("Device local content memory freed");

    freeMemory(&frameMemory);
    debug("Device local frame memory freed");
}
