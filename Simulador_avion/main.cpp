// ============================================================
// ARCHIVO: main.cpp - GLFW + Shaders + Menú Principal + Hotspots 3D
// DESCRIPCION: Punto de entrada para OpenGL 3.3+ moderno
// ============================================================

#include <glad/glad.h>
#include <GLFW/glfw3.h>
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>
#include <iostream>
#include <cmath>
#include <vector>
#include <string>
#include <algorithm>

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
// ESTRUCTURA Y BASE DE DATOS DE HOTSPOTS 3D (PUNTOS DE INTERÉS)
// ============================================================

struct Hotspot {
    std::string title;
    std::string description;
    glm::vec3 localPos;
    std::string keyword; // Para filtrar en el Modo Piezas
};

std::vector<std::vector<Hotspot>> planeHotspots = {
    // 1: A-10 Thunderbolt II
    {
        { "CANON GAU-8 AVENGER", "Canon rotatorio de 30mm de 7 tubos, nucleo del poder del avion.", glm::vec3(0.0f, -0.2f, 1.1f), "gun" },
        { "CABINA DE TITANIO", "Banera de titanio reforzado de 540 kg disenada para soportar impactos.", glm::vec3(0.0f, 0.25f, 0.6f), "cabin" },
        { "TURBOFANS TF34", "Motores montados arriba y atras para ocultar la firma termica y evitar proyectiles.", glm::vec3(0.0f, 0.45f, -0.7f), "motor" },
        { "ALAS RECTAS A-10", "Alas de gran superficie que permiten maniobras extremas a baja velocidad.", glm::vec3(-0.65f, 0.0f, 0.0f), "alas" },
        { "TREN REFORZADO", "Tren de aterrizaje de alta resistencia para operar en terrenos rusticos.", glm::vec3(0.0f, -0.6f, 0.2f), "tren" }
    },
    // 2: B-24 Liberator
    {
        { "COMPARTIMENTO DE BOMBAS", "Bodega central con compuertas enrollables que reducen la friccion.", glm::vec3(0.0f, -0.1f, -0.1f), "fuselaje" },
        { "CABINA DE MANDO B-24", "Estacion para pilotos y navegantes en cabina no presurizada de la SGM.", glm::vec3(0.0f, 0.15f, 0.7f), "interior" },
        { "MOTORES RADIALES", "Motores Pratt & Whitney R-1830 con turbocompresor para de de vuelo a gran altura.", glm::vec3(0.35f, 0.05f, 0.2f), "motores" },
        { "ALA DAVIS", "Ala de envergadura superior y baja friccion, clave para el enorme alcance.", glm::vec3(-0.65f, 0.05f, 0.0f), "alas" },
        { "TREN RETRACTIL LATERAL", "Primer tren de aterrizaje triciclo en bombarderos pesados.", glm::vec3(0.0f, -0.55f, 0.0f), "tren" }
    },
    // 3: Boeing 787 Dreamliner
    {
        { "FUSELAJE COMPOSITE 787", "Estructura de fibra de carbono que ofrece ligereza y ventanas mas amplias.", glm::vec3(0.0f, 0.15f, 0.6f), "fuselaje" },
        { "TURBOFAN GEnx", "Motores ultra silenciosos equipados con cubiertas traseras dentadas (chevrons).", glm::vec3(0.35f, -0.15f, 0.1f), "motores" },
        { "ESTABILIZADOR DE COLA", "Diseno aerodinamico optimizado para maxima estabilidad digital fly-by-wire.", glm::vec3(0.0f, 0.45f, -0.95f), "fuselaje" },
        { "ALAS FLEXIBLES", "Alas que se flexionan en vuelo para amortiguar y suavizar turbulencias.", glm::vec3(-0.75f, 0.0f, -0.1f), "fuselaje" },
        { "TREN DE TITANIO", "Estructura de amortiguacion avanzada en titanio para aterrizajes suaves.", glm::vec3(0.0f, -0.6f, 0.2f), "tren" }
    },
    // 4: MiG-29
    {
        { "CABINA Y MIRA DE CASCO", "Cabina burbuja con sistema de designacion de blancos en casco del piloto.", glm::vec3(0.0f, 0.2f, 0.45f), "cabina" },
        { "TURBOFANS KLIMOV RD-33", "Motores gemelos con tomas de aire auxiliares superiores para pistas sucias.", glm::vec3(0.15f, 0.02f, -0.5f), "cabina" },
        { "MISIL AIRE-AIRE R-73", "Misiles de corto alcance guiados por infrarrojos para combate cerrado.", glm::vec3(-0.45f, -0.1f, -0.1f), "misiles" },
        { "ALAS INTEGRADAS Y LERX", "Extensiones de borde de ataque que otorgan agilidad y angulos extremos.", glm::vec3(0.55f, -0.05f, -0.2f), "alas" },
        { "TREN RUSTICO", "Tren de aterrizaje de gran absorcion para pistas de tierra sucias.", glm::vec3(0.0f, -0.55f, 0.1f), "tren" }
    }
};

// ============================================================
// PROYECCIÓN 3D A COORDENADAS DE PANTALLA (2D)
// ============================================================

bool project3DToScreen(const glm::vec3& worldPos, const glm::mat4& view, const glm::mat4& proj, int width, int height, float& screenX, float& screenY) {
    glm::vec4 clipPos = proj * view * glm::vec4(worldPos, 1.0f);
    if (clipPos.w <= 0.0f) return false;
    
    glm::vec3 ndc = glm::vec3(clipPos) / clipPos.w;
    if (ndc.x < -1.0f || ndc.x > 1.0f || ndc.y < -1.0f || ndc.y > 1.0f || ndc.z < -1.0f || ndc.z > 1.0f) {
        return false;
    }
    
    screenX = (ndc.x + 1.0f) * 0.5f * (float)width;
    screenY = (1.0f - ndc.y) * 0.5f * (float)height;
    return true;
}

// ============================================================
// DIBUJAR BARRAS DE ESTADÍSTICAS EN EL PANEL (Estilo HUD Militar)
// ============================================================

void drawStatBar(float x, float y, int value, int maxVal, const UIColor& activeColor, const UIColor& inactiveColor) {
    float size = 14.0f; // Tamaño aumentado de los bloques de estadísticas
    float spacing = 6.0f;
    for (int i = 0; i < maxVal; ++i) {
        UIColor color = (i < value) ? activeColor : inactiveColor;
        UIRenderer::drawQuad(x + i * (size + spacing), y, size, size, color);
    }
}

// ============================================================
// DIBUJAR EL PANEL DE ESTADÍSTICAS GENERALES A LA IZQUIERDA
// ============================================================

void drawGeneralStatsPanel(int planeIdx) {
    float h = (float)ctx.windowHeight;
    
    float boxX = 24.0f;
    float boxY = 24.0f;
    float boxW = 360.0f; // Ancho aumentado para dar espacio a la fuente grande
    float boxH = h - 160.0f;
    
    UIColor colorYellow{1.0f, 0.85f, 0.0f, 1.0f};
    UIColor colorCyan{0.0f, 1.0f, 0.85f, 1.0f};
    UIColor colorWhite{1.0f, 1.0f, 1.0f, 1.0f};
    UIColor colorDarkGray{0.2f, 0.2f, 0.2f, 1.0f};
    
    // Panel de fondo
    UIRenderer::drawQuad(boxX, boxY, boxW, boxH, UIColor{0.02f, 0.04f, 0.07f, 0.88f});
    UIRenderer::drawBorder(boxX, boxY, boxW, boxH, 2.0f, colorCyan);
    
    // Decoraciones del HUD
    UIRenderer::drawQuad(boxX, boxY, 12.0f, 3.0f, colorYellow);
    UIRenderer::drawQuad(boxX, boxY, 3.0f, 12.0f, colorYellow);
    UIRenderer::drawQuad(boxX + boxW - 12.0f, boxY, 12.0f, 3.0f, colorYellow);
    UIRenderer::drawQuad(boxX + boxW - 3.0f, boxY, 3.0f, 12.0f, colorYellow);

    float currentY = boxY + 20.0f;
    
    // Título del Avión (Aumentado a 2.4f para excelente legibilidad)
    std::string name = ctx.planes[planeIdx].name;
    UIRenderer::drawText(boxX + 18.0f, currentY, name.c_str(), 2.4f, colorYellow);
    currentY += 40.0f;
    
    // Línea separadora
    UIRenderer::drawQuad(boxX + 18.0f, currentY, boxW - 36.0f, 2.0f, colorCyan);
    currentY += 24.0f;

    // Ficha técnica según el avión (Letras aumentadas a 1.6f, espaciado vertical de 26.0f)
    if (planeIdx == 0) { // A-10
        UIRenderer::drawText(boxX + 18.0f, currentY, "Avion de ataque a tierra.", 1.6f, colorWhite); currentY += 26.0f;
        UIRenderer::drawText(boxX + 18.0f, currentY, "Disenado para soporte cercano", 1.6f, colorWhite); currentY += 26.0f;
        UIRenderer::drawText(boxX + 18.0f, currentY, "y destruir blindados enemigos.", 1.6f, colorWhite); currentY += 38.0f;
        
        UIRenderer::drawText(boxX + 18.0f, currentY, "Vel. Max: 706 km/h", 1.7f, colorYellow); currentY += 30.0f;
        UIRenderer::drawText(boxX + 18.0f, currentY, "Alcance: 1,300 km", 1.7f, colorYellow); currentY += 42.0f;
        
        UIRenderer::drawText(boxX + 18.0f, currentY, "Blindaje:", 1.7f, colorWhite);
        drawStatBar(boxX + 190.0f, currentY + 3.0f, 5, 5, colorCyan, colorDarkGray);
        currentY += 34.0f;
        
        UIRenderer::drawText(boxX + 18.0f, currentY, "Fuego:", 1.7f, colorWhite);
        drawStatBar(boxX + 190.0f, currentY + 3.0f, 5, 5, colorCyan, colorDarkGray);
        currentY += 34.0f;

        UIRenderer::drawText(boxX + 18.0f, currentY, "Agilidad:", 1.7f, colorWhite);
        drawStatBar(boxX + 190.0f, currentY + 3.0f, 3, 5, colorCyan, colorDarkGray);
    }
    else if (planeIdx == 1) { // B-24
        UIRenderer::drawText(boxX + 18.0f, currentY, "Bombardero pesado de gran", 1.6f, colorWhite); currentY += 26.0f;
        UIRenderer::drawText(boxX + 18.0f, currentY, "capacidad y largo alcance de", 1.6f, colorWhite); currentY += 26.0f;
        UIRenderer::drawText(boxX + 18.0f, currentY, "la Segunda Guerra Mundial.", 1.6f, colorWhite); currentY += 38.0f;
        
        UIRenderer::drawText(boxX + 18.0f, currentY, "Vel. Max: 467 km/h", 1.7f, colorYellow); currentY += 30.0f;
        UIRenderer::drawText(boxX + 18.0f, currentY, "Alcance: 3,400 km", 1.7f, colorYellow); currentY += 42.0f;
        
        UIRenderer::drawText(boxX + 18.0f, currentY, "Blindaje:", 1.7f, colorWhite);
        drawStatBar(boxX + 190.0f, currentY + 3.0f, 4, 5, colorCyan, colorDarkGray);
        currentY += 34.0f;
        
        UIRenderer::drawText(boxX + 18.0f, currentY, "Capacidad:", 1.7f, colorWhite);
        drawStatBar(boxX + 190.0f, currentY + 3.0f, 5, 5, colorCyan, colorDarkGray);
        currentY += 34.0f;

        UIRenderer::drawText(boxX + 18.0f, currentY, "Velocidad:", 1.7f, colorWhite);
        drawStatBar(boxX + 190.0f, currentY + 3.0f, 3, 5, colorCyan, colorDarkGray);
    }
    else if (planeIdx == 2) { // Boeing 787
        UIRenderer::drawText(boxX + 18.0f, currentY, "Avion comercial ultra eficiente", 1.6f, colorWhite); currentY += 26.0f;
        UIRenderer::drawText(boxX + 18.0f, currentY, "fabricado principalmente en", 1.6f, colorWhite); currentY += 26.0f;
        UIRenderer::drawText(boxX + 18.0f, currentY, "fibra de carbono.", 1.6f, colorWhite); currentY += 38.0f;
        
        UIRenderer::drawText(boxX + 18.0f, currentY, "Vel. Max: 903 km/h", 1.7f, colorYellow); currentY += 30.0f;
        UIRenderer::drawText(boxX + 18.0f, currentY, "Pasajeros: 242-330", 1.7f, colorYellow); currentY += 42.0f;
        
        UIRenderer::drawText(boxX + 18.0f, currentY, "Eficiencia:", 1.7f, colorWhite);
        drawStatBar(boxX + 190.0f, currentY + 3.0f, 5, 5, colorCyan, colorDarkGray);
        currentY += 34.0f;
        
        UIRenderer::drawText(boxX + 18.0f, currentY, "Confort:", 1.7f, colorWhite);
        drawStatBar(boxX + 190.0f, currentY + 3.0f, 5, 5, colorCyan, colorDarkGray);
        currentY += 34.0f;

        UIRenderer::drawText(boxX + 18.0f, currentY, "Tecnologia:", 1.7f, colorWhite);
        drawStatBar(boxX + 190.0f, currentY + 3.0f, 5, 5, colorCyan, colorDarkGray);
    }
    else if (planeIdx == 3) { // MiG-29
        UIRenderer::drawText(boxX + 18.0f, currentY, "Caza supersonico bimotor de", 1.6f, colorWhite); currentY += 26.0f;
        UIRenderer::drawText(boxX + 18.0f, currentY, "superioridad aerea y alta", 1.6f, colorWhite); currentY += 26.0f;
        UIRenderer::drawText(boxX + 18.0f, currentY, "maniobrabilidad.", 1.6f, colorWhite); currentY += 38.0f;
        
        UIRenderer::drawText(boxX + 18.0f, currentY, "Vel. Max: 2,400 km/h", 1.7f, colorYellow); currentY += 30.0f;
        UIRenderer::drawText(boxX + 18.0f, currentY, "Techo: 18,000 m", 1.7f, colorYellow); currentY += 42.0f;
        
        UIRenderer::drawText(boxX + 18.0f, currentY, "Velocidad:", 1.7f, colorWhite);
        drawStatBar(boxX + 190.0f, currentY + 3.0f, 5, 5, colorCyan, colorDarkGray);
        currentY += 34.0f;
        
        UIRenderer::drawText(boxX + 18.0f, currentY, "Agilidad:", 1.7f, colorWhite);
        drawStatBar(boxX + 190.0f, currentY + 3.0f, 5, 5, colorCyan, colorDarkGray);
        currentY += 34.0f;

        UIRenderer::drawText(boxX + 18.0f, currentY, "Aceleracion:", 1.7f, colorWhite);
        drawStatBar(boxX + 190.0f, currentY + 3.0f, 5, 5, colorCyan, colorDarkGray);
    }
}

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
    if (button == GLFW_MOUSE_BUTTON_LEFT) {
        ctx.mousePressed = (action == GLFW_PRESS);
        
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

    if (key == GLFW_KEY_P) {
        Renderer::togglePartsMode();
        Model* currentModel = Renderer::getLoadedModel();
        if (currentModel) {
            viewZoom = currentModel->getRecommendedZoom();
        }
        return;
    }

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

    if (key == GLFW_KEY_I) viewRotationX += 10.0f;
    if (key == GLFW_KEY_K) viewRotationX -= 10.0f;
    if (key == GLFW_KEY_J) viewRotationY -= 10.0f;
    if (key == GLFW_KEY_L) viewRotationY += 10.0f;

    if (key == GLFW_KEY_Q) viewZoom += 0.5f;
    if (key == GLFW_KEY_E) viewZoom -= 0.5f;
    if (viewZoom > -0.5f) viewZoom = -0.5f;

    if (key == GLFW_KEY_W) viewY += 0.5f;
    if (key == GLFW_KEY_S) viewY -= 0.5f;
    if (key == GLFW_KEY_A) viewX -= 0.5f;
    if (key == GLFW_KEY_D) viewX += 0.5f;

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

    float w = (float)ctx.windowWidth;
    float h = (float)ctx.windowHeight;
    float cx = w / 2.0f;

    // 3. RENDERIZAR HOTSPOTS 3D (Miras holográficas interactivas)
    if (ctx.selectedPlane >= 0 && ctx.selectedPlane < (int)planeHotspots.size()) {
        const auto& hotspots = planeHotspots[ctx.selectedPlane];
        int hoveredIdx = -1;
        float hoveredX = 0.0f;
        float hoveredY = 0.0f;

        // --- SI MODO PIEZAS ESTÁ ACTIVO ---
        if (Renderer::isPartsModeActive()) {
            std::string partName = Renderer::getCurrentPartName();
            std::transform(partName.begin(), partName.end(), partName.begin(), ::tolower);

            // Filtrar y proyectar solo el hotspot que corresponda a la pieza activa
            for (size_t i = 0; i < hotspots.size(); ++i) {
                std::string kw = hotspots[i].keyword;
                std::transform(kw.begin(), kw.end(), kw.begin(), ::tolower);

                if (partName.find(kw) != std::string::npos) {
                    float sx, sy;
                    if (project3DToScreen(hotspots[i].localPos, view, projection, ctx.windowWidth, ctx.windowHeight, sx, sy)) {
                        float dist = std::sqrt(std::pow(ctx.mouseX - sx, 2) + std::pow(ctx.mouseY - sy, 2));
                        bool isHovered = (dist < 18.0f);
                        UIColor color = isHovered ? UIColor{1.0f, 0.85f, 0.0f, 1.0f} : UIColor{0.0f, 1.0f, 0.85f, 1.0f};

                        UIRenderer::drawBorder(sx - 7.0f, sy - 7.0f, 14.0f, 14.0f, 2.0f, color);
                        UIRenderer::drawQuad(sx - 2.0f, sy - 2.0f, 4.0f, 4.0f, color);

                        if (isHovered) {
                            hoveredIdx = (int)i;
                            hoveredX = sx;
                            hoveredY = sy;
                        }
                    }
                    break; 
                }
            }
        } 
        // --- SI EL AVIÓN ESTÁ ENTERO (Modo Piezas apagado) ---
        else {
            drawGeneralStatsPanel(ctx.selectedPlane);
        }

        // Si el cursor está encima de un hotspot de pieza activa, dibujar su tarjeta holográfica con letra más grande
        if (hoveredIdx != -1) {
            const auto& hp = hotspots[hoveredIdx];
            UIColor colorYellow{1.0f, 0.85f, 0.0f, 1.0f};
            UIColor colorCyan{0.0f, 1.0f, 0.85f, 1.0f};

            // Dibujar línea conectora de diagnóstico
            UIRenderer::drawQuad(hoveredX, hoveredY - 1.0f, 30.0f, 2.0f, colorYellow);
            UIRenderer::drawQuad(hoveredX + 30.0f - 2.0f, hoveredY - 3.0f, 6.0f, 6.0f, colorYellow);

            // Ajustar posición del panel flotante (Tamaño aumentado para legibilidad superior)
            float boxW = 380.0f;
            float boxH = 170.0f;
            float boxX = hoveredX + 30.0f;
            float boxY = hoveredY - boxH / 2.0f;

            if (boxX + boxW > w) {
                boxX = hoveredX - 30.0f - boxW;
                UIRenderer::drawQuad(hoveredX - 30.0f, hoveredY - 1.0f, 30.0f, 2.0f, colorYellow);
                UIRenderer::drawQuad(hoveredX - 30.0f - 4.0f, hoveredY - 3.0f, 6.0f, 6.0f, colorYellow);
            }
            if (boxY < 10.0f) boxY = 10.0f;
            if (boxY + boxH > h - 10.0f) boxY = h - boxH - 10.0f;

            // Fondo translúcido (cristal holográfico)
            UIRenderer::drawQuad(boxX, boxY, boxW, boxH, UIColor{0.02f, 0.04f, 0.07f, 0.88f});
            UIRenderer::drawBorder(boxX, boxY, boxW, boxH, 2.0f, colorCyan);

            // DetallesHUD esquineros
            UIRenderer::drawQuad(boxX, boxY, 12.0f, 3.0f, colorYellow);
            UIRenderer::drawQuad(boxX, boxY, 3.0f, 12.0f, colorYellow);
            UIRenderer::drawQuad(boxX + boxW - 12.0f, boxY, 12.0f, 3.0f, colorYellow);
            UIRenderer::drawQuad(boxX + boxW - 3.0f, boxY, 3.0f, 12.0f, colorYellow);

            // Título de la pieza (Letra grande 2.2f)
            UIRenderer::drawText(boxX + 16.0f, boxY + 18.0f, hp.title.c_str(), 2.2f, colorYellow);

            // Descripción (Letra 1.6f para legibilidad excelente, formateada en hasta 3 líneas dinámicas)
            std::string desc = hp.description;
            if (desc.length() > 26) {
                std::string desc1 = desc.substr(0, 26);
                std::string desc2 = desc.substr(26);
                size_t space = desc1.find_last_of(" ");
                if (space != std::string::npos && space > 10) {
                    desc1 = desc.substr(0, space);
                    desc2 = desc.substr(space + 1);
                }
                UIRenderer::drawText(boxX + 16.0f, boxY + 54.0f, desc1.c_str(), 1.6f, UIColor{0.85f, 0.9f, 0.95f, 1.0f});
                if (desc2.length() > 26) {
                    size_t space2 = desc2.substr(0, 26).find_last_of(" ");
                    if (space2 != std::string::npos && space2 > 10) {
                        std::string desc3 = desc2.substr(space2 + 1);
                        desc2 = desc2.substr(0, space2);
                        UIRenderer::drawText(boxX + 16.0f, boxY + 88.0f, desc2.c_str(), 1.6f, UIColor{0.85f, 0.9f, 0.95f, 1.0f});
                        if (desc3.length() > 26) desc3 = desc3.substr(0, 22) + "...";
                        UIRenderer::drawText(boxX + 16.0f, boxY + 122.0f, desc3.c_str(), 1.6f, UIColor{0.85f, 0.9f, 0.95f, 1.0f});
                    } else {
                        desc2 = desc2.substr(0, 22) + "...";
                        UIRenderer::drawText(boxX + 16.0f, boxY + 88.0f, desc2.c_str(), 1.6f, UIColor{0.85f, 0.9f, 0.95f, 1.0f});
                    }
                } else {
                    UIRenderer::drawText(boxX + 16.0f, boxY + 88.0f, desc2.c_str(), 1.6f, UIColor{0.85f, 0.9f, 0.95f, 1.0f});
                }
            } else {
                UIRenderer::drawText(boxX + 16.0f, boxY + 54.0f, desc.c_str(), 1.6f, UIColor{0.85f, 0.9f, 0.95f, 1.0f});
            }
        }
    }

    // 4. Renderizar HUD de interfaz de usuario encima usando UIRenderer
    // Nombre del modelo o pieza (Letra grande 2.4f, centrada)
    if (Renderer::isPartsModeActive()) {
        std::string partName = Renderer::getCurrentPartName();
        int currentIdx = Renderer::getCurrentPartIndex();
        int total = Renderer::getNumParts();
        std::string partHUD = partName + " (" + std::to_string(currentIdx + 1) + "/" + std::to_string(total) + ")";
        UIRenderer::drawText(cx - 175.0f, 24.0f, partHUD.c_str(), 2.4f, UIColor{1.0f, 1.0f, 1.0f, 1.0f});
    } else {
        UIRenderer::drawText(cx - 175.0f, 24.0f, ctx.planes[ctx.selectedPlane].name.c_str(), 2.4f, UIColor{1.0f, 1.0f, 1.0f, 1.0f});
    }

    // Botón Volver (Colocado al fondo de la barra de información izquierda)
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

    // Dibujar ayuda en la derecha (Para no solapar con el panel de info izquierdo)
    if (ctx.showHelp) {
        float boxW = 380.0f, boxH = 240.0f; // Tamaño aumentado de ayuda
        float boxX = w - boxW - 24.0f, boxY = 24.0f;
        UIRenderer::drawQuad(boxX, boxY, boxW, boxH, UIColor{ 0.05f, 0.05f, 0.08f, 0.85f });
        UIRenderer::drawBorder(boxX, boxY, boxW, boxH, 2.0f, UIColor{ 0.3f, 0.3f, 0.3f, 1.0f });

        float lineY = boxY + 20.0f;
        UIRenderer::drawText(boxX + 20.0f, lineY, "Controles:", 1.8f, UIColor{1, 1, 1, 1}); lineY += 34.0f;
        UIRenderer::drawText(boxX + 20.0f, lineY, "Arrastrar Click - Rotar", 1.5f, UIColor{0.7f, 0.7f, 0.7f, 1.0f}); lineY += 30.0f;
        UIRenderer::drawText(boxX + 20.0f, lineY, "Scroll / Q/E - Zoom", 1.5f, UIColor{0.7f, 0.7f, 0.7f, 1.0f}); lineY += 30.0f;
        UIRenderer::drawText(boxX + 20.0f, lineY, "WASD - Paneo", 1.5f, UIColor{0.7f, 0.7f, 0.7f, 1.0f}); lineY += 30.0f;
        UIRenderer::drawText(boxX + 20.0f, lineY, "P - Modo Piezas", 1.5f, UIColor{0.7f, 0.7f, 0.7f, 1.0f}); lineY += 30.0f;
        UIRenderer::drawText(boxX + 20.0f, lineY, "<- / -> - Cambiar Pieza", 1.5f, UIColor{0.7f, 0.7f, 0.7f, 1.0f});
    } else {
        UIRenderer::drawText(w - 440.0f, h - 30.0f, "Presiona H para ver los controles", 1.6f, UIColor{0.5f, 0.5f, 0.5f, 1.0f});
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
