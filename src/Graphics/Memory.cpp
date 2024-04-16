#include "Graphics/Memory.hpp"
#include "Graphics/Renderer.hpp"

void Memory::initialize(Renderer *owner) {
    this->owner = owner;

    type = std::numeric_limits<uint32_t>::max();

    size = 0;
    offset = 0;
}

void Memory::chooseType(uint32_t filter, vk::MemoryPropertyFlags properties) {
	for (auto index = 0u; index < owner->memoryProperties.memoryTypeCount; index++) {
		if ((filter & (1 << index)) && (owner->memoryProperties.memoryTypes[index].propertyFlags & properties) == properties) {
			type = index;
            break;
        }
    }
}

void Memory::allocate(vk::DeviceSize size) {
	vk::MemoryAllocateInfo memoryInfo {
		.allocationSize = size,
		.memoryTypeIndex = type
	};

	memory = owner->device.allocateMemory(memoryInfo);
}

void Memory::align(vk::DeviceSize alignment) {
	offset = (offset + alignment - 1) / alignment * alignment;
}

vk::DeviceSize Memory::bind(vk::Buffer &buffer) {
	auto requirements = owner->device.getBufferMemoryRequirements(buffer);

	align(requirements.alignment);
    auto bufferOffset = offset;

	owner->device.bindBufferMemory(buffer, memory, offset);
	offset += requirements.size;

	return bufferOffset;
}

vk::DeviceSize Memory::bind(vk::Image &image) {
	auto requirements = owner->device.getImageMemoryRequirements(image);

	align(requirements.alignment);
    auto imageOffset = offset;
    
	owner->device.bindImageMemory(image, memory, offset);
	offset += requirements.size;

	return imageOffset;
}

void Memory::destroy() {
	owner->device.freeMemory(memory);

    offset = 0;
    size = 0;

    type = std::numeric_limits<uint32_t>::max();

	owner = nullptr;
}
