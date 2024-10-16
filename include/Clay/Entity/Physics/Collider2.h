#pragma once
// standard lib
#include <optional>
#include <functional>
// third party
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
// project
#include "Clay/Application/Logger.h"
#include "Clay/Entity/Physics/PhysicsComponentBase.h"
#include "Clay/Graphics/Renderer.h"

namespace clay {

class Entity;

// TODO extending classes, BoxCollider2D, CircleCollider2D, Polygon2D
class Collider2 : public PhysicsComponentBase {
public:
    // (thisCollider, otherCollider)
    using OnCollisionCB = std::function<void(Collider2&, Collider2&)>;

    /** Possible shapes for a collider: TODO maybe class that has vertices */
    enum class Shape {CIRCLE, RECTANGLE, POLY};

    /** Description of the shape of this collider */
    Shape mShape_;

    /** Dimensions of this Collider. TODO maybe rename to scale */
    glm::vec3 mDimension_ = {1.f, 1.f, 1.f};

    glm::vec3 mPosition_ = {0,0,0};

    // todo how to allow control to remove specific functions
    std::vector<OnCollisionCB> mOnCollisionEnterCallbacks_;

    std::vector<OnCollisionCB> mOnCollisionExitCallbacks_;


    Collider2(Entity& parentEntity);

    ~Collider2();

    void render(const Renderer& theRenderer);

    /**
     * Update this rigid body and related Entity
     * @param dt Time since last update
    */
    void update(float dt) override;

    /** Get what kind of Physics component this is (used for polymorphism) */
    ComponentType getType() const override;

    bool isColliding(const Collider2* other) const;

    std::optional<glm::vec3> getCollisionNormal(Collider2* otherCollider) const;

    std::optional<glm::vec3> getCollisionMVT(Collider2* otherCollider) const;

    bool rayCollides(glm::vec3 rayOrigin, glm::vec3 rayDirection) const;

    Shape getShape() const;

    void setShape(const Shape& theShape);

    void setPosition(const glm::vec3 newPosition);

    glm::vec3 getDimension() const;

    glm::vec3 getPosition();

    void setDimension(const glm::vec3& newDimension);

    void addOnCollisionEnterCallback(const OnCollisionCB& cb);

    void addOnCollisionExitCallback(const OnCollisionCB& cb);

    void onCollisionEnter(Collider2& other);

    void onCollisionExit(Collider2& other);

private:
    /** Get the width / 2 that is also scaled with the related Entity */
    float getWidthDiv2() const;

    /** Get the height / 2 that is also scaled with the related Entity */
    float getHeightDiv2() const;

    /** Get the absolute Center position of this collider*/
    glm::vec3 getAbsolutePosition() const;


};

} // namespace clay