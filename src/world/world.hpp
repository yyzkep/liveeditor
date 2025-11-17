#ifndef WORLD_HPP
#define WORLD_HPP

#include "../rendering/camera/camera.hpp"
#include "../entity/base_entity.hpp"
#include "../rendering/shader.hpp"
#include <memory>
#include <vector>

class World : public BaseEntity {
  public:
    World(const char* name = "World");
    ~World();

    std::shared_ptr<BaseEntity> CreateEntity(
        const glm::vec3& position = glm::vec3(0.0f),
        const glm::vec3& rotation = glm::vec3(0.0f),
        const glm::vec3& scale = glm::vec3(1.0f),
        const char* name = "Entity"
    );

    void AddEntity(std::shared_ptr<BaseEntity> entity);
    void RemoveEntity(const std::shared_ptr<BaseEntity>& entity);
    void Clear();

    void SetCamera(std::shared_ptr<Camera> camera);
    std::shared_ptr<Camera> GetCamera() const;

    void DrawAll(float aspectRatio);
    size_t GetEntityCount() const { return entities_.size(); }

    std::vector<std::shared_ptr<BaseEntity>> GetEntities() { return entities_; }

    std::shared_ptr<BaseEntity> GetEntity(const std::string &name) {
        for (auto &e : entities_) {
            if (e->GetName() == name) {
                return e;
            }
        }
        return nullptr;
    }


  private:
    std::vector<std::shared_ptr<BaseEntity>> entities_;
    std::shared_ptr<BaseEntity> worldRoot_;
    std::shared_ptr<Camera> camera_;
};

#endif // WORLD_HPP
