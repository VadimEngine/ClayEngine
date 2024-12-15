#pragma once
// standard lib
#include <filesystem>
#include <memory>
#include <string>
#include <unordered_map>
// project
#include "clay/audio/Audio.h"
#include "clay/graphics/common/Font.h"
#include "clay/graphics/common/Mesh.h"
#include "clay/graphics/common/Model.h"
#include "clay/graphics/common/Shader.h"
#include "clay/graphics/common/ShaderProgram.h"
#include "clay/graphics/common/SpriteSheet.h"
#include "clay/graphics/common/Texture.h"

namespace clay {

class Resource {
public:
    /** Path to resource folder */
    static std::filesystem::path RESOURCE_PATH;

    /** Loaded/Built Mesh resources */
    std::unordered_map<std::string, std::unique_ptr<Mesh>> mMeshes_;
    /** Loaded/Built Model resources */
    std::unordered_map<std::string, std::unique_ptr<Model>> mModels_;
    /** Loaded/Built Model textures */
    std::unordered_map<std::string, std::unique_ptr<Texture>> mTextures_;
    /** Loaded/Built Shader resources */
    std::unordered_map<std::string, std::unique_ptr<ShaderProgram>> mShaders_;
    /** Loaded/Built Audio resources */
    std::unordered_map<std::string, std::unique_ptr<Audio>> mAudios_;
    /** Loaded/Built Fonts resources */
    std::unordered_map<std::string, std::unique_ptr<Font>> mFonts_;
    /** Loaded Sprite Sheets */
    std::unordered_map<std::string, std::unique_ptr<SpriteSheet>> mSpriteSheets;

    /** Constructor default */
    Resource();

    /** Destructor default */
    ~Resource();

    /**
     * Load the resource from given paths
     *
     * @tparam T Type of resource
     * @param resourcePaths Paths to load the resource from
     * @param resourceName Name to save the resource as for retrieval
     */
    template<typename T>
    void loadResource(const std::vector<std::filesystem::path>& resourcePaths, const std::string& resourceName);

    /**
     * Add a resource and transfer ownership to this resource container. Generally std::move should be used here
     *
     * @tparam T Type of resource
     * @param resource Resource rvalue reference
     * @param resourceName Name to save the resource as for retrieval
     */
    template<typename T>
    void addResource(std::unique_ptr<T> resource, const std::string& resourceName);

    /**
     * @brief Get a pointer to the loaded resource
     *
     * @tparam T Type of resource
     * @param resourceName Name of the resource to return
     */
    template<typename T>
    T* getResource(const std::string& resourceName);

    /**
     * Release the resource if it is owned in this resource container
     *
     * @tparam T Type of resource
     * @param resourceName Name of resource to related
     */
    template<typename T>
    void release(const std::string& resourceName);

    /**
     * Release all the resources in this container. (This not need to be called
     * when deleting a Resource Object since the destructor will manage that itself)
     */
    /** Release all resources */
    void releaseAll();
};
} // namespace clay