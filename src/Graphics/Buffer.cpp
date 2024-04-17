#include "Graphics/Buffer.hpp"
#include "Graphics/Renderer.hpp"

void Buffer::initalize(Renderer *owner) {
    this->owner = owner;

    memory = nullptr;

    size = 0;
    offset = 0;
}

void Buffer::create(vk::DeviceSize size, vk::BufferUsageFlags usage) {
    this->size = size;

	vk::BufferCreateInfo bufferInfo {
		.size = size,
		.usage = usage,
		.sharingMode = vk::SharingMode::eExclusive
	};

	buffer = owner->device.createBuffer(bufferInfo);
}

void Buffer::bindMemory(Memory *memory) {
    this->memory = memory;
    offset = memory->bind(buffer);
}

void Buffer::copy(Buffer &destination) {
	vk::BufferCopy bufferCopy {
		.srcOffset = 0,
		.dstOffset = 0,
		.size = size
	};

	auto commandBuffer = owner->beginSingleTimeCommand();
	commandBuffer.copyBuffer(buffer, destination.buffer, bufferCopy);
	owner->endSingleTimeCommand(commandBuffer);
}

void Buffer::copyToImage(Image &destination) {
	vk::BufferImageCopy region {
		.bufferOffset = 0,
		.bufferRowLength = 0,
		.bufferImageHeight = 0,
		.imageSubresource = vk::ImageSubresourceLayers {
			.aspectMask = vk::ImageAspectFlagBits::eColor,
			.mipLevel = 0,
			.baseArrayLayer = 0,
			.layerCount = 1,
		},
		.imageOffset = vk::Offset3D {
			.x = 0,
			.y = 0,
			.z = 0
		},
		.imageExtent = vk::Extent3D {
			.width = destination.width,
			.height = destination.height,
			.depth = 1
		}
	};

	auto commandBuffer = owner->beginSingleTimeCommand();
	commandBuffer.copyBufferToImage(buffer, destination.image, vk::ImageLayout::eTransferDstOptimal, 1, &region);
	owner->endSingleTimeCommand(commandBuffer);
}

void Buffer::destroy() {
	owner->device.destroyBuffer(buffer);

    offset = 0;
    size = 0;

    memory = nullptr;
    owner = nullptr;
}