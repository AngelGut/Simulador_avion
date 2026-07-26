// ============================================================
// ARCHIVO: renderer.cpp - CON CARGADOR DE MODELOS
// RESPONSABLE: Ronald (Rendering)
// DESCRIPCION: Renderizado 3D con carga de modelos OBJ/FBX
// ============================================================

#include "renderer.h"
#include "geometry.h"
#include "model_loader.h"
#include "config.h"
#include <GL/glew.h>
#include <cstdio>
#include <cmath>
#include <iostream>

namespace Renderer {

    // Variable estática para almacenar el modelo cargado
    static Model* loadedModel = nullptr;
    static int currentModelNumber = 5;

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

        std::cout << "Cargando modelo: " << modelPath << std::endl;
        if (loadedModel->loadModel(modelPath)) {
            std::cout << "✓ Modelo cargado exitosamente\n";
        }
        else {
            std::cerr << "✗ Error al cargar modelo. Se usará geometría procedural.\n";
            delete loadedModel;
            loadedModel = nullptr;
        }
    }

    // ============================================================
    // getLoadedModel() - Obtener referencia al modelo cargado
    // ============================================================
    Model* getLoadedModel() {
        return loadedModel;
    }

    // ============================================================
    // applyLighting() - Configuración de iluminación
    // ============================================================
    void applyLighting() {
        glEnable(GL_LIGHTING);
    }

    // ============================================================
    // applyColor() - Aplicar color
    // ============================================================
    void applyColor(float r, float g, float b) {
        glColor3f(r, g, b);
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
        // Si hay modelo cargado, dibujarlo
        if (loadedModel != nullptr && loadedModel->isLoaded()) {
            // Note: Draw es llamado desde main.cpp con shader
            loadedModel->draw(nullptr);
        }
        else {
            // Fallback: geometría procedural si no hay modelo
            std::cout << "Usando geometría procedural (modelo no disponible)\n";

            switch (layerNumber) {
            case 1: // Exterior
                GeometryBuilder::generateFuselage(0.0f, 0.0f, 30.0f, 300.0f);
                GeometryBuilder::generateWings();
                GeometryBuilder::generateMotors();
                break;

            case 2: // Estructura interna
                GeometryBuilder::generateStructure();
                break;

            case 3: // Sistemas
                GeometryBuilder::generateSystems();
                break;

            case 4: // Cabina
                GeometryBuilder::generateCabin();
                break;

            case 5: // Propulsion
                GeometryBuilder::generateMotors();
                GeometryBuilder::generateLandingGear();
                break;

            default:
                break;
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
    // loadModelByNumber() - Cargar modelo por número (1-5)
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
        case 5:
            modelPath = MODEL_5;
            modelName = NAME_5;
            break;
        default:
            std::cerr << "Modelo inválido. Opciones: 1-5\n";
            return;
        }

        currentModelNumber = modelNumber;
        std::cout << "\n--- Cargando: " << modelName << " ---\n";
        initModel(modelPath);
    }

    // ============================================================
    // getCurrentModelNumber() - Obtener número del modelo actual
    // ============================================================
    int getCurrentModelNumber() {
        return currentModelNumber;
    }

    // ============================================================
    // printHelp() - Mostrar controles disponibles
    // ============================================================
    void printHelp() {
        std::cout << "\n"
            << "==================================================\n"
            << "    Boeing 737 Visualizer 3D v2.1 (GLFW + Shaders)\n"
            << "==================================================\n"
            << " SELECCIONAR AERONAVE:\n"
            << "   1-5        Cambiar modelo\n"
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

} // namespace Renderer