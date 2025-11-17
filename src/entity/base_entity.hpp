#ifndef BASE_ENTITY_HPP
#define BASE_ENTITY_HPP

#include "../rendering/camera/camera.hpp"
#include "../rendering/mesh/mesh.hpp"
#include "../rendering/shader.hpp"
#include "params.hpp"
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <iostream>
#include <memory>
#include <string>
#include <utility>
#include <variant>

//
// === Transform ===
//
struct Transform {
    glm::vec3 position{0.0f};
    glm::vec3 rotation{0.0f};
    glm::vec3 scale{1.0f};

    [[nodiscard]] glm::mat4 GetModelMatrix() const {
        glm::mat4 model(1.0f);
        model = glm::translate(model, position);
        model = glm::rotate(model, glm::radians(rotation.x), glm::vec3(1, 0, 0));
        model = glm::rotate(model, glm::radians(rotation.y), glm::vec3(0, 1, 0));
        model = glm::rotate(model, glm::radians(rotation.z), glm::vec3(0, 0, 1));
        model = glm::scale(model, scale);
        return model;
    }


};

//
// === Renderable ===
//
class Renderable {
  public:
    Renderable() = default;
    Renderable(std::shared_ptr<Mesh> mesh, std::shared_ptr<Shader> shader)
        : mesh_(std::move(mesh)), shader_(std::move(shader)) 
    {
        if (shader_) {
            auto uniforms = shader_->GetActiveUniformValues();
            for (const auto& [name, value] : uniforms)
                params_.Set(name, value);
        }
    }

    void SetMesh(std::shared_ptr<Mesh> mesh) { mesh_ = std::move(mesh); }
    void SetShader(std::shared_ptr<Shader> shader) { shader_ = std::move(shader); }

    [[nodiscard]] bool IsValid() const { return mesh_ && shader_; }

    void Draw(const glm::mat4& model, const glm::mat4& view, const glm::mat4& projection) const {
        if (!IsValid()) return;

        shader_->Use();
        shader_->SetMat4("model", model);
        shader_->SetMat4("view", view);
        shader_->SetMat4("projection", projection);

        // Push all parameters to shader
        for (const auto& [key, val] : params_.All()) {
            std::visit([&](auto&& v) {
                using T = std::decay_t<decltype(v)>;
                if constexpr (std::is_same_v<T, int>)
                    shader_->SetInt(key, v);
                else if constexpr (std::is_same_v<T, float>)
                    shader_->SetFloat(key, v);
                else if constexpr (std::is_same_v<T, double>)
                    shader_->SetFloat(key, static_cast<float>(v));
                else if constexpr (std::is_same_v<T, bool>)
                    shader_->SetBool(key, v);
                else if constexpr (std::is_same_v<T, glm::vec2>)
                    shader_->SetVec2(key, v);
                else if constexpr (std::is_same_v<T, glm::vec3>)
                    shader_->SetVec3(key, v);
                else if constexpr (std::is_same_v<T, glm::vec4>)
                    shader_->SetVec4(key, v);
            }, val);
        }

        mesh_->Draw(*shader_);
    }

    void Draw(const glm::mat4& model, const Camera& cam, float aspectRatio) const {
        if (!IsValid()) return;

        glm::mat4 view = cam.GetViewMatrix();
        glm::mat4 projection = glm::perspective(glm::radians(cam.GetZoom()), aspectRatio, 0.1f, 100.0f);

        Draw(model, view, projection);

        shader_->SetVec3("viewPos", cam.GetPosition());
    }

    CParams& Params() { return params_; }
    const CParams& Params() const { return params_; }

  private:
    std::shared_ptr<Mesh> mesh_;
    std::shared_ptr<Shader> shader_;
    CParams params_;
};

//
// === BaseEntity ===
//
class BaseEntity {
  public:
    BaseEntity() = default;

    BaseEntity(glm::vec3 position, glm::vec3 rotation = glm::vec3(0.0f),
               glm::vec3 scale = glm::vec3(1.0f), const std::string& name = "Entity")
        : name_(std::move(name)) 
    {
        transform_.position = position;
        transform_.rotation = rotation;
        transform_.scale = scale;

        params_.Set("position", position);
        params_.Set("rotation", rotation);
        params_.Set("scale", scale);

        UpdateModel();

        std::cout << "[ENTITY] Created entity: " << name_
                << " at (" << position.x << ", " << position.y << ", " << position.z << ")"
                << " with scale (" << scale.x << ", " << scale.y << ", " << scale.z << ")"
                << std::endl;
    }

    virtual ~BaseEntity() = default;

    // --- Accessors ---
    [[nodiscard]] const std::string& GetName() const { return name_; }
    void SetName(std::string newName) { name_ = std::move(newName); }

    [[nodiscard]] const Transform& GetTransform() const { return transform_; }
    Transform& GetTransform() { return transform_; }

    [[nodiscard]] const Renderable& GetRenderable() const { return renderable_; }
    Renderable& GetRenderable() { return renderable_; }

    [[nodiscard]] glm::mat4 GetModelMatrix() const { return modelMatrix_; }

    CParams& Params() { return params_; }
    const CParams& Params() const { return params_; }

    // --- Behavior ---
    virtual void Draw(const Camera& camera, float aspectRatio) {
        if (!renderable_.IsValid()) return;
        UpdateModel();
        renderable_.Draw(modelMatrix_, camera, aspectRatio);
    }

    void UpdateTransformFromParams();
    void UpdateRenderableFromParams();

  protected:
    void UpdateModel() { modelMatrix_ = transform_.GetModelMatrix(); }

  protected:
    Transform transform_;
    Renderable renderable_;
    CParams params_;
    glm::mat4 modelMatrix_{1.0f};
    std::string name_{"Entity"};
};

#endif // BASE_ENTITY_HPP
