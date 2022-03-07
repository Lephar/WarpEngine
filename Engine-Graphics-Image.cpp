module;

#include "Engine-Graphics.hpp"

module Engine:Graphics;

namespace Engine::Graphics {
	extern vk::Device device;

	Image::Image(unsigned int imageWidth, unsigned int imageHeight, unsigned int mipLevels,
		vk::SampleCountFlagBits sampleCount, vk::Format imageFormat, vk::ImageUsageFlags usageFlags,
		vk::ImageAspectFlags aspectFlags, Memory& imageMemory) {
		
		createImage(imageWidth, imageHeight, mipLevels, sampleCount, imageFormat, usageFlags);
		bindImageMemory(imageMemory);
		createImageView(mipLevels, imageFormat, aspectFlags);
	}

	void Image::createImage(unsigned int imageWidth, unsigned int imageHeight, unsigned int mipLevels,
		vk::SampleCountFlagBits sampleCount, vk::Format imageFormat, vk::ImageUsageFlags usageFlags) {
		vk::ImageCreateInfo imageInfo{
			.imageType = vk::ImageType::e2D,
			.format = imageFormat,
			.extent = vk::Extent3D {
				.width = imageWidth,
				.height = imageHeight,
				.depth = 1
			},
			.mipLevels = mipLevels,
			.arrayLayers = 1,
			.samples = sampleCount,
			.tiling = vk::ImageTiling::eOptimal,
			.usage = usageFlags,
			.sharingMode = vk::SharingMode::eExclusive,
			.initialLayout = vk::ImageLayout::eUndefined
		};

		image = device.createImage(imageInfo);
	}

	void Image::createImageView(unsigned int mipLevels, vk::Format imageFormat, vk::ImageAspectFlags aspectFlags) {
		vk::ImageViewCreateInfo viewInfo{
			.image = image,
			.viewType = vk::ImageViewType::e2D,
			.format = imageFormat,
			.components = vk::ComponentMapping {
				.r = vk::ComponentSwizzle::eIdentity,
				.g = vk::ComponentSwizzle::eIdentity,
				.b = vk::ComponentSwizzle::eIdentity,
				.a = vk::ComponentSwizzle::eIdentity
			},
			.subresourceRange = vk::ImageSubresourceRange {
				.aspectMask = aspectFlags,
				.baseMipLevel = 0,
				.levelCount = mipLevels,
				.baseArrayLayer = 0,
				.layerCount = 1
			}
		};

		view = device.createImageView(viewInfo);
	}

	void Image::bindImageMemory(Memory& imageMemory) {
		vk::MemoryRequirements requirements = device.getImageMemoryRequirements(image);
		vk::DeviceSize offset = imageMemory.alignOffset(requirements);
		vk::DeviceMemory& memory = imageMemory.getMemoryHandle();
		device.bindImageMemory(image, memory, offset);
	}

	void Image::destroy() {
		if (view)
			device.destroyImageView(view);
	
		device.destroyImage(image);
	}
}
