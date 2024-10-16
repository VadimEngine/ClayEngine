#pragma once

namespace clay {

class Entity;

class PhysicsComponentBase {
public:
    /** Physics Component types*/
    enum class ComponentType {
        RIGID_BODY, COLLIDER
    };

protected:
    Entity& mParentEntity_;
public:
    /** If this Component is enabled (update is applied)*/
    bool mEnabled_ = true;

    PhysicsComponentBase(Entity& parentEntity);

    virtual void update(float dt) = 0;

    // virtual void render(Renderer, camera) = 0;

    /**
     * Set if this Component is enabled
     * @param isEnabled New enabled value
     */
    void setEnabled(const bool isEnabled);

    /** Get if this Component is enabled */
    bool isEnabled() const;

    /** Get the type of Physics Component */
    virtual ComponentType getType() const = 0;

    /** Get the Entity this Component is attached to*/
    Entity& getEntity();
};

} // namespace clay