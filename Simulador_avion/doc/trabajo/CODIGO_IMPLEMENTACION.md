# EJEMPLOS DE CÓDIGO - Modernización OpenGL

## 1. core/window.h - Wrapper GLFW

```cpp
#pragma once
#ifndef WINDOW_H
#define WINDOW_H

#include <GLFW/glfw3.h>
#include <string>

class Window {
private:
    GLFWwindow* handle;
    int width, height;
    bool vsync_enabled;
    
public:
    Window();
    ~Window();
    
    bool init(int w, int h, const std::string& title);
    void swapBuffers();
    bool shouldClose() const;
    void pollEvents();
    void setVsync(bool enabled);
    void getFramebufferSize(int& w, int& h) const;
    GLFWwindow* getHandle() const { return handle; }
    void close() { glfwSetWindowShouldClose(handle, GLFW_TRUE); }
};

#endif // WINDOW_H
```

---

## 2. core/window.cpp - Implementación GLFW

```cpp
#include "window.h"
#include <iostream>

Window::Window() : handle(nullptr), width(800), height(600), vsync_enabled(true) {}

Window::~Window() {
    if (handle) {
        glfwDestroyWindow(handle);
    }
    glfwTerminate();
}

bool Window::init(int w, int h, const std::string& title) {
    // Inicializar GLFW
    if (!glfwInit()) {
        std::cerr << "Error: GLFW no se inicializó\n";
        return false;
    }
    
    // Hints para contexto OpenGL 3.3 Core
    glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
    glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
    glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);
    glfwWindowHint(GLFW_OPENGL_FORWARD_COMPAT, GLFW_TRUE);
    
    // Crear ventana
    handle = glfwCreateWindow(w, h, title.c_str(), nullptr, nullptr);
    if (!handle) {
        std::cerr << "Error: No se pudo crear ventana GLFW\n";
        glfwTerminate();
        return false;
    }
    
    width = w;
    height = h;
    
    // Hacer el contexto actual
    glfwMakeContextCurrent(handle);
    setVsync(true);
    
    std::cout << "Ventana GLFW creada: " << w << "x" << h << "\n";
    return true;
}

void Window::swapBuffers() {
    glfwSwapBuffers(handle);
}

bool Window::shouldClose() const {
    return glfwWindowShouldClose(handle);
}

void Window::pollEvents() {
    glfwPollEvents();
}

void Window::setVsync(bool enabled) {
    glfwSwapInterval(enabled ? 1 : 0);
    vsync_enabled = enabled;
}

void Window::getFramebufferSize(int& w, int& h) const {
    glfwGetFramebufferSize(handle, &w, &h);
}
```

---

## 3. shaders/shader_manager.h

```cpp
#pragma once
#ifndef SHADER_MANAGER_H
#define SHADER_MANAGER_H

#include <GL/glew.h>
#include <glm/glm.hpp>
#include <string>
#include <map>

class ShaderProgram {
private:
    GLuint programID;
    std::map<std::string, GLint> uniformLocationCache;
    
    GLuint compileShader(const char* source, GLenum type);
    
public:
    ShaderProgram();
    ~ShaderProgram();
    
    bool load(const char* vertexPath, const char* fragmentPath);
    void use() const;
    void unuse() const;
    
    // Setters de uniforms
    void setUniform(const std::string& name, float value);
    void setUniform(const std::string& name, int value);
    void setUniform(const std::string& name, const glm::vec3& vec);
    void setUniform(const std::string& name, const glm::vec4& vec);
    void setUniform(const std::string& name, const glm::mat3& mat);
    void setUniform(const std::string& name, const glm::mat4& mat);
    
    GLuint getProgramID() const { return programID; }
    
private:
    GLint getUniformLocation(const std::string& name);
};

#endif // SHADER_MANAGER_H
```

---

## 4. shaders/shader_manager.cpp - Compilación de shaders

```cpp
#include "shader_manager.h"
#include <fstream>
#include <sstream>
#include <iostream>
#include <glm/gtc/type_ptr.hpp>

ShaderProgram::ShaderProgram() : programID(0) {}

ShaderProgram::~ShaderProgram() {
    if (programID != 0) {
        glDeleteProgram(programID);
    }
}

std::string readFile(const char* path) {
    std::ifstream file(path);
    if (!file.is_open()) {
        std::cerr << "Error: No se pudo abrir " << path << "\n";
        return "";
    }
    std::stringstream buffer;
    buffer << file.rdbuf();
    return buffer.str();
}

GLuint ShaderProgram::compileShader(const char* source, GLenum type) {
    GLuint shader = glCreateShader(type);
    glShaderSource(shader, 1, &source, nullptr);
    glCompileShader(shader);
    
    // Verificar compilación
    GLint success;
    GLchar infoLog[1024];
    glGetShaderiv(shader, GL_COMPILE_STATUS, &success);
    if (!success) {
        glGetShaderInfoLog(shader, 1024, nullptr, infoLog);
        std::cerr << "Error compilando shader (" 
                  << (type == GL_VERTEX_SHADER ? "VERTEX" : "FRAGMENT") 
                  << "):\n" << infoLog << "\n";
        glDeleteShader(shader);
        return 0;
    }
    
    return shader;
}

bool ShaderProgram::load(const char* vertexPath, const char* fragmentPath) {
    // Leer archivos
    std::string vertexCode = readFile(vertexPath);
    std::string fragmentCode = readFile(fragmentPath);
    
    if (vertexCode.empty() || fragmentCode.empty()) {
        return false;
    }
    
    // Compilar shaders
    GLuint vertexShader = compileShader(vertexCode.c_str(), GL_VERTEX_SHADER);
    GLuint fragmentShader = compileShader(fragmentCode.c_str(), GL_FRAGMENT_SHADER);
    
    if (vertexShader == 0 || fragmentShader == 0) {
        return false;
    }
    
    // Linkear programa
    programID = glCreateProgram();
    glAttachShader(programID, vertexShader);
    glAttachShader(programID, fragmentShader);
    glLinkProgram(programID);
    
    // Verificar linking
    GLint success;
    GLchar infoLog[1024];
    glGetProgramiv(programID, GL_LINK_STATUS, &success);
    if (!success) {
        glGetProgramInfoLog(programID, 1024, nullptr, infoLog);
        std::cerr << "Error linking shader program:\n" << infoLog << "\n";
        glDeleteProgram(programID);
        glDeleteShader(vertexShader);
        glDeleteShader(fragmentShader);
        return false;
    }
    
    // Limpiar
    glDeleteShader(vertexShader);
    glDeleteShader(fragmentShader);
    
    std::cout << "✓ Shader program compilado exitosamente\n";
    return true;
}

void ShaderProgram::use() const {
    glUseProgram(programID);
}

void ShaderProgram::unuse() const {
    glUseProgram(0);
}

GLint ShaderProgram::getUniformLocation(const std::string& name) {
    if (uniformLocationCache.find(name) != uniformLocationCache.end()) {
        return uniformLocationCache[name];
    }
    
    GLint location = glGetUniformLocation(programID, name.c_str());
    uniformLocationCache[name] = location;
    
    if (location == -1) {
        std::cerr << "Warning: Uniform '" << name << "' no encontrado\n";
    }
    
    return location;
}

void ShaderProgram::setUniform(const std::string& name, float value) {
    glUniform1f(getUniformLocation(name), value);
}

void ShaderProgram::setUniform(const std::string& name, int value) {
    glUniform1i(getUniformLocation(name), value);
}

void ShaderProgram::setUniform(const std::string& name, const glm::vec3& vec) {
    glUniform3fv(getUniformLocation(name), 1, glm::value_ptr(vec));
}

void ShaderProgram::setUniform(const std::string& name, const glm::vec4& vec) {
    glUniform4fv(getUniformLocation(name), 1, glm::value_ptr(vec));
}

void ShaderProgram::setUniform(const std::string& name, const glm::mat3& mat) {
    glUniformMatrix3fv(getUniformLocation(name), 1, GL_FALSE, glm::value_ptr(mat));
}

void ShaderProgram::setUniform(const std::string& name, const glm::mat4& mat) {
    glUniformMatrix4fv(getUniformLocation(name), 1, GL_FALSE, glm::value_ptr(mat));
}
```

---

## 5. graphics/vao_manager.h

```cpp
#pragma once
#ifndef VAO_MANAGER_H
#define VAO_MANAGER_H

#include <GL/glew.h>
#include <glm/glm.hpp>
#include <vector>

struct Vertex {
    glm::vec3 position;
    glm::vec3 normal;
    glm::vec3 color;  // Opcional: para color por vértice
};

class VAOMesh {
private:
    GLuint VAO, VBO, EBO;
    GLsizei indexCount;
    glm::vec3 meshColor;
    
public:
    VAOMesh();
    ~VAOMesh();
    
    void create(const std::vector<Vertex>& vertices,
                const std::vector<unsigned int>& indices);
    void bind() const;
    void unbind() const;
    void draw() const;
    void setColor(float r, float g, float b);
    void cleanup();
    
    GLuint getVAO() const { return VAO; }
    glm::vec3 getColor() const { return meshColor; }
};

#endif // VAO_MANAGER_H
```

---

## 6. graphics/vao_manager.cpp

```cpp
#include "vao_manager.h"
#include <iostream>

VAOMesh::VAOMesh() : VAO(0), VBO(0), EBO(0), indexCount(0), 
                     meshColor(0.85f, 0.85f, 0.85f) {}

VAOMesh::~VAOMesh() {
    cleanup();
}

void VAOMesh::create(const std::vector<Vertex>& vertices,
                     const std::vector<unsigned int>& indices) {
    indexCount = indices.size();
    
    // Generar VAO y VBO
    glGenVertexArrays(1, &VAO);
    glGenBuffers(1, &VBO);
    glGenBuffers(1, &EBO);
    
    // Bind VAO
    glBindVertexArray(VAO);
    
    // Vertex buffer
    glBindBuffer(GL_ARRAY_BUFFER, VBO);
    glBufferData(GL_ARRAY_BUFFER, vertices.size() * sizeof(Vertex), 
                 vertices.data(), GL_STATIC_DRAW);
    
    // Element buffer
    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, EBO);
    glBufferData(GL_ELEMENT_ARRAY_BUFFER, indices.size() * sizeof(unsigned int),
                 indices.data(), GL_STATIC_DRAW);
    
    // Vertex attributes
    // Atributo 0: Posición (3 floats)
    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, sizeof(Vertex),
                         (void*)offsetof(Vertex, position));
    glEnableVertexAttribArray(0);
    
    // Atributo 1: Normal (3 floats)
    glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, sizeof(Vertex),
                         (void*)offsetof(Vertex, normal));
    glEnableVertexAttribArray(1);
    
    // Atributo 2: Color (3 floats) - OPCIONAL
    glVertexAttribPointer(2, 3, GL_FLOAT, GL_FALSE, sizeof(Vertex),
                         (void*)offsetof(Vertex, color));
    glEnableVertexAttribArray(2);
    
    // Unbind
    glBindBuffer(GL_ARRAY_BUFFER, 0);
    glBindVertexArray(0);
    
    std::cout << "✓ VAO creado: " << vertices.size() << " vértices, "
              << indices.size() / 3 << " triángulos\n";
}

void VAOMesh::bind() const {
    glBindVertexArray(VAO);
}

void VAOMesh::unbind() const {
    glBindVertexArray(0);
}

void VAOMesh::draw() const {
    if (VAO == 0) return;
    
    bind();
    glDrawElements(GL_TRIANGLES, indexCount, GL_UNSIGNED_INT, 0);
    unbind();
}

void VAOMesh::setColor(float r, float g, float b) {
    meshColor = glm::vec3(r, g, b);
}

void VAOMesh::cleanup() {
    if (EBO != 0) glDeleteBuffers(1, &EBO);
    if (VBO != 0) glDeleteBuffers(1, &VBO);
    if (VAO != 0) glDeleteVertexArrays(1, &VAO);
}
```

---

## 7. graphics/camera.h

```cpp
#pragma once
#ifndef CAMERA_H
#define CAMERA_H

#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>

class Camera {
private:
    glm::vec3 position;
    glm::vec3 target;
    glm::vec3 up;
    
    float fov;
    float aspect;
    float nearPlane;
    float farPlane;
    
    // Para rotación Euler
    float rotationX, rotationY, rotationZ;
    
public:
    Camera();
    
    void setPosition(float x, float y, float z);
    void setTarget(float x, float y, float z);
    void setAspect(float a) { aspect = a; }
    void setFOV(float f) { fov = f; }
    
    void rotate(float x, float y, float z);
    void pan(float x, float y);
    void zoom(float delta);
    void reset();
    
    glm::mat4 getViewMatrix() const;
    glm::mat4 getProjectionMatrix() const;
    
    glm::vec3 getPosition() const { return position; }
    glm::vec3 getTarget() const { return target; }
};

#endif // CAMERA_H
```

---

## 8. graphics/camera.cpp

```cpp
#include "camera.h"
#include <cmath>
#include <glm/gtc/matrix_transform.hpp>

Camera::Camera() 
    : position(0.0f, 0.0f, -5.0f),
      target(0.0f, 0.0f, 0.0f),
      up(0.0f, 1.0f, 0.0f),
      fov(45.0f),
      aspect(1024.0f / 768.0f),
      nearPlane(0.1f),
      farPlane(500.0f),
      rotationX(0.0f),
      rotationY(0.0f),
      rotationZ(0.0f) {}

void Camera::setPosition(float x, float y, float z) {
    position = glm::vec3(x, y, z);
}

void Camera::setTarget(float x, float y, float z) {
    target = glm::vec3(x, y, z);
}

void Camera::rotate(float x, float y, float z) {
    rotationX += x;
    rotationY += y;
    rotationZ += z;
    
    // Clamp rotations
    if (rotationX > 360.0f) rotationX -= 360.0f;
    if (rotationX < -360.0f) rotationX += 360.0f;
    if (rotationY > 360.0f) rotationY -= 360.0f;
    if (rotationY < -360.0f) rotationY += 360.0f;
    if (rotationZ > 360.0f) rotationZ -= 360.0f;
    if (rotationZ < -360.0f) rotationZ += 360.0f;
}

void Camera::pan(float x, float y) {
    position.x += x;
    position.y += y;
    target.x += x;
    target.y += y;
}

void Camera::zoom(float delta) {
    position.z += delta;
    if (position.z > -0.5f) position.z = -0.5f;  // No muy cerca
    if (position.z < -100.0f) position.z = -100.0f;  // No muy lejos
}

void Camera::reset() {
    position = glm::vec3(0.0f, 0.0f, -5.0f);
    target = glm::vec3(0.0f, 0.0f, 0.0f);
    rotationX = rotationY = rotationZ = 0.0f;
}

glm::mat4 Camera::getViewMatrix() const {
    // Construir view matrix con rotaciones aplicadas
    glm::mat4 view = glm::lookAt(position, target, up);
    
    // Aplicar rotaciones Euler
    view = glm::rotate(view, glm::radians(rotationX), glm::vec3(1.0f, 0.0f, 0.0f));
    view = glm::rotate(view, glm::radians(rotationY), glm::vec3(0.0f, 1.0f, 0.0f));
    view = glm::rotate(view, glm::radians(rotationZ), glm::vec3(0.0f, 0.0f, 1.0f));
    
    return view;
}

glm::mat4 Camera::getProjectionMatrix() const {
    return glm::perspective(glm::radians(fov), aspect, nearPlane, farPlane);
}
```

---

## 9. shaders/vertex.glsl

```glsl
#version 330 core

layout(location = 0) in vec3 aPosition;
layout(location = 1) in vec3 aNormal;
layout(location = 2) in vec3 aColor;

// Matrices
uniform mat4 uModel;
uniform mat4 uView;
uniform mat4 uProjection;
uniform mat3 uNormalMatrix;

// Output al fragment shader
out VS_OUT {
    vec3 position;   // Posición en world space
    vec3 normal;     // Normal en world space
    vec3 color;      // Color del vértice
} vs_out;

void main() {
    // Transformar posición a world space
    vec4 worldPos = uModel * vec4(aPosition, 1.0);
    vs_out.position = worldPos.xyz;
    
    // Transformar normal (sin traslación)
    vs_out.normal = normalize(uNormalMatrix * aNormal);
    
    // Pasar color
    vs_out.color = aColor;
    
    // Posición final
    gl_Position = uProjection * uView * worldPos;
}
```

---

## 10. shaders/fragment.glsl

```glsl
#version 330 core

in VS_OUT {
    vec3 position;
    vec3 normal;
    vec3 color;
} fs_in;

// Parámetros de iluminación
uniform vec3 uObjectColor;
uniform vec3 uLightPos;
uniform vec3 uViewPos;
uniform vec3 uAmbient;
uniform vec3 uDiffuse;
uniform vec3 uSpecular;
uniform float uShininess;

out vec4 FragColor;

void main() {
    // Normalizar normal interpolada
    vec3 norm = normalize(fs_in.normal);
    vec3 lightDir = normalize(uLightPos - fs_in.position);
    vec3 viewDir = normalize(uViewPos - fs_in.position);
    
    // Componente ambiental
    vec3 ambient = uAmbient * uObjectColor;
    
    // Componente difusa
    float diff = max(dot(norm, lightDir), 0.0);
    vec3 diffuse = diff * uDiffuse * uObjectColor;
    
    // Componente especular
    vec3 reflectDir = reflect(-lightDir, norm);
    float spec = pow(max(dot(viewDir, reflectDir), 0.0), uShininess);
    vec3 specular = spec * uSpecular * uObjectColor;
    
    // Iluminación final
    vec3 result = ambient + diffuse + specular;
    
    // Usar color de vértice si está disponible
    // result *= fs_in.color;  // Descomentar para color por vértice
    
    FragColor = vec4(result, 1.0);
}
```

---

## 11. model_loader_ng.h (fragmento modificado)

```cpp
struct Mesh {
    std::vector<Vertex> vertices;
    std::vector<unsigned int> indices;
    VAOMesh vaoMesh;  // Nuevo: VAO/VBO
    glm::vec3 color;  // Color del mesh
    
    void draw();  // Ahora usa VAO
};

class Model {
private:
    std::vector<Mesh> meshes;
    bool loaded;
    float scale;
    glm::vec3 center;
    
    // ... rest de miembros
    
public:
    // ... métodos existentes
    
    void uploadToGPU();  // Nuevo: crear VAO para todos los meshes
    const std::vector<Mesh>& getMeshes() const { return meshes; }
};
```

---

## 12. model_loader_ng.cpp (fragmento modificado)

```cpp
void Model::uploadToGPU() {
    std::cout << "Subiendo modelo a GPU...\n";
    for (auto& mesh : meshes) {
        mesh.vaoMesh.create(mesh.vertices, mesh.indices);
        mesh.vaoMesh.setColor(mesh.color.x, mesh.color.y, mesh.color.z);
    }
    std::cout << "✓ Modelo cargado en GPU\n";
}

void Mesh::draw() {
    // Ahora usa VAO en lugar de glBegin/glEnd
    vaoMesh.draw();
}

bool Model::loadModel(const char* path) {
    Assimp::Importer importer;
    const aiScene* scene = importer.ReadFile(path,
        aiProcess_Triangulate |
        aiProcess_GenSmoothNormals |
        aiProcess_FlipUVs |
        aiProcess_CalcTangentSpace);

    if (!scene || scene->mFlags & AI_SCENE_FLAGS_INCOMPLETE || !scene->mRootNode) {
        std::cerr << "Error: " << importer.GetErrorString() << std::endl;
        return false;
    }

    std::cout << "Cargando: " << path << " (" << scene->mNumMeshes << " meshes)\n";
    
    glm::mat4 identity(1.0f);
    processNode(scene->mRootNode, scene, identity);
    normalizeModel();
    
    // Nuevo: subir a GPU después de normalizar
    uploadToGPU();
    
    loaded = true;
    return true;
}
```

---

## 13. graphics/renderer_ng.h

```cpp
#pragma once
#ifndef RENDERER_NG_H
#define RENDERER_NG_H

#include "shader_manager.h"
#include "camera.h"
#include "../model_loader_ng.h"
#include <glm/glm.hpp>

class Renderer {
private:
    ShaderProgram* shader;
    
    // Parámetros de iluminación
    glm::vec3 lightPos;
    glm::vec3 ambientColor;
    glm::vec3 diffuseColor;
    glm::vec3 specularColor;
    float shininess;
    
public:
    Renderer(ShaderProgram* s);
    
    void setupOpenGL();
    void drawModel(const Model& model, const glm::mat4& modelMatrix,
                   const Camera& camera);
    
    void setLighting(const glm::vec3& pos, const glm::vec3& ambient,
                     const glm::vec3& diffuse, const glm::vec3& specular,
                     float shine = 32.0f);
};

#endif // RENDERER_NG_H
```

---

## 14. graphics/renderer_ng.cpp (render loop)

```cpp
#include "renderer_ng.h"
#include <GL/glew.h>
#include <iostream>
#include <glm/gtc/matrix_transform.hpp>

Renderer::Renderer(ShaderProgram* s)
    : shader(s),
      lightPos(5.0f, 5.0f, 5.0f),
      ambientColor(0.3f, 0.3f, 0.3f),
      diffuseColor(0.9f, 0.9f, 0.9f),
      specularColor(1.0f, 1.0f, 1.0f),
      shininess(32.0f) {}

void Renderer::setupOpenGL() {
    glClearColor(0.05f, 0.05f, 0.1f, 1.0f);
    glEnable(GL_DEPTH_TEST);
    glDepthFunc(GL_LEQUAL);
    glEnable(GL_CULL_FACE);
    glCullFace(GL_BACK);
    glEnable(GL_BLEND);
    glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
    
    std::cout << "OpenGL setup completado\n";
}

void Renderer::drawModel(const Model& model, const glm::mat4& modelMatrix,
                        const Camera& camera) {
    shader->use();
    
    // Pasar matrices
    shader->setUniform("uModel", modelMatrix);
    shader->setUniform("uView", camera.getViewMatrix());
    shader->setUniform("uProjection", camera.getProjectionMatrix());
    
    // Normal matrix = transpose(inverse(mat3(model)))
    glm::mat3 normalMatrix = glm::transpose(glm::inverse(glm::mat3(modelMatrix)));
    shader->setUniform("uNormalMatrix", normalMatrix);
    
    // Parámetros de iluminación
    shader->setUniform("uLightPos", lightPos);
    shader->setUniform("uAmbient", ambientColor);
    shader->setUniform("uDiffuse", diffuseColor);
    shader->setUniform("uSpecular", specularColor);
    shader->setUniform("uShininess", shininess);
    shader->setUniform("uViewPos", camera.getPosition());
    
    // Dibujar todos los meshes
    const auto& meshes = model.getMeshes();
    for (const auto& mesh : meshes) {
        shader->setUniform("uObjectColor", mesh.color);
        mesh.draw();  // Usa VAO internamente
    }
}

void Renderer::setLighting(const glm::vec3& pos, const glm::vec3& ambient,
                           const glm::vec3& diffuse, const glm::vec3& specular,
                           float shine) {
    lightPos = pos;
    ambientColor = ambient;
    diffuseColor = diffuse;
    specularColor = specular;
    shininess = shine;
}
```

---

## 15. core/input_manager.h

```cpp
#pragma once
#ifndef INPUT_MANAGER_H
#define INPUT_MANAGER_H

#include "window.h"
#include "../graphics/camera.h"
#include <GLFW/glfw3.h>

class InputManager {
private:
    Camera* camera;
    Window* window;
    
    // Estado del teclado
    static InputManager* instance;
    
public:
    InputManager();
    
    void init(Window* w, Camera* c);
    void processInput(float deltaTime = 0.016f);  // ~60 FPS
    
    // Callback estático (requerido para GLFW)
    static void keyCallback(GLFWwindow* window, int key, int scancode,
                           int action, int mods);
};

#endif // INPUT_MANAGER_H
```

---

## 16. core/input_manager.cpp

```cpp
#include "input_manager.h"
#include <iostream>

InputManager* InputManager::instance = nullptr;

InputManager::InputManager() : camera(nullptr), window(nullptr) {}

void InputManager::init(Window* w, Camera* c) {
    camera = c;
    window = w;
    instance = this;
    
    // Registrar callback de GLFW
    glfwSetKeyCallback(w->getHandle(), InputManager::keyCallback);
}

void InputManager::processInput(float deltaTime) {
    GLFWwindow* glfwWindow = window->getHandle();
    
    // Rotación
    const float rotSpeed = 10.0f;
    if (glfwGetKey(glfwWindow, GLFW_KEY_I) == GLFW_PRESS) {
        camera->rotate(rotSpeed, 0, 0);  // Pitch
    }
    if (glfwGetKey(glfwWindow, GLFW_KEY_K) == GLFW_PRESS) {
        camera->rotate(-rotSpeed, 0, 0);
    }
    
    if (glfwGetKey(glfwWindow, GLFW_KEY_J) == GLFW_PRESS) {
        camera->rotate(0, -rotSpeed, 0);  // Yaw
    }
    if (glfwGetKey(glfwWindow, GLFW_KEY_L) == GLFW_PRESS) {
        camera->rotate(0, rotSpeed, 0);
    }
    
    if (glfwGetKey(glfwWindow, GLFW_KEY_R) == GLFW_PRESS) {
        camera->rotate(0, 0, -rotSpeed);  // Roll
    }
    if (glfwGetKey(glfwWindow, GLFW_KEY_T) == GLFW_PRESS) {
        camera->rotate(0, 0, rotSpeed);
    }
    
    // Zoom
    const float zoomSpeed = 0.5f;
    if (glfwGetKey(glfwWindow, GLFW_KEY_Q) == GLFW_PRESS) {
        camera->zoom(zoomSpeed);  // Alejar
    }
    if (glfwGetKey(glfwWindow, GLFW_KEY_E) == GLFW_PRESS) {
        camera->zoom(-zoomSpeed);  // Acercar
    }
    
    // Pan
    const float panSpeed = 0.5f;
    if (glfwGetKey(glfwWindow, GLFW_KEY_W) == GLFW_PRESS) {
        camera->pan(0, panSpeed);  // Arriba
    }
    if (glfwGetKey(glfwWindow, GLFW_KEY_S) == GLFW_PRESS) {
        camera->pan(0, -panSpeed);  // Abajo
    }
    if (glfwGetKey(glfwWindow, GLFW_KEY_A) == GLFW_PRESS) {
        camera->pan(-panSpeed, 0);  // Izquierda
    }
    if (glfwGetKey(glfwWindow, GLFW_KEY_D) == GLFW_PRESS) {
        camera->pan(panSpeed, 0);  // Derecha
    }
    
    // Reset
    if (glfwGetKey(glfwWindow, GLFW_KEY_SPACE) == GLFW_PRESS) {
        camera->reset();
        std::cout << "Cámara reseteada\n";
    }
    
    // Salir
    if (glfwGetKey(glfwWindow, GLFW_KEY_ESCAPE) == GLFW_PRESS) {
        window->close();
    }
}

void InputManager::keyCallback(GLFWwindow* window, int key, int scancode,
                              int action, int mods) {
    // Implementar callbacks puntuales si es necesario
    // Por ahora, usar processInput() es suficiente
}
```

---

## 17. main_ng.cpp - Loop principal simplificado

```cpp
#include <GL/glew.h>
#include <GLFW/glfw3.h>
#include <iostream>
#include <glm/glm.hpp>

#include "core/window.h"
#include "core/input_manager.h"
#include "graphics/camera.h"
#include "graphics/renderer_ng.h"
#include "shaders/shader_manager.h"
#include "model_loader_ng.h"
#include "config.h"

int main() {
    std::cout << "\n=== Boeing 737 Visualizer 3D (Modernizado) ===\n\n";
    
    // 1. Crear ventana GLFW
    Window window;
    if (!window.init(1024, 768, "Boeing 737 Visualizer 3D - OpenGL 3.3+")) {
        return -1;
    }
    
    // 2. Inicializar GLEW
    glewExperimental = GL_TRUE;
    if (glewInit() != GLEW_OK) {
        std::cerr << "Error: GLEW no se inicializó\n";
        return -1;
    }
    
    std::cout << "OpenGL " << glGetString(GL_VERSION) << "\n\n";
    
    // 3. Cargar y compilar shaders
    ShaderProgram shader;
    if (!shader.load("shaders/vertex.glsl", "shaders/fragment.glsl")) {
        std::cerr << "Error: Falló compilación de shaders\n";
        return -1;
    }
    
    // 4. Crear renderer y cámara
    Renderer renderer(&shader);
    renderer.setupOpenGL();
    
    Camera camera;
    camera.setAspect(1024.0f / 768.0f);
    
    // 5. Crear input manager
    InputManager inputManager;
    inputManager.init(&window, &camera);
    
    // 6. Seleccionar y cargar modelo
    std::cout << "\n=== SELECCIONA AERONAVE ===\n"
              << "1. " << NAME_1 << "\n"
              << "2. " << NAME_2 << "\n"
              << "3. " << NAME_3 << "\n"
              << "4. " << NAME_4 << "\n"
              << "5. " << NAME_5 << "\n"
              << "\nOpción (1-5): ";
    
    int modelNum = 5;
    std::cin >> modelNum;
    if (modelNum < 1 || modelNum > 5) modelNum = 5;
    
    const char* modelPath = nullptr;
    switch (modelNum) {
        case 1: modelPath = MODEL_1; break;
        case 2: modelPath = MODEL_2; break;
        case 3: modelPath = MODEL_3; break;
        case 4: modelPath = MODEL_4; break;
        case 5: modelPath = MODEL_5; break;
    }
    
    // 7. Cargar modelo
    Model model;
    if (!model.loadModel(modelPath)) {
        std::cerr << "Advertencia: Modelo no disponible\n";
    }
    
    // Ajustar cámara al modelo
    float zoom = model.getRecommendedZoom();
    camera.setPosition(0, 0, zoom);
    
    // 8. Configurar iluminación
    renderer.setLighting(
        glm::vec3(5.0f, 5.0f, 5.0f),    // posición
        glm::vec3(0.3f, 0.3f, 0.3f),    // ambiental
        glm::vec3(0.9f, 0.9f, 0.9f),    // difusa
        glm::vec3(1.0f, 1.0f, 1.0f),    // especular
        32.0f                            // shininess
    );
    
    std::cout << "\n=== CONTROLES ===\n"
              << "I/K: Rotar arriba/abajo | J/L: Rotar izq/der\n"
              << "R/T: Rotar CW/CCW | Q/E: Zoom out/in\n"
              << "W/A/S/D: Pan | ESPACIO: Reset | ESC: Salir\n\n";
    
    // 9. Loop principal
    glm::mat4 modelMatrix(1.0f);  // Identity
    
    while (!window.shouldClose()) {
        // Limpiar buffers
        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
        
        // Procesar input
        inputManager.processInput();
        
        // Renderizar
        renderer.drawModel(model, modelMatrix, camera);
        
        // Swap buffers
        window.swapBuffers();
        window.pollEvents();
    }
    
    std::cout << "\nCerrando aplicación...\n";
    return 0;
}
```

---

## RESUMEN DE MIGRACIONES CLAVE

| Concepto | Antes (GLUT) | Después (GLFW) |
|----------|------|--------|
| **Ventana** | `glutCreateWindow()` | `glfwCreateWindow()` |
| **Loop** | `glutMainLoop()` | `while (!glfwWindowShouldClose())` |
| **Input** | `glutKeyboardFunc()` | `glfwSetKeyCallback()` |
| **Proyección** | `gluPerspective()` | `glm::perspective()` |
| **Vista** | `gluLookAt()` | `glm::lookAt()` |
| **Rotaciones** | `glRotatef()` | `glm::rotate()` + matrices |
| **Dibujado** | `glBegin/glEnd` | `glDrawElements()` con VAO |
| **Iluminación** | `glLight()` en C++ | Uniforms en fragment shader |
| **Colores** | `glColor3f()` | Uniforms en shader |
| **Normales** | `glNormal3f()` en pipeline | Atributo + matrix normal |

