// ============================================================
// ARCHIVO: renderer.cpp - CON CARGADOR DE MODELOS Y MODO PIEZAS
// RESPONSABLE: Ronald (Rendering)
// DESCRIPCION: Renderizado 3D con carga de modelos GLB y Modo Piezas
// ============================================================
#include <glad/glad.h>
#include "renderer.h"
#include "geometry.h"
#include "model_loader.h"
#include "config.h"
#include "model_config.h"
#include <cstdio>
#include <cmath>
#include <iostream>
#include <vector>
#include <filesystem>
#include <algorithm>

namespace Renderer {

    // Variable estática para almacenar el modelo cargado
    static Model* loadedModel = nullptr;
    static int currentModelNumber = 1;

    // Variables de Modo Piezas
    static bool partsModeActive = false;
    static std::vector<std::string> partFiles;
    static std::vector<Model*> partModels;
    static int currentPartIndex = 0;
    static bool partsLoaded = false;

    // Variables de la Escenografía (Hangar)
    static unsigned int floorVAO = 0;
    static unsigned int floorVBO = 0;
    static unsigned int floorEBO = 0;
    static int floorIndexCount = 0;

    static unsigned int gridVAO = 0;
    static unsigned int gridVBO = 0;
    static int gridVertexCount = 0;

    static unsigned int windVAO = 0;
    static unsigned int windVBO = 0;
    static int windVertexCount = 0;

    static float smoothstep(float edge0, float edge1, float x) {
        float t = glm::clamp((x - edge0) / (edge1 - edge0), 0.0f, 1.0f);
        return t * t * (3.0f - 2.0f * t);
    }

    // Helper para resolver rutas relativas
    std::string resolvePath(const std::string& path) {
        namespace fs = std::filesystem;
        if (fs::exists(path)) return path;
        
        if (path.rfind("../", 0) == 0) {
            std::string sub = path.substr(3);
            if (fs::exists(sub)) return sub;
            
            std::string sub2 = "../" + path;
            if (fs::exists(sub2)) return sub2;
        }
        return path;
    }

    // Cargar piezas individuales usando filesystem
    void loadParts() {
        namespace fs = std::filesystem;

        // Limpiar piezas anteriores
        for (auto model : partModels) {
            delete model;
        }
        partModels.clear();
        partFiles.clear();
        currentPartIndex = 0;
        partsLoaded = false;

        std::string currentModelName = "";
        switch (currentModelNumber) {
            case 1: currentModelName = "a-10_thunderbolt_ii"; break;
            case 2: currentModelName = "b-24_liberator"; break;
            case 3: currentModelName = "boeing-787-_dreamliner"; break;
            case 4: currentModelName = "mig_29_9-13"; break;
            default: return;
        }

        const auto* info = ModelConfig::getModelInfo(currentModelName);
        if (!info || info->partsFolder.empty()) return;

        std::string folderPath = std::string(ModelConfig::MODELS_BASE_PATH) + info->partsFolder;
        std::string resolvedFolder = resolvePath(folderPath);

        if (!fs::exists(resolvedFolder)) {
            std::cerr << "Carpeta de piezas no existe: " << resolvedFolder << std::endl;
            return;
        }

        // Leer todos los archivos .glb de la carpeta
        for (const auto& entry : fs::directory_iterator(resolvedFolder)) {
            if (entry.is_regular_file() && (entry.path().extension() == ".glb" || entry.path().extension() == ".GLB")) {
                partFiles.push_back(entry.path().string());
            }
        }

        std::sort(partFiles.begin(), partFiles.end());

        if (partFiles.empty()) {
            std::cout << "No se encontraron piezas en: " << resolvedFolder << std::endl;
            return;
        }

        std::cout << "Cargando " << partFiles.size() << " piezas de forma perezosa...\n";
        for (const auto& file : partFiles) {
            Model* part = new Model();
            std::cout << "  Cargando pieza: " << file << std::endl;
            if (part->loadModel(file.c_str())) {
                partModels.push_back(part);
            } else {
                std::cerr << "  Error al cargar pieza: " << file << std::endl;
                delete part;
            }
        }

        partsLoaded = true;
        std::cout << "✓ " << partModels.size() << " piezas cargadas correctamente.\n";
    }

    // ============================================================
    // setupOpenGL() - Configuración para 3D (compatible OpenGL 3.3+)
    // ============================================================
    void setupOpenGL() {
        glClearColor(0.05f, 0.05f, 0.1f, 1.0f);
        glEnable(GL_DEPTH_TEST);
        glDepthFunc(GL_LEQUAL);
        glEnable(GL_MULTISAMPLE);
    }

    // ============================================================
    // initModel() - Inicializar y cargar modelo
    // ============================================================
    void initModel(const char* modelPath) {
        if (loadedModel != nullptr) {
            delete loadedModel;
        }

        loadedModel = new Model();

        std::string resolved = resolvePath(modelPath);
        std::cout << "Cargando modelo completo: " << resolved << std::endl;
        if (loadedModel->loadModel(resolved.c_str())) {
            std::cout << "✓ Modelo cargado exitosamente\n";
        }
        else {
            std::cerr << "✗ Error al cargar modelo. Se usará geometría procedural.\n";
            delete loadedModel;
            loadedModel = nullptr;
        }
    }

    // ============================================================
    // getLoadedModel() - Obtener referencia al modelo cargado (o pieza activa)
    // ============================================================
    Model* getLoadedModel() {
        if (partsModeActive && partsLoaded && !partModels.empty()) {
            if (currentPartIndex >= 0 && currentPartIndex < partModels.size()) {
                return partModels[currentPartIndex];
            }
        }
        return loadedModel;
    }

    // ============================================================
    // applyLighting() - Configuración de iluminación
    // ============================================================
    void applyLighting() {
        // Obsoleto en OpenGL 3.3 Core Profile
    }

    // ============================================================
    // applyColor() - Aplicar color
    // ============================================================
    void applyColor(float r, float g, float b) {
        // Obsoleto en OpenGL 3.3 Core Profile
    }

    // ============================================================
    // drawGrid() - Grilla de referencia (vacía para esta versión)
    // ============================================================
    void drawGrid() {
        // Deshabilitado para versión 3D de modelos
    }

    // ============================================================
    // drawLayer() - Renderizar capa seleccionada
    // ============================================================
    void drawLayer(int layerNumber) {
        if (partsModeActive) {
            if (!partsLoaded) {
                loadParts();
            }

            if (partsLoaded && !partModels.empty()) {
                if (currentPartIndex >= 0 && currentPartIndex < partModels.size()) {
                    partModels[currentPartIndex]->draw();
                }
            } else {
                // Fallback al modelo completo si no hay piezas
                if (loadedModel != nullptr && loadedModel->isLoaded()) {
                    loadedModel->draw();
                }
            }
        } else {
            // Dibujar el modelo completo
            if (loadedModel != nullptr && loadedModel->isLoaded()) {
                loadedModel->draw();
            }
        }
    }

    // ============================================================
    // drawLayerLabel() - Etiqueta de capa (HUD)
    // ============================================================
    void drawLayerLabel(int layerNumber) {
        // Deshabilitado en versión 3D de modelos
    }

    // ============================================================
    // loadModelByNumber() - Cargar modelo por número (1-4)
    // ============================================================
    void loadModelByNumber(int modelNumber) {
        const char* modelPath = nullptr;
        const char* modelName = nullptr;

        switch (modelNumber) {
        case 1:
            modelPath = MODEL_1;
            modelName = NAME_1;
            break;
        case 2:
            modelPath = MODEL_2;
            modelName = NAME_2;
            break;
        case 3:
            modelPath = MODEL_3;
            modelName = NAME_3;
            break;
        case 4:
            modelPath = MODEL_4;
            modelName = NAME_4;
            break;
        default:
            std::cerr << "Modelo inválido. Opciones: 1-4\n";
            return;
        }

        currentModelNumber = modelNumber;
        std::cout << "\n--- Cargando: " << modelName << " ---\n";

        // Resetear variables de piezas
        partsModeActive = false;
        partsLoaded = false;
        for (auto model : partModels) {
            delete model;
        }
        partModels.clear();
        partFiles.clear();
        currentPartIndex = 0;

        initModel(modelPath);
    }

    // ============================================================
    // getCurrentModelNumber() - Obtener número del modelo actual
    // ============================================================
    int getCurrentModelNumber() {
        return currentModelNumber;
    }

    // ============================================================
    // MODO PIEZAS
    // ============================================================
    void togglePartsMode() {
        partsModeActive = !partsModeActive;
        std::cout << "Modo piezas: " << (partsModeActive ? "ACTIVADO" : "DESACTIVADO") << std::endl;

        if (partsModeActive && !partsLoaded) {
            loadParts();
        }
    }

    void nextPart() {
        if (!partsModeActive || partModels.empty()) return;
        currentPartIndex = (currentPartIndex + 1) % partModels.size();
    }

    void prevPart() {
        if (!partsModeActive || partModels.empty()) return;
        currentPartIndex = (currentPartIndex - 1 + partModels.size()) % partModels.size();
    }

    bool isPartsModeActive() {
        return partsModeActive;
    }

    std::string getCurrentPartName() {
        if (!partsModeActive || partFiles.empty() || currentPartIndex < 0 || currentPartIndex >= partFiles.size()) {
            return "";
        }
        namespace fs = std::filesystem;
        fs::path p(partFiles[currentPartIndex]);
        return p.stem().string();
    }

    int getCurrentPartIndex() {
        return currentPartIndex;
    }

    int getNumParts() {
        return partModels.size();
    }

    // ============================================================
    // printHelp() - Mostrar controles disponibles
    // ============================================================
    void printHelp() {
        std::cout << "\n"
            << "==================================================\n"
            << "    Visualizador 3D GLB - Simulador de Aviones\n"
            << "==================================================\n"
            << " SELECCIONAR AERONAVE:\n"
            << "   1-4        Cambiar de avión (A-10, B-24, B787, MiG-29)\n"
            << "\n"
            << " MODO PIEZAS:\n"
            << "   P          Activar / Desactivar Modo Piezas\n"
            << "   <- / ->    Navegar entre piezas (Flechas)\n"
            << "\n"
            << " ROTACIÓN (Pitch/Yaw/Roll):\n"
            << "   I / K      Rotar arriba / abajo (Pitch)\n"
            << "   J / L      Rotar izquierda / derecha (Yaw)\n"
            << "   R / T      Rotar CW / CCW (Roll)\n"
            << "\n"
            << " ZOOM (Cámara):\n"
            << "   Q / E      Alejar / Acercar\n"
            << "\n"
            << " PAN (Mover vista):\n"
            << "   W / A / S / D    Arriba / Izq / Abajo / Der\n"
            << "\n"
            << " OTROS:\n"
            << "   ESPACIO    Reset vista\n"
            << "   H          Mostrar/ocultar esta ayuda\n"
            << "   ESC        Salir\n"
            << "==================================================\n"
            << "\n";
    }

    // ============================================================
    // ESCENOGRAFÍA DEL HANGAR 3D (Plataforma Circular + Estructura)
    // ============================================================
    static void initFloor() {
        std::vector<Vertex> vertices;
        std::vector<unsigned int> indices;

        float size = 15.0f;
        float y = -0.6f;
        glm::vec3 normal(0.0f, 1.0f, 0.0f);
        glm::vec3 color(0.15f, 0.15f, 0.2f); // Gris azulado oscuro

        // Vértices del quad del suelo
        Vertex v1 = { glm::vec3(-size, y, -size), normal, color, glm::vec2(0.0f, 0.0f) };
        Vertex v2 = { glm::vec3(size, y, -size), normal, color, glm::vec2(1.0f, 0.0f) };
        Vertex v3 = { glm::vec3(size, y, size), normal, color, glm::vec2(1.0f, 1.0f) };
        Vertex v4 = { glm::vec3(-size, y, size), normal, color, glm::vec2(0.0f, 1.0f) };

        vertices.push_back(v1);
        vertices.push_back(v2);
        vertices.push_back(v3);
        vertices.push_back(v4);

        indices.push_back(0); indices.push_back(1); indices.push_back(2);
        indices.push_back(0); indices.push_back(2); indices.push_back(3);

        floorIndexCount = (int)indices.size();

        glGenVertexArrays(1, &floorVAO);
        glGenBuffers(1, &floorVBO);
        glGenBuffers(1, &floorEBO);

        glBindVertexArray(floorVAO);
        glBindBuffer(GL_ARRAY_BUFFER, floorVBO);
        glBufferData(GL_ARRAY_BUFFER, vertices.size() * sizeof(Vertex), &vertices[0], GL_STATIC_DRAW);

        glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, floorEBO);
        glBufferData(GL_ELEMENT_ARRAY_BUFFER, indices.size() * sizeof(unsigned int), &indices[0], GL_STATIC_DRAW);

        glEnableVertexAttribArray(0);
        glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, sizeof(Vertex), (void*)0);

        glEnableVertexAttribArray(1);
        glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, sizeof(Vertex), (void*)offsetof(Vertex, normal));

        glEnableVertexAttribArray(2);
        glVertexAttribPointer(2, 3, GL_FLOAT, GL_FALSE, sizeof(Vertex), (void*)offsetof(Vertex, color));

        glEnableVertexAttribArray(3);
        glVertexAttribPointer(3, 2, GL_FLOAT, GL_FALSE, sizeof(Vertex), (void*)offsetof(Vertex, texCoords));

        glBindBuffer(GL_ARRAY_BUFFER, 0);
        glBindVertexArray(0);
    }

    static void initGridLines() {
        std::vector<Vertex> vertices;
        float size = 15.0f;
        float y = -0.59f; // Ligeramente arriba para evitar z-fighting
        glm::vec3 normal(0.0f, 1.0f, 0.0f);
        glm::vec3 color(0.3f, 0.4f, 0.5f); // Líneas celestes oscuras

        int divisions = 30;
        float step = (size * 2.0f) / divisions;

        for (int i = 0; i <= divisions; i++) {
            float coord = -size + i * step;

            // Línea paralela al eje Z
            vertices.push_back({ glm::vec3(coord, y, -size), normal, color, glm::vec2(0.0f, 0.0f) });
            vertices.push_back({ glm::vec3(coord, y, size), normal, color, glm::vec2(0.0f, 0.0f) });

            // Línea paralela al eje X
            vertices.push_back({ glm::vec3(-size, y, coord), normal, color, glm::vec2(0.0f, 0.0f) });
            vertices.push_back({ glm::vec3(size, y, coord), normal, color, glm::vec2(0.0f, 0.0f) });
        }

        gridVertexCount = (int)vertices.size();

        glGenVertexArrays(1, &gridVAO);
        glGenBuffers(1, &gridVBO);

        glBindVertexArray(gridVAO);
        glBindBuffer(GL_ARRAY_BUFFER, gridVBO);
        glBufferData(GL_ARRAY_BUFFER, vertices.size() * sizeof(Vertex), &vertices[0], GL_STATIC_DRAW);

        glEnableVertexAttribArray(0);
        glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, sizeof(Vertex), (void*)0);

        glEnableVertexAttribArray(1);
        glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, sizeof(Vertex), (void*)offsetof(Vertex, normal));

        glEnableVertexAttribArray(2);
        glVertexAttribPointer(2, 3, GL_FLOAT, GL_FALSE, sizeof(Vertex), (void*)offsetof(Vertex, color));

        glEnableVertexAttribArray(3);
        glVertexAttribPointer(3, 2, GL_FLOAT, GL_FALSE, sizeof(Vertex), (void*)offsetof(Vertex, texCoords));

        glBindBuffer(GL_ARRAY_BUFFER, 0);
        glBindVertexArray(0);
    }

    void drawHangar(float cameraY) {
        float floorY = -0.6f;
        if (cameraY < floorY) {
            // Desvanecimiento inteligente: no dibujar el piso si la cámara está debajo de él
            return;
        }

        if (floorVAO == 0) {
            initFloor();
            initGridLines();
        }

        GLint currentProgram = 0;
        glGetIntegerv(GL_CURRENT_PROGRAM, &currentProgram);
        GLint useTexLoc = glGetUniformLocation(currentProgram, "uUseTexture");
        GLint modelLoc = glGetUniformLocation(currentProgram, "uModel");

        // El piso y las líneas no usan textura
        if (useTexLoc != -1) glUniform1i(useTexLoc, 0);

        // Ajustar matriz de modelo a identidad
        if (modelLoc != -1) {
            glm::mat4 identity = glm::mat4(1.0f);
            glUniformMatrix4fv(modelLoc, 1, GL_FALSE, &identity[0][0]);
        }

        // 1. Dibujar suelo, plataforma circular y pared posterior
        glBindVertexArray(floorVAO);
        glDrawElements(GL_TRIANGLES, floorIndexCount, GL_UNSIGNED_INT, 0);

        // 2. Dibujar rejilla circular y líneas radiales
        glBindVertexArray(gridVAO);
        glDrawArrays(GL_LINES, 0, gridVertexCount);

        glBindVertexArray(0);
    }

    static void initWindTunnel() {
        std::vector<Vertex> vertices;
        glm::vec3 normal(0.0f, 1.0f, 0.0f);
        glm::vec3 baseColor(0.0f, 0.6f, 0.9f); // Celeste brillante

        int xCount = 11;
        int yCount = 5;
        int zSegments = 40;

        float xStart = -4.0f, xEnd = 4.0f;
        float yStart = -0.4f, yEnd = 1.2f;
        float zStart = -8.0f, zEnd = 8.0f;

        float xStep = (xEnd - xStart) / (xCount - 1);
        float yStep = (yEnd - yStart) / (yCount - 1);
        float zStep = (zEnd - zStart) / zSegments;

        for (int xi = 0; xi < xCount; xi++) {
            float x0 = xStart + xi * xStep;
            for (int yi = 0; yi < yCount; yi++) {
                float y0 = yStart + yi * yStep;

                // Generar segmentos para esta linea de corriente
                for (int zi = 0; zi < zSegments; zi++) {
                    float zA = zStart + zi * zStep;
                    float zB = zStart + (zi + 1) * zStep;

                    // Calcular deformacion para punto A
                    float xA = x0;
                    float yA = y0;
                    {
                        float d_fuse = sqrt(x0 * x0 + y0 * y0);
                        float f_fuse = exp(-zA * zA / 4.0f);
                        if (d_fuse < 1.8f) {
                            float scale = 1.0f + 0.4f * f_fuse / (d_fuse + 0.1f);
                            xA = x0 * scale;
                            yA = y0 * scale;
                        }
                        // Desviacion por las alas (cerca de Z=0, plano horizontal Y=-0.1f)
                        if (abs(x0) < 3.2f) {
                            float f_wing = exp(-(zA + 0.2f) * (zA + 0.2f) / 0.8f) * exp(-x0 * x0 / 8.0f);
                            yA += 0.25f * f_wing * (y0 > -0.1f ? 1.0f : -1.0f);
                        }
                    }

                    // Calcular deformacion para punto B
                    float xB = x0;
                    float yB = y0;
                    {
                        float d_fuse = sqrt(x0 * x0 + y0 * y0);
                        float f_fuse = exp(-zB * zB / 4.0f);
                        if (d_fuse < 1.8f) {
                            float scale = 1.0f + 0.4f * f_fuse / (d_fuse + 0.1f);
                            xB = x0 * scale;
                            yB = y0 * scale;
                        }
                        if (abs(x0) < 3.2f) {
                            float f_wing = exp(-(zB + 0.2f) * (zB + 0.2f) / 0.8f) * exp(-x0 * x0 / 8.0f);
                            yB += 0.25f * f_wing * (y0 > -0.1f ? 1.0f : -1.0f);
                        }
                    }

                    // Atenuacion de color en los extremos para desvanecimiento suave
                    float alphaA = 1.0f - smoothstep(3.5f, 5.0f, abs(zA));
                    float alphaB = 1.0f - smoothstep(3.5f, 5.0f, abs(zB));

                    glm::vec3 colorA = baseColor * alphaA;
                    glm::vec3 colorB = baseColor * alphaB;

                    // Mapear posicion Z normalizada en la coordenada de textura para animar flujo en el shader
                    float tA = (zA - zStart) / (zEnd - zStart);
                    float tB = (zB - zStart) / (zEnd - zStart);

                    vertices.push_back({ glm::vec3(xA, yA, zA), normal, colorA, glm::vec2(tA, 0.0f) });
                    vertices.push_back({ glm::vec3(xB, yB, zB), normal, colorB, glm::vec2(tB, 0.0f) });
                }
            }
        }

        windVertexCount = vertices.size();

        glGenVertexArrays(1, &windVAO);
        glGenBuffers(1, &windVBO);

        glBindVertexArray(windVAO);
        glBindBuffer(GL_ARRAY_BUFFER, windVBO);
        glBufferData(GL_ARRAY_BUFFER, vertices.size() * sizeof(Vertex), &vertices[0], GL_STATIC_DRAW);

        glEnableVertexAttribArray(0);
        glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, sizeof(Vertex), (void*)0);

        glEnableVertexAttribArray(1);
        glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, sizeof(Vertex), (void*)offsetof(Vertex, normal));

        glEnableVertexAttribArray(2);
        glVertexAttribPointer(2, 3, GL_FLOAT, GL_FALSE, sizeof(Vertex), (void*)offsetof(Vertex, color));

        glEnableVertexAttribArray(3);
        glVertexAttribPointer(3, 2, GL_FLOAT, GL_FALSE, sizeof(Vertex), (void*)offsetof(Vertex, texCoords));

        glBindBuffer(GL_ARRAY_BUFFER, 0);
        glBindVertexArray(0);
    }

    void drawWindTunnel(float time) {
        if (windVAO == 0) {
            initWindTunnel();
        }

        GLint currentProgram = 0;
        glGetIntegerv(GL_CURRENT_PROGRAM, &currentProgram);
        GLint useTexLoc = glGetUniformLocation(currentProgram, "uUseTexture");
        GLint useLightLoc = glGetUniformLocation(currentProgram, "uUseLighting");
        GLint modelLoc = glGetUniformLocation(currentProgram, "uModel");
        GLint simModeLoc = glGetUniformLocation(currentProgram, "uSimMode");
        GLint timeLoc = glGetUniformLocation(currentProgram, "uTime");

        // Desactivar textura e iluminacion para las lineas
        if (useTexLoc != -1) glUniform1i(useTexLoc, 0);
        if (useLightLoc != -1) glUniform1i(useLightLoc, 0);
        
        // Forzar simMode a 1 (Wind tunnel) y pasar el tiempo
        if (simModeLoc != -1) glUniform1i(simModeLoc, 1);
        if (timeLoc != -1) glUniform1f(timeLoc, time);

        // Matriz de modelo identidad para que este centrado en el hangar
        if (modelLoc != -1) {
            glm::mat4 identity = glm::mat4(1.0f);
            glUniformMatrix4fv(modelLoc, 1, GL_FALSE, &identity[0][0]);
        }

        glLineWidth(2.0f);
        glBindVertexArray(windVAO);
        glDrawArrays(GL_LINES, 0, windVertexCount);
        glBindVertexArray(0);
        glLineWidth(1.0f);

        // Restaurar estado de iluminacion
        if (useLightLoc != -1) glUniform1i(useLightLoc, 1);
    }

    void cleanupHangar() {
        if (floorVAO != 0) glDeleteVertexArrays(1, &floorVAO);
        if (floorVBO != 0) glDeleteBuffers(1, &floorVBO);
        if (floorEBO != 0) glDeleteBuffers(1, &floorEBO);
        if (gridVAO != 0) glDeleteVertexArrays(1, &gridVAO);
        if (gridVBO != 0) glDeleteBuffers(1, &gridVBO);
        if (windVAO != 0) glDeleteVertexArrays(1, &windVAO);
        if (windVBO != 0) glDeleteBuffers(1, &windVBO);
        floorVAO = floorVBO = floorEBO = gridVAO = gridVBO = windVAO = windVBO = windVertexCount = 0;
    }

} // namespace Renderer