#include "model_loader.h"
#include <iostream>
#include <fstream>
#include <sstream>
#include <cstring>
#include <cstdlib>
#include <algorithm>

// ============================================================
// OBJLoader - Implementación del loader de archivos .obj
// ============================================================

bool OBJLoader::isFormatSupported(const std::string& filePath) const {
    size_t pos = filePath.rfind('.');
    if (pos == std::string::npos) return false;
    std::string ext = filePath.substr(pos);
    std::transform(ext.begin(), ext.end(), ext.begin(), ::tolower);
    return ext == ".obj";
}

bool OBJLoader::load(const std::string& filePath, Model& model) {
    std::cout << "[OBJLoader] Cargando: " << filePath << std::endl;

    if (!isFormatSupported(filePath)) {
        std::cerr << "[OBJLoader] Formato no soportado" << std::endl;
        return false;
    }

    return parseOBJFile(filePath, model);
}

bool OBJLoader::parseOBJFile(const std::string& filePath, Model& model) {
    std::ifstream file(filePath);
    if (!file.is_open()) {
        std::cerr << "[OBJLoader] No se pudo abrir: " << filePath << std::endl;
        return false;
    }

    std::vector<Vec3> tempVertices;
    std::vector<Vec3> tempNormals;
    std::vector<Vec2> tempTexCoords;
    std::string line;
    int lineNum = 0;

    while (std::getline(file, line)) {
        lineNum++;
        if (line.empty() || line[0] == '#') continue;

        std::istringstream iss(line);
        std::string prefix;
        iss >> prefix;

        if (prefix == "v") {  // Vertex
            float x, y, z;
            if (iss >> x >> y >> z) {
                tempVertices.push_back(Vec3(x, y, z));
            }
        }
        else if (prefix == "vn") {  // Normal
            float x, y, z;
            if (iss >> x >> y >> z) {
                tempNormals.push_back(Vec3(x, y, z));
            }
        }
        else if (prefix == "vt") {  // Texture coordinate
            float u, v;
            if (iss >> u >> v) {
                tempTexCoords.push_back(Vec2(u, v));
            }
        }
        else if (prefix == "f") {  // Face
            std::string face;
            std::vector<unsigned int> faceIndices;

            while (iss >> face) {
                // Parsear formato: v, v/vt, v/vt/vn, o v//vn
                unsigned int vIdx = 0, vtIdx = 0, vnIdx = 0;
                size_t slash1 = face.find('/');

                if (slash1 == std::string::npos) {
                    // Solo índice de vértice
                    vIdx = std::stoul(face);
                } else {
                    vIdx = std::stoul(face.substr(0, slash1));
                    size_t slash2 = face.find('/', slash1 + 1);

                    if (slash2 == std::string::npos) {
                        // v/vt
                        if (slash1 + 1 < face.length()) {
                            vtIdx = std::stoul(face.substr(slash1 + 1));
                        }
                    } else {
                        // v/vt/vn o v//vn
                        if (slash1 + 1 < slash2) {
                            vtIdx = std::stoul(face.substr(slash1 + 1, slash2 - slash1 - 1));
                        }
                        if (slash2 + 1 < face.length()) {
                            vnIdx = std::stoul(face.substr(slash2 + 1));
                        }
                    }
                }

                // OBJ usa indexación 1-based
                if (vIdx > 0 && vIdx <= tempVertices.size()) {
                    model.vertices.push_back(tempVertices[vIdx - 1]);
                    model.indices.push_back(model.vertices.size() - 1);

                    if (vtIdx > 0 && vtIdx <= tempTexCoords.size()) {
                        model.texCoords.push_back(tempTexCoords[vtIdx - 1]);
                    }
                    if (vnIdx > 0 && vnIdx <= tempNormals.size()) {
                        model.normals.push_back(tempNormals[vnIdx - 1]);
                    }
                }
            }
        }
    }

    file.close();

    if (model.vertices.empty()) {
        std::cerr << "[OBJLoader] No se encontraron vértices en el archivo" << std::endl;
        return false;
    }

    std::cout << "[OBJLoader] Carga exitosa: " << model.vertices.size()
              << " vértices, " << model.indices.size() << " índices" << std::endl;
    return true;
}

// ============================================================
// FBXLoader - Stub (FBX es un formato binario complejo)
// ============================================================

bool FBXLoader::isFormatSupported(const std::string& filePath) const {
    size_t pos = filePath.rfind('.');
    if (pos == std::string::npos) return false;
    std::string ext = filePath.substr(pos);
    std::transform(ext.begin(), ext.end(), ext.begin(), ::tolower);
    return ext == ".fbx";
}

bool FBXLoader::load(const std::string& filePath, Model& model) {
    std::cout << "[FBXLoader] Intentando cargar: " << filePath << std::endl;
    std::cerr << "[FBXLoader] ADVERTENCIA: FBX requiere librería FBX SDK de Autodesk" << std::endl;
    std::cerr << "[FBXLoader] Por ahora, use herramientas externas para convertir a OBJ" << std::endl;
    return false;  // TODO: Implementar cuando FBX SDK esté disponible
}

bool FBXLoader::parseFBXFile(const std::string& filePath, Model& model) {
    // FBX es binario y requiere parsing complejo
    // Seria necesario usar:
    // - FBX SDK oficial (Autodesk) - licencia propietaria
    // - assimp - librería open source para importar modelos
    // - Conversión previa a OBJ
    return false;
}

// ============================================================
// BLENDLoader - Stub (BLEND es formato binario de Blender)
// ============================================================

bool BLENDLoader::isFormatSupported(const std::string& filePath) const {
    size_t pos = filePath.rfind('.');
    if (pos == std::string::npos) return false;
    std::string ext = filePath.substr(pos);
    std::transform(ext.begin(), ext.end(), ext.begin(), ::tolower);
    return ext == ".blend";
}

bool BLENDLoader::load(const std::string& filePath, Model& model) {
    std::cout << "[BLENDLoader] Intentando cargar: " << filePath << std::endl;
    std::cerr << "[BLENDLoader] ADVERTENCIA: BLEND requiere Blender Python API o conversión" << std::endl;
    std::cerr << "[BLENDLoader] Opción: blender -b archivo.blend -P script.py --render-output archivo.obj" << std::endl;
    return false;  // TODO: Implementar con Blender API
}

bool BLENDLoader::parseBLENDFile(const std::string& filePath, Model& model) {
    return false;
}

bool BLENDLoader::extractMeshFromBlend(const std::string& filePath, Model& model) {
    return false;
}

// ============================================================
// RARLoader - Descomprime y carga modelos dentro de RAR
// ============================================================

bool RARLoader::isFormatSupported(const std::string& filePath) const {
    size_t pos = filePath.rfind('.');
    if (pos == std::string::npos) return false;
    std::string ext = filePath.substr(pos);
    std::transform(ext.begin(), ext.end(), ext.begin(), ::tolower);
    return ext == ".rar";
}

bool RARLoader::load(const std::string& filePath, Model& model) {
    std::cout << "[RARLoader] Intentando cargar: " << filePath << std::endl;

    if (!isFormatSupported(filePath)) {
        std::cerr << "[RARLoader] No es un archivo RAR" << std::endl;
        return false;
    }

    std::string tempDir = "/tmp/rar_extract_" + std::to_string(time(nullptr));
    std::string foundModelPath;

    if (!findSupportedModelInRAR(filePath, foundModelPath)) {
        std::cerr << "[RARLoader] No se encontró modelo soportado en el RAR" << std::endl;
        return false;
    }

    // Extraer el RAR
    std::string extractCmd = "mkdir -p " + tempDir + " && unrar x \"" + filePath + "\" \"" + tempDir + "\" > /dev/null 2>&1";
    int result = system(extractCmd.c_str());

    if (result != 0) {
        std::cerr << "[RARLoader] Error al descomprimir (¿unrar instalado?)" << std::endl;
        return false;
    }

    // Buscar archivo de modelo extraído
    std::string modelPath = tempDir + "/" + foundModelPath;
    ModelManager& mgr = ModelManager::getInstance();
    auto loader = mgr.getLoaderForFormat(modelPath);

    if (!loader || !loader->load(modelPath, model)) {
        std::cerr << "[RARLoader] Error al cargar modelo extraído" << std::endl;
        system(("rm -rf " + tempDir).c_str());
        return false;
    }

    // Limpiar directorio temporal
    system(("rm -rf " + tempDir).c_str());

    std::cout << "[RARLoader] Carga exitosa desde RAR" << std::endl;
    return true;
}

bool RARLoader::findSupportedModelInRAR(const std::string& rarFilePath, std::string& foundModelPath) {
    // Listar contenido del RAR y buscar archivos .obj, .fbx, .blend
    std::string listCmd = "unrar l \"" + rarFilePath + "\" 2>/dev/null";
    FILE* pipe = popen(listCmd.c_str(), "r");

    if (!pipe) {
        std::cerr << "[RARLoader] No se pudo listar contenido del RAR" << std::endl;
        return false;
    }

    char buffer[256];
    std::vector<std::string> supportedExtensions = {".obj", ".fbx", ".blend"};
    bool found = false;

    while (fgets(buffer, sizeof(buffer), pipe)) {
        std::string line(buffer);
        std::transform(line.begin(), line.end(), line.begin(), ::tolower);

        for (const auto& ext : supportedExtensions) {
            if (line.find(ext) != std::string::npos) {
                // Extraer nombre del archivo (formato unrar: "nombre archivo")
                size_t lastSpace = line.rfind(' ');
                if (lastSpace != std::string::npos) {
                    foundModelPath = line.substr(lastSpace + 1);
                    // Limpiar espacios y newlines
                    foundModelPath.erase(0, foundModelPath.find_first_not_of(" \n\r\t"));
                    foundModelPath.erase(foundModelPath.find_last_not_of(" \n\r\t") + 1);
                    found = true;
                    break;
                }
            }
        }
        if (found) break;
    }

    pclose(pipe);
    return found;
}

// ============================================================
// ModelManager - Gestor central de modelos
// ============================================================

bool ModelManager::loadModel(const std::string& filePath, const std::string& modelName) {
    auto loader = getLoaderForFormat(filePath);

    if (!loader) {
        std::cerr << "[ModelManager] No hay loader disponible para: " << filePath << std::endl;
        return false;
    }

    Model model;
    model.name = modelName;

    if (!loader->load(filePath, model)) {
        std::cerr << "[ModelManager] Error al cargar modelo: " << filePath << std::endl;
        return false;
    }

    models[modelName] = model;
    std::cout << "[ModelManager] Modelo '" << modelName << "' cargado exitosamente" << std::endl;
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
    std::string lower = filePath;
    std::transform(lower.begin(), lower.end(), lower.begin(), ::tolower);

    if (lower.find(".obj") != std::string::npos) {
        return std::make_unique<OBJLoader>();
    }
    else if (lower.find(".fbx") != std::string::npos) {
        return std::make_unique<FBXLoader>();
    }
    else if (lower.find(".blend") != std::string::npos) {
        return std::make_unique<BLENDLoader>();
    }
    else if (lower.find(".rar") != std::string::npos) {
        return std::make_unique<RARLoader>();
    }

    return nullptr;
}
