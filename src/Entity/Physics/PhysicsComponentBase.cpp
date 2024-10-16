// forward declare
#include "Clay/Entity/Entity.h"
// class
#include "Clay/Entity/Physics/PhysicsComponentBase.h"

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