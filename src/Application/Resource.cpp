// class
#include "Clay/Application/Resource.h"

namespace clay {

std::filesystem::path Resource::RESOURCE_PATH = "";
std::filesystem::path Resource::SRC_PATH = "";

Resource::Resource() {}

Resource::~Resource() {}

template<typename T>
void Resource::loadResource(const std::vector<std::filesystem::path>& resourcePath, const std::string& resourceName) {
    if constexpr (std::is_same_v<T, Mesh>) {
        std::vector<Mesh> loadedMeshes;
        Mesh::loadMeshes(resourcePath[0], loadedMeshes);

        if (loadedMeshes.size() == 1) {
            std::unique_ptr<Mesh> meshPtr = std::make_unique<Mesh>(std::move(loadedMeshes[0]));
            mMeshes_[resourceName] = std::move(meshPtr);
        } else {
            LOG_W("%s contains multiple meshes so it is saved as a Model", resourceName.c_str());
            auto newModel = std::make_unique<Model>();
            newModel->addMeshes(std::move(loadedMeshes));
            mModels_[resourceName] = std::move(newModel);
        }
    } else if constexpr (std::is_same_v<T, Model>) {
        auto pModel = std::make_unique<Model>();
        pModel->loadMesh(resourcePath[0]);
        mModels_[resourceName] = std::move(pModel);
    } else if constexpr(std::is_same_v<T, Texture>) {
        mTextures_[resourceName] = std::make_unique<Texture>(resourcePath[0], true);
    } else if constexpr (std::is_same_v<T, Shader>) {
        mShaders_[resourceName] = std::make_unique<Shader>(resourcePath[0].c_str(),resourcePath[1].c_str());
    } else if constexpr (std::is_same_v<T, Audio>) {
        mAudios_[resourceName] = std::make_unique<Audio>(resourcePath[0].string());
    } else if constexpr (std::is_same_v<T, Font>) {
        mFonts_[resourceName] = std::make_unique<Font>(resourcePath[0]);
    }
}

template<typename T>
void Resource::addResource(std::unique_ptr<T> resource, const std::string& resourceName) {
    if constexpr (std::is_same_v<T, Mesh>) {
        mMeshes_[resourceName] = std::move(resource);
    } else if constexpr (std::is_same_v<T, Model>) {
        mModels_[resourceName] = std::move(resource);
    } else if constexpr (std::is_same_v<T, Texture>) {
        mTextures_[resourceName] = std::move(resource);
    } else if constexpr (std::is_same_v<T, Shader>) {
        // Handle Shader case
    } else if constexpr (std::is_same_v<T, Audio>) {
        mAudios_[resourceName] = std::move(resource);
    } else if constexpr (std::is_same_v<T, Font>) {
        mFonts_[resourceName] = std::move(resource);
    } else if constexpr (std::is_same_v<T, SpriteSheet>) {
        mSpriteSheets[resourceName] = std::move(resource);
    }
}

template<typename T>
T* Resource::getResource(const std::string& resourceName) {
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
    } else if constexpr (std::is_same_v<T, Shader>) {
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
void Resource::release(const std::string& resourceName) {
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
    } else if constexpr (std::is_same_v<T, Shader>) {
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

void Resource::releaseAll() {
    mMeshes_.clear();
    mModels_.clear();
    mTextures_.clear();
    mShaders_.clear();
    mAudios_.clear();
    mFonts_.clear();
    mSpriteSheets.clear();
}

// Explicit instantiate template for expected types
template void Resource::loadResource<Mesh>(const std::vector<std::filesystem::path>& resourcePath, const std::string& resourceName);
template void Resource::loadResource<Model>(const std::vector<std::filesystem::path>& resourcePath, const std::string& resourceName);
template void Resource::loadResource<Texture>(const std::vector<std::filesystem::path>& resourcePath, const std::string& resourceName);
template void Resource::loadResource<Shader>(const std::vector<std::filesystem::path>& resourcePath, const std::string& resourceName);
template void Resource::loadResource<Audio>(const std::vector<std::filesystem::path>& resourcePath, const std::string& resourceName);
template void Resource::loadResource<Font>(const std::vector<std::filesystem::path>& resourcePath, const std::string& resourceName);
// No load for SpriteSheet

template void Resource::addResource(std::unique_ptr<Mesh> resource, const std::string& resourceName);
template void Resource::addResource(std::unique_ptr<Model> resource, const std::string& resourceName);
template void Resource::addResource(std::unique_ptr<Texture> resource, const std::string& resourceName);
template void Resource::addResource(std::unique_ptr<Shader> resource, const std::string& resourceName);
template void Resource::addResource(std::unique_ptr<Audio> resource, const std::string& resourceName);
template void Resource::addResource(std::unique_ptr<Font> resource, const std::string& resourceName);
template void Resource::addResource(std::unique_ptr<SpriteSheet> resource, const std::string& resourceName);

template Mesh* Resource::getResource(const std::string& resourceName);
template Model* Resource::getResource(const std::string& resourceName);
template Texture* Resource::getResource(const std::string& resourceName);
template Shader* Resource::getResource(const std::string& resourceName);
template Audio* Resource::getResource(const std::string& resourceName);
template Font* Resource::getResource(const std::string& resourceName);
template SpriteSheet* Resource::getResource(const std::string& resourceName);

template void Resource::release<Mesh>(const std::string& resourceName);
template void Resource::release<Model>(const std::string& resourceName);
template void Resource::release<Texture>(const std::string& resourceName);
template void Resource::release<Shader>(const std::string& resourceName);
template void Resource::release<Audio>(const std::string& resourceName);
template void Resource::release<Font>(const std::string& resourceName);
template void Resource::release<SpriteSheet>(const std::string& resourceName);

} // namespace clay