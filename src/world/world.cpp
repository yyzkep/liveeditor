#include "world.hpp"
#include <algorithm>
#include <iostream>


World::World(const char* name) {
    //fuck c++
    worldRoot_ = std::move(CreateEntity(glm::vec3(0.f), glm::vec3(0.f), glm::vec3(1.f), name));
}

World::~World() noexcept {
    RemoveEntity(worldRoot_);
}

std::shared_ptr<BaseEntity> World::CreateEntity(
    const glm::vec3& position,
    const glm::vec3& rotation,
    const glm::vec3& scale,
    const char* name
) {
    auto entity = std::make_shared<BaseEntity>(position, rotation, scale, name);
    entities_.push_back(entity);

    std::cout << "[WORLD] Created entity: " << name
              << " (Total: " << entities_.size() << ")" << std::endl;
    return entity;
}

void World::AddEntity(std::shared_ptr<BaseEntity> entity) {
    if (!entity) {
        std::cerr << "[WORLD][WARN] Attempted to add null entity." << std::endl;
        return;
    }

    entities_.push_back(std::move(entity));
    std::cout << "[WORLD] Added entity (Total: " << entities_.size() << ")" << std::endl;
}

void World::RemoveEntity(const std::shared_ptr<BaseEntity>& entity) {
    if (!entity) {
        std::cerr << "[WORLD][WARN] Attempted to remove null entity." << std::endl;
        return;
    }

    auto it = std::find(entities_.begin(), entities_.end(), entity);
    if (it != entities_.end()) {
        std::cout << "[WORLD] Removing entity: " << (*it)->GetName() << std::endl;
        entities_.erase(it);
    } else {
        std::cerr << "[WORLD][WARN] Entity not found for removal." << std::endl;
    }
}

void World::Clear() {
    std::cout << "[WORLD] Clearing all entities (" << entities_.size() << ")" << std::endl;
    entities_.clear();
}

void World::SetCamera(Camera& camera) {
    camera_ = camera;
}

Camera World::GetCamera() const {
    return camera_;
}

void World::DrawAll(float aspectRatio) {
    if (entities_.empty())
        return;

    for (auto& entity : entities_) {
        if (entity)
            entity->Draw(camera_, aspectRatio);
    }
}
