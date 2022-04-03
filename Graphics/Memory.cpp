module;

#include "Graphics.hpp"

module Engine:Graphics;

namespace Engine::Graphics {
	extern vk::PhysicalDeviceProperties physicalDeviceProperties;
	extern vk::Device device;

	unsigned int sharedMemoryIndex;
	vk::DeviceSize sharedMemorySize;
	vk::DeviceMemory sharedMemory;
	unsigned char* sharedMemoryPointer;

	unsigned int deviceMemoryIndex;
	vk::DeviceSize deviceMemorySize;
	vk::DeviceSize deviceMemoryOffset;
	vk::DeviceMemory deviceMemory;

	vk::DeviceSize deviceBufferSize;
	vk::Buffer deviceBuffer;

	vk::DeviceSize sharedBufferStride;
	vk::DeviceSize sharedBufferSize;
	vk::Buffer sharedBuffer;

	void initializeMemoryProperties() {
		// TODO: Better memory index selection
		sharedMemoryIndex = 4;
		deviceMemoryIndex = 1;

		sharedMemorySize = 256;
		deviceMemorySize = 256;

		deviceMemoryOffset = 0;
	}

	void allocateMemories() {
		vk::MemoryAllocateInfo sharedMemoryInfo {
			.allocationSize = sharedMemorySize,
			.memoryTypeIndex = sharedMemoryIndex
		};

		vk::MemoryAllocateInfo deviceMemoryInfo {
			.allocationSize = deviceMemorySize,
			.memoryTypeIndex = deviceMemoryIndex
		};

		sharedMemory = device.allocateMemory(sharedMemoryInfo);
		deviceMemory = device.allocateMemory(deviceMemoryInfo);

		sharedMemoryPointer = static_cast<unsigned char*>(device.mapMemory(sharedMemory, 0, VK_WHOLE_SIZE));
	}

	void initializeBufferProperties() {
		sharedBufferStride = physicalDeviceProperties.limits.minUniformBufferOffsetAlignment;

		deviceBufferSize = 256;
		sharedBufferSize = 256;
	}

	void createBuffers() {
		vk::BufferCreateInfo sharedBufferInfo {
			.size = sharedBufferSize,
			.usage =
				vk::BufferUsageFlagBits::eTransferSrc |
				vk::BufferUsageFlagBits::eUniformBuffer,
			.sharingMode = vk::SharingMode::eExclusive
		};

		vk::BufferCreateInfo deviceBufferInfo {
			.size = deviceBufferSize,
			.usage =
				vk::BufferUsageFlagBits::eTransferDst |
				vk::BufferUsageFlagBits::eIndexBuffer |
				vk::BufferUsageFlagBits::eVertexBuffer,
			.sharingMode = vk::SharingMode::eExclusive
		};

		sharedBuffer = device.createBuffer(sharedBufferInfo);
		deviceBuffer = device.createBuffer(deviceBufferInfo);
	}

	void alignDeviceMemoryOffset(vk::MemoryRequirements requirements) {
		if (deviceMemoryOffset % requirements.alignment)
			deviceMemoryOffset = (deviceMemoryOffset / requirements.alignment + 1) * requirements.alignment;

		deviceMemoryOffset += requirements.size;
	}

	void bindBufferMemories() {
		device.bindBufferMemory(sharedBuffer, sharedMemory, 0);

		vk::MemoryRequirements deviceBufferRequirements = device.getBufferMemoryRequirements(deviceBuffer);
		device.bindBufferMemory(deviceBuffer, deviceMemory, 0);
		alignDeviceMemoryOffset(deviceBufferRequirements);
	}

	void createObjects() {
		initializeMemoryProperties();
		allocateMemories();
		initializeBufferProperties();
		createBuffers();
		bindBufferMemories();
	}

	void destroyObjects() {
		device.destroyBuffer(deviceBuffer);
		device.destroyBuffer(sharedBuffer);

		device.unmapMemory(sharedMemory);

		device.freeMemory(deviceMemory);
		device.freeMemory(sharedMemory);
	}
}