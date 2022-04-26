#define CGLTF_IMPLEMENTATION
#define STB_IMAGE_IMPLEMENTATION

#include "Manager.hpp"

namespace Engine::Manager {
	std::filesystem::path assetFolder;
	cgltf_options assetOptions;

	std::unordered_map<std::string, Material> materials;
	std::vector<Mesh> meshes;

	void initialize() {
		assetFolder = std::filesystem::current_path() / "Assets";
	}

	Image loadImage(const char* filename) {
		auto path = assetFolder / filename;
		
		Image image{};
		int width, height, channels;

		image.data = stbi_load(path.string().c_str(), &width, &height, &channels, STBI_rgb_alpha);

		image.width = width;
		image.height = height;
		image.channels = channels;

		return image;
	}

	void loadMaterial(cgltf_material& materialData) {
		Material material{};

		material.color = loadImage(materialData.pbr_metallic_roughness.base_color_texture.texture->image->uri);
		material.normal = loadImage(materialData.normal_texture.texture->image->uri);
		material.roughness = loadImage(materialData.pbr_metallic_roughness.metallic_roughness_texture.texture->image->uri);
		material.emissiveness = loadImage(materialData.emissive_texture.texture->image->uri);

		materials.emplace(materialData.name, material);
	}

	void loadMesh(cgltf_mesh*& meshData, glm::mat4& transformation) {
		for (unsigned primitiveIndex = 0; primitiveIndex < meshData->primitives_count; primitiveIndex++) {
			auto& primitive = meshData->primitives[primitiveIndex];

			Mesh mesh{
				.transformation = transformation,
				.materialName = primitive.material->name
			};
			
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

			meshes.push_back(mesh);
		}
	}

	void loadNode(cgltf_node*& nodeData, glm::mat4 parentTransformation) {
		auto& mesh = nodeData->mesh;

		glm::mat4 transformation = parentTransformation;

		// TODO: Implement this one!
		if (nodeData->has_matrix) {
			
		}
		else { // NOTICE: Don't change the order!
			if (nodeData->has_scale) {

			}
			if (nodeData->has_rotation) {

			}
			if (nodeData->has_translation) {

			}
		}

		if (mesh) {
			std::cout << mesh->name << std::endl;
			loadMesh(mesh, transformation);
		}

		for (unsigned childIndex = 0; childIndex < nodeData->children_count; childIndex++) {
			auto& child = nodeData->children[childIndex];

			loadNode(child, transformation);
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

		for (unsigned materialIndex = 0; materialIndex < data->materials_count; materialIndex++) {
			auto& material = data->materials[materialIndex];

			loadMaterial(material);
		}

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
				glm::mat4 identityTransformation{1.0f};

				loadNode(node, identityTransformation);
			}
		}

		cgltf_free(data);
	}
}