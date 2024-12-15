#pragma once
// project
#include "clay/entity/physics/Collider.h"

namespace clay {

class Entity;

class CircleCollider2D : public Collider {
public:
    CircleCollider2D(Entity& parentEntity);

    bool isColliding(const Collider& other) const override;

    std::optional<glm::vec3> getCollisionMTV(const Collider& other) const;
};

} // namespace clay