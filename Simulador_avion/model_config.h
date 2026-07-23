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
    constexpr const char* MODELS_BASE_PATH = "./models/";

    // ============================================================
    // Estructura de metadatos de modelo
    // ============================================================
    struct ModelInfo {
        std::string name;           // Nombre único del modelo
        std::string filename;       // Nombre del archivo (relativo a MODELS_BASE_PATH)
        std::string format;         // Formato (OBJ, FBX, BLEND, RAR)
        std::string description;    // Descripción
        bool requiresConversion;    // ¿Necesita conversión previa?
        std::string conversionHint; // Hint sobre cómo convertir
    };

    // ============================================================
    // Configuración de los 5 modelos (Assimp soporta todos)
    // ============================================================
    const std::vector<ModelInfo> AVAILABLE_MODELS = {
        {
            "boeing_737_800",
            "American Airlines Boeing 737-800.blend",
            "BLEND",
            "Boeing 737-800 de American Airlines",
            false,
            "Assimp carga directamente"
        },
        {
            "kawasaki_ki61",
            "kawasaki-ki-61.blend",
            "BLEND",
            "Cazabombardero Kawasaki Ki-61",
            false,
            "Assimp carga directamente"
        },
        {
            "ms406",
            "uploads_files_5625074_MS-406.rar",
            "RAR",
            "Avión MS-406 (descomprimir primero)",
            true,
            "Descomprimir manualmente, luego copiar archivo a models/"
        },
        {
            "mystere_iv_provence",
            "MYSTERE IV N 117 Provence Nancy.fbx",
            "FBX",
            "MYSTERE IV N°117 de Provenza",
            false,
            "Assimp carga directamente"
        },
        {
            "millennium_falcon",
            "Millennium Falcon star wars.obj",
            "OBJ",
            "Nave Millennium Falcon",
            false,
            "Assimp carga directamente"
        }
    };

    // ============================================================
    // Funciones de utilidad
    // ============================================================
    inline ModelInfo* getModelInfo(const std::string& modelName) {
        for (auto& model : AVAILABLE_MODELS) {
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
