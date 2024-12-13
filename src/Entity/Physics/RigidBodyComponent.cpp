// forward declare
#include "Clay/Entity/Entity.h"
// class
#include "Clay/Entity/Physics/RigidBodyComponent.h"

namespace clay {

RigidBodyComponent::RigidBodyComponent(Entity& parentEntity)
    : PhysicsComponentBase(parentEntity), mCollider_(*this){}

RigidBodyComponent::~RigidBodyComponent() {}

void RigidBodyComponent::update(float dt) {
    if (mMobile_) {
        /// gravity down
        glm::vec3 currVel = mParentEntity_.getVelocity();
        currVel.y += -9.81f * mGravityScale_ * dt;

        mParentEntity_.setVelocity(currVel);
    }
}

PhysicsComponentBase::ComponentType RigidBodyComponent::getType() const {
    return PhysicsComponentBase::ComponentType::RIGID_BODY;
}

ColliderOLD& RigidBodyComponent::getCollider() {
    return mCollider_;
}

bool RigidBodyComponent::handleIfCollision(RigidBodyComponent* other) {
    std::optional<glm::vec3> thisCollisionNormal = mCollider_.getCollisionNormal(&(other->getCollider()));

    if (thisCollisionNormal.has_value()) {
        bool collision = false;
        Entity& thisEntity = mParentEntity_;
        Entity& otherEntity = other->getEntity();

        if (this->mMobile_) {
            thisEntity.setVelocity(
                glm::reflect(thisEntity.getVelocity(), glm::normalize( thisCollisionNormal.value()))
            );
            collision = true;
        }

        if (other->mMobile_) {
            otherEntity.setVelocity(
                glm::reflect(otherEntity.getVelocity(), glm::normalize( -thisCollisionNormal.value()))
            );
            collision = true;
        }
        return collision;
    } else {
        return false;
    }
}

void RigidBodyComponent::applyIfAttraction(RigidBodyComponent* other, float dt) {
    if (mAttractive_ && other->mAttractive_) {
        float distance = glm::length(mParentEntity_.getPosition() - other->mParentEntity_.getPosition());

        if (distance != 0) {
            float scale = 5;

            glm::vec3 dir = glm::vec3(
                mParentEntity_.getPosition().x - other->mParentEntity_.getPosition().x,
                mParentEntity_.getPosition().y - other->mParentEntity_.getPosition().y,
                0.0f
            );

            glm::vec3 newVeli = (mParentEntity_.getVelocity() - (dir * static_cast<float>(scale * (1.0f / pow(distance, 2.0f)) * dt)));
            glm::vec3 newVelj = (other->mParentEntity_.getVelocity() + (dir * static_cast<float>(scale * (1.0f / pow(distance, 2.0f)) * dt)));

            if (mMobile_) {
                mParentEntity_.setVelocity(newVeli);
            }

            if (other->mMobile_) {
                other->mParentEntity_.setVelocity(newVelj);
            }
        }
    }
}

float RigidBodyComponent::getMass() {
    return mMass_;
}

float RigidBodyComponent::getGravityScale() {
    return mGravityScale_;
}

bool RigidBodyComponent::isMobile() {
    return mMobile_;
}

bool RigidBodyComponent::isAttractive() {
    return mAttractive_;
}

void RigidBodyComponent::setMass(float newMass) {
    mMass_ = newMass;
}

void RigidBodyComponent::setGravityScale(float newGravity) {
    mGravityScale_ = newGravity;
}

void RigidBodyComponent::setMobile(bool newMobile) {
    mMobile_ = newMobile;
}

void RigidBodyComponent::setAttractive(bool newAttractive) {
    mAttractive_ = newAttractive;
}

} // namespace clay