// forward declare
#include "Clay/Entity/Entity.h"
#include "Clay/Entity/Physics/CircleCollider2D.h"
// class
#include "Clay/Entity/Physics/BoxCollider2D.h"

namespace clay {

BoxCollider2D::BoxCollider2D(Entity& parentEntity)
    : Collider(parentEntity) {
}

void BoxCollider2D::update(float dt) {
    // TODO don't do this. instead when accessing position, check the parent
    mPosition_ = mParentEntity_.getPosition();
}

void BoxCollider2D::render(const Renderer& theRenderer) {
    glm::mat4 translationMatrix = glm::translate(glm::mat4(1.0f), mPosition_);
    // TODO use other matrices
    theRenderer.renderRectangleSimple(translationMatrix, {1,1,1,1});
}

bool BoxCollider2D::isColliding(const Collider& other) const {
    if (const BoxCollider2D* otherBox = dynamic_cast<const BoxCollider2D*>(&other)) {
        // other is rectangle
        glm::vec3 halfExtents1 = this->mScale_ / 2.0f;
        glm::vec3 halfExtents2 = otherBox->mScale_ / 2.0f;

        // Calculate centers of each rectangle
        glm::vec3 center1 = this->mPosition_;
        glm::vec3 center2 = otherBox->mPosition_;

        // Calculate distance between centers of the rectangles
        glm::vec3 distance = center2 - center1;
        glm::vec3 minDistance = halfExtents1 + halfExtents2;
        // -4.31602e+08, -4.31602e+08, -4.31602e+08
        // -1.42484, 0, 0
        // Check if there is separation on any axis
        if (std::abs(distance.x) >= minDistance.x ||
            std::abs(distance.y) >= minDistance.y ||
            std::abs(distance.z) >= minDistance.z) {
            // There is separation on at least one axis, so the rectangles are not colliding
            return false;
        } else {
            return true;
        }
    }
    /*
    else if (const BoxCollider2D* otherBox = dynamic_cast<const BoxCollider2D*>(&other)) {
        // Calculate the closest point on the rectangle to the circle's center
        glm::vec3 closestPoint;
        closestPoint.x = std::clamp(circle.getPosition().x, rect.mPosition_.x - rect.mScale_.x, rect.mPosition_.x + rect.mScale_.x);
        closestPoint.y = std::clamp(circle.getPosition().y, rect.mPosition_.y - rect.mScale_.y, rect.mPosition_.y + rect.mScale_.y);
        closestPoint.z = std::clamp(circle.getPosition().z, rect.mPosition_.z - rect.mScale_.z, rect.mPosition_.z + rect.mScale_.z);

        // Calculate the distance squared between the circle's center and the closest point on the rectangle
        float distanceSquared = glm::distance2(circle.getPosition(), closestPoint);

        // If the distance squared is less than or equal to the circle's radius squared, they are colliding
        return distanceSquared <= (circle.getRadius() * circle.getRadius());
    }
    */

    // else other is circle
    return false;
}

std::optional<glm::vec3> BoxCollider2D::getCollisionMTV(const Collider& other) const {
    if (const BoxCollider2D* otherBox = dynamic_cast<const BoxCollider2D*>(&other)) {
        // Initialize MTV to zero vector

        // Calculate half extents
        glm::vec3 halfExtents1 = this->mScale_ / 2.0f;
        glm::vec3 halfExtents2 = otherBox->mScale_ / 2.0f;

        // Calculate centers of each rectangle
        glm::vec3 center1 = this->mPosition_;
        glm::vec3 center2 = otherBox->mPosition_;

        // Calculate distance between centers of the rectangles
        glm::vec3 distance = center2 - center1;
        glm::vec3 minDistance = halfExtents1 + halfExtents2;

        // Calculate overlap on each axis
        glm::vec3 overlap = minDistance - glm::abs(distance);

        // If there is separation on any axis, there's no collision
        if (overlap.x <= 0 || overlap.y <= 0 || overlap.z <= 0) {
            return std::nullopt;
        } else {
            glm::vec3 mtv = glm::vec3(0.0f);
            // Determine which axis has the smallest overlap
            if (overlap.x < overlap.y && overlap.x < overlap.z) {
                // MTV on x-axis
                float sign = (distance.x < 0) ? -1.0f : 1.0f;
                mtv.x = overlap.x * sign;
            } else if (overlap.y < overlap.x && overlap.y < overlap.z) {
                // MTV on y-axis
                float sign = (distance.y < 0) ? -1.0f : 1.0f;
                mtv.y = overlap.y * sign;
            } else {
                // MTV on z-axis
                float sign = (distance.z < 0) ? -1.0f : 1.0f;
                mtv.z = overlap.z * sign;
            }

            return mtv;
        }

    }
    return std::nullopt;
}

} // namespace clay