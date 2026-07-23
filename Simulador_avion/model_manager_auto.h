#pragma once
#ifndef MODEL_MANAGER_AUTO_H
#define MODEL_MANAGER_AUTO_H

#include "model_loader.h"
#include <string>
#include <vector>
#include <filesystem>

namespace fs = std::filesystem;

// ============================================================
// ModelManagerAuto - Carga automática de modelos desde carpeta
// ============================================================
class ModelManagerAuto {
public:
    static ModelManagerAuto& getInstance() {
        static ModelManagerAuto instance;
        return instance;
    }

    // Cargar todos los modelos de la carpeta models/
    int loadAllModelsFromFolder(const std::string& folderPath = "./models/");

    // Obtener lista de modelos cargados
    std::vector<std::string> getAvailableModels() const;

    // Obtener modelo por nombre
    Model* getModel(const std::string& modelName);

    // Obtener estadísticas
    int getTotalModelsLoaded() const;
    void printModelsInfo() const;

    // Limpiar todos
    void clearAll();

private:
    ModelManagerAuto() = default;
    ~ModelManagerAuto() = default;

    ModelManagerAuto(const ModelManagerAuto&) = delete;
    ModelManagerAuto& operator=(const ModelManagerAuto&) = delete;

    ModelManager& modelMgr = ModelManager::getInstance();

    // Métodos auxiliares
    bool isModelFile(const fs::path& filePath) const;
    std::string extractModelName(const fs::path& filePath) const;
};

#endif // MODEL_MANAGER_AUTO_H
