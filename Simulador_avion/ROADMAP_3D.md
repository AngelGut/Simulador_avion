# 🚀 Roadmap: De Geometría Procedural a Modelos 3D Cargables

## Estado Actual (v1.0)
- ✅ Proyección 3D con perspectiva
- ✅ Iluminación básica (OpenGL)
- ✅ Geometría procedural (fuselaje, alas, motores generados en código)
- ✅ Controles de rotación (I/K/J/L/R/T)
- ✅ Zoom y pan de cámara
- 🔴 **NO carga modelos desde archivos**

---

## 📋 Fase A: Cargador OBJ Simple (PRE-ENTREGA MAÑANA)

### Objetivo
Cargar modelos 3D reales (`.obj`) desde archivos en lugar de geometría procedural.

### Cambios Necesarios

#### 1. **Instalar Assimp** (vcpkg)
```bash
vcpkg install assimp:x64-windows
```

#### 2. **Nuevo archivo: `model_loader.h`**
```cpp
#include <assimp/Importer.hpp>
#include <assimp/scene.h>
#include <vector>
#include <glm/glm.hpp>

class Model {
public:
    struct Mesh {
        std::vector<glm::vec3> vertices;
        std::vector<glm::vec3> normals;
        std::vector<unsigned int> indices;
    };
    
    std::vector<Mesh> meshes;
    
    bool loadOBJ(const char* filename);
    void draw();
};
```

#### 3. **Nuevo archivo: `model_loader.cpp`**
- Implementar `Model::loadOBJ()` usando Assimp
- Leer vértices, normales, índices
- Almacenar en estructura `Mesh`

#### 4. **Modificar `renderer.cpp`**
```cpp
// Reemplazar:
GeometryBuilder::generateFuselage(...);  // Geometry procedural

// Con:
model.draw();  // Modelo cargado desde archivo
```

#### 5. **Archivo de configuración: `config.h`**
```cpp
#define MODEL_PATH "C:/ruta/al/boeing.obj"
```

### Stack Fase A
```
boeing.obj (archivo)
    ↓
Assimp.ReadFile()
    ↓
aiMesh (vértices + índices)
    ↓
Model::meshes[]
    ↓
glBegin(GL_TRIANGLES) → dibuja
    ↓
OpenGL 1.1 (Fixed Pipeline) + GLUT
```

### Ventajas Fase A
- ✅ Mínimos cambios en código existente
- ✅ GLUT sigue funcionando sin cambios
- ✅ Modelos cargables en 20 minutos
- ✅ Listo para pre-entrega mañana

### Limitaciones Fase A
- 🔴 Sin shaders modernos (solo iluminación fixed)
- 🔴 Rendimiento limitado con modelos complejos
- 🔴 Sin texturas
- 🔴 OpenGL 1.1 deprecado

---

## 🎨 Fase B: Stack Moderno con GLFW + GLM + Shaders (DESPUÉS)

### Objetivo
Evolucionan a arquitectura moderna con shaders GLSL y mejor rendimiento.

### Cambios Completos

#### 1. **Reemplazar GLUT → GLFW**
```cpp
// Viejo (GLUT)
glutInit(&argc, argv);
glutCreateWindow("Boeing");
glutMainLoop();

// Nuevo (GLFW)
glfwInit();
GLFWwindow* window = glfwCreateWindow(800, 600, "Boeing", NULL, NULL);
while (!glfwWindowShouldClose(window)) {
    render();
    glfwSwapBuffers(window);
}
```

#### 2. **Agregar GLM para matemáticas 3D**
```cpp
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>

// Matrices explícitas en lugar de glTranslatef()
glm::mat4 model = glm::mat4(1.0f);
model = glm::translate(model, glm::vec3(0, 0, 0));
model = glm::rotate(model, glm::radians(angle), glm::vec3(0, 1, 0));
```

#### 3. **Shaders GLSL Modernos**
```glsl
// vertex.glsl (OpenGL 3.3+)
#version 330 core
layout(location = 0) in vec3 aPosition;
layout(location = 1) in vec3 aNormal;

uniform mat4 uModel;
uniform mat4 uView;
uniform mat4 uProjection;

void main() {
    gl_Position = uProjection * uView * uModel * vec4(aPosition, 1.0);
}

// fragment.glsl
#version 330 core
out vec4 FragColor;
void main() {
    FragColor = vec4(1.0, 1.0, 1.0, 1.0);
}
```

#### 4. **Cambios en main.cpp**
- Inicializar GLFW en lugar de GLUT
- Crear contexto OpenGL 3.3+
- Compilar shaders
- Loop principal moderno

#### 5. **Cambios en renderer.cpp**
- Usar `glUseProgram()` para shaders
- Pasar matrices vía `glUniformMatrix4fv()`
- VAO/VBO en lugar de `glBegin()/glEnd()`

### Stack Fase B
```
boeing.obj
    ↓
Assimp (igual que Fase A)
    ↓
Model::meshes[] → VAO/VBO
    ↓
Shaders GLSL (vertex + fragment)
    ↓
GLM (transformaciones como matrices)
    ↓
OpenGL 3.3+ (Modern Pipeline)
    ↓
GLFW (ventana moderna)
```

### Ventajas Fase B
- ✅ Shaders modernos (iluminación Phong, PBR, etc.)
- ✅ Mejor rendimiento (VAO/VBO)
- ✅ Texturas y mapas normales
- ✅ Sombras, reflexiones, efectos avanzados
- ✅ OpenGL 3.3+ (soportado en cualquier GPU)

### Trabajo Estimado Fase B
- Reescribir `main.cpp`: ~200 líneas
- Nuevos shaders: ~100 líneas
- Cambiar `renderer.cpp`: ~150 líneas
- Total: ~2-3 horas

---

## 📅 Timeline

| Fase | Cuando | Duración | Resultado |
|------|--------|----------|-----------|
| **A** | Mañana (antes pre-entrega) | 30 min | Modelos 3D cargables OBJ |
| **B** | Después (evolución) | 2-3 horas | Shaders modernos + mejor visual |

---

## ✅ Checklist Fase A (HOY)

- [ ] Instalar Assimp con vcpkg
- [ ] Crear `model_loader.h` y `model_loader.cpp`
- [ ] Implementar `Model::loadOBJ()`
- [ ] Crear `config.h` con ruta del modelo
- [ ] Modificar `renderer.cpp` para usar `model.draw()`
- [ ] Compilar y probar con `boeing.obj`
- [ ] Commit a rama `angel`

---

## ✅ Checklist Fase B (DESPUÉS)

- [ ] Reescribir `main.cpp` para GLFW
- [ ] Implementar loop principal GLFW
- [ ] Crear shaders vertex.glsl y fragment.glsl
- [ ] Cambiar renderer para usar shaders
- [ ] Implementar VAO/VBO
- [ ] Integrar GLM para matrices
- [ ] Compilar y probar
- [ ] Evolucionar shaders (Phong, PBR, etc.)

---

## 🔗 Referencias

### Assimp
- https://assimp.org/
- `#include <assimp/Importer.hpp>`

### GLFW (para Fase B)
- https://www.glfw.org/

### GLM (para Fase B)
- https://github.com/g-truc/glm

### Modelos Disponibles
- `American Airlines Boeing 737-800.blend` → Exportar a OBJ
- `Millennium Falcon star wars.obj` ✅ Listo
- `MS-406.fbx` → Convertir a OBJ (Assimp lo hace)
- `MYSTERE IV N 117 Provence Nancy.fbx` → Convertir a OBJ

---

## 🎯 Resumen

**Fase A (Mañana):** Assimp carga OBJ, GLUT muestra, listo para pre-entrega.  
**Fase B (Después):** Evolucionar a GLFW + shaders modernos para mejor visual y rendimiento.

Ambas usan **Assimp** para cargar modelos. La diferencia es el "wrapper" (GLUT vs GLFW) y cómo se renderizan (fixed pipeline vs shaders).
