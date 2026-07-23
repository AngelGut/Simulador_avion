#pragma once
#ifndef MODEL_LOADER_H
#define MODEL_LOADER_H

#include <vector>
#include <string>
#include <memory>
#include <map>

// ============================================================
// Estructuras matemáticas básicas (sin dependencias externas)
// ============================================================
struct Vec2 {
    float x, y;
    Vec2(float x = 0.0f, float y = 0.0f) : x(x), y(y) {}
};

struct Vec3 {
    float x, y, z;
    Vec3(float x = 0.0f, float y = 0.0f, float z = 0.0f) : x(x), y(y), z(z) {}
};

// ============================================================
// Model - Estructura para almacenar datos de un modelo 3D
// ============================================================
struct Model {
    std::vector<Vec3> vertices;
    std::vector<Vec3> normals;
    std::vector<Vec2> texCoords;
    std::vector<unsigned int> indices;

    std::string name;
    Vec3 position;
    Vec3 scale;
    float rotation;  // Rotación en eje Y (grados)

    Model() : position(0.0f), scale(1.0f), rotation(0.0f) {}

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
// OBJLoader - Carga archivos .obj (Wavefront OBJ)
// ============================================================
class OBJLoader : public ModelLoader {
public:
    bool load(const std::string& filePath, Model& model) override;
    bool isFormatSupported(const std::string& filePath) const override;

private:
    bool parseOBJFile(const std::string& filePath, Model& model);
};

// ============================================================
// FBXLoader - Carga archivos .fbx (Autodesk FBX)
// Nota: Requiere librería FBX SDK de Autodesk o alternativa
// ============================================================
class FBXLoader : public ModelLoader {
public:
    bool load(const std::string& filePath, Model& model) override;
    bool isFormatSupported(const std::string& filePath) const override;

private:
    bool parseFBXFile(const std::string& filePath, Model& model);
};

// ============================================================
// BLENDLoader - Carga archivos .blend (Blender)
// Nota: Requiere parsing de formato binario de Blender
// ============================================================
class BLENDLoader : public ModelLoader {
public:
    bool load(const std::string& filePath, Model& model) override;
    bool isFormatSupported(const std::string& filePath) const override;

private:
    bool parseBLENDFile(const std::string& filePath, Model& model);
    bool extractMeshFromBlend(const std::string& filePath, Model& model);
};

// ============================================================
// RARLoader - Stub (descompresión deshabilitada por seguridad)
// ============================================================
class RARLoader : public ModelLoader {
public:
    bool load(const std::string& filePath, Model& model) override;
    bool isFormatSupported(const std::string& filePath) const override;
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
};

#endif // MODEL_LOADER_H
