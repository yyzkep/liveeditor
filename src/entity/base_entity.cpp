#include "base_entity.hpp"
#include "../rendering/loaders/obj_loader.hpp"
#include "../rendering/loaders/shader_loader.hpp"
#include <glm/gtc/matrix_transform.hpp>
#include <iostream>

//
// === Param Synchronization ===
//
void BaseEntity::UpdateTransformFromParams() {
    transform_.position = params_.GetOr("position", transform_.position);
    transform_.rotation = params_.GetOr("rotation", transform_.rotation);
    transform_.scale    = params_.GetOr("scale", transform_.scale);
    UpdateModel();
}

void BaseEntity::UpdateRenderableFromParams() {
    std::string meshName   = params_.GetOr<std::string>("mesh", "");
    std::string shaderName = params_.GetOr<std::string>("shader", "");

    if (meshName.empty() && shaderName.empty()) return;

    std::shared_ptr<Mesh> meshPtr = nullptr;
    std::shared_ptr<Shader> shaderPtr = nullptr;

    // --- Load Mesh ---
    if (!meshName.empty()) {
        std::string meshPath = "assets/models/" + meshName + ".obj";
        meshPtr = std::make_shared<Mesh>(MeshLoader::LoadOBJ(meshPath));
    }

    // --- Load Shader ---
    if (!shaderName.empty()) {
        std::string vertPath = "assets/shaders/" + shaderName + ".vert";
        std::string fragPath = "assets/shaders/" + shaderName + ".frag";
        shaderPtr = ShaderLoader::LoadShader(vertPath, fragPath);
    }

    renderable_.SetMesh(meshPtr);
    renderable_.SetShader(shaderPtr);

    if (!meshPtr)
        std::cout << "[ENTITY][WARN] Mesh '" << meshName << "' not found." << std::endl;
    if (!shaderPtr)
        std::cout << "[ENTITY][WARN] Shader '" << shaderName << "' not found." << std::endl;
}
