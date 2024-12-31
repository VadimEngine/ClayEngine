#include "clay/entity/Entity.h"
// class
#include "clay/entity/physics/PhysicsComponentBase.h"

namespace clay {

PhysicsComponentBase::PhysicsComponentBase(Entity& parentEntity)
    : mParentEntity_(parentEntity) {}


void PhysicsComponentBase::setEnabled(const bool isEnabled) {
    mEnabled_ = isEnabled;
}

bool PhysicsComponentBase::isEnabled() const {
    return mEnabled_;
}

Entity& PhysicsComponentBase::getEntity() {
    return mParentEntity_;
}

} // namespace clay