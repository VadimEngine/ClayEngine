// third party
#include <glm/gtc/matrix_transform.hpp>
// project
#include "clay/utils/common/Logger.h"
// class
#include "clay/graphics/common/Mesh.h"

namespace clay {

void Mesh::parseMeshes(IGraphicsAPI& graphicsAPI, utils::FileData& fileData, std::vector<Mesh>& meshList) {
    Assimp::Importer import;
    const aiScene* scene = import.ReadFileFromMemory(
            fileData.data.get(),
            fileData.size,
            aiProcess_Triangulate | aiProcess_GenSmoothNormals | aiProcess_CalcTangentSpace,
            "obj" // Pass a file extension if needed, e.g., "obj"
    );
    if (!scene || scene->mFlags & AI_SCENE_FLAGS_INCOMPLETE || !scene->mRootNode) {
        LOG_E("ERROR::ASSIMP::%s", import.GetErrorString());
        return;
    }
    // Process the Assimp node and add to mMeshes_
    processNode(graphicsAPI, scene->mRootNode, scene, meshList);
}

void Mesh::processNode(IGraphicsAPI& graphicsAPI, aiNode *node, const aiScene *scene, std::vector<Mesh>& meshList) {
    // process all the node's meshes (if any)
    for (unsigned int i = 0; i < node->mNumMeshes; ++i) {
        aiMesh* mesh = scene->mMeshes[node->mMeshes[i]];
        meshList.push_back(processMesh(graphicsAPI, mesh, scene));
    }
    // then do the same for each of its children
    for (unsigned int i = 0; i < node->mNumChildren; ++i) {
        processNode(graphicsAPI, node->mChildren[i], scene, meshList);
    }
}

Mesh Mesh::processMesh(IGraphicsAPI& graphicsAPI, aiMesh *mesh, const aiScene *scene) {
    std::vector<Mesh::Vertex> vertices;
    std::vector<unsigned int> indices;

    for (unsigned int i = 0; i < mesh->mNumVertices; ++i) {
        Mesh::Vertex vertex;
        glm::vec3 vector; // we declare a placeholder vector since assimp uses its own vector class that doesn't directly convert to glm's vec3 class so we transfer the data to this placeholder glm::vec3 first.
        // positions
        vector.x = mesh->mVertices[i].x;
        vector.y = mesh->mVertices[i].y;
        vector.z = mesh->mVertices[i].z;
        vertex.position = vector;
        // normals
        if (mesh->HasNormals()) {
            vector.x = mesh->mNormals[i].x;
            vector.y = mesh->mNormals[i].y;
            vector.z = mesh->mNormals[i].z;
            vertex.normal = vector;
        } else {
            vertex.normal = {0,0,0};
        }
        // texture coordinates
        if(mesh->mTextureCoords[0]) { // does the mesh contain texture coordinates?
            glm::vec2 vec;
            // a vertex can contain up to 8 different texture coordinates. We thus make the assumption that we won't
            // use models where a vertex can have multiple texture coordinates so we always take the first set (0).
            vec.x = mesh->mTextureCoords[0][i].x;
            vec.y = mesh->mTextureCoords[0][i].y;
            vertex.texCoord = vec;

        } else {
            vertex.texCoord = glm::vec2(0.0f, 0.0f);
        }

        if (mesh->HasTangentsAndBitangents()) {
            // tangent
            vector.x = mesh->mTangents[i].x;
            vector.y = mesh->mTangents[i].y;
            vector.z = mesh->mTangents[i].z;
            vertex.tangent = vector;
            // bitangent
            vector.x = mesh->mBitangents[i].x;
            vector.y = mesh->mBitangents[i].y;
            vector.z = mesh->mBitangents[i].z;
            vertex.bitangent = vector;
        } else {
            vertex.tangent = {0,0,0};
            vertex.bitangent = {0,0,0};
        }

        vertices.push_back(vertex);
    }

    // now walk through each of the mesh's faces (a face is a mesh its triangle) and retrieve the corresponding vertex indices.
    for (unsigned int i = 0; i < mesh->mNumFaces; ++i) {
        aiFace face = mesh->mFaces[i];
        // retrieve all indices of the face and store them in the indices vector
        for(unsigned int j = 0; j < face.mNumIndices; ++j) {
            indices.push_back(face.mIndices[j]);
        }
    }

    // TODO material/texture logic

    return Mesh(graphicsAPI, vertices, indices);
}

Mesh::Mesh(IGraphicsAPI& graphicsAPI, const std::vector<Vertex>& vertices, const std::vector<unsigned int>& indices) 
    : mGraphicsAPI_(graphicsAPI) {
    this->vertices = vertices;
    this->indices = indices;
    buildOpenGLproperties();
}

Mesh::~Mesh() {}

void Mesh::render(const ShaderProgram& theShader) const {
    mGraphicsAPI_.bindVertexArray(mVAO);
    mGraphicsAPI_.drawElements(IGraphicsAPI::PrimitiveTopology::TRIANGLE_LIST, static_cast<unsigned int>(indices.size()), IGraphicsAPI::DataType::UINT, 0);
    mGraphicsAPI_.bindVertexArray(0);
}

void Mesh::buildOpenGLproperties() {
    // create buffers/arrays
    mGraphicsAPI_.genVertexArrays(1, &mVAO);
    mGraphicsAPI_.genBuffer(1, &mVBO_);
    mGraphicsAPI_.genBuffer(1, &mEBO_);

    mGraphicsAPI_.bindVertexArray(mVAO);
    // load data into vertex buffers
    mGraphicsAPI_.bindBuffer(IGraphicsAPI::BufferTarget::ARRAY_BUFFER, mVBO_);
    // bind vertices
    mGraphicsAPI_.bufferData(IGraphicsAPI::BufferTarget::ARRAY_BUFFER, vertices.size() * sizeof(Vertex), &vertices[0], IGraphicsAPI::DataUsage::STATIC_DRAW);

    mGraphicsAPI_.bindBuffer(IGraphicsAPI::BufferTarget::ELEMENT_ARRAY_BUFFER, mEBO_);
    mGraphicsAPI_.bufferData(IGraphicsAPI::BufferTarget::ELEMENT_ARRAY_BUFFER, indices.size() * sizeof(unsigned int), &indices[0], IGraphicsAPI::DataUsage::STATIC_DRAW);

    // set the vertex attribute pointers
    // vertex Positions
    mGraphicsAPI_.enableVertexAttribArray(0);
    mGraphicsAPI_.vertexAttribPointer(0, 3, IGraphicsAPI::DataType::FLOAT, false, sizeof(Vertex), (void*)0);
    // vertex normals
    mGraphicsAPI_.enableVertexAttribArray(1);
    mGraphicsAPI_.vertexAttribPointer(1, 3, IGraphicsAPI::DataType::FLOAT, false, sizeof(Vertex), (void*)offsetof(Vertex, normal));
    // vertex texture coords
    mGraphicsAPI_.enableVertexAttribArray(2);
    mGraphicsAPI_.vertexAttribPointer(2, 2, IGraphicsAPI::DataType::FLOAT, false, sizeof(Vertex), (void*)offsetof(Vertex, texCoord));
    // vertex tangent
    mGraphicsAPI_.enableVertexAttribArray(3);
    mGraphicsAPI_.vertexAttribPointer(3, 3, IGraphicsAPI::DataType::FLOAT, false, sizeof(Vertex), (void*)offsetof(Vertex, tangent));
    // vertex bitangent
    mGraphicsAPI_.enableVertexAttribArray(4);
    mGraphicsAPI_.vertexAttribPointer(4, 3, IGraphicsAPI::DataType::FLOAT, false, sizeof(Vertex), (void*)offsetof(Vertex, bitangent));

    mGraphicsAPI_.bindVertexArray(0);
}

} // namespace clay