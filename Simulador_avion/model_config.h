#pragma once
#ifndef MODEL_CONFIG_H
#define MODEL_CONFIG_H

#include <string>
#include <vector>

// ============================================================
// Configuración de rutas y modelos 3D
// ============================================================

namespace ModelConfig {

    // Ruta base donde se almacenan los modelos
    constexpr const char* MODELS_BASE_PATH = "../models/";

    // ============================================================
    // Estructura de metadatos de modelo
    // ============================================================
    struct ModelInfo {
        std::string name;           // Nombre único del modelo
        std::string filename;       // Nombre del archivo (relativo a MODELS_BASE_PATH)
        std::string format;         // Formato
        std::string description;    // Descripción
        std::string partsFolder;    // Carpeta de piezas desarmadas (relativo a MODELS_BASE_PATH)
    };

    // ============================================================
    // Configuración de los 4 modelos GLB
    // ============================================================
    const std::vector<ModelInfo> AVAILABLE_MODELS = {
        {
            "a-10_thunderbolt_ii",
            "a-10_thunderbolt_ii.glb",
            "GLB",
            "Avión de ataque a tierra A-10 Thunderbolt II",
            "Desarmados/a10"
        },
        {
            "b-24_liberator",
            "b-24_liberator.glb",
            "GLB",
            "Bombardero pesado B-24 Liberator",
            "Desarmados/b-24"
        },
        {
            "boeing-787-_dreamliner",
            "boeing-787-_dreamliner.glb",
            "GLB",
            "Avión comercial de pasajeros Boeing 787",
            "Desarmados/boing"
        },
        {
            "mig_29_9-13",
            "mig_29_9-13.glb",
            "GLB",
            "Caza de combate supersónico MiG-29",
            "Desarmados/mig"
        }
    };

    // ============================================================
    // Funciones de utilidad
    // ============================================================
    inline const ModelInfo* getModelInfo(const std::string& modelName) {
        for (const auto& model : AVAILABLE_MODELS) {
            if (model.name == modelName) {
                return &model;
            }
        }
        return nullptr;
    }

    inline std::string getFullPath(const std::string& filename) {
        return std::string(MODELS_BASE_PATH) + filename;
    }

} // namespace ModelConfig

#endif // MODEL_CONFIG_H