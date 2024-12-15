// class
#include "clay/graphics/Model.h"

namespace clay {

Model::Model() {}

Model::~Model() {}

void Model::addMesh(const Mesh& newMesh){
    mMeshes_.push_back(newMesh);
}

void Model::addSharedMesh(Mesh* newMesh) {
    mSharedMeshes_.push_back(newMesh);
}

void Model::addMeshes(std::vector<Mesh>&& meshes) {
    mMeshes_ = std::move(meshes);
}

void Model::loadMesh(const std::filesystem::path& meshPath) {
    Mesh::loadMeshes(meshPath, mMeshes_);
}

void Model::render(const Shader& shader) const {
    for (auto& each: mMeshes_) {
        each.render(shader);
    }

    for (auto each: mSharedMeshes_) {
        each->render(shader);
    }
}

} // namespace clay