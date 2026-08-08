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

    // Centrar texto aproximadamente (stb_easy_font: ~8px alto, ~6-7px ancho por char a scale=1)
    float textScale = 2.0f;
    float approxTextWidth = label.size() * 7.0f * textScale;
    float textX = x + (w - approxTextWidth) / 2.0f;
    float textY = y + (h - 8.0f * textScale) / 2.0f;
    drawText(textX, textY, label, textScale, UIColor{ 1,1,1,1 });

    return hovered && mousePressed;
}