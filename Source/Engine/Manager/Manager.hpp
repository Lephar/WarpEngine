#pragma once

#include <cgltf/cgltf.h>
#include <glm/glm.hpp>

#include <filesystem>

#ifndef NDEBUG
#include <iostream>
#endif //NDEBUG

namespace Engine::Manager {
	void initialize();
	void loadFile(const char* filename);
}