// standard lib
#include <stdexcept>
// class
#include "clay/application/common/Resources.h"

namespace clay {

std::filesystem::path Resources::RESOURCE_PATH = "";

std::function<utils::FileData(const std::string&)> Resources::loadFileToMemory;
std::function<utils::ImageData(const std::string&)> Resources::loadImageFileToMemory;


Resources::Resources() = default;

Resources::~Resources() = default;

template<typename T>
void Resources::loadResource(const std::vector<std::string>& resourcePath, const std::string& resourceName) {
    if constexpr (std::is_same_v<T, Mesh>) {
        utils::FileData loadedFile = loadFileToMemory(resourcePath[0]);
        
        std::vector<Mesh> loadedMeshes;
        Mesh::parseMeshes(*mGraphicsAPI_, loadedFile, loadedMeshes);

        if (loadedMeshes.size() == 1) {
            std::unique_ptr<Mesh> meshPtr = std::make_unique<Mesh>(std::move(loadedMeshes[0]));
            mMeshes_[resourceName] = std::move(meshPtr);
        } else {
            LOG_E("%s contains %zu meshes", resourceName.c_str(), loadedMeshes.size());
            throw std::runtime_error("Invalid number of Meshes in Mesh Resource");
        }
    } else if constexpr (std::is_same_v<T, Model>) {
        utils::FileData loadedFile = loadFileToMemory(resourcePath[0]);
        auto pModel = std::make_unique<Model>();

        std::vector<Mesh> loadedMeshes;
        Mesh::parseMeshes(*mGraphicsAPI_, loadedFile, loadedMeshes);
        pModel->addMeshes(std::move(loadedMeshes));
        mModels_[resourceName] = std::move(pModel);
    } else if constexpr(std::is_same_v<T, Texture>) {
        // Texture
        utils::ImageData loadedFile = loadImageFileToMemory(resourcePath[0]);
        // TODO how to pass in gammaCorrect. Maybe have loadResource() take in a resource param (different class per resource) object with additional detai
        mTextures_[resourceName] = std::make_unique<Texture>(*mGraphicsAPI_, loadedFile, true);
    } else if constexpr (std::is_same_v<T, ShaderProgram>) {
        auto shaderProgram = std::make_unique<ShaderProgram>(*mGraphicsAPI_);

        for (const auto& eachPath: resourcePath) {
            const size_t lastColon =  eachPath.find_last_of(':');
            const std::string shaderPath = eachPath.substr(0, lastColon);
            // TODO consider std::string_view?
            const std::string shaderType = eachPath.substr(lastColon + 1);

            utils::FileData fileData = loadFileToMemory(shaderPath);

            ShaderCreateInfo::Type shaderTypeEnum;

            if (shaderType == "VERTEX") {
                shaderTypeEnum = ShaderCreateInfo::Type::VERTEX;
            } else if (shaderType == "TESSELLATION_CONTROL") {
                shaderTypeEnum = ShaderCreateInfo::Type::TESSELLATION_CONTROL;
            } else if (shaderType == "TESSELLATION_EVALUATION") {
                shaderTypeEnum = ShaderCreateInfo::Type::TESSELLATION_EVALUATION;
            } else if (shaderType == "GEOMETRY") {
                shaderTypeEnum = ShaderCreateInfo::Type::GEOMETRY;
            } else if (shaderType == "FRAGMENT") {
                shaderTypeEnum = ShaderCreateInfo::Type::FRAGMENT;
            } else if (shaderType == "COMPUTE") {
                shaderTypeEnum = ShaderCreateInfo::Type::COMPUTE;
            } else {
                throw std::runtime_error("Invalid shader path suffix");
            }

            shaderProgram->addShader({
                shaderTypeEnum,
                std::string(reinterpret_cast<char*>(fileData.data.get()), fileData.size).c_str(),
                fileData.size
            });
        }

        shaderProgram->linkProgram();
        mShaders_[resourceName] = std::move(shaderProgram);
    } else if constexpr (std::is_same_v<T, Audio>) {
        utils::FileData loadedFile = loadFileToMemory(resourcePath[0]);
        mAudios_[resourceName] = std::make_unique<Audio>(loadedFile);
    } else if constexpr (std::is_same_v<T, Font>) {
        utils::FileData loadedFile = loadFileToMemory(resourcePath[0]);
        mFonts_[resourceName] = std::make_unique<Font>(*mGraphicsAPI_, loadedFile);
    }
}

template<typename T>
void Resources::addResource(std::unique_ptr<T> resource, const std::string& resourceName) {
    if constexpr (std::is_same_v<T, Mesh>) {
        mMeshes_[resourceName] = std::move(resource);
    } else if constexpr (std::is_same_v<T, Model>) {
        mModels_[resourceName] = std::move(resource);
    } else if constexpr (std::is_same_v<T, Texture>) {
        mTextures_[resourceName] = std::move(resource);
    } else if constexpr (std::is_same_v<T, ShaderProgram>) {
        mShaders_[resourceName] = std::move(resource);
    } else if constexpr (std::is_same_v<T, Audio>) {
        mAudios_[resourceName] = std::move(resource);
    } else if constexpr (std::is_same_v<T, Font>) {
        mFonts_[resourceName] = std::move(resource);
    } else if constexpr (std::is_same_v<T, SpriteSheet>) {
        mSpriteSheets[resourceName] = std::move(resource);
    }
}

template<typename T>
T* Resources::getResource(const std::string& resourceName) {
    if constexpr (std::is_same_v<T, Mesh>) {
        auto it = mMeshes_.find(resourceName);
        if (it != mMeshes_.end()) {
            return it->second.get();
        }
    } else if constexpr (std::is_same_v<T, Model>) {
        auto it = mModels_.find(resourceName);
        if (it != mModels_.end()) {
            return it->second.get();
        }
    } else if constexpr(std::is_same_v<T, Texture>) {
        auto it = mTextures_.find(resourceName);
        if (it != mTextures_.end()) {
            return it->second.get();
        }
    } else if constexpr (std::is_same_v<T, ShaderProgram>) {
        auto it = mShaders_.find(resourceName);
        if (it != mShaders_.end()) {
            return it->second.get();
        }
    } else if constexpr (std::is_same_v<T, Audio>) {
        auto it = mAudios_.find(resourceName);
        if (it != mAudios_.end()) {
            return it->second.get();
        }
    } else if constexpr (std::is_same_v<T, Font>) {
        auto it = mFonts_.find(resourceName);
        if (it != mFonts_.end()) {
            return it->second.get();
        }
    } else if constexpr (std::is_same_v<T, SpriteSheet>) {
        auto it = mSpriteSheets.find(resourceName);
        if (it != mSpriteSheets.end()) {
            return it->second.get();
        }
    }

    // Return nullptr if resource not found or type mismatch
    return nullptr;
}

template<typename T>
void Resources::release(const std::string& resourceName) {
    if constexpr (std::is_same_v<T, Mesh>) {
        auto it = mMeshes_.find(resourceName);
        if (it != mMeshes_.end()) {
            mMeshes_.erase(it);
        }
    } else if constexpr (std::is_same_v<T, Model>) {
        auto it = mModels_.find(resourceName);
        if (it != mModels_.end()) {
            mModels_.erase(it);
        }
    } else if constexpr(std::is_same_v<T, Texture>) {
        auto it = mTextures_.find(resourceName);
        if (it != mTextures_.end()) {
            mTextures_.erase(it);
        }
    } else if constexpr (std::is_same_v<T, ShaderProgram>) {
        auto it = mShaders_.find(resourceName);
        if (it != mShaders_.end()) {
            mShaders_.erase(it);
        }
    } else if constexpr (std::is_same_v<T, Audio>) {
        auto it = mAudios_.find(resourceName);
        if (it != mAudios_.end()) {
            mAudios_.erase(it);
        }
    } else if constexpr (std::is_same_v<T, Font>) {
        auto it = mFonts_.find(resourceName);
        if (it != mFonts_.end()) {
            mFonts_.erase(it);
        }
    } else if constexpr (std::is_same_v<T, SpriteSheet>) {
        auto it = mSpriteSheets.find(resourceName);
        if (it != mSpriteSheets.end()) {
            mSpriteSheets.erase(it);
        }
    }
}

void Resources::releaseAll() {
    mMeshes_.clear();
    mModels_.clear();
    mTextures_.clear();
    mShaders_.clear();
    mAudios_.clear();
    mFonts_.clear();
    mSpriteSheets.clear();
}

// Explicit instantiate template for expected types
template void Resources::loadResource<Mesh>(const std::vector<std::string>& resourcePath, const std::string& resourceName);
template void Resources::loadResource<Model>(const std::vector<std::string>& resourcePath, const std::string& resourceName);
template void Resources::loadResource<Texture>(const std::vector<std::string>& resourcePath, const std::string& resourceName);
template void Resources::loadResource<ShaderProgram>(const std::vector<std::string>& resourcePath, const std::string& resourceName);
template void Resources::loadResource<Audio>(const std::vector<std::string>& resourcePath, const std::string& resourceName);
template void Resources::loadResource<Font>(const std::vector<std::string>& resourcePath, const std::string& resourceName);
// No load for SpriteSheet

template void Resources::addResource(std::unique_ptr<Mesh> resource, const std::string& resourceName);
template void Resources::addResource(std::unique_ptr<Model> resource, const std::string& resourceName);
template void Resources::addResource(std::unique_ptr<Texture> resource, const std::string& resourceName);
template void Resources::addResource(std::unique_ptr<ShaderProgram> resource, const std::string& resourceName);
template void Resources::addResource(std::unique_ptr<Audio> resource, const std::string& resourceName);
template void Resources::addResource(std::unique_ptr<Font> resource, const std::string& resourceName);
template void Resources::addResource(std::unique_ptr<SpriteSheet> resource, const std::string& resourceName);

template Mesh* Resources::getResource(const std::string& resourceName);
template Model* Resources::getResource(const std::string& resourceName);
template Texture* Resources::getResource(const std::string& resourceName);
template ShaderProgram* Resources::getResource(const std::string& resourceName);
template Audio* Resources::getResource(const std::string& resourceName);
template Font* Resources::getResource(const std::string& resourceName);
template SpriteSheet* Resources::getResource(const std::string& resourceName);

template void Resources::release<Mesh>(const std::string& resourceName);
template void Resources::release<Model>(const std::string& resourceName);
template void Resources::release<Texture>(const std::string& resourceName);
template void Resources::release<ShaderProgram>(const std::string& resourceName);
template void Resources::release<Audio>(const std::string& resourceName);
template void Resources::release<Font>(const std::string& resourceName);
template void Resources::release<SpriteSheet>(const std::string& resourceName);

} // namespace clay