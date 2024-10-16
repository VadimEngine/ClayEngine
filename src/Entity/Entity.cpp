// forward declare
#include "Clay/Application/Scene.h"
// class
#include "Clay/Entity/Entity.h"

namespace clay {

Entity::Entity(Scene& scene)
: mScene_(scene) {}

Entity::~Entity() {}

void Entity::update(float dt) {
    for (auto& pair : mPhysicsComponents_) {
        for (auto& ptr: pair.second) {
            ptr->update(dt);
        }
    }

    mPosition_ += mVelocity_ * dt;
}

void Entity::render(const Renderer& theRenderer) const{
    // translation matrix for position
    glm::mat4 translationMatrix = glm::translate(glm::mat4(1.0f), mPosition_);
    // rotation matrix
    glm::mat4 rotationMatrix = glm::rotate(glm::mat4(1), glm::radians(mRotation_.x), glm::vec3(1.0f, 0.0f, 0.0f));
    rotationMatrix = glm::rotate(rotationMatrix, glm::radians(mRotation_.y), glm::vec3(0.0f, 1.0f, 0.0f));
    rotationMatrix = glm::rotate(rotationMatrix, glm::radians(mRotation_.z), glm::vec3(0.0f, 0.0f, 1.0f));
    // scale matrix
    glm::mat4 scaleMatrix = glm::scale(glm::mat4(1.0f), mScale_);

    const glm::mat4 modelMat = translationMatrix * rotationMatrix * scaleMatrix;
    for (const auto& eachRenderable : mRenderableComponents_) {
        if (eachRenderable->isEnabled()) {
            eachRenderable->render(theRenderer, modelMat);
        }
    }
}

void Entity::renderHighlight(const Renderer& theRenderer) const {
    // TODO how to set a color for this?
    getCollider()->render(theRenderer);
}

 std::vector<std::unique_ptr<BaseRenderable>>& Entity::getRenderableComponents() {
    return mRenderableComponents_;
}

void Entity::addRenderable(BaseRenderable* newRenderable) {
    mRenderableComponents_.emplace_back(newRenderable);
}

template<typename T>
T* Entity::addRenderable() {
    T* component = new T();
    mRenderableComponents_.emplace_back(component);
    return component;
}

glm::vec3 Entity::getPosition() const {
    return mPosition_;
}

glm::vec3 Entity::getRotation() const {
    return mRotation_;
}

glm::vec3 Entity::getScale() const {
    return mScale_;
}

glm::vec3 Entity::getVelocity() const {
    return mVelocity_;
}

void Entity::setPosition(const glm::vec3& newPosition) {
    mPosition_ = newPosition;
    if (mCollider_ != nullptr) {
        mCollider_->setPosition(newPosition);
    }
}

void Entity::setRotation(const glm::vec3& newRotation) {
    mRotation_ = newRotation;
}

void Entity::setScale(const glm::vec3& newScale) {
    mScale_ = newScale;
}

void Entity::setVelocity(const glm::vec3& newVelocity) {
    mVelocity_ = newVelocity;
}

void Entity::setCollider2(Collider2* newCollider) {
    mCollider_ = newCollider;
}

Collider2* Entity::getCollider() const {
    return mCollider_;
}

template<typename T>
T* Entity::addPhysicsComponent() {
    T* component = new T(*this);
    mPhysicsComponents_[component->getType()].emplace_back(component);
    return component;
}

template<typename T>
void Entity::addPhysicsComponent(T* component) {
    mPhysicsComponents_[component->getType()].emplace_back(component);
}

template<typename T>
T* Entity::getPhysicsComponent() {
    // Use a temporary Component to get the seeking type
    auto it = mPhysicsComponents_.find(T(*this).getType());
    if (it != mPhysicsComponents_.end() && !it->second.empty()) {
        return dynamic_cast<T*>(it->second[0].get());
    }
    // Component type not found
    return nullptr;
}

template<typename T>
std::vector<T*> Entity::getPhysicsComponents() {
    std::vector<T*> result;
    auto it = mPhysicsComponents_.find(T(*this).getType());
    if (it != mPhysicsComponents_.end()) {
        for (auto& component : it->second) {
            T* castedComponent = dynamic_cast<T*>(component.get());
            if (castedComponent != nullptr) {
                result.push_back(castedComponent);
            }
        }
    }
    return result;
}

// Explicit instantiate template for expected types
template ModelRenderable* Entity::addRenderable<ModelRenderable>();
template SpriteRenderable* Entity::addRenderable<SpriteRenderable>();
template TextRenderable* Entity::addRenderable<TextRenderable>();

template RigidBodyComponent* Entity::addPhysicsComponent<RigidBodyComponent>();
template void Entity::addPhysicsComponent(RigidBodyComponent* component);
template RigidBodyComponent* Entity::getPhysicsComponent<RigidBodyComponent>();
template std::vector<RigidBodyComponent*> Entity::getPhysicsComponents<RigidBodyComponent>();

template Collider2* Entity::addPhysicsComponent<Collider2>();
template void Entity::addPhysicsComponent(Collider2* component);
template Collider2* Entity::getPhysicsComponent<Collider2>();
template std::vector<Collider2*> Entity::getPhysicsComponents<Collider2>();

template BoxCollider2D* Entity::addPhysicsComponent<BoxCollider2D>();
template void Entity::addPhysicsComponent(BoxCollider2D* component);
template BoxCollider2D* Entity::getPhysicsComponent<BoxCollider2D>();
template std::vector<BoxCollider2D*> Entity::getPhysicsComponents<BoxCollider2D>();

} // namespace clay