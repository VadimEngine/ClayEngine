#pragma once
// class
#include "clay/entity/physics/Collider.h"

namespace clay {

class Entity;

class BoxCollider2D : public Collider {
public:
    BoxCollider2D(Entity& parentEntity);

    bool isColliding(const Collider& other) const override;

    // virtual bool isColliding(const glm::vec3 rayOrigin, glm::vec3 rayDir) const = 0;

    void update(float dt);

    void render(const Renderer& theRenderer);

    std::optional<glm::vec3> getCollisionMTV(const Collider& other) const;

};

} // namespace clay