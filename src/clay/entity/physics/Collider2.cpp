// forward declare
#include "clay/entity/Entity.h"
// class
#include "clay/entity/physics/Collider2.h"

namespace clay {

Collider2::Collider2(Entity& parentEntity)
    : PhysicsComponentBase(parentEntity) {
    mShape_ = Collider2::Shape::RECTANGLE;
}

Collider2::~Collider2() {}

void Collider2::update(float dt) {}

PhysicsComponentBase::ComponentType Collider2::getType() const {
    return PhysicsComponentBase::ComponentType::COLLIDER;
}

bool Collider2::isColliding(const Collider2* other) const {
    return false;
}

void Collider2::render(const Renderer& theRenderer) {
    glm::mat4 translationMatrix = glm::translate(glm::mat4(1.0f), mPosition_);

    theRenderer.renderRectangleSimple(translationMatrix, {1,1,1,1});
}

std::optional<glm::vec3> Collider2::getCollisionNormal(Collider2* otherCollider) const {
    if (mShape_ == Collider2::Shape::CIRCLE && otherCollider->mShape_ == Collider2::Shape::RECTANGLE) {
        glm::vec3 circleCenter = this->getAbsolutePosition();
        glm::vec3 rectP1 = otherCollider->getAbsolutePosition() - glm::vec3(otherCollider->getWidthDiv2(), -otherCollider->getHeightDiv2(), 0);
        glm::vec3 rectP2 = otherCollider->getAbsolutePosition() + glm::vec3(otherCollider->getWidthDiv2(), -otherCollider->getHeightDiv2(), 0);

        // Closest point in rectangle to the circle center
        float closestX = glm::clamp(circleCenter.x, rectP1.x, rectP2.x);
        // p1 is top left which is greater than p2 on y axis
        float closestY = glm::clamp(circleCenter.y, rectP2.y, rectP1.y);

        glm::vec3 posDiff = glm::vec3{closestX, closestY, 0} - circleCenter;

        if (abs(posDiff.x) <= this->getWidthDiv2() && abs(posDiff.y) <= this->getHeightDiv2()) {
            // To get vector from A to B you do B - A
            // find normal pointing from Rectangle to circle (from other pointing to this)
            float diffX = circleCenter.x - closestX;
            float diffY = circleCenter.y - closestY;

            return glm::normalize(glm::vec3{diffX, diffY, 0});
        }
    } else if (mShape_ == Collider2::Shape::RECTANGLE && otherCollider->mShape_ == Collider2::Shape::CIRCLE) {
        glm::vec3 circleCenter = otherCollider->getAbsolutePosition();
        glm::vec3 rectP1 = this->getAbsolutePosition() - glm::vec3(this->getWidthDiv2(), -this->getHeightDiv2(), 0);
        glm::vec3 rectP2 = this->getAbsolutePosition() + glm::vec3(this->getWidthDiv2(), -this->getHeightDiv2(), 0);

        // Closest point in rectangle to
        float closestX = glm::clamp(circleCenter.x, rectP1.x, rectP2.x);
        // p1 is top left which is greater than p2 on y axis
        float closestY = glm::clamp(circleCenter.y, rectP2.y, rectP1.y);

        glm::vec3 posDiff = circleCenter - glm::vec3{closestX, closestY, 0};

        if (abs(posDiff.x) <= otherCollider->getWidthDiv2() && abs(posDiff.y) <= otherCollider->getHeightDiv2()) {
            // To get vector from A to B you do B - A
            // find normal pointing from Circle to Rectangle (from other pointing to this)
            float diffX = closestX - circleCenter.x;
            float diffY = closestY - circleCenter.y;
            return glm::normalize(glm::vec3{diffX, diffY, 0});
        }
    } else if (mShape_ == Collider2::Shape::CIRCLE && otherCollider->mShape_ == Collider2::Shape::CIRCLE) {
        if (glm::distance(this->getAbsolutePosition(), otherCollider->getAbsolutePosition()) <= this->getWidthDiv2() + otherCollider->getHeightDiv2()) {
            auto norm = glm::normalize(this->getAbsolutePosition() - otherCollider->getAbsolutePosition());
            return norm;
        }
    } else if (mShape_ == Collider2::Shape::RECTANGLE && otherCollider->mShape_ == Collider2::Shape::RECTANGLE) {
        // treat rect as circle for now
        if (glm::distance(this->getAbsolutePosition(), otherCollider->getAbsolutePosition()) <= this->getWidthDiv2() + otherCollider->getHeightDiv2()) {
            auto norm = glm::normalize(this->getAbsolutePosition() - otherCollider->getAbsolutePosition());
            return norm;
        }
    }

    // no collision
    return std::nullopt;
}

std::optional<glm::vec3> Collider2::getCollisionMVT(Collider2* otherCollider) const {
    glm::vec3 thisReferencePoint;
    glm::vec3 otherReferencePoint;

    // if shapes are the same then use center as references
    thisReferencePoint = this->getAbsolutePosition();
    otherReferencePoint = otherCollider->getAbsolutePosition();

    glm::vec3 posDiff = otherReferencePoint - thisReferencePoint;

    glm::vec2 thisHalfSize = glm::vec2{this->getWidthDiv2(), this->getHeightDiv2()};
    glm::vec2 otherHalfSize = glm::vec2{otherCollider->getWidthDiv2(), otherCollider->getHeightDiv2()};

    float xOverlap = thisHalfSize.x + otherHalfSize.x - std::abs(posDiff.x);
    float yOverlap = thisHalfSize.y + otherHalfSize.y - std::abs(posDiff.y);

    if (xOverlap > 0 && yOverlap > 0) {
        if (xOverlap < yOverlap) {
            float sign = (posDiff.x > 0) ? 1.0f : -1.0f;
            return glm::vec3{ -xOverlap * sign, 0.0f, 0.0f } / 2.f; // moveableCount
        } else {
            float sign = (posDiff.y > 0) ? 1.0f : -1.0f;
            return glm::vec3{ 0.0f, -yOverlap * sign, 0.0f } / 2.f; // moveableCount
        }
    } else {
        return std::nullopt;
    }

    return std::nullopt;
}

bool Collider2::rayCollides(glm::vec3 rayOrigin, glm::vec3 rayDirection) const {
    float rectWidth = mDimension_.x;
    float rectHeight = mDimension_.y;
    glm::vec3 rectCenter = mPosition_;

    // Calculate half extents
    float halfWidth = rectWidth * 0.5f;
    float halfHeight = rectHeight * 0.5f;

    // Calculate the normal of the rectangle (assuming it's aligned with the axes)
    glm::vec3 rectNormal = glm::vec3(0.0f, 0.0f, 1.0f); // Assuming the rectangle is in the XY plane

    // Calculate the vector from the ray origin to the rectangle center
    glm::vec3 toCenter = rayOrigin - rectCenter;

    // Calculate the distance from the ray origin to the plane of the rectangle
    float distanceToPlane = glm::dot(toCenter, rectNormal);

    // Check if the ray is parallel or pointing away from the plane
    if (glm::abs(distanceToPlane) > 0.001f) {
        // Calculate the point of intersection with the plane
        glm::vec3 intersectionPoint = rayOrigin + (distanceToPlane * rayDirection);

        // Check if the intersection point is within the rectangle bounds
        if (glm::abs(intersectionPoint.x - rectCenter.x) <= halfWidth &&
            glm::abs(intersectionPoint.y - rectCenter.y) <= halfHeight) {
            // The ray intersects with the rectangle
            return true;
        }
    }

    // No intersection
    return false;
}

Collider2::Shape Collider2::getShape() const {
    return mShape_;
}

void Collider2::setShape(const Collider2::Shape& theShape) {
    mShape_ = theShape;
}

void Collider2::setPosition(const glm::vec3 newPosition) {
    mPosition_ = newPosition;
}

glm::vec3 Collider2::getDimension() const {
    return mDimension_;
}

glm::vec3 Collider2::getPosition() {
    return mPosition_;
}

void Collider2::setDimension(const glm::vec3& newDimension) {
    mDimension_ = newDimension;
}

/** Get the width / 2 that is also scaled with the related Entity */
float Collider2::getWidthDiv2() const {
     return mDimension_.x / 2.f;
}

/** Get the height / 2 that is also scaled with the related Entity */
float Collider2::getHeightDiv2() const {
     return mDimension_.y / 2.f;
}

/** Get the absolute Center position of this collider*/
glm::vec3 Collider2::getAbsolutePosition() const {
    return mPosition_;
}

void Collider2::addOnCollisionEnterCallback(const OnCollisionCB& cb) {
    mOnCollisionEnterCallbacks_.push_back(cb);
}

void Collider2::addOnCollisionExitCallback(const OnCollisionCB& cb) {
    mOnCollisionExitCallbacks_.push_back(cb);
}

void Collider2::onCollisionEnter(Collider2& other) {
    // TODO keep a list of current collision and only call the function if the collision is new
    for (const auto& func : mOnCollisionEnterCallbacks_) {
        func(other, other);
    }
}

void Collider2::onCollisionExit(Collider2& other) {
    // this can be called in update once an item is removed from the collision list (check on every update)
    for (const auto& func : mOnCollisionExitCallbacks_) {
        func(other, other);
    }
}

} // namespace clay