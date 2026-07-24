// ============================================================
// ARCHIVO: renderer.cpp - CON CARGADOR DE MODELOS
// RESPONSABLE: Ronald (Rendering)
// DESCRIPCION: Renderizado 3D con carga de modelos OBJ/FBX
// ============================================================

#include "renderer.h"
#include "geometry.h"
#include "model_loader.h"
#include <GL/glut.h>
#include <cstdio>
#include <cmath>
#include <iostream>

namespace Renderer {

    // Variable estática para almacenar el modelo cargado
    static Model* loadedModel = nullptr;
    static int currentModelNumber = 5;

    // ============================================================
    // setupOpenGL() - Configuración para 3D
    // ============================================================
    void setupOpenGL() {
        glClearColor(0.05f, 0.05f, 0.1f, 1.0f);

        // Habilitar test de profundidad para 3D
        glEnable(GL_DEPTH_TEST);
        glDepthFunc(GL_LEQUAL);

        // Habilitar iluminación
        glEnable(GL_LIGHTING);
        glEnable(GL_LIGHT0);
        glEnable(GL_COLOR_MATERIAL);
        glColorMaterial(GL_FRONT_AND_BACK, GL_AMBIENT_AND_DIFFUSE);

        // Parámetros de iluminación
        GLfloat ambientLight[] = { 0.3f, 0.3f, 0.3f, 1.0f };
        GLfloat diffuseLight[] = { 0.9f, 0.9f, 0.9f, 1.0f };
        GLfloat specularLight[] = { 1.0f, 1.0f, 1.0f, 1.0f };
        GLfloat lightPosition[] = { 5.0f, 5.0f, 5.0f, 0.0f };

        glLightfv(GL_LIGHT0, GL_AMBIENT, ambientLight);
        glLightfv(GL_LIGHT0, GL_DIFFUSE, diffuseLight);
        glLightfv(GL_LIGHT0, GL_SPECULAR, specularLight);
        glLightfv(GL_LIGHT0, GL_POSITION, lightPosition);

        // Suavizado de líneas
        glEnable(GL_LINE_SMOOTH);
        glHint(GL_LINE_SMOOTH_HINT, GL_NICEST);
        glLineWidth(1.5f);
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
            loadedModel->draw();
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

} // namespace Renderer