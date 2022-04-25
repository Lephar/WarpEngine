#define CGLTF_IMPLEMENTATION

#include "Manager.hpp"

#include <iostream>

namespace Engine::Manager {
	std::filesystem::path assetFolder;
	cgltf_options assetOptions;

	void initialize() {
		assetFolder = std::filesystem::current_path() / "Assets";
	}

	void loadMesh(cgltf_mesh*& mesh) {
		for (unsigned primitiveIndex = 0; primitiveIndex < mesh->primitives_count; primitiveIndex++) {
			auto& primitive = mesh->primitives[primitiveIndex];

			Mesh mesh{};
			
			for (unsigned attributeIndex = 0; attributeIndex < primitive.attributes_count; attributeIndex++) {
				auto& attribute = primitive.attributes[attributeIndex];

				auto& accessor = attribute.data;
				auto& view = accessor->buffer_view;
				auto& buffer = view->buffer;

				auto data = static_cast<unsigned char*>(buffer->data) + view->offset;

				if (attribute.type == cgltf_attribute_type_position) {
					mesh.positions.resize(accessor->count);
					std::memcpy(mesh.positions.data(), data, view->size);
				}
				else if (attribute.type == cgltf_attribute_type_normal) {
					mesh.normals.resize(accessor->count);
					std::memcpy(mesh.normals.data(), data, view->size);
				}
				else if (attribute.type == cgltf_attribute_type_tangent) {
					mesh.tangents.resize(accessor->count);
					std::memcpy(mesh.tangents.data(), data, view->size);
				}
				else if (attribute.type == cgltf_attribute_type_texcoord) {
					mesh.texcoords.resize(accessor->count);
					std::memcpy(mesh.texcoords.data(), data, view->size);
				}
			}
		}
	}

	void loadNode(cgltf_node*& node) {
		auto& mesh = node->mesh;

		if (mesh) {
			std::cout << mesh->name << std::endl;
			loadMesh(mesh);
		}

		else
			std::cout << node->name << std::endl;

		for (unsigned childIndex = 0; childIndex < node->children_count; childIndex++) {
			auto& child = node->children[childIndex];

			loadNode(child);
		}
	}

	void loadFile(const char* filename) {
		auto path = assetFolder / filename;

		cgltf_data* data = nullptr;
		cgltf_result result = cgltf_parse_file(&assetOptions, path.string().c_str(), &data);

#ifndef NDEBUG
		if (result != cgltf_result_success) {
			std::cerr << "Error parsing asset " << filename << " with error code " << result << std::endl;
			return;
		}

		result = cgltf_validate(data);

		if (result != cgltf_result_success) {
			std::cerr << "Error validating asset " << filename << " with error code " << result << std::endl;
			cgltf_free(data); 
			return;
		}
#endif // NDEBUF

		result = cgltf_load_buffers(&assetOptions, data, path.string().c_str());

#ifndef NDEBUG
		if (result != cgltf_result_success) {
			std::cerr << "Error loading asset " << filename << " with error code " << result << std::endl;
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