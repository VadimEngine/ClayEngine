#pragma once
// standard lib
#include <optional>
// third party
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>

namespace clay {

// Forward Declare RigidBody
class RigidBodyComponent;

// TODO remove this class (Replace with Collider2)
class ColliderOLD {

// TODO maybe local position, scale and rotation?

public:
    /** Possible shapes for a collider */
    enum class Shape {CIRCLE, RECTANGLE};

    /**
     * Constructor
     * @param theRigidBody Rigid body this Collider is for
    */
    ColliderOLD(RigidBodyComponent& theRigidBody);

    /** Destructor */
    ~ColliderOLD();

    /**
     * If there is a collision, get the normal vector from the other Collider
     * @param otherCollider Other Collider that may be colliding with this Collider
     */
    std::optional<glm::vec3> getCollisionNormal(ColliderOLD* otherCollider) const;

    /**
     * If there is a collision, get the Minimum translation vector that should be applied to this collider
     * @param otherCollider Other Collider that may be colliding with this Collider
     */
    std::optional<glm::vec3> getCollisionMVT(ColliderOLD* otherCollider) const;

    /** Get Shape type of this collider */
    Shape getShape() const;

    /**
     * set Shape type of this collider
     * @param theShape New Shape type
     */
    void setShape(const Shape& theShape);

    /** Get The Dimensions of this Collider */
    glm::vec3 getDimension() const;

    /**
     * Replace the dimensions of this collider
     * @param newDimension New collider dimensions
     */
    void setDimension(const glm::vec3& newDimension);

    /**
     * Update of the dimensions of this collider
     * @param axis Axis to update (x=0, y=1, z=2)
     * @param newValue New value for dimension
     */
    void setDimension(int axis, float newValue);

    // TODO onEnter/onCollide/onExit?

private:
    /** Get the width / 2 that is also scaled with the related Entity */
    float getWidthDiv2() const;

    /** Get the height / 2 that is also scaled with the related Entity */
    float getHeightDiv2() const;

    /** Get the absolute Center position of this collider*/
    glm::vec3 getAbsolutePosition() const;

    /** Description of the shape of this collider */
    Shape mShape_;

    /** The rigid body this collider is tied to */
    // TODO remove this since a collider should be applyable without a rigidbody
    RigidBodyComponent& mRigidBody_;

    /** Dimensions of this Collider */
    glm::vec3 mDimension_ = {1.f, 1.f, 1.f};
};

} // namespace clay