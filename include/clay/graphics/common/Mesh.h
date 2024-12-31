#pragma once
// standard lib
#include <vector>
#include <filesystem>
// third party
#include <assimp/Importer.hpp>
#include <assimp/scene.h>
#include <assimp/postprocess.h>
#include <glm/vec2.hpp>
#include <glm/vec3.hpp>
#include <clay/graphics/common/IGraphicsAPI.h>
// project
#include "clay/graphics/common/ShaderProgram.h"

namespace clay {

class Mesh {
public:
    /**
     * Reads an obj file and populates the given list with the meshes
     *
     * @param path obj file path
     * @param meshList
     */
    static void loadMeshes(IGraphicsAPI& graphicsAPI, const std::filesystem::path& path, std::vector<Mesh>& meshList);

    /** Mesh Vertex info*/
    struct Vertex {
        glm::vec3 position;
        glm::vec3 normal;
        glm::vec2 texCoord;
        glm::vec3 tangent;
        glm::vec3 bitangent;
    };
    /** Mesh Texture info*/
    struct Texture {
        unsigned int id;
        std::string type;
        std::string path;
    };

    /** Vertices of this mesh */
    std::vector<Vertex> vertices;
    /** Order to render the vertices in */
    std::vector<unsigned int> indices;
    /** Textures TODO use this*/
    std::vector<Texture> textures;
    /** Vertex Array Object for this mesh */
    unsigned int mVAO;

    /**
     * Constructor
     * @param vertices Vertices for this Mesh
     * @param indices Render order of the vertices
     */
    Mesh(IGraphicsAPI& graphicsAPI, const std::vector<Vertex>& vertices, const std::vector<unsigned int>& indices);

    /**
     * @brief Destructor
     */
    ~Mesh();

    /**
     * Render the mesh with the given shader. TODO use textures
     * @param theShader Shader that will be used to draw. Textures will be bound to the shader before rendering
     */
    void render(const ShaderProgram& theShader) const;

private:
    /**
     * Process a node (and child nodes recursively) in a assimp object and add to
     * @param aiNode Assimp node
     * @param aiScene Assimp scene
     */
    static void processNode(IGraphicsAPI& graphicsAPI, aiNode *node, const aiScene *scene, std::vector<Mesh>& meshList);

    /**
     * Process an Assimp Mesh and add to list of meshes
     * @param mesh Child node
     * @param scene Assimp Scene
     */
    static Mesh processMesh(IGraphicsAPI& graphicsAPI, aiMesh *mesh, const aiScene *scene);

    /** Initializes the OpenGl properties for this mesh*/
    void buildOpenGLproperties();

    /** Vertex Buffer Object for this mesh*/
    unsigned int mVBO_;
    /** Element Buffer Object for this Mesh */
    unsigned int mEBO_;

    IGraphicsAPI& mGraphicsAPI_;
};

} // namespace clay