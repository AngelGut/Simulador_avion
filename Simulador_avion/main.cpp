// ============================================================
// ARCHIVO: main.cpp - GLFW + Shaders + Menú Principal (Fusión Fiel)
// DESCRIPCION: Punto de entrada para OpenGL 3.3+ moderno
// ============================================================

#include <glad/glad.h>
#include <GLFW/glfw3.h>
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>
#include <iostream>
#include <cmath>

#include "app_state.h"
#include "ui_renderer.h"
#include "screens.h"
#include "asset_loader.h"
#include "renderer.h"
#include "shader.h"
#include "model_loader.h"
#include "config.h"

// Contexto global del menú
AppContext ctx;

// Estado de cámara y visor original
Shader* shaderProgram = nullptr;
float viewRotationX = 15.0f; // Pitch (elevación orbital inicial)
float viewRotationY = 45.0f; // Yaw (rotación orbital inicial)
float viewRotationZ = 0.0f;  // Roll (no usado)
float viewZoom = -5.0f;      // Radio orbital (negativo)
float viewX = 0.0f;          // Paneo X
float viewY = 0.0f;          // Paneo Y

bool leftMouseButtonPressed = false;
double lastMouseX = 0.0;
double lastMouseY = 0.0;

// ============================================================
// CALLBACKS DE INTERACCIÓN
// ============================================================

void framebuffer_size_callback(GLFWwindow* window, int width, int height) {
    glViewport(0, 0, width, height);
    ctx.windowWidth = width;
    ctx.windowHeight = height;
    UIRenderer::resize(width, height);
}

void mouse_button_callback(GLFWwindow* window, int button, int action, int mods) {
    // Registrar evento de click para el Menú
    if (button == GLFW_MOUSE_BUTTON_LEFT) {
        ctx.mousePressed = (action == GLFW_PRESS);
        
        // Registrar para rotación de cámara en el visor
        if (ctx.state == AppState::VIEWER) {
            if (action == GLFW_PRESS) {
                leftMouseButtonPressed = true;
                glfwGetCursorPos(window, &lastMouseX, &lastMouseY);
            } else if (action == GLFW_RELEASE) {
                leftMouseButtonPressed = false;
            }
        }
    }
}

void cursorPosCallback(GLFWwindow* window, double xpos, double ypos) {
    if (ctx.state == AppState::VIEWER && leftMouseButtonPressed) {
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
    if (ctx.state == AppState::VIEWER) {
        float zoomSensitivity = 0.5f;
        viewZoom += (float)yoffset * zoomSensitivity;
        if (viewZoom > -0.5f) viewZoom = -0.5f;
    }
}

void keyCallback(GLFWwindow* window, int key, int scancode, int action, int mods) {
    if (action != GLFW_PRESS || ctx.state != AppState::VIEWER) return;

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

    // Rotación PITCH continua con I/K
    if (key == GLFW_KEY_I) viewRotationX += 10.0f;
    if (key == GLFW_KEY_K) viewRotationX -= 10.0f;

    // Rotación YAW continua con J/L
    if (key == GLFW_KEY_J) viewRotationY -= 10.0f;
    if (key == GLFW_KEY_L) viewRotationY += 10.0f;

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
        ctx.showHelp = !ctx.showHelp;
    }
}

// ============================================================
// VISOR DE AVIONES CON SOPORTE COMPLETO DE TEXTURAS Y HANGAR
// ============================================================

void renderViewerState(GLFWwindow* window) {
    // 1. Cargar el avión dinámicamente si cambia
    int selectedModelNum = ctx.selectedPlane + 1; // 1-4
    if (Renderer::getCurrentModelNumber() != selectedModelNum || !Renderer::getLoadedModel()) {
        Renderer::loadModelByNumber(selectedModelNum);
        
        // Ajustar el zoom recomendado del modelo
        Model* loaded = Renderer::getLoadedModel();
        if (loaded) {
            viewZoom = loaded->getRecommendedZoom();
        }
    }

    // 2. Limpieza de pantalla y profundidad para 3D
    glClearColor(0.05f, 0.05f, 0.1f, 1.0f);
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
    glEnable(GL_DEPTH_TEST);
    glDepthFunc(GL_LEQUAL);

    if (!shaderProgram) return;
    shaderProgram->use();

    // Matrices de proyección y vista orbital
    float aspect = (float)ctx.windowWidth / (float)ctx.windowHeight;
    glm::mat4 projection = glm::perspective(glm::radians(45.0f), aspect, 0.1f, 500.0f);

    float radius = std::abs(viewZoom);
    if (viewRotationX > 89.0f) viewRotationX = 89.0f;
    if (viewRotationX < -89.0f) viewRotationX = -89.0f;

    float camX = radius * cos(glm::radians(viewRotationX)) * sin(glm::radians(viewRotationY));
    float camY = radius * sin(glm::radians(viewRotationX));
    float camZ = radius * cos(glm::radians(viewRotationX)) * cos(glm::radians(viewRotationY));

    float actualCamY = camY + viewY;

    glm::mat4 view = glm::lookAt(
        glm::vec3(camX + viewX, actualCamY, camZ),
        glm::vec3(viewX, viewY, 0.0f),
        glm::vec3(0.0f, 1.0f, 0.0f)
    );

    glm::mat4 model = glm::mat4(1.0f);

    // Pasar matrices al Shader
    shaderProgram->setMat4("uModel", model);
    shaderProgram->setMat4("uView", view);
    shaderProgram->setMat4("uProjection", projection);

    // Configurar luces del hangar
    shaderProgram->setVec3("uLightPos", glm::vec3(0.0f, 5.0f, 0.0f)); // Luz cenital
    shaderProgram->setVec3("uViewPos", glm::vec3(camX + viewX, actualCamY, camZ));
    shaderProgram->setVec3("uLightColor", glm::vec3(1.0f, 1.0f, 1.0f));

    // Dibujar hangar
    Renderer::drawHangar(actualCamY);

    // Dibujar avión
    Renderer::drawLayer(1);

    // Desactivar depth test para dibujar el HUD plano encima
    glDisable(GL_DEPTH_TEST);

    // 3. Renderizar HUD de interfaz de usuario encima usando UIRenderer
    float w = (float)ctx.windowWidth;
    float h = (float)ctx.windowHeight;
    float cx = w / 2.0f;

    // Nombre del modelo o pieza
    if (Renderer::isPartsModeActive()) {
        std::string partName = Renderer::getCurrentPartName();
        int currentIdx = Renderer::getCurrentPartIndex();
        int total = Renderer::getNumParts();
        std::string partHUD = partName + " (" + std::to_string(currentIdx + 1) + "/" + std::to_string(total) + ")";
        UIRenderer::drawText(cx - 150.0f, 24.0f, partHUD.c_str(), 2.0f, UIColor{1.0f, 1.0f, 1.0f, 1.0f});
    } else {
        UIRenderer::drawText(cx - 150.0f, 24.0f, ctx.planes[ctx.selectedPlane].name.c_str(), 2.0f, UIColor{1.0f, 1.0f, 1.0f, 1.0f});
    }

    // Botón Volver
    float backW = 140.0f, backH = 44.0f;
    bool backClicked = UIRenderer::drawButton(
        24.0f, h - 68.0f, backW, backH, "Volver",
        ctx.mouseX, ctx.mouseY, ctx.mousePressed,
        UIColor{ 0.3f, 0.3f, 0.35f, 1.0f }, UIColor{ 0.4f, 0.4f, 0.46f, 1.0f }
    );
    if (backClicked) {
        ctx.state = AppState::MENU;
        if (Renderer::isPartsModeActive()) {
            Renderer::togglePartsMode();
        }
    }

    // Dibujar ayuda
    if (ctx.showHelp) {
        float boxW = 320.0f, boxH = 190.0f;
        float boxX = w - boxW - 24.0f, boxY = 24.0f;
        UIRenderer::drawQuad(boxX, boxY, boxW, boxH, UIColor{ 0.05f, 0.05f, 0.08f, 0.85f });
        UIRenderer::drawBorder(boxX, boxY, boxW, boxH, 2.0f, UIColor{ 0.3f, 0.3f, 0.3f, 1.0f });

        float lineY = boxY + 16.0f;
        UIRenderer::drawText(boxX + 16.0f, lineY, "Controles:", 1.4f, UIColor{1, 1, 1, 1}); lineY += 26.0f;
        UIRenderer::drawText(boxX + 16.0f, lineY, "Arrastrar Click - Rotar", 1.2f, UIColor{0.7f, 0.7f, 0.7f, 1.0f}); lineY += 22.0f;
        UIRenderer::drawText(boxX + 16.0f, lineY, "Scroll / Q/E - Zoom", 1.2f, UIColor{0.7f, 0.7f, 0.7f, 1.0f}); lineY += 22.0f;
        UIRenderer::drawText(boxX + 16.0f, lineY, "WASD - Paneo", 1.2f, UIColor{0.7f, 0.7f, 0.7f, 1.0f}); lineY += 22.0f;
        UIRenderer::drawText(boxX + 16.0f, lineY, "P - Modo Piezas", 1.2f, UIColor{0.7f, 0.7f, 0.7f, 1.0f}); lineY += 22.0f;
        UIRenderer::drawText(boxX + 16.0f, lineY, "<- / -> - Cambiar Pieza", 1.2f, UIColor{0.7f, 0.7f, 0.7f, 1.0f});
    } else {
        UIRenderer::drawText(cx - 150.0f, h - 30.0f, "Presiona H para ver los controles", 1.3f, UIColor{0.5f, 0.5f, 0.5f, 1.0f});
    }
}

// ============================================================
// MAIN
// ============================================================

int main(int argc, char** argv) {
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
    glfwSetCursorPosCallback(window, cursorPosCallback);
    glfwSetScrollCallback(window, scrollCallback);
    glfwSetKeyCallback(window, keyCallback);

    UIRenderer::init(ctx.windowWidth, ctx.windowHeight);
    ModelRenderer::init();
    
    // Cargar shaders de texturas original
    try {
        shaderProgram = new Shader("shaders/vertex.glsl", "shaders/fragment.glsl");
        std::cout << "✓ Shader de texturas e iluminacion cargado exitosamente\n";
    }
    catch (const std::exception& e) {
        std::cerr << "Error shaders: " << e.what() << std::endl;
        return -1;
    }

    // Iniciar carga dinamica de aviones del menu
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
            // Dibujar la escena completa usando nuestro visualizador
            renderViewerState(window);
            break;
        }

        ctx.mousePressed = false; 

        glfwSwapBuffers(window);
        glfwPollEvents();

        if (glfwGetKey(window, GLFW_KEY_ESCAPE) == GLFW_PRESS)
            glfwSetWindowShouldClose(window, true);
    }

    // Limpieza final
    Renderer::cleanupHangar();
    delete shaderProgram;
    UIRenderer::shutdown();
    glfwDestroyWindow(window);
    glfwTerminate();
    return 0;
}
