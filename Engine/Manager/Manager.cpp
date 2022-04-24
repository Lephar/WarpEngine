#define CGLTF_IMPLEMENTATION

#include "Manager.hpp"

#include <iostream>

namespace Engine::Manager {
	std::filesystem::path assetFolder;
	cgltf_options assetOptions;

	void initialize() {
		assetFolder = std::filesystem::current_path() / "Assets";
	}

	void loadNode(cgltf_node*& node) {
		if(node->mesh)
			std::cout << node->mesh->name << std::endl;
		else
			std::cout << node->name << std::endl;

		for (unsigned childIndex = 0; childIndex < node->children_count; childIndex++) {
			auto& child = node->children[childIndex];

			loadNode(child);
		}
	}

	void loadFile(const char* filename) {
		auto path = assetFolder / filename;
		std::cout << path.string().c_str() << std::endl;

		cgltf_data* data = nullptr;
		cgltf_result result = cgltf_parse_file(&assetOptions, path.string().c_str(), &data);

#ifndef NDEBUG
		if (result != cgltf_result_success) {
			std::cerr << "Error loading asset named " << filename << " with error code " << result << std::endl;
			return;
		}

		result = cgltf_validate(data);

		if (result != cgltf_result_success) {
			std::cerr << "Error validating asset named " << filename << " with error code " << result << std::endl;
			cgltf_free(data); 
			return;
		}
#endif // NDEBUF
		for (unsigned sceneIndex = 0; sceneIndex < data->scenes_count; sceneIndex++) {
			auto& scene = data->scenes[sceneIndex];

			for (unsigned nodeIndex = 0; nodeIndex < scene.nodes_count; nodeIndex++) {
				auto& node = scene.nodes[nodeIndex];

				loadNode(node);
			}
		}

		cgltf_free(data);
	}
}