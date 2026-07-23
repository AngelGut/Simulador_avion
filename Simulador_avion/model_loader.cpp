#include "model_loader.h"
#include <iostream>
#include <fstream>
#include <sstream>
#include <algorithm>

// Assimp headers
#include <assimp/Importer.hpp>
#include <assimp/scene.h>
#include <assimp/postprocess.h>

// ============================================================
// AssimpLoader - Implementación usando Assimp
// ============================================================

AssimpLoader::AssimpLoader() {
    supportedExtensions = {
        ".obj", ".fbx", ".blend", ".dae", ".stl",
        ".3ds", ".ase", ".gltf", ".glb", ".ply",
        ".lwo", ".lws", ".x", ".md5", ".md3"
    };
}

bool AssimpLoader::isFormatSupported(const std::string& filePath) const {
    size_t pos = filePath.rfind('.');
    if (pos == std::string::npos) return false;

    std::string ext = filePath.substr(pos);
    std::transform(ext.begin(), ext.end(), ext.begin(), ::tolower);

    return std::find(supportedExtensions.begin(), supportedExtensions.end(), ext)
        != supportedExtensions.end();
}

bool AssimpLoader::load(const std::string& filePath, Model& model) {
    std::cout << "[AssimpLoader] Cargando: " << filePath << std::endl;

    if (!isFormatSupported(filePath)) {
        std::cerr << "[AssimpLoader] Formato no soportado: " << filePath << std::endl;
        return false;
    }

    return parseWithAssimp(filePath, model);
}

bool AssimpLoader::parseWithAssimp(const std::string& filePath, Model& model) {
    Assimp::Importer importer;

    // Flags de post-procesamiento
    unsigned int flags = aiProcess_Triangulate           // Triangular todas las caras
                       | aiProcess_GenNormals            // Generar normales si no existen
                       | aiProcess_FixInfacingNormals    // Arreglar normales invertidas
                       | aiProcess_JoinIdenticalVertices // Juntar vértices duplicados
                       | aiProcess_OptimizeMeshes;       // Optimizar mallas

    const aiScene* scene = importer.ReadFile(filePath.c_str(), flags);

    if (!scene || scene->mFlags & AI_SCENE_FLAGS_INCOMPLETE || !scene->mRootNode) {
        std::cerr << "[AssimpLoader] Error: " << importer.GetErrorString() << std::endl;
        return false;
    }

    if (scene->mNumMeshes == 0) {
        std::cerr << "[AssimpLoader] No meshes found in file" << std::endl;
        return false;
    }

    // Cargar todas las mallas
    for (unsigned int m = 0; m < scene->mNumMeshes; ++m) {
        aiMesh* mesh = scene->mMeshes[m];

        // Vértices
        for (unsigned int i = 0; i < mesh->mNumVertices; ++i) {
            model.vertices.push_back(glm::vec3(
                mesh->mVertices[i].x,
                mesh->mVertices[i].y,
                mesh->mVertices[i].z
            ));
        }

        // Normales
        if (mesh->HasNormals()) {
            for (unsigned int i = 0; i < mesh->mNumVertices; ++i) {
                model.normals.push_back(glm::vec3(
                    mesh->mNormals[i].x,
                    mesh->mNormals[i].y,
                    mesh->mNormals[i].z
                ));
            }
        }

        // Coordenadas de textura (si existen)
        if (mesh->HasTextureCoords(0)) {
            for (unsigned int i = 0; i < mesh->mNumVertices; ++i) {
                model.texCoords.push_back(glm::vec2(
                    mesh->mTextureCoords[0][i].x,
                    mesh->mTextureCoords[0][i].y
                ));
            }
        }

        // Índices (caras)
        for (unsigned int i = 0; i < mesh->mNumFaces; ++i) {
            aiFace& face = mesh->mFaces[i];
            for (unsigned int j = 0; j < face.mNumIndices; ++j) {
                model.indices.push_back(face.mIndices[j]);
            }
        }
    }

    if (model.vertices.empty()) {
        std::cerr << "[AssimpLoader] No vertices loaded" << std::endl;
        return false;
    }

    std::cout << "[AssimpLoader] Carga exitosa: "
              << model.vertices.size() << " vértices, "
              << model.indices.size() / 3 << " triángulos" << std::endl;

    return true;
}

// ============================================================
// ModelManager - Implementación
// ============================================================

bool ModelManager::loadModel(const std::string& filePath, const std::string& modelName) {
    auto loader = getLoaderForFormat(filePath);

    if (!loader) {
        std::cerr << "[ModelManager] No loader available for: " << filePath << std::endl;
        return false;
    }

    Model model;
    model.name = modelName;

    if (!loader->load(filePath, model)) {
        std::cerr << "[ModelManager] Error loading: " << filePath << std::endl;
        return false;
    }

    models[modelName] = model;
    std::cout << "[ModelManager] Model '" << modelName << "' loaded successfully" << std::endl;
    return true;
}

Model* ModelManager::getModel(const std::string& modelName) {
    auto it = models.find(modelName);
    if (it != models.end()) {
        return &it->second;
    }
    return nullptr;
}

std::vector<std::string> ModelManager::getLoadedModels() const {
    std::vector<std::string> names;
    for (const auto& pair : models) {
        names.push_back(pair.first);
    }
    return names;
}

void ModelManager::unloadModel(const std::string& modelName) {
    models.erase(modelName);
}

void ModelManager::clearAll() {
    models.clear();
}

int ModelManager::getModelVertexCount(const std::string& modelName) const {
    auto it = models.find(modelName);
    if (it != models.end()) {
        return it->second.vertices.size();
    }
    return 0;
}

int ModelManager::getModelTriangleCount(const std::string& modelName) const {
    auto it = models.find(modelName);
    if (it != models.end()) {
        return it->second.indices.size() / 3;
    }
    return 0;
}

std::unique_ptr<ModelLoader> ModelManager::getLoaderForFormat(const std::string& filePath) {
    // Usar AssimpLoader para todo (soporta todos los formatos)
    auto loader = std::make_unique<AssimpLoader>();

    if (loader->isFormatSupported(filePath)) {
        return loader;
    }

    return nullptr;
}
