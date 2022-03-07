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

	vk::DeviceSize Memory::alignOffset(vk::MemoryRequirements requirements) {
		if (offset % requirements.alignment)
			offset = (offset / requirements.alignment + 1) * requirements.alignment;

		vk::DeviceSize bindOffset = offset;
		offset += requirements.size;

		return bindOffset;
	}

	vk::DeviceMemory& Memory::getMemoryHandle() {
		return memory;
	}

	void Memory::free() {
		device.freeMemory(memory);
	}
}