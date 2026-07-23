#pragma once
#ifndef MODEL_LOADER_H
#define MODEL_LOADER_H

#include <vector>
#include <string>
#include <memory>
#include <map>
#include <glm/glm.hpp>

// ============================================================
// Model - Estructura para almacenar datos de un modelo 3D
// ============================================================
struct Model {
    std::vector<glm::vec3> vertices;
    std::vector<glm::vec3> normals;
    std::vector<glm::vec2> texCoords;
    std::vector<unsigned int> indices;

    std::string name;
    glm::vec3 position;
    glm::vec3 scale;
    float rotation;  // Rotación en eje Y (grados)

    Model() : position(0.0f, 0.0f, 0.0f), scale(1.0f, 1.0f, 1.0f), rotation(0.0f) {}

    bool isEmpty() const {
        return vertices.empty();
    }

    void clear() {
        vertices.clear();
        normals.clear();
        texCoords.clear();
        indices.clear();
    }
};

// ============================================================
// ModelLoader - Clase base para loaders de modelos
// ============================================================
class ModelLoader {
public:
    virtual ~ModelLoader() = default;

    virtual bool load(const std::string& filePath, Model& model) = 0;
    virtual bool isFormatSupported(const std::string& filePath) const = 0;
};

// ============================================================
// AssimpLoader - Carga cualquier formato soportado por Assimp
// Soporta: OBJ, FBX, BLEND, DAE, STL, 3DS, y más
// ============================================================
class AssimpLoader : public ModelLoader {
public:
    AssimpLoader();
    bool load(const std::string& filePath, Model& model) override;
    bool isFormatSupported(const std::string& filePath) const override;

private:
    bool parseWithAssimp(const std::string& filePath, Model& model);
    std::vector<std::string> supportedExtensions;
};

// ============================================================
// ModelManager - Gestor central de modelos 3D
// ============================================================
class ModelManager {
public:
    static ModelManager& getInstance() {
        static ModelManager instance;
        return instance;
    }

    // Cargar un modelo desde archivo
    bool loadModel(const std::string& filePath, const std::string& modelName);

    // Obtener un modelo por nombre
    Model* getModel(const std::string& modelName);

    // Listar todos los modelos cargados
    std::vector<std::string> getLoadedModels() const;

    // Limpiar un modelo
    void unloadModel(const std::string& modelName);

    // Limpiar todos los modelos
    void clearAll();

    // Obtener estadísticas de un modelo
    int getModelVertexCount(const std::string& modelName) const;
    int getModelTriangleCount(const std::string& modelName) const;

private:
    ModelManager() = default;
    ~ModelManager() = default;

    ModelManager(const ModelManager&) = delete;
    ModelManager& operator=(const ModelManager&) = delete;

    std::map<std::string, Model> models;
    std::unique_ptr<ModelLoader> getLoaderForFormat(const std::string& filePath);
    AssimpLoader assimpLoader;  // Único loader necesario
};

#endif // MODEL_LOADER_H
