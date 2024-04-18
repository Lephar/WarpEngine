#include "Graphics/Image.hpp"
#include "Graphics/Renderer.hpp"

void Image::create(Renderer *owner, uint32_t width, uint32_t height, vk::Format format, vk::ImageUsageFlags usage, vk::ImageAspectFlags aspects, vk::SampleCountFlagBits samples, uint32_t mips) {
    this->owner = owner;

    this->width = width;
    this->height = height;
	this->format = format;
    this->usage = usage;
    this->aspects = aspects;
    this->samples = samples;
    this->mips = mips;

	vk::ImageCreateInfo imageInfo {
		.imageType = vk::ImageType::e2D,
		.format = format,
		.extent = vk::Extent3D {
			.width = width,
			.height = height,
			.depth = 1
		},
		.mipLevels = mips,
		.arrayLayers = 1,
		.samples = samples,
		.tiling = vk::ImageTiling::eOptimal,
		.usage = usage,
		.sharingMode = vk::SharingMode::eExclusive,
		.initialLayout = vk::ImageLayout::eUndefined
	};

	image = owner->getDevice().createImage(imageInfo);

	imageCreated = true;
}

void Image::bindMemory(Memory *memory) {
	if(!imageCreated)
		return;

	memory->bind(image);

	memoryBound = true;
}

void Image::createView() {
	if(!memoryBound)
		return;

	vk::ImageViewCreateInfo viewInfo {
		.image = image,
		.viewType = vk::ImageViewType::e2D,
		.format = format,
		.components = vk::ComponentMapping {
			.r = vk::ComponentSwizzle::eIdentity,
			.g = vk::ComponentSwizzle::eIdentity,
			.b = vk::ComponentSwizzle::eIdentity,
			.a = vk::ComponentSwizzle::eIdentity
		},
		.subresourceRange = vk::ImageSubresourceRange {
			.aspectMask = aspects,
			.baseMipLevel = 0,
			.levelCount = mips,
			.baseArrayLayer = 0,
			.layerCount = 1
		}
	};

	view = owner->getDevice().createImageView(viewInfo);

	viewCreated = true;
}

void Image::copyFromBuffer(vk::Buffer &source) {
	if(!memoryBound)
		return;

	vk::BufferImageCopy region {
		.bufferOffset = 0,
		.bufferRowLength = 0,
		.bufferImageHeight = 0,
		.imageSubresource = vk::ImageSubresourceLayers {
			.aspectMask = aspects,
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
			.width = width,
			.height = height,
			.depth = 1
		}
	};

	auto commandBuffer = owner->beginSingleTimeCommand();
	commandBuffer.copyBufferToImage(source, image, vk::ImageLayout::eTransferDstOptimal, 1, &region);
	owner->endSingleTimeCommand(commandBuffer);
}

void Image::transitionLayout(vk::ImageLayout oldLayout, vk::ImageLayout newLayout) {
	if(!memoryBound)
		return;

	vk::ImageMemoryBarrier imageMemoryBarrier {
		.srcAccessMask = vk::AccessFlags{},
		.dstAccessMask = vk::AccessFlags{},
		.oldLayout = oldLayout,
		.newLayout = newLayout,
		.srcQueueFamilyIndex = VK_QUEUE_FAMILY_IGNORED,
		.dstQueueFamilyIndex = VK_QUEUE_FAMILY_IGNORED,
		.image = image,
		.subresourceRange = vk::ImageSubresourceRange {
			.aspectMask = aspects,
			.baseMipLevel = 0,
			.levelCount = mips,
			.baseArrayLayer = 0,
			.layerCount = 1
		}
	};

    auto commandBuffer = owner->beginSingleTimeCommand();
	commandBuffer.pipelineBarrier(vk::PipelineStageFlagBits::eTopOfPipe, vk::PipelineStageFlagBits::eBottomOfPipe, vk::DependencyFlags{}, 0, nullptr, 0, nullptr, 1, &imageMemoryBarrier);
    owner->endSingleTimeCommand(commandBuffer);
}

void Image::destroy() {
	if(viewCreated) {
    	owner->getDevice().destroyImageView(view);
		viewCreated = false;
	}

    if(imageCreated) {
    	owner->getDevice().destroyImage(image);
		imageCreated = false;
	}

	memoryBound = false;

    owner = nullptr;
}