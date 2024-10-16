// forward declare
#include "Clay/Entity/Entity.h"
#include "Clay/Entity/Physics/BoxCollider2D.h"
// class
#include "Clay/Entity/Physics/CircleCollider2D.h"

namespace clay {

CircleCollider2D::CircleCollider2D(Entity& parentEntity)
 : Collider(parentEntity) {
}

bool CircleCollider2D::isColliding(const Collider& other) const {
    // TODO how to avoid making this too duplicate of BoxCollider2D.
    // maybe have a function that handles that pair but is simplified by having an order, so circle->circle circle->rect rect->rect (order matters to avoid duplicate code)
    // this function could be in collider and have these children call it

    if (const BoxCollider2D* otherBox = dynamic_cast<const BoxCollider2D*>(&other)) {
        // other is rectangle
    } else if (const BoxCollider2D* otherBox = dynamic_cast<const BoxCollider2D*>(&other)) {
        // Other is circle
    }


    // else other is circle
    return false;
}

std::optional<glm::vec3> CircleCollider2D::getCollisionMTV(const Collider& other) const {
    return std::nullopt;
}

/*
// ovals:
// Check if two circles are colliding
bool areCirclesColliding(const glm::vec3& center1, float radius1, const glm::vec3& center2, float radius2) {
    float distanceSquared = glm::distance2(center1, center2);
    float combinedRadius = radius1 + radius2;
    return distanceSquared <= (combinedRadius * combinedRadius);
}

bool areOvalRectangleColliding(const Oval& oval, const Rectangle& rect) {
    // Calculate half extents of the rectangle
    glm::vec3 halfExtentsRect = rect.scale / 2.0f;

    // Calculate half extents of the oval
    glm::vec3 halfExtentsOval = oval.scale / 2.0f;

    // Calculate the radius of the circle that represents the oval
    // Choose the maximum scale component as the radius
    float radiusOval = glm::max(halfExtentsOval.x, glm::max(halfExtentsOval.y, halfExtentsOval.z));

    // Transform the oval's center to the rectangle's local space
    glm::vec3 ovalCenterLocal = oval.center - rect.center;

    // Check collision between the rectangle and the transformed circle
    return areCirclesColliding(ovalCenterLocal, radiusOval, glm::vec3(0.0f), glm::length(halfExtentsRect));
}

*/

/*

OR
struct Rectangle {
    glm::vec2 center;
    glm::vec2 halfExtents; // Half of width and half of height
};

struct ScaledCircle {
    glm::vec2 center;
    float radius;
    glm::vec2 scale; // Scale along x and y axes
};

bool areColliding(const ScaledCircle& circle, const Rectangle& rect) {
    // Transform the rectangle into the local space of the scaled circle
    glm::vec2 rectCenterLocal = rect.center - circle.center;

    // Calculate the closest point on the boundary of the circle to the center of the rectangle
    glm::vec2 closestPoint;
    closestPoint.x = std::clamp(rectCenterLocal.x, -circle.radius * circle.scale.x, circle.radius * circle.scale.x);
    closestPoint.y = std::clamp(rectCenterLocal.y, -circle.radius * circle.scale.y, circle.radius * circle.scale.y);

    // Calculate the distance squared between the closest point and the center of the rectangle
    float distanceSquared = glm::length2(rectCenterLocal - closestPoint);

    // If the distance squared is less than or equal to the radius squared, they are colliding
    return distanceSquared <= (circle.radius * circle.radius);
}

*/

} // namespace clay