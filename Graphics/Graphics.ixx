module;

#include "Graphics.hpp"

export module Engine:Graphics;

namespace Engine::Graphics {
	struct Buffer {
		vk::Buffer buffer;
		vk::DeviceSize size;
		vk::DeviceSize offset;
	};

	struct Image {
		vk::Image image;
		vk::ImageView view;
		unsigned int width;
		unsigned int height;
	};

	void initialize();
	void terminate();
}
