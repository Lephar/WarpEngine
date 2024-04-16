#include "Graphics/Image.hpp"
#include "Graphics/Renderer.hpp"

void Image::initialize(Renderer *owner) {
    this->owner = owner;
}

void Image::create(uint32_t width, uint32_t height, vk::Format format, vk::ImageUsageFlags usage, vk::SampleCountFlagBits samples, uint32_t mips) {
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

	image = owner->device.createImage(imageInfo);
}

void Image::createView(vk::Format format, vk::ImageAspectFlags aspects, uint32_t mips) {
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

	view = owner->device.createImageView(viewInfo);
}

void Image::bindMemory(Memory *memory) {
    this->memory = memory;
	memory->bind(image);
}

void Image::transitionLayout(vk::ImageAspectFlags aspectFlags, vk::ImageLayout oldLayout, vk::ImageLayout newLayout) {
	vk::ImageMemoryBarrier imageMemoryBarrier {
		.srcAccessMask = vk::AccessFlags{},
		.dstAccessMask = vk::AccessFlags{},
		.oldLayout = oldLayout,
		.newLayout = newLayout,
		.srcQueueFamilyIndex = VK_QUEUE_FAMILY_IGNORED,
		.dstQueueFamilyIndex = VK_QUEUE_FAMILY_IGNORED,
		.image = image,
		.subresourceRange = vk::ImageSubresourceRange {
			.aspectMask = aspectFlags,
			.baseMipLevel = 0,
			.levelCount = 1,
			.baseArrayLayer = 0,
			.layerCount = 1
		}
	};

    auto commandBuffer = owner->beginSingleTimeCommand();
	commandBuffer.pipelineBarrier(vk::PipelineStageFlagBits::eTopOfPipe, vk::PipelineStageFlagBits::eBottomOfPipe, vk::DependencyFlags{}, 0, nullptr, 0, nullptr, 1, &imageMemoryBarrier);
    owner->endSingleTimeCommand(commandBuffer);
}