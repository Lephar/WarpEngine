#pragma once

#include <cgltf/cgltf.h>
#include <stb/stb_image.h>
#include <glm/glm.hpp>

#include <filesystem>
#include <unordered_map>

#ifndef NDEBUG
#include <iostream>
#endif //NDEBUG

namespace Engine::Manager {
	struct Image {
		unsigned width;
		unsigned height;
		unsigned channels;
		stbi_uc* data;
	};

	struct Material {
		Image color;
		Image normal;
		Image roughness;
		Image emissiveness;
	};

	struct Mesh {
		glm::mat4 transformation;
		std::string materialName;
		std::vector<glm::vec3> positions;
		std::vector<glm::vec3> normals;
		std::vector<glm::vec4> tangents;
		std::vector<glm::vec2> texcoords;
	};

	void initialize();
	void loadFile(const char* filename);
}