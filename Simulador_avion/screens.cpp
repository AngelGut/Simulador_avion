#include "screens.h"
#include "ui_renderer.h"

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

void Screens::renderLoading(AppContext& ctx) {
    float w = (float)ctx.windowWidth;
    float h = (float)ctx.windowHeight;
    float cx = w / 2.0f;

    ctx.loadingProgress += ctx.deltaTime * 0.4f;
    if (ctx.loadingProgress >= 1.0f) {
        ctx.loadingProgress = 1.0f;
        ctx.assetsLoaded = true;
        ctx.state = AppState::WELCOME;
    }

    // Título
    drawCentered("Visualizador de Aviones", cx, h / 2.0f - 130.0f, 3.0f, white);
    // Subtítulo
    drawCentered("Cargando modelos 3D...", cx, h / 2.0f - 80.0f, 1.5f, gray);

    // Barra de progreso con borde
    float barWidth = 420.0f, barHeight = 22.0f;
    float barX = cx - barWidth / 2.0f;
    float barY = h / 2.0f;

    UIRenderer::drawBorder(barX - 2, barY - 2, barWidth + 4, barHeight + 4, 2.0f, grayDim);
    UIRenderer::drawQuad(barX, barY, barWidth, barHeight, barBg);
    UIRenderer::drawQuad(barX, barY, barWidth * ctx.loadingProgress, barHeight, accent);

    // Porcentaje
    std::string percent = std::to_string((int)(ctx.loadingProgress * 100)) + "%";
    drawCentered(percent, cx, barY + 40.0f, 1.5f, gray);

    // Footer
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

    // Título + subtítulo
    drawCentered("Selecciona un avion", cx, 70.0f, 3.0f, white);
    drawCentered("Elige un modelo para comenzar la exploracion", cx, 115.0f, 1.5f, gray);

    // Tarjetas
    int count = (int)ctx.planes.size();
    float cardW = 320.0f, cardH = 240.0f;
    float gap = 50.0f;
    float totalWidth = count * cardW + (count - 1) * gap;
    float startX = cx - totalWidth / 2.0f;
    float cardY = h / 2.0f - cardH / 2.0f + 30.0f;

    for (int i = 0; i < count; i++) {
        float cardX = startX + i * (cardW + gap);
        bool hovered = (ctx.mouseX >= cardX && ctx.mouseX <= cardX + cardW &&
            ctx.mouseY >= cardY && ctx.mouseY <= cardY + cardH);

        // Fondo de tarjeta + borde (se resalta en hover)
        UIRenderer::drawQuad(cardX, cardY, cardW, cardH, cardBg);
        UIRenderer::drawBorder(cardX, cardY, cardW, cardH, 2.0f, hovered ? accent : grayDim);

        // "Placeholder" de imagen: un recuadro donde luego irá el preview 3D
        float thumbMargin = 24.0f;
        float thumbH = 130.0f;
        UIRenderer::drawQuad(cardX + thumbMargin, cardY + thumbMargin,
            cardW - thumbMargin * 2, thumbH,
            UIColor{ 0.09f, 0.09f, 0.13f, 1.0f });
        drawCentered("Vista previa 3D", cardX + cardW / 2.0f, cardY + thumbMargin + thumbH / 2.0f - 6.0f, 1.2f, grayDim);

        // Nombre del avión
        drawCentered(ctx.planes[i].name, cardX + cardW / 2.0f, cardY + thumbMargin + thumbH + 20.0f, 1.6f, white);

        // Click en toda la tarjeta
        if (hovered && ctx.mousePressed) {
            ctx.selectedPlane = i;
            ctx.state = AppState::VIEWER;
        }
    }

    // Footer
    drawCentered("Universidad Central Del Este", cx, h - 30.0f, 1.3f, grayDim);
}