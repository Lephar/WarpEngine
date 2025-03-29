#include "meta.h"

#include "window.h"
#include "content.h"
#include "material.h"
#include "primitive.h"

vec3 worldUp;

Player player;
Camera camera;
Primitive skybox;

void loadPlayer(vec3 position, vec3 forward, vec3 right) {
    glm_vec3_copy(position, player.position);
    glm_vec3_copy(forward,  player.forward);
    glm_vec3_copy(right,    player.right);
}

void persective() {
    float aspectRatio = ((float) extent.width) / ((float) extent.height);
    glmc_perspective_rh_zo(camera.fieldOfView, aspectRatio, camera.nearPlane, camera.farPlane, uniformBuffer + camera.projOffset);
}

void lookAt() {
    vec3 target;
    glmc_vec3_add(player.position, player.forward, target);
    glmc_lookat_rh_zo(player.position, target, worldUp, uniformBuffer + camera.viewOffset);
}

void combine() {
    glmc_mat4_mul(uniformBuffer + camera.projOffset, uniformBuffer + camera.viewOffset, uniformBuffer + camera.combOffset);
}

void loadCamera(float fieldOfView, float nearPlane, float farPlane) {
    camera.fieldOfView = fieldOfView;
    camera.nearPlane   = nearPlane;
    camera.farPlane    = farPlane;

    camera.viewOffset  = uniformBufferSize;
    uniformBufferSize += sizeof(mat4);

    camera.projOffset  = uniformBufferSize;
    uniformBufferSize += sizeof(mat4);

    camera.combOffset  = uniformBufferSize;
    uniformBufferSize += sizeof(mat4);

    persective();
    lookAt    ();
    combine   ();
}

void loadSkybox(cgltf_data *data) {
    cgltf_scene *sceneData = data->scene;
    cgltf_node  *nodeData  = sceneData->nodes[0];
    cgltf_mesh  *meshData  = nodeData->mesh;

    cgltf_primitive *primitiveData = &meshData->primitives[0];
    cgltf_material  *materialData  = primitiveData->material;

    Material *material = &materials[materialCount];

    if( findMaterial(materialData) >= materialCount) {
        loadMaterial(material, materialData);
    }

    mat4 transform;
    cgltf_node_transform_world(nodeData, (cgltf_float *) transform);

    loadPrimitive(&skybox, primitiveData, transform);
}
