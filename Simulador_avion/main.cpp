// ============================================================
// ARCHIVO: main.cpp - GLFW + Shaders (Fase B Moderna)
// DESCRIPCION: Punto de entrada para OpenGL 3.3+ moderno
// ============================================================
/*
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

float viewRotationX = 15.0f; // Pitch (elevación orbital inicial)
float viewRotationY = 45.0f; // Yaw (rotación orbital inicial)
float viewRotationZ = 0.0f;  // Roll (no se usa en órbita)
float viewZoom = -5.0f;      // Radio de órbita (negativo)
float viewX = 0.0f;          // Paneo horizontal
float viewY = 0.0f;          // Paneo vertical

int windowWidth = 1024;
int windowHeight = 768;

bool showHelp = false;

// Variables para control de mouse
bool leftMouseButtonPressed = false;
double lastMouseX = 0.0;
double lastMouseY = 0.0;

// ============================================================
// CALLBACKS GLFW
// ============================================================

void windowSizeCallback(GLFWwindow* window, int width, int height) {
    windowWidth = width;
    windowHeight = height;
    glViewport(0, 0, width, height);
}

void mouseButtonCallback(GLFWwindow* window, int button, int action, int mods) {
    if (button == GLFW_MOUSE_BUTTON_LEFT) {
        if (action == GLFW_PRESS) {
            leftMouseButtonPressed = true;
            glfwGetCursorPos(window, &lastMouseX, &lastMouseY);
        } else if (action == GLFW_RELEASE) {
            leftMouseButtonPressed = false;
        }
    }
}

void cursorPosCallback(GLFWwindow* window, double xpos, double ypos) {
    if (leftMouseButtonPressed) {
        double deltaX = xpos - lastMouseX;
        double deltaY = ypos - lastMouseY;

        float sensitivity = 0.2f;
        viewRotationY += (float)deltaX * sensitivity;
        viewRotationX += (float)deltaY * sensitivity;

        lastMouseX = xpos;
        lastMouseY = ypos;
    }
}

void scrollCallback(GLFWwindow* window, double xoffset, double yoffset) {
    float zoomSensitivity = 0.5f;
    viewZoom += (float)yoffset * zoomSensitivity;
    if (viewZoom > -0.5f) viewZoom = -0.5f;
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
        viewRotationX = 15.0f;
        viewRotationY = 45.0f;
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
    glfwSetMouseButtonCallback(window, mouseButtonCallback);
    glfwSetCursorPosCallback(window, cursorPosCallback);
    glfwSetScrollCallback(window, scrollCallback);
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
        }
        else {
            title += "Sin piezas";
        }
    }
    else {
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

    // Cámara Orbital (Yaw/Pitch/Radius)
    float radius = std::abs(viewZoom);

    // Limitar Pitch para evitar giro de 360 grados vertical (polar lock)
    if (viewRotationX > 89.0f) viewRotationX = 89.0f;
    if (viewRotationX < -89.0f) viewRotationX = -89.0f;

    float camX = radius * cos(glm::radians(viewRotationX)) * sin(glm::radians(viewRotationY));
    float camY = radius * sin(glm::radians(viewRotationX));
    float camZ = radius * cos(glm::radians(viewRotationX)) * cos(glm::radians(viewRotationY));

    // Altura real considerando el desplazamiento vertical (pan)
    float actualCamY = camY + viewY;

    // Vista orbital de cámara
    glm::mat4 view = glm::lookAt(
        glm::vec3(camX + viewX, actualCamY, camZ),
        glm::vec3(viewX, viewY, 0.0f),
        glm::vec3(0.0f, 1.0f, 0.0f)
    );

    // El modelo se queda fijo sobre el suelo
    glm::mat4 model = glm::mat4(1.0f);

    // Pasar matrices al shader
    shaderProgram->setMat4("uModel", model);
    shaderProgram->setMat4("uView", view);
    shaderProgram->setMat4("uProjection", projection);

    // Parámetros de iluminación (luz cenital fija en el hangar)
    shaderProgram->setVec3("uLightPos", glm::vec3(0.0f, 5.0f, 0.0f));
    shaderProgram->setVec3("uViewPos", glm::vec3(camX + viewX, actualCamY, camZ));
    shaderProgram->setVec3("uLightColor", glm::vec3(1.0f, 1.0f, 1.0f));

    // Dibujar el Hangar (piso + rejilla) pasándole la altura de la cámara
    Renderer::drawHangar(actualCamY);

    // Dibujar avión/pieza
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
    Renderer::cleanupHangar();
    delete shaderProgram;
    glfwDestroyWindow(window);
    glfwTerminate();

    return 0;
} */
#include <glad/glad.h>
#include <GLFW/glfw3.h>
#include <iostream>
#include "app_state.h"
#include "ui_renderer.h"
#include "screens.h"
#include "asset_loader.h"
#include "model_renderer.h"

AppContext ctx;

void framebuffer_size_callback(GLFWwindow* window, int width, int height) {
    glViewport(0, 0, width, height);
    ctx.windowWidth = width;
    ctx.windowHeight = height;
    UIRenderer::resize(width, height);
}

void mouse_button_callback(GLFWwindow* window, int button, int action, int mods) {
    if (button == GLFW_MOUSE_BUTTON_LEFT) {
        ctx.mousePressed = (action == GLFW_PRESS);
    }
}

int main() {
    if (!glfwInit()) {
        std::cout << "Error iniciando GLFW\n";
        return -1;
    }

    glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
    glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
    glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);

    GLFWwindow* window = glfwCreateWindow(ctx.windowWidth, ctx.windowHeight, "Simulador de Avion", NULL, NULL);
    if (!window) {
        std::cout << "Error creando ventana GLFW\n";
        glfwTerminate();
        return -1;
    }
    glfwMakeContextCurrent(window);

    if (!gladLoadGLLoader((GLADloadproc)glfwGetProcAddress)) {
        std::cout << "Error cargando GLAD\n";
        return -1;
    }

    glViewport(0, 0, ctx.windowWidth, ctx.windowHeight);
    glfwSetFramebufferSizeCallback(window, framebuffer_size_callback);
    glfwSetMouseButtonCallback(window, mouse_button_callback);

    UIRenderer::init(ctx.windowWidth, ctx.windowHeight);
    ModelRenderer::init();

    startAssetLoading(ctx);

    ctx.lastFrameTime = (float)glfwGetTime();

    while (!glfwWindowShouldClose(window)) {
        float currentTime = (float)glfwGetTime();
        ctx.deltaTime = currentTime - ctx.lastFrameTime;
        ctx.lastFrameTime = currentTime;
        ctx.totalTime += ctx.deltaTime;

        glfwGetCursorPos(window, &ctx.mouseX, &ctx.mouseY);

        glClearColor(0.09f, 0.09f, 0.13f, 1.0f);
        glClear(GL_COLOR_BUFFER_BIT);

        switch (ctx.state) {
        case AppState::LOADING:
            Screens::renderLoading(ctx);
            break;
        case AppState::WELCOME:
            Screens::renderWelcome(ctx);
            break;
        case AppState::MENU:
            Screens::renderMenu(ctx);
            break;
        case AppState::VIEWER:
            Screens::renderViewer(ctx, window);
            break;
        }

        ctx.mousePressed = false; 

        glfwSwapBuffers(window);
        glfwPollEvents();

        if (glfwGetKey(window, GLFW_KEY_ESCAPE) == GLFW_PRESS)
            glfwSetWindowShouldClose(window, true);
    }

    UIRenderer::shutdown();
    glfwTerminate();
    return 0;
}
