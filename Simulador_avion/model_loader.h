// ============================================================
// ARCHIVO: model_loader.h
// DESCRIPCION: Cargador de modelos 3D usando Assimp
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
};

struct Mesh {
    std::vector<Vertex> vertices;
    std::vector<unsigned int> indices;

    void draw();
};

class Model {
private:
    std::vector<Mesh> meshes;
    bool loaded;
    float scale;
    glm::vec3 center;

    void processNode(aiNode* node, const aiScene* scene, const glm::mat4& parentTransform);
    void processMesh(aiMesh* mesh, const aiScene* scene, const glm::mat4& nodeTransform);
    void normalizeModel();

public:
    Model();
    ~Model();

    bool loadModel(const char* path);
    void draw();
    bool isLoaded() const { return loaded; }
    float getRecommendedZoom() const;
    glm::vec3 getCenter() const { return center; }
    float getScale() const { return scale; }
};

#endif // MODEL_LOADER_H
