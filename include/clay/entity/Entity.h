#pragma once
// standard lib
#include <unordered_map>
#include <vector>
// third party
#include <glm/vec3.hpp>
// project
#include "clay/entity/physics/BoxCollider2D.h"
#include "clay/entity/physics/Collider2.h"
#include "clay/entity/physics/PhysicsComponentBase.h"
#include "clay/entity/physics/RigidBodyComponent.h"
#include "clay/entity/render/BaseRenderable.h"
#include "clay/entity/render/ModelRenderable.h"
#include "clay/entity/render/SpriteRenderable.h"
#include "clay/entity/render/TextRenderable.h"

namespace clay {

class Scene;

class Entity {
public:
    /** Constructor */
    Entity(Scene& scene);

    /** Destructor */
    virtual ~Entity();

    /**
     * @brief Update this Entity
     *
     * @param dt Time (is seconds) since last update
     */
    virtual void update(float dt);

    /**
     * Render all renderable components of this Entity
     * @param theRenderer Helping Object for rendering
     */
    virtual void render(const Renderer& theRenderer) const;

    /** Get this Entity's position */
    glm::vec3 getPosition() const;

    /** Get this Entity's rotation */
    glm::vec3 getRotation() const;

    /** Get this Entity's scale */
    glm::vec3 getScale() const;

    /** Get the velocity of this Entity*/
    glm::vec3 getVelocity() const;

    /**
     * @brief Render the highlight of this Entity
     *
     * @param theRenderer Helping Rendering Object
     */
    virtual void renderHighlight(const Renderer& theRenderer) const;

    /** Get the list of renderable component */
    std::vector<std::unique_ptr<BaseRenderable>>& getRenderableComponents();

    /**
     * @brief Add a Renderable for this Entity. Renderables are owned by the entity and deleted when the Entity is deleted
     *
     * @param newRenderable New Renderable
     */
    void addRenderable(BaseRenderable* newRenderable);

    /**
     * @brief Add a new Renderable Component to this Entity. This will be owned by the entity
     *
     * @tparam T Type of Renderable Component that extends BaseRenderable
     * @return T* The generated Renderable component
     */
    template<typename T>
    T* addRenderable();

    // TODO addPosition, addRotation, addScale

    /**
     * Set this Entity's position
     * @param newPosition New position vector
     */
    virtual void setPosition(const glm::vec3& newPosition);

    /**
     * Set this Entity's rotation in degrees
     * @param newRotation New Rotation vector (In degrees)
     */
    void setRotation(const glm::vec3& newRotation);

    /**
     * Set this Entity's scale
     * @param newScale New scale vector
     */
    void setScale(const glm::vec3& newScale);

    /**
     * Set the velocity of this Entity
     * @param newVelocity New velocity vector
     */
    void setVelocity(const glm::vec3& newVelocity);

    /**
     * @brief Set the Collider2 object
     *
     * @param newCollider New Collider
     */
    void setCollider2(Collider2* newCollider);

    Collider2* getCollider() const;

    /**
     * @brief Add a physics component of the specified class
     *
     * @tparam T Physics Component that extends PhysicsComponentBase
     * @return T* Generated Physics component
     */
    template<typename T>
    T* addPhysicsComponent();

    /**
     * @brief Add the passed in physics component
     *
     * @tparam T Physics Component that extends PhysicsComponentBase
     * @param component New compontent
     */
    template<typename T>
    void addPhysicsComponent(T* component);

    /**
     * @brief Get a pointer to the physics component of the specified class if it exists.
     * If there are multiple then the first instance is returned
     *
     * @tparam T Physics component that extends PhysicsComponentBase
     * @return T* Component of the given type
     */
    template<typename T>
    T* getPhysicsComponent();

    /**
     * @brief Get a list of pointers to the physics component of the specified class if it exist
     *
     * @tparam T Physics component that extends PhysicsComponentBase
     * @return std::vector<T*> List of Component of the given type
     */
    template<typename T>
    std::vector<T*> getPhysicsComponents();

protected:
    /** Scene this Entity is in*/
    Scene& mScene_;
    /** Collider TODO fix this*/
    Collider2* mCollider_ = nullptr;
    /** Entity Position */
    glm::vec3 mPosition_ = {0.0f, 0.0f, 0.0f};
    /** Entity Rotation in degrees */
    glm::vec3 mRotation_ = { 0.0f, 0.0f, 0.0f };
    /** Entity Scale */
    glm::vec3 mScale_ = { 1.0f, 1.0f, 1.0f };
    /** Velocity*/
    glm::vec3 mVelocity_ = {0.0f, 0.0f, 0.0f};
    /** List of all rendering components attached to this Entity */
    std::vector<std::unique_ptr<BaseRenderable>> mRenderableComponents_;
    /** Physics components attached to this Entity*/
    std::unordered_map<
        PhysicsComponentBase::ComponentType,
        std::vector<std::unique_ptr<PhysicsComponentBase>>
    > mPhysicsComponents_;
};

} // namespace clay