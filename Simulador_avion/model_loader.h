// ============================================================
// ARCHIVO: model_loader.h
// DESCRIPCION: Cargador de modelos 3D usando Assimp + VAO/VBO
// ============================================================

#ifndef MODEL_LOADER_H
#define MODEL_LOADER_H

#include <assimp/Importer.hpp>
#include <assimp/scene.h>
#include <assimp/postprocess.h>
#include <vector>
#include <string>
#include <memory>
#include <map>
#include <glm/glm.hpp>

struct Vertex {
    glm::vec3 position;
    glm::vec3 normal;
    glm::vec3 color;
    glm::vec2 texCoords;
};

struct Mesh {
    std::vector<Vertex> vertices;
    std::vector<unsigned int> indices;

    unsigned int VAO, VBO, EBO;
    unsigned int textureID;
    bool hasTexture;

    Mesh() : VAO(0), VBO(0), EBO(0), textureID(0), hasTexture(false) {}
    ~Mesh();

    void setupMesh();
    void draw();
};

class Model {
private:
    std::vector<Mesh> meshes;
    bool loaded;
    float scale;
    glm::vec3 center;
    std::string modelDirectory;

    void processNode(aiNode* node, const aiScene* scene, const glm::mat4& parentTransform);
    void processMesh(aiMesh* mesh, const aiScene* scene, const glm::mat4& nodeTransform);
    void normalizeModel();
    glm::vec3 extractColorFromMaterial(aiMaterial* material);
    glm::vec3 getMeshColorByIndex(int meshIndex);
    unsigned int loadTextureFromMaterial(aiMaterial* material, const aiScene* scene);

public:
    Model();
    ~Model();

    bool loadModel(const char* path);
    void draw(class Shader* shader = nullptr);
    bool isLoaded() const { return loaded; }
    float getRecommendedZoom() const;
    glm::vec3 getCenter() const { return center; }
    float getScale() const { return scale; }
};

#endif // MODEL_LOADER_H
