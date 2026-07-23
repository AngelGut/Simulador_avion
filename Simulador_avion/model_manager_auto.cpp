#include "model_manager_auto.h"
#include <iostream>
#include <algorithm>
#include <iomanip>

int ModelManagerAuto::loadAllModelsFromFolder(const std::string& folderPath) {
    std::cout << "\n" << std::string(60, '=') << std::endl;
    std::cout << "  Cargando modelos desde: " << folderPath << std::endl;
    std::cout << std::string(60, '=') << std::endl << std::endl;

    if (!fs::exists(folderPath)) {
        std::cerr << "❌ Error: Carpeta no existe: " << folderPath << std::endl;
        return 0;
    }

    if (!fs::is_directory(folderPath)) {
        std::cerr << "❌ Error: No es una carpeta: " << folderPath << std::endl;
        return 0;
    }

    int loadedCount = 0;
    std::vector<std::string> supportedExtensions = {".obj", ".fbx", ".blend", ".rar"};

    try {
        for (const auto& entry : fs::directory_iterator(folderPath)) {
            if (!entry.is_regular_file()) continue;

            std::string extension = entry.path().extension().string();
            std::transform(extension.begin(), extension.end(), extension.begin(), ::tolower);

            // Verificar si es un formato soportado
            bool isSupported = std::find(
                supportedExtensions.begin(),
                supportedExtensions.end(),
                extension
            ) != supportedExtensions.end();

            if (!isSupported) continue;

            // Extraer nombre del modelo
            std::string modelName = extractModelName(entry.path());
            std::string fullPath = entry.path().string();

            // Intentar cargar
            std::cout << "🔄 Cargando: " << entry.path().filename().string() << " ... ";

            if (modelMgr.loadModel(fullPath, modelName)) {
                Model* model = modelMgr.getModel(modelName);
                if (model) {
                    std::cout << "✅ ("
                              << model->vertices.size() << " vértices, "
                              << model->indices.size() / 3 << " triángulos)"
                              << std::endl;
                    loadedCount++;
                } else {
                    std::cout << "⚠️ Cargado pero no accesible" << std::endl;
                }
            } else {
                std::cout << "❌ Error" << std::endl;
            }
        }
    }
    catch (const std::exception& e) {
        std::cerr << "❌ Exception: " << e.what() << std::endl;
    }

    std::cout << std::endl << std::string(60, '=') << std::endl;
    std::cout << "  Resumen: " << loadedCount << " modelo(s) cargado(s)" << std::endl;
    std::cout << std::string(60, '=') << std::endl << std::endl;

    return loadedCount;
}

std::vector<std::string> ModelManagerAuto::getAvailableModels() const {
    return modelMgr.getLoadedModels();
}

Model* ModelManagerAuto::getModel(const std::string& modelName) {
    return modelMgr.getModel(modelName);
}

int ModelManagerAuto::getTotalModelsLoaded() const {
    return modelMgr.getLoadedModels().size();
}

void ModelManagerAuto::printModelsInfo() const {
    auto models = modelMgr.getLoadedModels();

    if (models.empty()) {
        std::cout << "📭 Sin modelos cargados" << std::endl;
        return;
    }

    std::cout << "\n" << std::string(70, '=') << std::endl;
    std::cout << "  MODELOS DISPONIBLES" << std::endl;
    std::cout << std::string(70, '=') << std::endl;

    std::cout << std::left
              << std::setw(25) << "Nombre"
              << std::setw(15) << "Vértices"
              << std::setw(15) << "Triángulos"
              << std::endl;
    std::cout << std::string(70, '-') << std::endl;

    for (const auto& name : models) {
        Model* model = modelMgr.getModel(name);
        if (model) {
            std::cout << std::left
                      << std::setw(25) << name
                      << std::setw(15) << model->vertices.size()
                      << std::setw(15) << (model->indices.size() / 3)
                      << std::endl;
        }
    }

    std::cout << std::string(70, '=') << std::endl << std::endl;
}

void ModelManagerAuto::clearAll() {
    modelMgr.clearAll();
}

bool ModelManagerAuto::isModelFile(const fs::path& filePath) const {
    std::string ext = filePath.extension().string();
    std::transform(ext.begin(), ext.end(), ext.begin(), ::tolower);
    return ext == ".obj" || ext == ".fbx" || ext == ".blend";
}

std::string ModelManagerAuto::extractModelName(const fs::path& filePath) const {
    std::string filename = filePath.filename().string();
    // Remover extensión
    size_t dotPos = filename.rfind('.');
    if (dotPos != std::string::npos) {
        return filename.substr(0, dotPos);
    }
    return filename;
}
