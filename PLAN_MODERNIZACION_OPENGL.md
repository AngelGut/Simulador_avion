# PLAN DE MODERNIZACIÓN: OpenGL 1.1 GLUT → OpenGL 3.3+ GLFW/Shaders

## Estado Actual
- **Framework**: GLUT
- **Pipeline**: Fixed function pipeline (matrix stack, glLight, glColor)
- **Dibujado**: glBegin/glEnd
- **Modelo**: Assimp → vertices sin VAO/VBO
- **Iluminación**: Fixed pipeline (GL_LIGHTING, glLightfv)

## Estructura de Archivos (Nueva/Modificada)

```
Simulador_avion/
├── src/
│   ├── shaders/
│   │   ├── vertex.glsl           [NUEVO]
│   │   ├── fragment.glsl         [NUEVO]
│   │   └── shader_manager.cpp/h  [NUEVO]
│   ├── graphics/
│   │   ├── vao_manager.cpp/h     [NUEVO]  - Gestión VAO/VBO
│   │   ├── renderer_ng.cpp/h     [NUEVO]  - Renderer moderno
│   │   └── camera.cpp/h          [NUEVO]  - Cámara con GLM
│   ├── core/
│   │   ├── window.cpp/h          [NUEVO]  - Wrapper GLFW
│   │   └── input_manager.cpp/h   [NUEVO]  - Input (callbacks)
│   ├── model_loader_ng.cpp/h     [MODIFICADO] - Cargador con VAO
│   ├── main_ng.cpp               [MODIFICADO] - Entrada GLFW
│   ├── layer_manager_ng.cpp/h    [MODIFICADO] - Compatible
│   └── geometry_ng.cpp/h         [MODIFICADO] - Geometría → VAO
```

---

## PLAN DE IMPLEMENTACIÓN (15 pasos)

### FASE 1: INFRAESTRUCTURA BASE (Pasos 1-4)

#### Paso 1: Crear sistema de ventana GLFW
**Archivo**: `core/window.cpp/h`

Wrapper que encapsula GLFW:
```cpp
class Window {
    GLFWwindow* handle;
    int width, height;
public:
    bool init(int w, int h, const char* title);
    void swapBuffers();
    bool shouldClose();
    void pollEvents();
    void getFramebufferSize(int& w, int& h);
    GLFWwindow* getHandle();
};
```

**Puntos técnicos**:
- `glfwInit()`, `glfwCreateWindow()`, `glfwMakeContextCurrent()`
- Reemplaza `glutInit()`, `glutCreateWindow()`, `glutMainLoop()`
- Contexto OpenGL 3.3 core: `glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3)` + `GLFW_CONTEXT_VERSION_MINOR, 3` + `GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE`

---

#### Paso 2: Compilación de shaders y ShaderProgram
**Archivo**: `shaders/shader_manager.cpp/h`

Clase que maneja compilación de shaders GLSL:
```cpp
class ShaderProgram {
    GLuint programID;
    std::map<std::string, GLint> uniformCache;
public:
    bool load(const char* vertPath, const char* fragPath);
    void use();
    void setUniform(const char* name, const glm::mat4& mat);
    void setUniform(const char* name, const glm::vec3& vec);
    void setUniform(const char* name, float value);
    void setUniform(const char* name, int value);
};
```

**Pasos de compilación**:
- Leer vertex.glsl y fragment.glsl (archivos de texto)
- `glCreateShader(GL_VERTEX_SHADER)`, `glShaderSource()`, `glCompileShader()`
- `glCreateShader(GL_FRAGMENT_SHADER)` (igual)
- `glCreateProgram()`, `glAttachShader()`, `glLinkProgram()`
- Validar con `glGetShaderiv(shader, GL_COMPILE_STATUS)` e `glGetShaderInfoLog()`

**Manejo de errores**: Loguear mensaje de compilación si falla

---

#### Paso 3: Gestión de VAO/VBO
**Archivo**: `graphics/vao_manager.cpp/h`

Clase que encapsula VAO/VBO para meshes:
```cpp
class VAOMesh {
    GLuint VAO, VBO, EBO;
    GLsizei indexCount;
    glm::vec3 color;  // Para compatibilidad con colores por mesh
public:
    void create(const std::vector<Vertex>& vertices, 
                const std::vector<unsigned int>& indices);
    void bind();
    void draw();
    void setColor(float r, float g, float b);
};
```

**Detalles técnicos**:
- `glGenVertexArrays()`, `glGenBuffers()` para VAO y VBO
- `glBindVertexArray()`, `glBindBuffer()`
- `glBufferData()` para subir datos a GPU
- `glVertexAttribPointer()` para especificar layout de atributos:
  - Atributo 0: posición (3 floats)
  - Atributo 1: normal (3 floats)
  - Atributo 2 (OPCIONAL): color (3 floats) - si está disponible en Vertex
- `glDrawElements()` para renderizar

**Formato de Vertex struct** (actualizar):
```cpp
struct Vertex {
    glm::vec3 position;
    glm::vec3 normal;
    glm::vec3 color;  // OPCIONAL - para soporte futuro
};
```

---

#### Paso 4: Sistema de cámara con GLM
**Archivo**: `graphics/camera.cpp/h`

Reemplaza matrix stack (glMatrixMode, gluLookAt, gluPerspective):
```cpp
class Camera {
    glm::vec3 position, target, up;
    float fov, aspect, near, far;
public:
    glm::mat4 getViewMatrix();
    glm::mat4 getProjectionMatrix();
    void setPosition(float x, float y, float z);
    void rotate(float x, float y, float z);  // Ángulos de Euler
    void pan(float x, float y);
    void zoom(float delta);
};
```

**Equivalencias**:
- `glm::perspective(glm::radians(fov), aspect, near, far)` ↔ `gluPerspective()`
- `glm::lookAt(position, target, up)` ↔ `gluLookAt()`
- Pasar matrices a shader via uniforms

---

### FASE 2: MIGRACIÓN DE SHADERS (Pasos 5-6)

#### Paso 5: Crear vertex.glsl
**Archivo**: `shaders/vertex.glsl`

```glsl
#version 330 core

layout(location = 0) in vec3 aPosition;
layout(location = 1) in vec3 aNormal;
// layout(location = 2) in vec3 aColor;  // Opcional si se usa

uniform mat4 uModel;
uniform mat4 uView;
uniform mat4 uProjection;
uniform mat3 uNormalMatrix;  // inverse(transpose(mat3(uModel)))

out vec3 vNormal;
out vec3 vPosition;  // Posición en world space
// out vec3 vColor;  // Opcional

void main() {
    vPosition = (uModel * vec4(aPosition, 1.0)).xyz;
    vNormal = normalize(uNormalMatrix * aNormal);
    // vColor = aColor;  // Opcional
    
    gl_Position = uProjection * uView * vec4(vPosition, 1.0);
}
```

**Conceptos clave**:
- `layout(location = ...)` vincula atributos con indices de VAO
- `uniform` para matrices y datos globales
- `out` para interpolar datos al fragment shader
- Normal matrix para transformar normales correctamente

---

#### Paso 6: Crear fragment.glsl
**Archivo**: `shaders/fragment.glsl`

```glsl
#version 330 core

in vec3 vNormal;
in vec3 vPosition;
// in vec3 vColor;  // Opcional

uniform vec3 uObjectColor;  // Color del objeto (antes: glColor3f)
uniform vec3 uLightPos;     // Posición de luz (antes: glLightfv position)
uniform vec3 uViewPos;      // Posición cámara (para especular)
uniform vec3 uAmbient;      // Luz ambiental (antes: GL_AMBIENT)
uniform vec3 uDiffuse;      // Luz difusa (antes: GL_DIFFUSE)
uniform vec3 uSpecular;     // Especular (antes: GL_SPECULAR)
uniform float uShininess;   // Brillo

out vec4 FragColor;

void main() {
    // Normalizar normal interpolada
    vec3 norm = normalize(vNormal);
    vec3 lightDir = normalize(uLightPos - vPosition);
    
    // Luz ambiental
    vec3 ambient = uAmbient * uObjectColor;
    
    // Luz difusa
    float diff = max(dot(norm, lightDir), 0.0);
    vec3 diffuse = diff * uDiffuse * uObjectColor;
    
    // Especular
    vec3 viewDir = normalize(uViewPos - vPosition);
    vec3 reflectDir = reflect(-lightDir, norm);
    float spec = pow(max(dot(viewDir, reflectDir), 0.0), uShininess);
    vec3 specular = spec * uSpecular;
    
    vec3 result = ambient + diffuse + specular;
    FragColor = vec4(result, 1.0);
}
```

**Mapeo de iluminación fixed pipeline**:
| Fixed Pipeline | Shader |
|---|---|
| `glLight(GL_AMBIENT)` | `uniform vec3 uAmbient` |
| `glLight(GL_DIFFUSE)` | `uniform vec3 uDiffuse` |
| `glLight(GL_SPECULAR)` | `uniform vec3 uSpecular` |
| `glColorMaterial() + glColor3f()` | `uniform vec3 uObjectColor` |
| Normal proveniente de geometría | Matriz normal en VS |

---

### FASE 3: MODERNIZACIÓN DE main.cpp (Paso 7)

#### Paso 7: Reescribir main_ng.cpp con GLFW
**Archivo**: `main_ng.cpp`

**Estructura reemplazando GLUT**:
```cpp
#include <GLFW/glfw3.h>
#include "core/window.h"
#include "graphics/camera.h"
#include "graphics/renderer_ng.h"
#include "model_loader_ng.h"

int main() {
    Window window;
    window.init(1024, 768, "Boeing 737 Visualizer 3D");
    
    // Inicializar GLEW
    glewInit();
    
    // Recursos
    ShaderProgram shader;
    shader.load("shaders/vertex.glsl", "shaders/fragment.glsl");
    
    Camera camera;
    camera.setPosition(0, 0, -5);
    
    Renderer renderer(&shader);
    Model model;
    model.loadModel("path/to/model.obj");
    model.uploadToGPU();  // Nueva función: crear VAO/VBO
    
    // Loop principal (reemplaza glutMainLoop)
    while (!window.shouldClose()) {
        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
        
        // Input
        glfwPollEvents();  // Reemplaza GLUT polling
        
        // Actualizar
        updateCameraFromInput(camera);  // Manejo de teclado
        
        // Render
        shader.use();
        shader.setUniform("uView", camera.getViewMatrix());
        shader.setUniform("uProjection", camera.getProjectionMatrix());
        renderer.drawModel(model);
        
        window.swapBuffers();
    }
    
    glfwTerminate();
    return 0;
}
```

**Cambios de callbacks**:
- `glutKeyboardFunc()` → `glfwSetKeyCallback(window->getHandle(), keyCallback)`
- `glutReshapeFunc()` → `glfwSetFramebufferSizeCallback()`
- `glutTimerFunc()` → Loop manual con `glfwGetTime()`
- `glutPostRedisplay()` → Simplemente continuar loop

**Input manager** (`core/input_manager.cpp`):
- Variables globales de estado (rotX, rotY, zoom, pan)
- Callback de teclado que modifica estas variables
- Función para aplicar transformaciones a cámara

---

### FASE 4: MIGRACIÓN DE MODEL_LOADER (Pasos 8-9)

#### Paso 8: Actualizar Vertex struct y procesamiento Assimp
**Archivo**: `model_loader_ng.cpp/h`

**Cambios en Vertex**:
```cpp
struct Vertex {
    glm::vec3 position;
    glm::vec3 normal;
    glm::vec3 color;  // Nuevo: soporte para color de vértice
};
```

**En Model::processMesh()**:
- Mantener la lógica de carga Assimp (igual)
- Agregar posibilidad de color por vértice (si material tiene diffuse)
- Eliminar llamadas a `Mesh::draw()` con glBegin/glEnd

**Nueva función**:
```cpp
void Model::uploadToGPU() {
    for (auto& mesh : meshes) {
        VAOMesh vaoMesh;
        vaoMesh.create(mesh.vertices, mesh.indices);
        mesh.vaoMesh = std::move(vaoMesh);  // Guardar VAO en Mesh
    }
}
```

**Actualizar Mesh struct**:
```cpp
struct Mesh {
    std::vector<Vertex> vertices;
    std::vector<unsigned int> indices;
    VAOMesh vaoMesh;  // Nuevo: VAO/VBO en GPU
    
    void draw();  // Ahora usa vaoMesh, no glBegin/glEnd
};
```

---

#### Paso 9: Implementar Mesh::draw() con VAO
**Archivo**: `model_loader_ng.cpp`

**Reemplazar glBegin/glEnd por VAO**:

Antes:
```cpp
void Mesh::draw() {
    glBegin(GL_TRIANGLES);
    for (unsigned int i = 0; i < indices.size(); i++) {
        unsigned int idx = indices[i];
        const Vertex& v = vertices[idx];
        glNormal3f(v.normal.x, v.normal.y, v.normal.z);
        glVertex3f(v.position.x, v.position.y, v.position.z);
    }
    glEnd();
}
```

Después:
```cpp
void Mesh::draw() {
    vaoMesh.bind();
    vaoMesh.draw();  // Internamente: glDrawElements()
}
```

---

### FASE 5: MODERNIZACIÓN DE RENDERER (Pasos 10-11)

#### Paso 10: Reescribir Renderer::setupOpenGL()
**Archivo**: `graphics/renderer_ng.cpp/h`

**Nueva clase Renderer**:
```cpp
class Renderer {
    ShaderProgram* shader;
    glm::vec3 lightPos;
    glm::vec3 ambientColor;
    glm::vec3 diffuseColor;
    glm::vec3 specularColor;
    
public:
    Renderer(ShaderProgram* s);
    
    void setupOpenGL();
    void drawModel(const Model& model, const glm::mat4& modelMatrix);
    void setLighting(glm::vec3 pos, glm::vec3 ambient, 
                     glm::vec3 diffuse, glm::vec3 specular);
};

void Renderer::setupOpenGL() {
    glClearColor(0.05f, 0.05f, 0.1f, 1.0f);
    glEnable(GL_DEPTH_TEST);
    glDepthFunc(GL_LEQUAL);
    glEnable(GL_CULL_FACE);  // Nuevo: optimización
    glCullFace(GL_BACK);
    
    // No más glEnable(GL_LIGHTING) - iluminación está en shader
}
```

**Cambios principales**:
- `glEnable(GL_LIGHTING)` → Eliminar (iluminación en shader)
- `glLight*()` → Pasar uniforms al shader
- `glColor3f()` → Pasar color como uniform
- `glMatrixMode()` + stack → GLM matrices

---

#### Paso 11: Implementar Renderer::drawModel()
**Archivo**: `graphics/renderer_ng.cpp`

```cpp
void Renderer::drawModel(const Model& model, const glm::mat4& modelMatrix) {
    shader->use();
    
    // Pasar matrices
    shader->setUniform("uModel", modelMatrix);
    shader->setUniform("uView", viewMatrix);  // Pasa desde main
    shader->setUniform("uProjection", projMatrix);  // Pasa desde main
    
    // Normal matrix = transpose(inverse(mat3(model)))
    glm::mat3 normalMatrix = glm::transpose(glm::inverse(glm::mat3(modelMatrix)));
    shader->setUniform("uNormalMatrix", normalMatrix);
    
    // Iluminación
    shader->setUniform("uLightPos", lightPos);
    shader->setUniform("uAmbient", ambientColor);
    shader->setUniform("uDiffuse", diffuseColor);
    shader->setUniform("uSpecular", specularColor);
    
    // Dibujar meshes
    for (const auto& mesh : model.getMeshes()) {
        shader->setUniform("uObjectColor", mesh.color);
        mesh.draw();
    }
}
```

---

### FASE 6: GEOMETRÍA PROCEDURAL (Pasos 12-13)

#### Paso 12: Refactorizar GeometryBuilder para VAO
**Archivo**: `geometry_ng.cpp/h`

**Estrategia**: Convertir funciones que usan glBegin/glEnd a generadores de geometría procedural

**Nuevo enfoque**:
```cpp
namespace GeometryBuilder {
    // Retorna Mesh en lugar de dibujar directo
    Mesh generateFuselage(float centerX, float centerY, float radius, float length);
    Mesh generateWings();
    Mesh generateMotors();
    // ... etc
}
```

**Ejemplo para generateFuselage()**:
```cpp
Mesh GeometryBuilder::generateFuselage(...) {
    std::vector<Vertex> vertices;
    std::vector<unsigned int> indices;
    
    // Usar algoritmo de Bresenham para obtener puntos de circunferencia
    // Construir vértices del cilindro
    for (float t = 0; t < length; t += step) {
        for (float angle = 0; angle < 2*PI; angle += angleStep) {
            Vertex v;
            v.position = glm::vec3(
                centerX + radius * cos(angle),
                centerY + radius * sin(angle),
                t
            );
            v.normal = glm::normalize(glm::vec3(cos(angle), sin(angle), 0));
            v.color = glm::vec3(0.85f, 0.85f, 0.85f);  // Color por defecto
            vertices.push_back(v);
        }
    }
    
    // Generar índices para triángulos
    // ... conectar vértices en tiras
    
    Mesh mesh;
    mesh.vertices = vertices;
    mesh.indices = indices;
    return mesh;
}
```

**Llamadas desde Renderer::drawLayer()**:
```cpp
void Renderer::drawLayer(int layerNumber) {
    if (loadedModel && loadedModel->isLoaded()) {
        renderer->drawModel(*loadedModel, modelMatrix);
    } else {
        // Geometría procedural
        Mesh fuselage = GeometryBuilder::generateFuselage(...);
        fuselage.uploadToGPU();  // Crear VAO
        fuselage.draw();
    }
}
```

---

#### Paso 13: Mantener compatibilidad LayerManager
**Archivo**: `layer_manager_ng.cpp/h`

**Cambios mínimos** (la clase es simple):
- Funcionalidad igual: `setActiveLayer()`, `toggleVisibility()`, `isVisible()`
- En main_ng.cpp, usar `layerManager.getActiveLayer()` para determinar qué mesh dibujar
- No requiere cambios profundos de lógica

```cpp
void Renderer::drawLayer(int layerNumber) {
    // layerManager.getActiveLayer() ahora usado en main_ng
    // Dibujar según la capa
    switch(layerNumber) {
        case 1:
            if (hasModel) drawModel(extModel);
            else drawGeometry(GeometryBuilder::generateExterior());
            break;
        // ... etc
    }
}
```

---

### FASE 7: INTEGRACIÓN Y TESTING (Pasos 14-15)

#### Paso 14: Actualizar CMakeLists.txt / Build System
**Archivos**: `CMakeLists.txt`, proyectos IDE

**Cambios de dependencias**:
```cmake
# Quitar
find_package(GLUT REQUIRED)
include_directories(${GLUT_INCLUDE_DIR})

# Agregar
find_package(glfw3 REQUIRED)
find_package(GLEW REQUIRED)
find_package(OpenGL REQUIRED)
find_package(assimp REQUIRED)
find_package(glm REQUIRED)

# Linking
target_link_libraries(Simulador_avion 
    glfw 
    GLEW::GLEW 
    OpenGL::OpenGL
    assimp::assimp
)

# Include paths
include_directories(${GLM_INCLUDE_DIRS})
```

**Archivos a compilar**:
```cmake
set(SOURCES
    src/main_ng.cpp
    src/core/window.cpp
    src/core/input_manager.cpp
    src/graphics/camera.cpp
    src/graphics/vao_manager.cpp
    src/graphics/renderer_ng.cpp
    src/shaders/shader_manager.cpp
    src/model_loader_ng.cpp
    src/geometry_ng.cpp
    src/layer_manager_ng.cpp
    src/utils.cpp
)
```

---

#### Paso 15: Testing y validación de compatibilidad
**Checklist de Testing**:

1. **Ventana GLFW** ✓
   - [ ] Ventana se abre correctamente
   - [ ] Redimensionamiento sin fallos
   - [ ] Cierre limpio

2. **Shaders** ✓
   - [ ] vertex.glsl compila sin errores
   - [ ] fragment.glsl compila sin errores
   - [ ] Program linkage exitoso
   - [ ] Uniforms se pasan correctamente

3. **Carga de modelo** ✓
   - [ ] Assimp carga modelo OBJ/FBX
   - [ ] VAO/VBO se crean correctamente
   - [ ] Datos suben a GPU sin errores

4. **Renderizado** ✓
   - [ ] Modelo se renderiza con shaders
   - [ ] Iluminación funciona (difusa + especular)
   - [ ] Colores se aplican correctamente
   - [ ] Normales calculadas correctamente

5. **Interacción** ✓
   - [ ] Rotación X/Y/Z funciona
   - [ ] Zoom acerca/aleja
   - [ ] Pan mueve vista
   - [ ] Reset vista restaura posición inicial
   - [ ] Cambio de modelo (1-5) funciona

6. **Compatibilidad** ✓
   - [ ] LayerManager integrado sin cambios grandes
   - [ ] Geometría procedural renderiza con VAO
   - [ ] Config.h sigue siendo usado para rutas
   - [ ] Rendimiento ≥ versión anterior

7. **Validación** ✓
   - [ ] Ejecutar con debugger (gdb/Visual Studio)
   - [ ] Verificar memoria con Valgrind/Dr.Memory
   - [ ] Validación de OpenGL con apitrace

---

## CONSIDERACIONES TÉCNICAS FINALES

### Compatibilidad Backward
- **Mantener**: Config.h, rutas de modelos, estructura LayerManager
- **Deprecar gradualmente**: Geometry.cpp (glBegin/glEnd) → sustituir con VAO version
- **No romper**: Interfaz pública de Model, no cambiar paths de modelos

### Performance
- **VAO/VBO**: Upload una sola vez en loadModel() → draw() es instant
- **Vertex cache**: GPU maneja índices automáticamente
- **Especular**: Calculado per-fragment (más exacto que fixed pipeline)
- **Culling**: Habilitar back-face culling (glCullFace)

### Debugging
- **Shader compilation errors**: Capturar con `glGetShaderInfoLog()`, loguear en console
- **VAO binding**: Verificar con `glGetError()` después de cada operación
- **Matrix math**: GLM maneja column-major (OpenGL standard) automáticamente
- **Uniforms**: Cache con nombre → evitar búsquedas repetidas

### Migraciones futuras
- **Normal mapping**: Agregar atributo tangent en Vertex
- **Texturas**: Agregar UV coordinates en Vertex + sampler2D en fragment shader
- **Deferred rendering**: VAO/VBO están listos para G-buffer
- **Instancing**: Usar glDrawElementsInstanced() para múltiples aviones

---

## TIMELINE SUGERIDO

| Fase | Pasos | Duración |
|------|-------|----------|
| Infraestructura | 1-4 | ~8-12 horas |
| Shaders | 5-6 | ~4-6 horas |
| Main loop | 7 | ~4-6 horas |
| Model loader | 8-9 | ~4-6 horas |
| Renderer | 10-11 | ~4-6 horas |
| Geometría | 12-13 | ~6-8 horas |
| Build & Testing | 14-15 | ~6-8 horas |
| **TOTAL** | | ~36-52 horas |

---

## ARCHIVOS CLAVE DE REFERENCIA

| Archivo actual | Reemplazado por | Cambios principales |
|---|---|---|
| main.cpp | main_ng.cpp | GLUT → GLFW, matrix stack → GLM |
| renderer.cpp/h | renderer_ng.cpp/h | Fixed pipeline → shaders, iluminación uniforms |
| model_loader.cpp/h | model_loader_ng.cpp/h | glBegin/glEnd → VAO/VBO, uploadToGPU() |
| geometry.cpp/h | geometry_ng.cpp/h | glBegin/glEnd → generadores de Mesh |
| (nuevo) | core/window.cpp/h | Wrapper GLFW |
| (nuevo) | core/input_manager.cpp/h | Manejo de input GLFW |
| (nuevo) | graphics/camera.cpp/h | Cámara GLM |
| (nuevo) | graphics/vao_manager.cpp/h | VAO/VBO encapsulación |
| (nuevo) | shaders/shader_manager.cpp/h | Compilación/linking de shaders |
| (nuevo) | shaders/vertex.glsl | VS: matrices + normales |
| (nuevo) | shaders/fragment.glsl | FS: iluminación Phong |
