// forward declare
#include "Clay/Entity/Entity.h"
#include "Clay/Entity/Physics/RigidBodyComponent.h"
// class
#include "Clay/Entity/Physics/ColliderOLD.h"

namespace clay {

ColliderOLD::ColliderOLD(RigidBodyComponent& theRigidBody)
    :mRigidBody_(theRigidBody) {}

ColliderOLD::~ColliderOLD() {}

std::optional<glm::vec3> ColliderOLD::getCollisionNormal(ColliderOLD* otherCollider) const {
    if (mShape_ == ColliderOLD::Shape::CIRCLE && otherCollider->mShape_ == ColliderOLD::Shape::RECTANGLE) {
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
    } else if (mShape_ == ColliderOLD::Shape::RECTANGLE && otherCollider->mShape_ == ColliderOLD::Shape::CIRCLE) {
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
    } else if (mShape_ == ColliderOLD::Shape::CIRCLE && otherCollider->mShape_ == ColliderOLD::Shape::CIRCLE) {
        if (glm::distance(this->getAbsolutePosition(), otherCollider->getAbsolutePosition()) <= this->getWidthDiv2() + otherCollider->getHeightDiv2()) {
            auto norm = glm::normalize(this->getAbsolutePosition() - otherCollider->getAbsolutePosition());
            return norm;
        }
    }

    // no collision
    return std::nullopt;
}

ColliderOLD::Shape ColliderOLD::getShape() const {
    return mShape_;
}

void ColliderOLD::setShape(const Shape& theShape) {
    mShape_ = theShape;
}

std::optional<glm::vec3> ColliderOLD::getCollisionMVT(ColliderOLD* otherCollider) const {
    int moveableCount = this->mRigidBody_.isMobile() + otherCollider->mRigidBody_.isMobile();

    if (moveableCount == 0) {
        return std::nullopt;
    }

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
            return glm::vec3{ -xOverlap * sign, 0.0f, 0.0f } / (float)moveableCount;
        } else {
            float sign = (posDiff.y > 0) ? 1.0f : -1.0f;
            return glm::vec3{ 0.0f, -yOverlap * sign, 0.0f } / (float)moveableCount;
        }
    } else {
        return std::nullopt;
    }

    return std::nullopt;
}

glm::vec3 ColliderOLD::getDimension() const {
    return mDimension_;
}

void ColliderOLD::setDimension(const glm::vec3& newDimension) {
    mDimension_ = newDimension;
}

void ColliderOLD::setDimension(int axis, float newValue) {
    mDimension_[axis] = newValue;
}

float ColliderOLD::getWidthDiv2() const {
    return mDimension_.x / 2.f * mRigidBody_.getEntity().getScale().x;
}

float ColliderOLD::getHeightDiv2() const {
    return mDimension_.y / 2.f * mRigidBody_.getEntity().getScale().y;
}

glm::vec3 ColliderOLD::getAbsolutePosition() const {
    return mRigidBody_.getEntity().getPosition();
}

} // namespace clay