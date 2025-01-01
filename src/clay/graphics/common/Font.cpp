// standard lib
// third party
#include <ft2build.h>
#include FT_FREETYPE_H
#include <glm/gtc/type_ptr.hpp>
// project
#include "clay/application/Logger.h"

// class
#include "clay/graphics/common/Font.h"

namespace clay {

Font::Font(IGraphicsAPI& graphicsAPI, utils::FileData& fileData)
: mGraphicsAPI_(graphicsAPI) {
    // Initialize the FreeType library
    FT_Library ft;
    if (FT_Init_FreeType(&ft)) {
        LOG_E("ERROR::FREETYPE::Could not init FreeType Library");
        return;
    }

    // Create a face object from the memory buffer
    FT_Face face;
    FT_Error error = FT_New_Memory_Face(ft,
                                        reinterpret_cast<const FT_Byte*>(fileData.data.get()),
                                        static_cast<FT_Long>(fileData.size),
                                        0,
                                        &face);

    if (error) {
        LOG_E("ERROR::FREETYPE::Failed to load font from memory. Error code: %d", error);
        FT_Done_FreeType(ft);
        return;
    }

    FT_Set_Pixel_Sizes(face, 0, 48);

    // disable byte-alignment restriction
    mGraphicsAPI_.pixelStore(IGraphicsAPI::PixelAlignment::UNPACK_ALIGNMENT, 1);

    // load first 128 characters of ASCII set
    for (unsigned char c = 0; c < 128; ++c) {
        // Load character glyph
        if (FT_Load_Char(face, c, FT_LOAD_RENDER)) {
            LOG_E("ERROR::FREETYTPE: Failed to load Glyph");
            continue;
        }
        // generate texture
        unsigned int texture;
        mGraphicsAPI_.genTextures(1, &texture);
        mGraphicsAPI_.bindTexture(IGraphicsAPI::TextureTarget::TEXTURE_2D, texture);
        mGraphicsAPI_.texImage2D(
            IGraphicsAPI::TextureTarget::TEXTURE_2D,
            0,
            IGraphicsAPI::TextureFormat::RED, // TODO different for gles
            face->glyph->bitmap.width,
            face->glyph->bitmap.rows,
            0,
            IGraphicsAPI::TextureFormat::RED, // TODO different for gles
            IGraphicsAPI::DataType::UBYTE,
            face->glyph->bitmap.buffer
        );
        // set texture options
        graphicsAPI.texParameter(IGraphicsAPI::TextureTarget::TEXTURE_2D, IGraphicsAPI::TextureParameterType::TEXTURE_WRAP_S, IGraphicsAPI::TextureParameterOption::CLAMP_TO_EDGE);
        graphicsAPI.texParameter(IGraphicsAPI::TextureTarget::TEXTURE_2D, IGraphicsAPI::TextureParameterType::TEXTURE_WRAP_T, IGraphicsAPI::TextureParameterOption::CLAMP_TO_EDGE);
        graphicsAPI.texParameter(IGraphicsAPI::TextureTarget::TEXTURE_2D, IGraphicsAPI::TextureParameterType::TEXTURE_MIN_FILTER, IGraphicsAPI::TextureParameterOption::LINEAR);
        graphicsAPI.texParameter(IGraphicsAPI::TextureTarget::TEXTURE_2D, IGraphicsAPI::TextureParameterType::TEXTURE_MAG_FILTER, IGraphicsAPI::TextureParameterOption::LINEAR);
        // now store character for later use
        Character character = {
            texture,
            glm::ivec2(face->glyph->bitmap.width, face->glyph->bitmap.rows),
            glm::ivec2(face->glyph->bitmap_left, face->glyph->bitmap_top),
            static_cast<unsigned int>(face->glyph->advance.x)
        };
        mCharacterFrontInfo_.insert(std::pair<char, Character>(c, character));
    }
    graphicsAPI.bindTexture(IGraphicsAPI::TextureTarget::TEXTURE_2D, 0);

    // destroy FreeType once we're finished
    FT_Done_Face(face);
    FT_Done_FreeType(ft);

    mGraphicsAPI_.genVertexArrays(1, &mTextVAO_);
    mGraphicsAPI_.genBuffer(1, &mTextVBO_);
    mGraphicsAPI_.bindVertexArray(mTextVAO_);
    mGraphicsAPI_.bindBuffer(IGraphicsAPI::BufferTarget::ARRAY_BUFFER, mTextVBO_);
    mGraphicsAPI_.bufferData(IGraphicsAPI::BufferTarget::ARRAY_BUFFER, sizeof(float) * 6 * 4, NULL, IGraphicsAPI::DataUsage::DYNAMIC_DRAW);
    mGraphicsAPI_.enableVertexAttribArray(0);
    mGraphicsAPI_.vertexAttribPointer(0, 4, IGraphicsAPI::DataType::FLOAT, false, 4 * sizeof(float), (void*)0);
    mGraphicsAPI_.bindBuffer(IGraphicsAPI::BufferTarget::ARRAY_BUFFER, 0);
    mGraphicsAPI_.bindVertexArray(0);
}

Font::~Font() {
    // TODO release VAO/VBO?
}

const Font::Character* Font::getCharInfo(char theChar) const {
    auto it = mCharacterFrontInfo_.find(theChar);

    if (it != mCharacterFrontInfo_.end()) {
        return &(it->second);
    } else {
        return nullptr;
    }
}

unsigned int Font::getVAO() const {
    return mTextVAO_;
}

unsigned int Font::getVBO() const {
    return mTextVBO_;
}

} // namespace clay