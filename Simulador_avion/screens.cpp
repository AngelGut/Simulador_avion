#include "screens.h"
#include "ui_renderer.h"
#include "model_renderer.h"
#include <GLFW/glfw3.h>
#include <glm/gtc/matrix_transform.hpp>
#include <algorithm>

namespace {
    UIColor bg{ 0.09f, 0.09f, 0.13f, 1.0f };
    UIColor accent{ 0.2f, 0.5f, 0.9f, 1.0f };
    UIColor accentHover{ 0.3f, 0.6f, 1.0f, 1.0f };
    UIColor white{ 1.0f, 1.0f, 1.0f, 1.0f };
    UIColor gray{ 0.55f, 0.55f, 0.62f, 1.0f };
    UIColor grayDim{ 0.35f, 0.35f, 0.42f, 1.0f };
    UIColor barBg{ 0.2f, 0.2f, 0.25f, 1.0f };
    UIColor cardBg{ 0.14f, 0.14f, 0.19f, 1.0f };

    void drawCentered(const std::string& text, float centerX, float y, float scale, UIColor color) {
        float w = UIRenderer::getTextWidth(text, scale);
        UIRenderer::drawText(centerX - w / 2.0f, y, text, scale, color);
    }
}

namespace {
    void initCameraForModel(AppContext& ctx, int planeIndex) {
        if (planeIndex < 0 || planeIndex >= (int)ctx.gpuModels.size()) return;
        GLModel& m = ctx.gpuModels[planeIndex];

        ctx.cameraTarget = m.center;
        ctx.cameraDistance = m.radius * 2.5f;
        ctx.cameraYaw = 0.0f;

        ctx.cameraDefaultTarget = ctx.cameraTarget;
        ctx.cameraDefaultDistance = ctx.cameraDistance;
        ctx.cameraDefaultYaw = ctx.cameraYaw;
    }
}

void Screens::renderViewer(AppContext& ctx, GLFWwindow* window) {
    float w = (float)ctx.windowWidth;
    float h = (float)ctx.windowHeight;
    float cx = w / 2.0f;

    if (ctx.selectedPlane < 0 || ctx.selectedPlane >= (int)ctx.gpuModels.size()) {
        ctx.state = AppState::MENU;
        return;
    }

    GLModel& model = ctx.gpuModels[ctx.selectedPlane];
    float minDist = model.radius * 0.4f;
    float maxDist = model.radius * 8.0f;
    float panSpeed = model.radius * 0.8f;
    float zoomSpeed = model.radius * 1.2f;
    float rotSpeed = 1.5f; // radianes por segundo

    // --- Input de cámara (mientras la tecla está sostenida) ---
    glm::vec3 right(cos(ctx.cameraYaw), 0.0f, -sin(ctx.cameraYaw));
    glm::vec3 worldUp(0.0f, 1.0f, 0.0f);

    if (glfwGetKey(window, GLFW_KEY_A) == GLFW_PRESS) ctx.cameraTarget -= right * panSpeed * ctx.deltaTime;
    if (glfwGetKey(window, GLFW_KEY_D) == GLFW_PRESS) ctx.cameraTarget += right * panSpeed * ctx.deltaTime;
    if (glfwGetKey(window, GLFW_KEY_W) == GLFW_PRESS) ctx.cameraTarget += worldUp * panSpeed * ctx.deltaTime;
    if (glfwGetKey(window, GLFW_KEY_S) == GLFW_PRESS) ctx.cameraTarget -= worldUp * panSpeed * ctx.deltaTime;

    if (glfwGetKey(window, GLFW_KEY_Q) == GLFW_PRESS) ctx.cameraDistance -= zoomSpeed * ctx.deltaTime;
    if (glfwGetKey(window, GLFW_KEY_E) == GLFW_PRESS) ctx.cameraDistance += zoomSpeed * ctx.deltaTime;
    ctx.cameraDistance = glm::clamp(ctx.cameraDistance, minDist, maxDist);

    if (glfwGetKey(window, GLFW_KEY_R) == GLFW_PRESS) ctx.cameraYaw -= rotSpeed * ctx.deltaTime;
    if (glfwGetKey(window, GLFW_KEY_T) == GLFW_PRESS) ctx.cameraYaw += rotSpeed * ctx.deltaTime;

    if (glfwGetKey(window, GLFW_KEY_SPACE) == GLFW_PRESS) {
        ctx.cameraTarget = ctx.cameraDefaultTarget;
        ctx.cameraDistance = ctx.cameraDefaultDistance;
        ctx.cameraYaw = ctx.cameraDefaultYaw;
    }

    // --- Construir cámara orbital ---
    glm::vec3 eyeOffset(sin(ctx.cameraYaw) * ctx.cameraDistance,
        ctx.cameraDistance * 0.3f,
        cos(ctx.cameraYaw) * ctx.cameraDistance);
    glm::vec3 eye = ctx.cameraTarget + eyeOffset;
    glm::mat4 view = glm::lookAt(eye, ctx.cameraTarget, glm::vec3(0, 1, 0));

    float aspect = (h > 0.0f) ? (w / h) : 1.0f;
    float nearPlane = (model.radius > 0.001f) ? (model.radius * 0.01f) : 0.1f;
    float farPlane = 500.0f; // Evitar que el plano lejano recorte la escenografía del hangar
    glm::mat4 proj = glm::perspective(glm::radians(45.0f), aspect, nearPlane, farPlane);

    // --- Render 3D a pantalla completa ---
    ModelRenderer::renderModel(model, view, proj, 0, 0, w, h, ctx.windowWidth, ctx.windowHeight);

    // --- HUD encima ---
    drawCentered(ctx.planes[ctx.selectedPlane].name, cx, 24.0f, 2.2f, white);

    // Botón Volver
    float backW = 140.0f, backH = 44.0f;
    bool backClicked = UIRenderer::drawButton(
        24.0f, h - 68.0f, backW, backH, "Volver",
        ctx.mouseX, ctx.mouseY, ctx.mousePressed,
        UIColor{ 0.3f, 0.3f, 0.35f, 1.0f }, UIColor{ 0.4f, 0.4f, 0.46f, 1.0f }
    );
    if (backClicked) ctx.state = AppState::MENU;

    // Toggle de ayuda con tecla H (detección de un solo press, no continua)
    static bool hWasPressed = false;
    bool hPressed = glfwGetKey(window, GLFW_KEY_H) == GLFW_PRESS;
    if (hPressed && !hWasPressed) ctx.showHelp = !ctx.showHelp;
    hWasPressed = hPressed;

    if (ctx.showHelp) {
        float boxW = 320.0f, boxH = 190.0f;
        float boxX = w - boxW - 24.0f, boxY = 24.0f;
        UIRenderer::drawQuad(boxX, boxY, boxW, boxH, UIColor{ 0.05f, 0.05f, 0.08f, 0.85f });
        UIRenderer::drawBorder(boxX, boxY, boxW, boxH, 2.0f, grayDim);

        float lineY = boxY + 16.0f;
        UIRenderer::drawText(boxX + 16.0f, lineY, "Controles:", 1.4f, white); lineY += 26.0f;
        UIRenderer::drawText(boxX + 16.0f, lineY, "WASD - Mover vista", 1.2f, gray); lineY += 22.0f;
        UIRenderer::drawText(boxX + 16.0f, lineY, "Q/E - Zoom", 1.2f, gray); lineY += 22.0f;
        UIRenderer::drawText(boxX + 16.0f, lineY, "R/T - Rotar", 1.2f, gray); lineY += 22.0f;
        UIRenderer::drawText(boxX + 16.0f, lineY, "Espacio - Reiniciar vista", 1.2f, gray); lineY += 22.0f;
        UIRenderer::drawText(boxX + 16.0f, lineY, "H - Mostrar/ocultar ayuda", 1.2f, gray); lineY += 22.0f;
        UIRenderer::drawText(boxX + 16.0f, lineY, "ESC - Salir", 1.2f, gray);
    }
    else {
        drawCentered("Presiona H para ver los controles", cx, h - 30.0f, 1.3f, grayDim);
    }
}

void Screens::renderLoading(AppContext& ctx) {
    float w = (float)ctx.windowWidth;
    float h = (float)ctx.windowHeight;
    float cx = w / 2.0f;

    int total = (int)ctx.planes.size();
    int loaded = ctx.modelsLoadedCount.load();
    ctx.loadingProgress = total > 0 ? (float)loaded / (float)total : 0.0f;

    if (ctx.loadingDone.load()) {
        if (!ctx.modelsUploaded) {
            ModelRenderer::uploadAllModels(ctx);
            ctx.modelsUploaded = true;
        }
        ctx.assetsLoaded = true;
        ctx.state = AppState::WELCOME;
    }

    drawCentered("Visualizador de Aviones", cx, h / 2.0f - 130.0f, 3.0f, white);
    drawCentered("Cargando modelos 3D...", cx, h / 2.0f - 80.0f, 1.5f, gray);

    float barWidth = 420.0f, barHeight = 22.0f;
    float barX = cx - barWidth / 2.0f;
    float barY = h / 2.0f;

    UIRenderer::drawBorder(barX - 2, barY - 2, barWidth + 4, barHeight + 4, 2.0f, grayDim);
    UIRenderer::drawQuad(barX, barY, barWidth, barHeight, barBg);
    UIRenderer::drawQuad(barX, barY, barWidth * ctx.loadingProgress, barHeight, accent);

    std::string countText = std::to_string(loaded) + "/" + std::to_string(total) + " modelos";
    drawCentered(countText, cx, barY + 40.0f, 1.5f, gray);

    drawCentered("Universidad Central Del Este", cx, h - 40.0f, 1.3f, grayDim);
}

void Screens::renderWelcome(AppContext& ctx) {
    float w = (float)ctx.windowWidth;
    float h = (float)ctx.windowHeight;
    float cx = w / 2.0f;

    // Título
    drawCentered("Visualizador de Aviones", cx, h / 2.0f - 180.0f, 4.0f, white);

    // Línea divisoria decorativa bajo el título
    float lineWidth = 300.0f;
    UIRenderer::drawQuad(cx - lineWidth / 2.0f, h / 2.0f - 105.0f, lineWidth, 3.0f, accent);

    // Subtítulo descriptivo
    drawCentered("Explora modelos 3D de aeronaves en detalle", cx, h / 2.0f - 75.0f, 1.6f, gray);

    // Botón Comenzar (con borde)
    float btnW = 240.0f, btnH = 64.0f;
    float btnX = cx - btnW / 2.0f;
    float btnY = h / 2.0f - 10.0f;

    UIRenderer::drawBorder(btnX - 3, btnY - 3, btnW + 6, btnH + 6, 2.0f, accent);
    bool clicked = UIRenderer::drawButton(
        btnX, btnY, btnW, btnH, "Comenzar",
        ctx.mouseX, ctx.mouseY, ctx.mousePressed,
        accent, accentHover
    );
    if (clicked) ctx.state = AppState::MENU;

    std::string uniText = "Universidad Central Del Este";
    float uniWidth = UIRenderer::getTextWidth(uniText, 1.4f);
    UIRenderer::drawText(cx - uniWidth / 2.0f, h - 36.0f, uniText, 1.4f, grayDim);

    // Esquina inferior derecha: versión/año (detalle de pulido)
    std::string version = "v0.1";
    float vw = UIRenderer::getTextWidth(version, 1.2f);
    UIRenderer::drawText(w - vw - 24.0f, h - 32.0f, version, 1.2f, grayDim);
}

void Screens::renderMenu(AppContext& ctx) {
    float w = (float)ctx.windowWidth;
    float h = (float)ctx.windowHeight;
    float cx = w / 2.0f;

    // Fondo plano (sin gradiente, versión simple)
    UIRenderer::drawQuad(0, 0, w, h, bg);

    drawCentered("Selecciona un avion", cx, 70.0f, 3.0f, white);
    drawCentered("Elige un modelo para comenzar la exploracion", cx, 115.0f, 1.5f, gray);

    int count = (int)ctx.planes.size();
    float sideMargin = 60.0f;
    float gap = 40.0f;
    float availableWidth = w - sideMargin * 2.0f;
    float cardW = (availableWidth - gap * (count - 1)) / (float)count;
    cardW = std::min(cardW, 320.0f);
    float totalWidth = count * cardW + (count - 1) * gap;
    float startX = cx - totalWidth / 2.0f;
    float cardH = 240.0f;
    float cardY = h / 2.0f - cardH / 2.0f + 30.0f;

    for (int i = 0; i < count; i++) {
        float cardX = startX + i * (cardW + gap);
        bool hovered = (ctx.mouseX >= cardX && ctx.mouseX <= cardX + cardW &&
            ctx.mouseY >= cardY && ctx.mouseY <= cardY + cardH);

        UIRenderer::drawQuad(cardX, cardY, cardW, cardH, cardBg);
        UIRenderer::drawBorder(cardX, cardY, cardW, cardH, 2.0f, hovered ? accent : grayDim);

        float thumbMargin = 20.0f;
        float thumbH = 130.0f;
        float thumbX = cardX + thumbMargin;
        float thumbY = cardY + thumbMargin;
        float thumbW = cardW - thumbMargin * 2;

        if (i < (int)ctx.gpuModels.size() && ctx.gpuModels[i].ready) {
            ModelRenderer::renderPreview(ctx.gpuModels[i], thumbX, thumbY, thumbW, thumbH,
                ctx.windowWidth, ctx.windowHeight, ctx.totalTime);
        }
        else {
            UIRenderer::drawQuad(thumbX, thumbY, thumbW, thumbH, UIColor{ 0.09f, 0.09f, 0.13f, 1.0f });
            drawCentered("No disponible", cardX + cardW / 2.0f, thumbY + thumbH / 2.0f - 6.0f, 1.2f, grayDim);
        }

        drawCentered(ctx.planes[i].name, cardX + cardW / 2.0f, cardY + thumbMargin + thumbH + 20.0f, 1.5f, white);

        if (hovered && ctx.mousePressed) {
            ctx.selectedPlane = i;
            initCameraForModel(ctx, i);
            ctx.state = AppState::VIEWER;
        }
    }

    // Botón: volver a la pantalla de bienvenida
    float backW = 140.0f, backH = 44.0f;
    bool backClicked = UIRenderer::drawButton(
        24.0f, h - 68.0f, backW, backH, "Volver",
        ctx.mouseX, ctx.mouseY, ctx.mousePressed,
        UIColor{ 0.3f, 0.3f, 0.35f, 1.0f }, UIColor{ 0.4f, 0.4f, 0.46f, 1.0f }
    );
    if (backClicked) ctx.state = AppState::WELCOME;

    drawCentered("Universidad Central Del Este", cx, h - 30.0f, 1.3f, grayDim);
}