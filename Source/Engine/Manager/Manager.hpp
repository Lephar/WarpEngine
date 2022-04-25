#pragma once

#include <cgltf/cgltf.h>
#include <glm/glm.hpp>
#include <SDL2/SDL_image.h>

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