#include "Graphics/Swapchain.hpp"
#include "Graphics/Renderer.hpp"

void Swapchain::create(Renderer *owner, size_t size, vk::Extent2D extent, DeviceInfo deviceInfo) {
    auto surfaceCapabilities = deviceInfo.surfaceCapabilities;

	auto surfaceFormatChoosen = false;
	auto supportedSurfaceFormats = deviceInfo.surfaceFormats;
	auto surfaceFormat = supportedSurfaceFormats.front();

	std::vector<vk::SurfaceFormatKHR> preferredSurfaceFormats {
		vk::SurfaceFormatKHR {
			vk::Format::eB8G8R8A8Srgb,
			vk::ColorSpaceKHR::eSrgbNonlinear
		}
	};

	for(auto preferredSurfaceFormat : preferredSurfaceFormats) {
		for(auto supportedSurfaceFormat : supportedSurfaceFormats) {
			if(preferredSurfaceFormat.format == supportedSurfaceFormat.format && preferredSurfaceFormat.colorSpace == supportedSurfaceFormat.colorSpace) {
				surfaceFormat = preferredSurfaceFormat;
				surfaceFormatChoosen = true;
				break;
			}
		}

		if(surfaceFormatChoosen)
			break;
	}

	auto presentModeChoosen = false;
	auto supportedPresentModes = deviceInfo.presentModes;
	auto presentMode = supportedPresentModes.front();

	std::vector<vk::PresentModeKHR> preferredPresentModes {
		vk::PresentModeKHR::eMailbox,
		vk::PresentModeKHR::eImmediate,
		vk::PresentModeKHR::eFifoRelaxed,
		vk::PresentModeKHR::eFifo
	};

	for(auto& preferredPresentMode : preferredPresentModes) {
		for(auto& supportedPresentMode : supportedPresentModes) {
			if(preferredPresentMode == supportedPresentMode) {
				presentMode = preferredPresentMode;
				presentModeChoosen = true;
				break;
			}
		}

		if(presentModeChoosen)
			break;
	}

	size = std::clamp(3u, surfaceCapabilities.minImageCount, surfaceCapabilities.maxImageCount);

	vk::SwapchainCreateInfoKHR swapchainInfo {
		.surface = surface,
		.minImageCount = size,
		.imageFormat = surfaceFormat.format,
		.imageColorSpace = surfaceFormat.colorSpace,
		.imageExtent = extent,
		.imageArrayLayers = 1,
		.imageUsage = vk::ImageUsageFlagBits::eColorAttachment,
		.imageSharingMode = vk::SharingMode::eExclusive,
		.queueFamilyIndexCount = 0,
		.pQueueFamilyIndices = nullptr,
		.preTransform = surfaceCapabilities.currentTransform,
		.compositeAlpha = vk::CompositeAlphaFlagBitsKHR::eOpaque,
		.presentMode = presentMode,
		.clipped = true,
		.oldSwapchain = nullptr
	};

	swapchain = owner->getDevice().createSwapchainKHR(swapchainInfo);
	auto swapchainImageHandles = owner->getDevice().getSwapchainImagesKHR(swapchain);

	std::vector<Image> swapchainImages{size};

	for(auto imageIndex = 0u; imageIndex < size; imageIndex++) {
		auto &swapchainImageHandle = swapchainImageHandles.at(imageIndex);
		auto &swapchainImage = swapchainImages.at(imageIndex);

		swapchainImage.wrap(owner, swapchainImageHandle, extent.width, extent.height, surfaceFormat.format, vk::ImageUsageFlagBits::eColorAttachment | vk::ImageUsageFlagBits::eTransferDst, vk::ImageAspectFlagBits::eColor, vk::SampleCountFlagBits::e1, 1);
		swapchainImage.transitionLayout(vk::ImageLayout::eUndefined, vk::ImageLayout::ePresentSrcKHR);
	}
}
