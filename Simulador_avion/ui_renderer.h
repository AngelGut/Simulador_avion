#pragma once
#include <glm/glm.hpp>
#include <string>
#include <vector>

struct UIColor { float r, g, b, a; };

namespace UIRenderer {
    void init(int screenWidth, int screenHeight);
    void resize(int screenWidth, int screenHeight);
    void shutdown();

    // Rectángulo relleno (botones, fondos, barras de progreso)
    void drawQuad(float x, float y, float w, float h, UIColor color);

    // Texto usando stb_easy_font (x,y = esquina superior izquierda, scale = tamaño)
    void drawText(float x, float y, const std::string& text, float scale, UIColor color);

    // ui_renderer.h
    void drawBorder(float x, float y, float w, float h, float thickness, UIColor color);

    // Botón: quad + texto centrado + retorna true si fue clickeado este frame
    bool drawButton(float x, float y, float w, float h,
        const std::string& label,
        double mouseX, double mouseY, bool mousePressed,
        UIColor baseColor, UIColor hoverColor);

    // Botón de audio compacto con icono de bocina (Normal vs Ensordecida con X roja)
    bool drawAudioButton(float x, float y, float w, float h,
        bool isMuted, double mouseX, double mouseY, bool mousePressed);

    float getTextWidth(const std::string& text, float scale);

    // Grafico de simulacion 2D
    void drawSimGraph(float x, float y, float w, float h, 
                      const std::vector<float>& history, 
                      float maxVal, const std::string& title, 
                      const std::string& unit);
}
