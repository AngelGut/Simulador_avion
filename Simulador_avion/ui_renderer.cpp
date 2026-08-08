#include "ui_renderer.h"
#include <glad/glad.h>   // o el loader de GL que estés usando (GLEW/GLAD)
#include <glm/gtc/matrix_transform.hpp>
#define STB_EASY_FONT_IMPLEMENTATION
#include "stb_easy_font.h"
#include <vector>

namespace {
    GLuint uiShaderProgram;
    GLuint quadVAO, quadVBO;
    glm::mat4 projection;

    const char* vertexSrc = R"(
        #version 330 core
        layout(location = 0) in vec2 aPos;
        uniform mat4 uProjection;
        uniform mat4 uModel;
        void main() {
            gl_Position = uProjection * uModel * vec4(aPos, 0.0, 1.0);
        }
    )";

    const char* fragmentSrc = R"(
        #version 330 core
        out vec4 FragColor;
        uniform vec4 uColor;
        void main() {
            FragColor = uColor;
        }
    )";

    GLuint compileShader(GLenum type, const char* src) {
        GLuint shader = glCreateShader(type);
        glShaderSource(shader, 1, &src, nullptr);
        glCompileShader(shader);
        return shader;
    }
}

void UIRenderer::init(int screenWidth, int screenHeight) {
    GLuint vs = compileShader(GL_VERTEX_SHADER, vertexSrc);
    GLuint fs = compileShader(GL_FRAGMENT_SHADER, fragmentSrc);
    uiShaderProgram = glCreateProgram();
    glAttachShader(uiShaderProgram, vs);
    glAttachShader(uiShaderProgram, fs);
    glLinkProgram(uiShaderProgram);
    glDeleteShader(vs);
    glDeleteShader(fs);

    // Quad unitario (0,0) a (1,1), lo escalamos con la matriz modelo
    float vertices[] = {
        0.0f, 0.0f,
        1.0f, 0.0f,
        1.0f, 1.0f,
        0.0f, 1.0f
    };

    glGenVertexArrays(1, &quadVAO);
    glGenBuffers(1, &quadVBO);
    glBindVertexArray(quadVAO);
    glBindBuffer(GL_ARRAY_BUFFER, quadVBO);
    glBufferData(GL_ARRAY_BUFFER, sizeof(vertices), vertices, GL_STATIC_DRAW);
    glVertexAttribPointer(0, 2, GL_FLOAT, GL_FALSE, 2 * sizeof(float), (void*)0);
    glEnableVertexAttribArray(0);
    glBindVertexArray(0);

    resize(screenWidth, screenHeight);
}

void UIRenderer::shutdown() {
    glDeleteVertexArrays(1, &quadVAO);
    glDeleteBuffers(1, &quadVBO);
    glDeleteProgram(uiShaderProgram);
}

float UIRenderer::getTextWidth(const std::string& text, float scale) {
    return stb_easy_font_width((char*)text.c_str()) * scale;
}

// ui_renderer.cpp
void UIRenderer::drawBorder(float x, float y, float w, float h, float thickness, UIColor color) {
    drawQuad(x, y, w, thickness, color);                          // arriba
    drawQuad(x, y + h - thickness, w, thickness, color);           // abajo
    drawQuad(x, y, thickness, h, color);                           // izquierda
    drawQuad(x + w - thickness, y, thickness, h, color);           // derecha
}

void UIRenderer::resize(int screenWidth, int screenHeight) {
    // Origen (0,0) arriba-izquierda, como coordenadas de pantalla típicas de UI
    projection = glm::ortho(0.0f, (float)screenWidth, (float)screenHeight, 0.0f, -1.0f, 1.0f);
}

void UIRenderer::drawQuad(float x, float y, float w, float h, UIColor color) {
    glUseProgram(uiShaderProgram);

    glm::mat4 model = glm::mat4(1.0f);
    model = glm::translate(model, glm::vec3(x, y, 0.0f));
    model = glm::scale(model, glm::vec3(w, h, 1.0f));

    glUniformMatrix4fv(glGetUniformLocation(uiShaderProgram, "uProjection"), 1, GL_FALSE, &projection[0][0]);
    glUniformMatrix4fv(glGetUniformLocation(uiShaderProgram, "uModel"), 1, GL_FALSE, &model[0][0]);
    glUniform4f(glGetUniformLocation(uiShaderProgram, "uColor"), color.r, color.g, color.b, color.a);

    glBindVertexArray(quadVAO);
    glDrawArrays(GL_TRIANGLE_FAN, 0, 4);
    glBindVertexArray(0);
}

void drawTextRaw(float x, float y, const std::string& text, float scale, UIColor color) {
    static char buffer[99999];
    int numQuads = stb_easy_font_print(0, 0, (char*)text.c_str(), nullptr, buffer, sizeof(buffer));

    if (numQuads == 0) return;

    // --- Generar índices: 2 triángulos por quad ---
    std::vector<unsigned int> indices;
    indices.reserve(numQuads * 6);
    for (int i = 0; i < numQuads; i++) {
        unsigned int base = i * 4;
        // Triángulo 1: 0,1,2
        indices.push_back(base + 0);
        indices.push_back(base + 1);
        indices.push_back(base + 2);
        // Triángulo 2: 2,3,0
        indices.push_back(base + 2);
        indices.push_back(base + 3);
        indices.push_back(base + 0);
    }

    glUseProgram(uiShaderProgram);

    glm::mat4 model = glm::mat4(1.0f);
    model = glm::translate(model, glm::vec3(x, y, 0.0f));
    model = glm::scale(model, glm::vec3(scale, scale, 1.0f));

    glUniformMatrix4fv(glGetUniformLocation(uiShaderProgram, "uProjection"), 1, GL_FALSE, &projection[0][0]);
    glUniformMatrix4fv(glGetUniformLocation(uiShaderProgram, "uModel"), 1, GL_FALSE, &model[0][0]);
    glUniform4f(glGetUniformLocation(uiShaderProgram, "uColor"), color.r, color.g, color.b, color.a);

    GLuint vao, vbo, ebo;
    glGenVertexArrays(1, &vao);
    glGenBuffers(1, &vbo);
    glGenBuffers(1, &ebo);

    glBindVertexArray(vao);

    glBindBuffer(GL_ARRAY_BUFFER, vbo);
    glBufferData(GL_ARRAY_BUFFER, numQuads * 4 * 16, buffer, GL_STREAM_DRAW);
    // stride = 16 bytes (3 floats x,y,z + 4 bytes de color), solo usamos x,y
    glVertexAttribPointer(0, 2, GL_FLOAT, GL_FALSE, 16, (void*)0);
    glEnableVertexAttribArray(0);

    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, ebo);
    glBufferData(GL_ELEMENT_ARRAY_BUFFER, indices.size() * sizeof(unsigned int), indices.data(), GL_STREAM_DRAW);

    glDrawElements(GL_TRIANGLES, (GLsizei)indices.size(), GL_UNSIGNED_INT, 0);

    glBindVertexArray(0);
    glDeleteVertexArrays(1, &vao);
    glDeleteBuffers(1, &vbo);
    glDeleteBuffers(1, &ebo);
}

void UIRenderer::drawText(float x, float y, const std::string& text, float scale, UIColor color) {
    float offset = 0.3f * scale;
    float shadowOffset = 0.6f * scale;
    
    // 1. Sombra negra (efecto negrita para la sombra)
    UIColor shadowColor{0.0f, 0.0f, 0.0f, 0.75f};
    drawTextRaw(x + shadowOffset + offset, y + shadowOffset, text, scale, shadowColor);
    drawTextRaw(x + shadowOffset, y + shadowOffset + offset, text, scale, shadowColor);
    drawTextRaw(x + shadowOffset, y + shadowOffset, text, scale, shadowColor);

    // 2. Texto principal en negrita (bolded)
    drawTextRaw(x + offset, y, text, scale, color);
    drawTextRaw(x, y + offset, text, scale, color);
    drawTextRaw(x + offset, y + offset, text, scale, color);
    drawTextRaw(x, y, text, scale, color);
}

bool UIRenderer::drawButton(float x, float y, float w, float h,
    const std::string& label,
    double mouseX, double mouseY, bool mousePressed,
    UIColor baseColor, UIColor hoverColor) {
    bool hovered = (mouseX >= x && mouseX <= x + w && mouseY >= y && mouseY <= y + h);
    drawQuad(x, y, w, h, hovered ? hoverColor : baseColor);

    float textScale = 2.0f;
    float approxTextWidth = getTextWidth(label, textScale);
    float textX = x + (w - approxTextWidth) / 2.0f;
    float textY = y + (h - 8.0f * textScale) / 2.0f;
    drawText(textX, textY, label, textScale, UIColor{ 1,1,1,1 });

    return hovered && mousePressed;
}

bool UIRenderer::drawAudioButton(float x, float y, float w, float h,
    bool isMuted, double mouseX, double mouseY, bool mousePressed) {
    bool hovered = (mouseX >= x && mouseX <= x + w && mouseY >= y && mouseY <= y + h);

    UIColor bg = hovered ? (isMuted ? UIColor{ 0.35f, 0.35f, 0.40f, 1.0f } : UIColor{ 0.25f, 0.70f, 0.95f, 1.0f })
                         : (isMuted ? UIColor{ 0.20f, 0.20f, 0.25f, 1.0f } : UIColor{ 0.15f, 0.55f, 0.85f, 1.0f });

    drawQuad(x, y, w, h, bg);
    drawBorder(x, y, w, h, 2.0f, isMuted ? UIColor{ 0.45f, 0.45f, 0.50f, 1.0f } : UIColor{ 0.40f, 0.80f, 1.00f, 1.0f });

    // Dibujar icono 2D de bocina
    float iconSize = 20.0f;
    float ix = x + 10.0f;
    float iy = y + (h - iconSize) / 2.0f;

    UIColor iconColor = isMuted ? UIColor{ 0.75f, 0.75f, 0.80f, 1.0f } : UIColor{ 1.0f, 1.0f, 1.0f, 1.0f };

    // 1. Cuerpo rectangular de la bocina
    drawQuad(ix, iy + 5.0f, 5.0f, 10.0f, iconColor);

    // 2. Cono trapezoidal de la bocina
    for (int step = 0; step < 6; step++) {
        float stepX = ix + 5.0f + step * 1.2f;
        float stepH = 10.0f + step * 1.8f;
        float stepY = iy + (iconSize - stepH) / 2.0f;
        drawQuad(stepX, stepY, 1.3f, stepH, iconColor);
    }

    if (!isMuted) {
        // Ondas de sonido cuando está sonando (Sound waves ON)
        float wx = ix + 15.0f;
        drawQuad(wx, iy + 6.0f, 2.0f, 8.0f, iconColor);
        drawQuad(wx + 4.0f, iy + 3.0f, 2.0f, 14.0f, iconColor);
    } else {
        // X roja sobre la bocina (Muted Speaker OFF)
        UIColor redSlash{ 0.95f, 0.25f, 0.25f, 1.0f };
        float sx = ix - 2.0f;
        float sy = iy - 2.0f;
        float sSize = iconSize + 4.0f;

        for (int i = 0; i < (int)sSize; i += 2) {
            drawQuad(sx + i, sy + i, 2.5f, 2.5f, redSlash);
            drawQuad(sx + sSize - i, sy + i, 2.5f, 2.5f, redSlash);
        }
    }

    // Texto de la tecla [M]
    drawText(x + w - 32.0f, y + (h - 14.0f) / 2.0f, "[M]", 1.6f, iconColor);

    return hovered && mousePressed;
}