#pragma once
// standard lib
#include <unordered_map>
// project
#include "clay/entity/render/BaseRenderable.h"
#include "clay/graphics/Model.h"

namespace clay {

class ModelRenderable : public BaseRenderable {
public:
    /**
     * Constructor
     * @param pModel Model for this Renderable
     * @param pShader Shader used for this Renderable
     */
    ModelRenderable(const Model* pModel = nullptr, const Shader* pShader = nullptr);

    /** Destructor*/
    ~ModelRenderable();

    /**
     * Render this model
     * @param theRenderer Helper Renderer object
     * @param parentModelMat Model matrix with the model transforms
     */
    void render(const Renderer& theRenderer, const glm::mat4& parentModelMat) const override;

    /** Get the Model for this Renderable */
    const Model* getModel() const;

    /** Get the Shader used for this Renderable */
    const Shader* getShader() const;

    /**
     * Set the model this Renderable will render
     * @param pModel the Model
     */
    void setModel(Model* pModel);

    /**
     * Set the Shader this Renderable will use
     * @param pShader the Shader
     */
    void setShader(Shader* pShader);

    /**
     * Set a texture when rendering this renderable with the assign Texture unit
     * @param textureUnit The unit this texture is binded to
     * @param textureId texture Id
     * @param uniformName uniform this texture is binded to
     */
    void setTexture(unsigned int textureUnit, unsigned int textureId, const std::string& uniformName);

    /**
     * Set if wireframes should be rendered
     * @param enable if wireframe rendering is enabled
     */
    void setWireframeRendering(const bool enable);

    void setSubTextureSize(const glm::vec2& size);

    void setSubTextureTopLeft(const glm::vec2& pos);

private:
    /**The Model of this Model Renderable */
    const Model* mpModel_ = nullptr;
    /** The Shader used to render this model */
    const Shader* mpShader_ = nullptr;
    /** Map of texture ids and the uniform name for the shader*/
    std::unordered_map<unsigned int, std::pair<unsigned int, std::string>> mTextureByUnit_;
    /** If the wire frames are also rendered */
    bool renderWireframe_ = false;
    /** Size of the sub texture rendered on this model */
    glm::vec2 mSubTextureSize = {1.f, 1.f};
    /** Top left corner of the sub texture rendered on this model */
    glm::vec2 mSubTextureTopLeft = {0.f, 0.f};
};

} // namespace clay