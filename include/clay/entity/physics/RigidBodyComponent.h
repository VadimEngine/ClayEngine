#pragma once
// standard lib
#include <optional>
// third party
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
// project
#include "clay/entity/physics/ColliderOLD.h"
#include "clay/entity/physics/PhysicsComponentBase.h"

namespace clay {

class Entity;

class RigidBodyComponent : public PhysicsComponentBase {
public:
    /**
     * Constructor
     * @param parentEntity Entity this Rigid body is attached to
    */
    RigidBodyComponent(Entity& parentEntity);

    /** Destructor */
    ~RigidBodyComponent();

    /**
     * Update this rigid body and related Entity
     * @param dt Time since last update
    */
    void update(float dt) override;

    /** Get what kind of Physics component this is (used for polymorphism) */
    ComponentType getType() const override;

    /** Get the collider for this rigid body*/
    ColliderOLD& getCollider();

    /**
     * Do collision actions on the Entity/RigidBody if there is a collision
     * @param otherCollider Other Collider that may be colliding with this Collider
     */
    bool handleIfCollision(RigidBodyComponent* other);

    /**
     * Do attraction actions on the Entity/RigidBody if there is an attraction
     * @param otherCollider Other Collider that may be attracting
     */
    void applyIfAttraction(RigidBodyComponent* other, float dt);

    /** Get the mass of this RigidBody*/
    float getMass();

    /** Get the Gravity Scale of this RigidBody*/
    float getGravityScale();

    /** Get if this rigidBody is moveable */
    bool isMobile();

    /** Get if this rigidBody is attractive */
    bool isAttractive();

    /**
     * Set the mass of this rigid body
     * @param newMass New mass
     */
    void setMass(float newMass);

    /**
     * Set the gravity scale of this rigid body
     * @param newGravity New gravity
     */
    void setGravityScale(float newGravity);

    /**
     * Set if this rigid body is mobile
     * @param newMobile new mobile status
     */
    void setMobile(bool newMobile);

    /**
     * Set if this rigid body is attractive
     * @param newMobile new attractive status
     */
    void setAttractive(bool newAttractive);

private:
    /** Max speed */
    float mMaxSpeed_ = 100.f;

    /** Mass of this component */
    float mMass_ = 1.f;

    /** Scale of gravity applied */
    float mGravityScale_ = 0.f;

    /** If this component moves when responding to interactions. TODO better name? static/Kinematic? */
    bool mMobile_ = true;

    /** If this component applies and responds to attractive forces with other attractive components*/
    bool mAttractive_ = false;

    // TODO drag

    /** Collider used for interacting with other components */
    ColliderOLD mCollider_;
};

} // namespace clay