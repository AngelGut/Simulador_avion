#include "model_renderer.h"
#include "app_state.h"
#include <glm/gtc/matrix_transform.hpp>

namespace {
    GLuint meshShaderProgram;

    const char* vertexSrc = R"(
        #version 330 core
        layout(location = 0) in vec3 aPos;
        layout(location = 1) in vec3 aNormal;
        uniform mat4 uMVP;
        uniform mat4 uModel;
        out vec3 vNormal;
        void main() {
            gl_Position = uMVP * vec4(aPos, 1.0);
            vNormal = mat3(transpose(inverse(uModel))) * aNormal;
        }
    )";

    const char* fragmentSrc = R"(
        #version 330 core
        in vec3 vNormal;
        out vec4 FragColor;
        uniform vec3 uBaseColor;
        void main() {
            vec3 N = normalize(vNormal);
            vec3 L = normalize(vec3(0.5, 0.8, 0.6));
            float diff = max(dot(N, L), 0.2);
            FragColor = vec4(uBaseColor * diff, 1.0);
        }
    )";

    GLuint compileShader(GLenum type, const char* src) {
        GLuint shader = glCreateShader(type);
        glShaderSource(shader, 1, &src, nullptr);
        glCompileShader(shader);
        return shader;
    }

    void uploadOneModel(const CPU::Model& cpuModel, GLModel& outModel) {
        if (!cpuModel.loaded || cpuModel.meshes.empty()) return;

        glm::vec3 minB(std::numeric_limits<float>::max());
        glm::vec3 maxB(std::numeric_limits<float>::lowest());

        for (const auto& mesh : cpuModel.meshes) {
            GLSubMesh sub;
            sub.indexCount = (unsigned int)mesh.indices.size();

            glGenVertexArrays(1, &sub.VAO);
            glGenBuffers(1, &sub.VBO);
            glGenBuffers(1, &sub.EBO);

            glBindVertexArray(sub.VAO);

            glBindBuffer(GL_ARRAY_BUFFER, sub.VBO);
            glBufferData(GL_ARRAY_BUFFER, mesh.vertices.size() * sizeof(CPU::Vertex), mesh.vertices.data(), GL_STATIC_DRAW);

            glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, sub.EBO);
            glBufferData(GL_ELEMENT_ARRAY_BUFFER, mesh.indices.size() * sizeof(unsigned int), mesh.indices.data(), GL_STATIC_DRAW);

            glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, sizeof(CPU::Vertex), (void*)0);
            glEnableVertexAttribArray(0);
            glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, sizeof(CPU::Vertex), (void*)offsetof(CPU::Vertex, normal));
            glEnableVertexAttribArray(1);

            glBindVertexArray(0);
            outModel.subMeshes.push_back(sub);

            for (const auto& v : mesh.vertices) {
                minB = glm::min(minB, v.position);
                maxB = glm::max(maxB, v.position);
            }
        }

        outModel.center = (minB + maxB) * 0.5f;
        outModel.radius = glm::length(maxB - minB) * 0.5f;
        if (outModel.radius < 0.001f) outModel.radius = 1.0f; // evita división/zoom raro si el modelo es degenerado
        outModel.ready = true;
    }
}

void ModelRenderer::init() {
    GLuint vs = compileShader(GL_VERTEX_SHADER, vertexSrc);
    GLuint fs = compileShader(GL_FRAGMENT_SHADER, fragmentSrc);
    meshShaderProgram = glCreateProgram();
    glAttachShader(meshShaderProgram, vs);
    glAttachShader(meshShaderProgram, fs);
    glLinkProgram(meshShaderProgram);
    glDeleteShader(vs);
    glDeleteShader(fs);
}

void ModelRenderer::uploadAllModels(AppContext& ctx) {
    ctx.gpuModels.resize(ctx.loadedModels.size());
    for (size_t i = 0; i < ctx.loadedModels.size(); i++) {
        uploadOneModel(ctx.loadedModels[i], ctx.gpuModels[i]);
    }
}

void ModelRenderer::renderModel(GLModel& model, const glm::mat4& view, const glm::mat4& proj,
    float uiX, float uiY, float uiW, float uiH,
    int windowWidth, int windowHeight) {
    if (!model.ready) return;

    int glX = (int)uiX;
    int glY = (int)(windowHeight - (uiY + uiH));
    int glW = (int)uiW;
    int glH = (int)uiH;
    if (glW <= 0 || glH <= 0) return;

    glEnable(GL_SCISSOR_TEST);
    glScissor(glX, glY, glW, glH);
    glViewport(glX, glY, glW, glH);
    glEnable(GL_DEPTH_TEST);

    glClearColor(0.09f, 0.09f, 0.13f, 1.0f);
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

    glm::mat4 modelMat = glm::mat4(1.0f);
    glm::mat4 mvp = proj * view * modelMat;

    glUseProgram(meshShaderProgram);
    glUniformMatrix4fv(glGetUniformLocation(meshShaderProgram, "uMVP"), 1, GL_FALSE, &mvp[0][0]);
    glUniformMatrix4fv(glGetUniformLocation(meshShaderProgram, "uModel"), 1, GL_FALSE, &modelMat[0][0]);
    glUniform3f(glGetUniformLocation(meshShaderProgram, "uBaseColor"), 0.75f, 0.78f, 0.85f);

    for (auto& sub : model.subMeshes) {
        glBindVertexArray(sub.VAO);
        glDrawElements(GL_TRIANGLES, sub.indexCount, GL_UNSIGNED_INT, 0);
    }
    glBindVertexArray(0);

    glDisable(GL_DEPTH_TEST);
    glDisable(GL_SCISSOR_TEST);
    glViewport(0, 0, windowWidth, windowHeight);
}

void ModelRenderer::renderPreview(GLModel& model, float uiX, float uiY, float uiW, float uiH,
    int windowWidth, int windowHeight, float timeSeconds) {
    if (!model.ready) return;

    float aspect = uiW / uiH;
    glm::mat4 proj = glm::perspective(glm::radians(45.0f), aspect, model.radius * 0.01f, model.radius * 10.0f);

    float angle = timeSeconds * 0.6f;
    glm::vec3 offset(sin(angle) * model.radius * 2.2f, model.radius * 0.6f, cos(angle) * model.radius * 2.2f);
    glm::mat4 view = glm::lookAt(model.center + offset, model.center, glm::vec3(0, 1, 0));

    renderModel(model, view, proj, uiX, uiY, uiW, uiH, windowWidth, windowHeight);
}