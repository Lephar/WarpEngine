#pragma once

#include <cgltf/cgltf.h>
#include <stb/stb_image.h>
#include <glm/glm.hpp>

#include <filesystem>

#ifndef NDEBUG
#include <iostream>
#endif //NDEBUG

namespace Engine::Manager {
	struct Mesh {
		std::vector<glm::vec3> positions;
		std::vector<glm::vec3> normals;
		std::vector<glm::vec4> tangents;
		std::vector<glm::vec2> texcoords;
	};

	void initialize();
	void loadFile(const char* filename);
}