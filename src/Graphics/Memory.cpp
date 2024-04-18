#include "Graphics/Memory.hpp"
#include "Graphics/Renderer.hpp"

void Memory::allocate(Renderer *owner, vk::PhysicalDeviceMemoryProperties memoryProperties, uint32_t filter, vk::MemoryPropertyFlags properties, vk::DeviceSize size) {
    this->owner = owner;
	this->size = size;
	offset = 0;

	for (auto index = 0u; index < memoryProperties.memoryTypeCount; index++) {
		if ((filter & (1 << index)) && (memoryProperties.memoryTypes[index].propertyFlags & properties) == properties) {
			type = index;
            break;
        }
    }

	vk::MemoryAllocateInfo memoryInfo {
		.allocationSize = size,
		.memoryTypeIndex = type
	};

	memory = owner->getDevice().allocateMemory(memoryInfo);

	allocated = true;
}

void Memory::align(vk::DeviceSize alignment) {
	offset = (offset + alignment - 1) / alignment * alignment;
}

vk::DeviceSize Memory::bind(vk::Buffer &buffer) {
	auto requirements = owner->getDevice().getBufferMemoryRequirements(buffer);

	align(requirements.alignment);
    auto bufferOffset = offset;

	owner->getDevice().bindBufferMemory(buffer, memory, offset);
	offset += requirements.size;

	return bufferOffset;
}

vk::DeviceSize Memory::bind(vk::Image &image) {
	auto requirements = owner->getDevice().getImageMemoryRequirements(image);

	align(requirements.alignment);
    auto imageOffset = offset;
    
	owner->getDevice().bindImageMemory(image, memory, offset);
	offset += requirements.size;

	return imageOffset;
}

void Memory::destroy() {
	owner->getDevice().freeMemory(memory);
	
	allocated = false;

    offset = 0;
    size = 0;

    type = std::numeric_limits<uint32_t>::max();

	owner = nullptr;
}
