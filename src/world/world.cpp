#include "world.hpp"
#include <algorithm>
#include <iostream>

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

void World::DrawAll(float aspectRatio) {
    if (!camera_) {
        std::cerr << "[WORLD][ERROR] Cannot draw: No active camera set." << std::endl;
        return;
    }

    if (entities_.empty())
        return;

    for (auto& entity : entities_) {
        if (entity)
            entity->Draw(*camera_, aspectRatio);
    }
}
