// ============================================================
// ARCHIVO: main.cpp - GLFW + Shaders (Fase B Moderna)
// DESCRIPCION: Punto de entrada para OpenGL 3.3+ moderno
// ============================================================

#include <GL/glew.h>
#include <GLFW/glfw3.h>
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>
#include <iostream>
#include <cmath>
#include "renderer.h"
#include "shader.h"
#include "model_loader.h"
#include "config.h"

// ============================================================
// VARIABLES GLOBALES
// ============================================================

GLFWwindow* window = nullptr;
Shader* shaderProgram = nullptr;

// Estado de cámara
glm::vec3 cameraPos(0.0f, 0.0f, 4.0f);
glm::vec3 cameraTarget(0.0f, 0.0f, 0.0f);
glm::vec3 cameraUp(0.0f, 1.0f, 0.0f);

float viewRotationX = 0.0f;
float viewRotationY = 0.0f;
float viewRotationZ = 0.0f;
float viewZoom = -5.0f;
float viewX = 0.0f;
float viewY = 0.0f;

int windowWidth = 1024;
int windowHeight = 768;

bool showHelp = false;

// ============================================================
// CALLBACKS GLFW
// ============================================================

void windowSizeCallback(GLFWwindow* window, int width, int height) {
    windowWidth = width;
    windowHeight = height;
    glViewport(0, 0, width, height);
}

void keyCallback(GLFWwindow* window, int key, int scancode, int action, int mods) {
    if (action != GLFW_PRESS) return;

    Model* model = Renderer::getLoadedModel();

    // Seleccionar modelo (1-4)
    if (key >= GLFW_KEY_1 && key <= GLFW_KEY_4) {
        int modelNumber = key - GLFW_KEY_1 + 1;
        Renderer::loadModelByNumber(modelNumber);
        
        // Restablecer zoom para el nuevo modelo
        Model* newModel = Renderer::getLoadedModel();
        if (newModel) {
            viewZoom = newModel->getRecommendedZoom();
        }
        return;
    }

    // Activar / desactivar modo piezas (P)
    if (key == GLFW_KEY_P) {
        Renderer::togglePartsMode();
        Model* currentModel = Renderer::getLoadedModel();
        if (currentModel) {
            viewZoom = currentModel->getRecommendedZoom();
        }
        return;
    }

    // Navegar entre piezas (Flecha Izq / Flecha Der)
    if (key == GLFW_KEY_RIGHT) {
        Renderer::nextPart();
        Model* currentModel = Renderer::getLoadedModel();
        if (currentModel) {
            viewZoom = currentModel->getRecommendedZoom();
        }
        return;
    }
    if (key == GLFW_KEY_LEFT) {
        Renderer::prevPart();
        Model* currentModel = Renderer::getLoadedModel();
        if (currentModel) {
            viewZoom = currentModel->getRecommendedZoom();
        }
        return;
    }

    // Rotación PITCH (I/K)
    if (key == GLFW_KEY_I) viewRotationX += 10.0f;
    if (key == GLFW_KEY_K) viewRotationX -= 10.0f;

    // Rotación YAW (J/L)
    if (key == GLFW_KEY_J) viewRotationY -= 10.0f;
    if (key == GLFW_KEY_L) viewRotationY += 10.0f;

    // Rotación ROLL (R/T)
    if (key == GLFW_KEY_R) viewRotationZ -= 10.0f;
    if (key == GLFW_KEY_T) viewRotationZ += 10.0f;

    // ZOOM (Q/E)
    if (key == GLFW_KEY_Q) viewZoom += 0.5f;
    if (key == GLFW_KEY_E) viewZoom -= 0.5f;
    if (viewZoom > -0.5f) viewZoom = -0.5f;

    // PAN (W/A/S/D)
    if (key == GLFW_KEY_W) viewY += 0.5f;
    if (key == GLFW_KEY_S) viewY -= 0.5f;
    if (key == GLFW_KEY_A) viewX -= 0.5f;
    if (key == GLFW_KEY_D) viewX += 0.5f;

    // RESET (ESPACIO)
    if (key == GLFW_KEY_SPACE) {
        viewRotationX = 0.0f;
        viewRotationY = 0.0f;
        viewRotationZ = 0.0f;
        
        Model* currentModel = Renderer::getLoadedModel();
        if (currentModel) {
            viewZoom = currentModel->getRecommendedZoom();
        } else {
            viewZoom = -5.0f;
        }
        
        viewX = 0.0f;
        viewY = 0.0f;
        std::cout << "Vista reseteada\n";
    }

    // AYUDA (H)
    if (key == GLFW_KEY_H) {
        showHelp = !showHelp;
        Renderer::printHelp();
    }

    // SALIR (ESC)
    if (key == GLFW_KEY_ESCAPE) {
        glfwSetWindowShouldClose(window, true);
    }
}

// ============================================================
// INICIALIZACIÓN
// ============================================================

bool initGLFW() {
    if (!glfwInit()) {
        std::cerr << "Error: No se pudo inicializar GLFW\n";
        return false;
    }

    glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
    glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
    glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);

#ifdef __APPLE__
    glfwWindowHint(GLFW_OPENGL_FORWARD_COMPAT, GL_TRUE);
#endif

    window = glfwCreateWindow(windowWidth, windowHeight, "Boeing 737 Visualizer 3D - Fase B Moderna", NULL, NULL);
    if (!window) {
        std::cerr << "Error: No se pudo crear ventana GLFW\n";
        glfwTerminate();
        return false;
    }

    glfwMakeContextCurrent(window);
    glfwSetFramebufferSizeCallback(window, windowSizeCallback);
    glfwSetKeyCallback(window, keyCallback);
    glfwSwapInterval(1);

    return true;
}

bool initGLEW() {
    glewExperimental = GL_TRUE;
    GLenum err = glewInit();
    if (err != GLEW_OK) {
        std::cerr << "Error: " << glewGetErrorString(err) << std::endl;
        return false;
    }

    std::cout << "OpenGL Version: " << glGetString(GL_VERSION) << std::endl;
    std::cout << "GLSL Version: " << glGetString(GL_SHADING_LANGUAGE_VERSION) << std::endl;

    return true;
}

bool initOpenGL() {
    glClearColor(0.05f, 0.05f, 0.1f, 1.0f);
    glEnable(GL_DEPTH_TEST);
    glDepthFunc(GL_LEQUAL);

    // Cargar shaders
    try {
        shaderProgram = new Shader("shaders/vertex.glsl", "shaders/fragment.glsl");
        std::cout << "✓ Shaders cargados exitosamente\n";
    }
    catch (const std::exception& e) {
        std::cerr << "Error al cargar shaders: " << e.what() << std::endl;
        return false;
    }

    return true;
}

void printModelMenu() {
    std::cout << "\n"
        << "==================================================\n"
        << "    SELECCIONA UNA AERONAVE (1-4)\n"
        << "==================================================\n"
        << " 1  " << NAME_1 << "\n"
        << " 2  " << NAME_2 << "\n"
        << " 3  " << NAME_3 << "\n"
        << " 4  " << NAME_4 << "\n"
        << "==================================================\n"
        << "\n";
}

void updateWindowTitle() {
    if (!window) return;
    
    std::string title = "Simulador de Avion - ";
    int modelNum = Renderer::getCurrentModelNumber();
    std::string modelName = "";
    switch (modelNum) {
        case 1: modelName = NAME_1; break;
        case 2: modelName = NAME_2; break;
        case 3: modelName = NAME_3; break;
        case 4: modelName = NAME_4; break;
    }
    title += modelName;

    if (Renderer::isPartsModeActive()) {
        title += " | [MODO PIEZAS] ";
        int currentIdx = Renderer::getCurrentPartIndex();
        int total = Renderer::getNumParts();
        if (total > 0) {
            title += Renderer::getCurrentPartName() + " (" + std::to_string(currentIdx + 1) + "/" + std::to_string(total) + ")";
        } else {
            title += "Sin piezas";
        }
    } else {
        title += " | [VISTA COMPLETA]";
    }
    
    glfwSetWindowTitle(window, title.c_str());
}

// ============================================================
// LOOP PRINCIPAL
// ============================================================

void render() {
    updateWindowTitle();
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

    if (!shaderProgram) return;

    shaderProgram->use();

    // Matrices de transformación
    glm::mat4 projection = glm::perspective(glm::radians(45.0f),
        (float)windowWidth / (float)windowHeight, 0.1f, 500.0f);

    // Vista con rotaciones
    glm::mat4 view = glm::lookAt(
        glm::vec3(viewX, viewY, viewZoom),
        glm::vec3(0.0f, 0.0f, 0.0f),
        glm::vec3(0.0f, 1.0f, 0.0f)
    );

    // Modelo con rotaciones
    glm::mat4 model = glm::mat4(1.0f);
    model = glm::rotate(model, glm::radians(viewRotationX), glm::vec3(1.0f, 0.0f, 0.0f));
    model = glm::rotate(model, glm::radians(viewRotationY), glm::vec3(0.0f, 1.0f, 0.0f));
    model = glm::rotate(model, glm::radians(viewRotationZ), glm::vec3(0.0f, 0.0f, 1.0f));

    // Pasar matrices al shader
    shaderProgram->setMat4("uModel", model);
    shaderProgram->setMat4("uView", view);
    shaderProgram->setMat4("uProjection", projection);

    // Parámetros de iluminación
    shaderProgram->setVec3("uLightPos", glm::vec3(5.0f, 5.0f, 5.0f));
    shaderProgram->setVec3("uViewPos", glm::vec3(viewX, viewY, viewZoom));
    shaderProgram->setVec3("uLightColor", glm::vec3(0.9f, 0.9f, 0.9f));

    // Dibujar modelo
    Renderer::drawLayer(1);
}

// ============================================================
// MAIN
// ============================================================

int main(int argc, char** argv) {
    std::cout << "\n"
        << "====================================================\n"
        << "   Boeing 737 Visualizer 3D v2.1\n"
        << "   Con GLFW + Shaders OpenGL 3.3+\n"
        << "   Presiona H para ver controles\n"
        << "====================================================\n"
        << "\n";

    // Inicializar GLFW
    if (!initGLFW()) {
        return -1;
    }

    // Inicializar GLEW
    if (!initGLEW()) {
        glfwTerminate();
        return -1;
    }

    // Inicializar OpenGL
    if (!initOpenGL()) {
        glfwTerminate();
        return -1;
    }

    // Cargar modelo
    printModelMenu();

    int selectedModel = 0;
    std::cout << "Ingresa el número de la aeronave (1-4): ";
    std::cin >> selectedModel;

    if (selectedModel >= 1 && selectedModel <= 4) {
        Renderer::loadModelByNumber(selectedModel);
    }
    else {
        std::cout << "Opción inválida. Cargando modelo por defecto (1)...\n";
        Renderer::loadModelByNumber(1);
    }

    // Ajustar zoom recomendado para el modelo inicial
    Model* initialModel = Renderer::getLoadedModel();
    if (initialModel) {
        viewZoom = initialModel->getRecommendedZoom();
    }

    Renderer::printHelp();

    // Loop principal
    double lastTime = glfwGetTime();
    int frameCount = 0;

    while (!glfwWindowShouldClose(window)) {
        render();

        glfwSwapBuffers(window);
        glfwPollEvents();

        // FPS counter
        double currentTime = glfwGetTime();
        frameCount++;
        if (currentTime - lastTime >= 1.0) {
            std::cout << "FPS: " << frameCount << std::endl;
            frameCount = 0;
            lastTime = currentTime;
        }
    }

    // Limpiar
    delete shaderProgram;
    glfwDestroyWindow(window);
    glfwTerminate();

    return 0;
}
