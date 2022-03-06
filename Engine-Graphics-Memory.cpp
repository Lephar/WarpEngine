module;

#include "Engine-Graphics.hpp"

module Engine:Graphics;

namespace Engine::Graphics {
	extern vk::Device device;

	Memory::Memory(unsigned int typeIndex, vk::DeviceSize allocationSize) : size(allocationSize), offset(0) {
		vk::MemoryAllocateInfo allocateInfo {
			.allocationSize = size,
			.memoryTypeIndex = typeIndex
		};

		memory = device.allocateMemory(allocateInfo);
	}

	vk::DeviceSize Memory::alignOffset(vk::DeviceSize alignment) {
		if (offset % alignment)
			offset = (offset / alignment + 1) * alignment;

		return offset;
	}

	vk::DeviceMemory& Memory::getMemoryHandle() {
		return memory;
	}

	vk::DeviceSize Memory::bindBuffer(vk::Buffer& buffer) {
		vk::MemoryRequirements requirements = device.getBufferMemoryRequirements(buffer);

		vk::DeviceSize bufferOffset = alignOffset(requirements.alignment);
		device.bindBufferMemory(buffer, memory, offset);
		offset += requirements.size;

		return bufferOffset;
	}

	vk::DeviceSize Memory::bindImage(vk::Image& image) {
		vk::MemoryRequirements requirements = device.getImageMemoryRequirements(image);

		vk::DeviceSize imageOffset = alignOffset(requirements.alignment);
		device.bindImageMemory(image, memory, offset);
		offset += requirements.size;

		return imageOffset;
	}

	void Memory::free() {
		device.freeMemory(memory);
	}
}