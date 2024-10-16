#pragma once
// forward declare
#include "Clay/Entity/Render/BaseRenderable.h"
// class
#include "Clay/Graphics/SpriteSheet.h"

namespace clay {

class SpriteRenderable : public BaseRenderable {
public:
    /**
     * Constructor
     * @param pSprite Sprite for this Renderable
     */
    SpriteRenderable(SpriteSheet::Sprite* pSprite = nullptr);

    /** Destructor */
    ~SpriteRenderable();

    /**
     * Draw this renderable
     * @param theRenderer Helper Render object
     * @param parentModelMat Model matrix of the parent to render relative to
     */
    void render(const Renderer& theRenderer, const glm::mat4& parentModelMat) const override;

    /**
     * Set the Sprite this Renderable will draw
     * @param pSprite the Sprite
     */
    void setSprite(SpriteSheet::Sprite* pSprite);

    /** Get the Sprite for this Renderable */
    SpriteSheet::Sprite* getSprite() const;

private:
    /** The Sprite for this Renderable */
    SpriteSheet::Sprite* mpSprite_;
};

} // namespace clay