#pragma once
#include <glad/glad.h>
#include <glm/glm.hpp>
#include <vector>
#include "model.h" // Model, MeshData (CPU-side, ya cargado con Assimp)

struct AppContext; // forward declaration, evita incluir app_state.h aquí

struct GLSubMesh {
    GLuint VAO = 0, VBO = 0, EBO = 0;
    unsigned int indexCount = 0;
};

struct GLModel {
    std::vector<GLSubMesh> subMeshes;
    glm::vec3 center{ 0.0f };
    float radius = 1.0f;
    bool ready = false;
};

namespace ModelRenderer {
    void init();
    void uploadAllModels(AppContext& ctx);

    // Nueva: recibe la matriz de vista ya calculada (para cámara manual del VIEWER)
    void renderModel(GLModel& model, const glm::mat4& view, const glm::mat4& proj,
        float uiX, float uiY, float uiW, float uiH,
        int windowWidth, int windowHeight);

    // La que ya tenías, para las tarjetas del menú (cámara automática tipo turntable)
    void renderPreview(GLModel& model, float uiX, float uiY, float uiW, float uiH,
        int windowWidth, int windowHeight, float timeSeconds);
}

namespace ModelRenderer {
    void init();
    void uploadAllModels(AppContext& ctx);
    void renderPreview(GLModel& model,
        float uiX, float uiY, float uiW, float uiH, // rectángulo en coords de UI (origen arriba-izq)
        int windowWidth, int windowHeight,
        float timeSeconds);
}