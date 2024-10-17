#include "memory.h"

#include "helper.h"
#include "buffer.h"
#include "image.h"

#include "swapchain.h"

extern VkExtent2D extent;
extern VkDevice device;
extern Swapchain swapchain;

VkPhysicalDeviceMemoryProperties memoryProperties;

Memory  imageMemory;
Memory deviceMemory;
Memory   hostMemory;

VkDeviceSize alignMemory(Memory *memory, VkMemoryRequirements memoryRequirements) {
    VkDeviceSize bindOffset = (memory->offset + memoryRequirements.alignment - 1) / memoryRequirements.alignment * memoryRequirements.alignment;

    memory->offset = bindOffset + memoryRequirements.size;

    return bindOffset;
}

void allocateMemory(Memory *memory, VkMemoryRequirements memoryRequirements, VkMemoryPropertyFlags requiredProperties) {
    memory->requiredProperties = requiredProperties;
    memory->typeIndex = UINT32_MAX;
    memory->offset = 0;

    for(uint32_t memoryIndex = 0; memoryIndex < memoryProperties.memoryTypeCount; memoryIndex++) {
        if((memoryRequirements.memoryTypeBits & (1 << memoryIndex)) && (memoryProperties.memoryTypes[memoryIndex].propertyFlags & requiredProperties) == requiredProperties) {
            memory->typeIndex = memoryIndex; // TODO: Implement an actual logic
            break;
        }
    }

    assert(memory->typeIndex < memoryProperties.memoryTypeCount);

    // TODO: Size is arbitrary, implement real logic
    memory->size = memoryProperties.memoryHeaps[memoryProperties.memoryTypes[memory->typeIndex].heapIndex].size / 4;

    VkMemoryAllocateInfo memoryInfo = {
        .sType = VK_STRUCTURE_TYPE_MEMORY_ALLOCATE_INFO,
        .pNext = NULL,
        .allocationSize = memory->size,
        .memoryTypeIndex = memory->typeIndex
    };

    vkAllocateMemory(device, &memoryInfo, NULL, &memory->memory);
}

void allocateMemories() {
    Image image;
    Buffer buffer;
    VkDeviceSize size;
    VkMemoryRequirements memoryRequirements;

    createImage(&image, extent.width, extent.height, 1, VK_SAMPLE_COUNT_1_BIT, swapchain.surfaceFormat.format,
        VK_IMAGE_USAGE_COLOR_ATTACHMENT_BIT | VK_IMAGE_USAGE_TRANSFER_DST_BIT);
    vkGetImageMemoryRequirements(device, image.image, &memoryRequirements);
    size = memoryRequirements.size;

    allocateMemory(&imageMemory, memoryRequirements, VK_MEMORY_PROPERTY_DEVICE_LOCAL_BIT);
    destroyImage(&image);

    debug("Image memory allocated: %ld bytes", imageMemory.size);
    debug("\tSuitable type indices:\t%08u", byte_to_binary(memoryRequirements.memoryTypeBits));
    debug("\tSelected type index:\t%u", imageMemory.typeIndex);

    createBuffer(&buffer, VK_BUFFER_USAGE_INDEX_BUFFER_BIT | VK_BUFFER_USAGE_VERTEX_BUFFER_BIT | VK_BUFFER_USAGE_UNIFORM_BUFFER_BIT | VK_BUFFER_USAGE_TRANSFER_DST_BIT, size);
    vkGetBufferMemoryRequirements(device, buffer.buffer, &memoryRequirements);

    allocateMemory(&deviceMemory, memoryRequirements, VK_MEMORY_PROPERTY_DEVICE_LOCAL_BIT);
    destroyBuffer(&buffer);

    debug("Device memory allocated: %ld bytes", deviceMemory.size);
    debug("\tSuitable type indices:\t%08u", byte_to_binary(memoryRequirements.memoryTypeBits));
    debug("\tSelected type index:\t%u", deviceMemory.typeIndex);

    createBuffer(&buffer, VK_BUFFER_USAGE_TRANSFER_SRC_BIT, size);
    vkGetBufferMemoryRequirements(device, buffer.buffer, &memoryRequirements);

    allocateMemory(&hostMemory, memoryRequirements, VK_MEMORY_PROPERTY_HOST_VISIBLE_BIT | VK_MEMORY_PROPERTY_HOST_COHERENT_BIT);
    destroyBuffer(&buffer);

    debug("Host memory allocated: %ld bytes", hostMemory.size);
    debug("\tSuitable type indices:\t%08u", byte_to_binary(memoryRequirements.memoryTypeBits));
    debug("\tSelected type index:\t%u", hostMemory.typeIndex);
}

void freeMemory(Memory *memory) {
    vkFreeMemory(device, memory->memory, NULL);

    memory->requiredProperties = 0;
    memory->typeIndex = UINT32_MAX;
    memory->size = 0;
    memory->offset = 0;
}

void freeMemories() {
    freeMemory(&  hostMemory);
    debug("Host memory freed");

    freeMemory(&deviceMemory);
    debug("Device memory freed");

    freeMemory(& imageMemory);
    debug("Image memory freed");
}
