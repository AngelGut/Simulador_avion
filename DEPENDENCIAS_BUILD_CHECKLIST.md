# DEPENDENCIAS, BUILD Y CHECKLIST DE VALIDACIÓN

## FASE 0: VERIFICAR DEPENDENCIAS

### Dependencias a Remover
```bash
❌ GLUT (Windows: freeglut, Linux: freeglut3-dev)
```

### Dependencias a Mantener
```bash
✓ Assimp (ya instalado - model loading)
✓ GLM (ya disponible - matemáticas)
✓ C++ 11+ compiler (GCC 7.0+, Clang 5.0+, MSVC 2017+)
```

### Dependencias a Agregar

#### Windows (Visual Studio 2019+)
```
GLFW3:
  - Descargar: https://www.glfw.org/download.html (precompiled binaries)
  - Path típico: C:/Libraries/glfw-3.3.8/
  - Includes: glfw-3.3.8/include
  - Libs: glfw-3.3.8/lib-vc2019/glfw3.lib

GLEW:
  - Descargar: http://glew.sourceforge.net/
  - Path típico: C:/Libraries/glew-2.2.0/
  - Includes: glew-2.2.0/include
  - Libs: glew-2.2.0/lib/Release/x64/glew32s.lib (static)
  - Define: GLEW_STATIC en preprocessor

OpenGL: 
  - Incluida en Windows SDK (nativo)
```

#### Linux (Ubuntu/Debian)
```bash
sudo apt update
sudo apt install -y \
  libglfw3-dev \
  libglew-dev \
  libglm-dev \
  libassimp-dev \
  freeimage-dev \
  mesa-common-dev

# Verificar instalación
pkg-config --cflags --libs glfw3
pkg-config --cflags --libs glew
```

#### macOS (Homebrew)
```bash
brew install glfw3 glew glm assimp
```

---

## CONFIGURACIÓN DEL PROYECTO

### Estructura de directorios final

```
Simulador_avion/
├── CMakeLists.txt                 [MODIFICADO]
├── src/
│   ├── main_ng.cpp               [NUEVO]
│   ├── config.h                  [SIN CAMBIOS]
│   ├── layer_manager.cpp/h       [SIN CAMBIOS]
│   │
│   ├── core/
│   │   ├── window.cpp/h          [NUEVO]
│   │   ├── input_manager.cpp/h   [NUEVO]
│   │   └── CMakeLists.txt        [NUEVO]
│   │
│   ├── graphics/
│   │   ├── camera.cpp/h          [NUEVO]
│   │   ├── vao_manager.cpp/h     [NUEVO]
│   │   ├── renderer_ng.cpp/h     [NUEVO]
│   │   └── CMakeLists.txt        [NUEVO]
│   │
│   ├── shaders/
│   │   ├── shader_manager.cpp/h  [NUEVO]
│   │   ├── vertex.glsl           [NUEVO]
│   │   ├── fragment.glsl         [NUEVO]
│   │   └── CMakeLists.txt        [NUEVO]
│   │
│   ├── model_loader_ng.cpp/h     [MODIFICADO]
│   ├── geometry_ng.cpp/h         [MODIFICADO]
│   ├── utils.cpp/h               [SIN CAMBIOS]
│   │
│   ├── include/
│   │   └── [Assimp, GLM headers]
│   │
│   └── source/
│       ├── *.obj / *.fbx / *.blend [Modelos - sin cambios]
│
├── build/                        [Directorio de compilación]
├── shaders/                      [Copiar aquí: vertex.glsl, fragment.glsl]
└── docs/
    ├── PLAN_MODERNIZACION_OPENGL.md
    ├── CODIGO_IMPLEMENTACION.md
    └── DEPENDENCIAS_BUILD_CHECKLIST.md [Este archivo]
```

---

## CMAKE CONFIGURATION

### CMakeLists.txt Principal (Raíz)

```cmake
cmake_minimum_required(VERSION 3.16)
project(Simulador_avion)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

# ============================================================
# CONFIGURACIÓN DE PATHS (MODIFICAR SEGÚN TU SISTEMA)
# ============================================================

# Windows - Descomentar y ajustar paths
# set(GLFW_ROOT "C:/Libraries/glfw-3.3.8")
# set(GLEW_ROOT "C:/Libraries/glew-2.2.0")

# Linux - Usar pkg-config (automático)
# macOS - Automático con Homebrew

# ============================================================
# ENCONTRAR DEPENDENCIAS
# ============================================================

find_package(OpenGL REQUIRED)
find_package(assimp REQUIRED)
find_package(glm REQUIRED)

# GLFW
find_package(glfw3 REQUIRED)

# GLEW
find_package(GLEW REQUIRED)
if(NOT GLEW_FOUND)
    message(FATAL_ERROR "GLEW no encontrado")
endif()

# ============================================================
# DEFINICIONES Y FLAGS
# ============================================================

# GLEW_STATIC si usas versión estática (Windows)
if(MSVC)
    add_definitions(-DGLEW_STATIC)
    set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} /W4")
else()
    set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -Wall -Wextra -pedantic")
endif()

# ============================================================
# ARCHIVOS FUENTE
# ============================================================

set(SOURCES
    src/main_ng.cpp
    src/config.h
    src/layer_manager.cpp
    src/layer_manager.h
    src/utils.cpp
    src/utils.h
    
    src/core/window.cpp
    src/core/window.h
    src/core/input_manager.cpp
    src/core/input_manager.h
    
    src/graphics/camera.cpp
    src/graphics/camera.h
    src/graphics/vao_manager.cpp
    src/graphics/vao_manager.h
    src/graphics/renderer_ng.cpp
    src/graphics/renderer_ng.h
    
    src/shaders/shader_manager.cpp
    src/shaders/shader_manager.h
    
    src/model_loader_ng.cpp
    src/model_loader_ng.h
    
    src/geometry_ng.cpp
    src/geometry_ng.h
)

# ============================================================
# EXECUTABLE
# ============================================================

add_executable(Simulador_avion ${SOURCES})

# ============================================================
# INCLUDE DIRECTORIES
# ============================================================

target_include_directories(Simulador_avion PRIVATE
    ${CMAKE_CURRENT_SOURCE_DIR}/src
    ${CMAKE_CURRENT_SOURCE_DIR}/src/include
    ${OPENGL_INCLUDE_DIR}
    ${GLEW_INCLUDE_DIRS}
    ${GLFW_INCLUDE_DIRS}
    ${GLM_INCLUDE_DIRS}
)

# ============================================================
# LINKING
# ============================================================

target_link_libraries(Simulador_avion
    PRIVATE
        OpenGL::OpenGL
        GLEW::GLEW
        glfw
        assimp::assimp
)

# ============================================================
# COPY SHADERS A BUILD DIR
# ============================================================

add_custom_command(TARGET Simulador_avion POST_BUILD
    COMMAND ${CMAKE_COMMAND} -E make_directory 
            $<CONFIG:Debug>$<CONFIG:Release>$<CONFIG:RelWithDebInfo>/shaders
    COMMAND ${CMAKE_COMMAND} -E copy_directory
            ${CMAKE_SOURCE_DIR}/src/shaders
            $<CONFIG:Debug>$<CONFIG:Release>$<CONFIG:RelWithDebInfo>/shaders
    COMMENT "Copiando shaders a directorio de build"
)

# ============================================================
# PRINT CONFIG
# ============================================================

message(STATUS "=== SIMULADOR AVIÓN BUILD ===")
message(STATUS "OpenGL: ${OPENGL_LIBRARIES}")
message(STATUS "GLEW: ${GLEW_LIBRARIES}")
message(STATUS "GLFW: ${GLFW_LIBRARIES}")
message(STATUS "Assimp: ${ASSIMP_LIBRARIES}")
message(STATUS "C++ Standard: ${CMAKE_CXX_STANDARD}")
```

---

## BUILD INSTRUCTIONS

### Windows (Visual Studio)

```bash
cd Simulador_avion
mkdir build
cd build

# Generar solución Visual Studio
cmake -G "Visual Studio 16 2019" -A x64 ^
  -DGLFW_ROOT="C:/Libraries/glfw-3.3.8" ^
  -DGLEW_ROOT="C:/Libraries/glew-2.2.0" ..

# Compilar
cmake --build . --config Release

# Ejecutar
Release/Simulador_avion.exe
```

### Linux

```bash
cd Simulador_avion
mkdir build
cd build

# Generar Makefile
cmake -DCMAKE_BUILD_TYPE=Release ..

# Compilar (paralelo con -j4)
cmake --build . -j4

# Ejecutar
./Simulador_avion
```

### macOS

```bash
cd Simulador_avion
mkdir build
cd build

cmake -DCMAKE_BUILD_TYPE=Release ..
cmake --build .

./Simulador_avion
```

---

## CHECKLIST DE VALIDACIÓN

### PASO 1: Compilación ✓

- [ ] `cmake` genera archivos sin errores
- [ ] Compilación sin warnings críticos
- [ ] Linkediting exitoso (todos los símbolos resueltos)
- [ ] No hay errores de "undefined reference"

**Debugging si falla:**
```bash
# Linux/macOS: Ver símbolos no resueltos
ldd ./Simulador_avion

# Windows: Ver símbolos en dependencias
dumpbin /depends Simulador_avion.exe

# Verbose cmake
cmake --debug-output
```

---

### PASO 2: Inicialización ✓

En consola, verificar output:
```
====================================================
   Boeing 737 Visualizer 3D (Modernizado)
====================================================

OpenGL 3.3.0 (o superior)

=== SELECCIONA AERONAVE ===
1. American Airlines Boeing 737-800
...
Opción (1-5): 5
```

**Validar cada componente:**
- [ ] GLFW window se abre (1024x768)
- [ ] GLEW inicializa sin errores
- [ ] OpenGL version >= 3.3
- [ ] No hay crashes en startup

**Debugging si falla:**
```cpp
// En main_ng.cpp, agregar logs
std::cout << "Ventana creada: " << (window.getHandle() != nullptr) << "\n";
std::cout << "GLEW init: " << (glewInit() == GLEW_OK) << "\n";
std::cout << "Shader compiled: " << shader.getProgramID() << "\n";
```

---

### PASO 3: Carga de Shaders ✓

Consola debe mostrar:
```
✓ vertex.glsl compilado
✓ fragment.glsl compilado
✓ Shader program compilado exitosamente
```

**Validar:**
- [ ] Ambos shaders compilados sin errores
- [ ] Uniforms se encontraron (nombre en uniforms log)
- [ ] Program linking exitoso

**Debugging si falla:**
```cpp
// En shader_manager.cpp, agregar:
if (glGetError() != GL_NO_ERROR) {
    std::cerr << "OpenGL Error después de compilación\n";
}
```

---

### PASO 4: Carga de Modelo ✓

Consola debe mostrar:
```
Cargando: [modelo path] (5 meshes)
  Mesh: 12345 vértices, 4000 triángulos
  ...
✓ Modelo cargado exitosamente

Subiendo modelo a GPU...
✓ VAO creado: 12345 vértices, 4000 triángulos
✓ Modelo cargado en GPU
```

**Validar:**
- [ ] Assimp carga archivo correctamente
- [ ] Número de meshes > 0
- [ ] VAO creados sin errores
- [ ] No hay memory leaks (Valgrind)

**Debugging si falla:**
```bash
# Verificar archivo existe
ls -la "path/to/model.obj"

# Usar assimp tool
assimp info "path/to/model.obj"
```

---

### PASO 5: Renderizado ✓

Ventana debe mostrar:
- [ ] Modelo renderizado en pantalla
- [ ] Colores correctos (grises por defecto)
- [ ] Sombreado Phong visible (no plano)
- [ ] Especulares visibles (puntos brillantes)
- [ ] Sin flickering o artefactos

**Visual test:**
```
Rotar modelo (I/K) → ¿Se ve 3D?
Cambiar ángulo → ¿Cambia iluminación?
Acercar (E) → ¿Crece sin clipping?
Alejar (Q) → ¿Desaparece limpiamente?
```

**Debugging si falla:**
```glsl
// En fragment.glsl, agregar:
// FragColor = vec4(vNormal, 1.0);  // Ver normales
// FragColor = vec4(normalize(fs_in.normal), 1.0);
```

---

### PASO 6: Interacción ✓

| Tecla | Esperado | Validar |
|-------|----------|---------|
| **I/K** | Rotación pitch (arriba/abajo) | [ ] |
| **J/L** | Rotación yaw (izq/der) | [ ] |
| **R/T** | Rotación roll (CW/CCW) | [ ] |
| **Q/E** | Zoom out/in | [ ] |
| **W/A/S/D** | Pan arriba/izq/abajo/der | [ ] |
| **ESPACIO** | Reset vista a inicial | [ ] |
| **1-5** | Cambiar modelo (si se implementa) | [ ] |
| **H** | Mostrar ayuda (si se implementa) | [ ] |
| **ESC** | Cierre limpio (exit 0) | [ ] |

**Debugging:**
```cpp
// En input_manager.cpp, loguear:
std::cout << "Key pressed: " << key << "\n";
std::cout << "Camera position: " << camera.getPosition().z << "\n";
```

---

### PASO 7: Performance ✓

**FPS Meter** (agregar a main_ng.cpp):
```cpp
static float lastTime = 0;
static int frameCount = 0;
float currentTime = glfwGetTime();
frameCount++;

if (currentTime - lastTime >= 1.0) {
    std::cout << "FPS: " << frameCount << "\n";
    frameCount = 0;
    lastTime = currentTime;
}
```

**Validar:**
- [ ] FPS >= 60 (sin lag)
- [ ] Consumo GPU < 80% (si hay GPU monitor)
- [ ] Sin stuttering
- [ ] Sin memory leaks (ejecutar 5+ minutos)

**Benchmarking:**
```bash
# Linux: usar perf o profilers
perf stat ./Simulador_avion

# macOS: Instruments
instruments -t "System Trace" ./Simulador_avion

# Valgrind (memory check)
valgrind --leak-check=full ./Simulador_avion
```

---

### PASO 8: Compatibilidad Layer Manager ✓

Si se mantiene layer_manager:
- [ ] `layerManager.init()` ejecuta sin error
- [ ] `layerManager.getActiveLayer()` retorna layer válida
- [ ] Cambio de capas no causa crashes

```cpp
// En main_ng.cpp
layerManager.setActiveLayer(1);
if (layerManager.isVisible(1)) {
    // Renderizar capa 1
}
```

---

### PASO 9: Compatibilidad Geometría Procedural ✓

Si modelo falla, fallback a geometría procedural:
- [ ] Geometría procedural renderiza sin error
- [ ] No hay crashes si falta archivo de modelo
- [ ] Colores aplicados correctamente

```cpp
// En renderer_ng.cpp, drawLayer()
if (!model.isLoaded()) {
    Mesh fuselage = GeometryBuilder::generateFuselage(...);
    fuselage.uploadToGPU();
    fuselage.draw();
}
```

---

### PASO 10: Validación de OpenGL Errors ✓

Agregar error checking en puntos críticos:

```cpp
// Función helper
void checkGLError(const char* context) {
    GLenum error = glGetError();
    if (error != GL_NO_ERROR) {
        std::cerr << context << " - OpenGL Error: " 
                  << error << " (0x" << std::hex << error << ")\n";
    }
}

// Usar en render loop
glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
checkGLError("Clear");

shader.use();
checkGLError("Shader use");

renderer.drawModel(model, modelMatrix, camera);
checkGLError("Draw model");
```

**Código de errores comunes:**
| Código | Significado | Causa |
|--------|------------|-------|
| 0x0500 | `GL_INVALID_ENUM` | Parámetro inválido en función GL |
| 0x0501 | `GL_INVALID_VALUE` | Valor fuera de rango |
| 0x0502 | `GL_INVALID_OPERATION` | Operación no válida en estado actual |
| 0x0505 | `GL_OUT_OF_MEMORY` | GPU sin memoria |
| 0x0506 | `GL_INVALID_FRAMEBUFFER_OPERATION` | FBO inválido |

---

## TROUBLESHOOTING

### ❌ Problema: "GLFW no encontrado"
```bash
# Verificar instalación
pkg-config --list-all | grep glfw
sudo apt reinstall libglfw3-dev

# O compilar GLFW manualmente
git clone https://github.com/glfw/glfw.git
cd glfw
cmake .
make && sudo make install
```

### ❌ Problema: "GLEW_STATIC undefined"
```cpp
// En CMakeLists.txt, agregar antes de find_package(GLEW)
add_definitions(-DGLEW_STATIC)
```

### ❌ Problema: "Shaders no compilan"
```
Verificar:
1. Archivo existe en directorio correcto
2. Sintaxis GLSL es válida (usar validador glslang)
3. Version #version 330 core en ambos shaders
4. Uniforms declarados en ambos shaders donde se usen
```

### ❌ Problema: "Modelo cargado pero no se ve"
```
Verificar:
1. VAO bind/unbind correcto
2. Índices correctos (no fuera de rango)
3. Matrices proyección/view correctas
4. Normal matrix calculada correctamente
5. Luz posición visible desde cámara
6. Culling face no oculta todo: GL_BACK es correcto
```

### ❌ Problema: "Flickering o tearing"
```cpp
// En window.cpp, activar VSync
glfwSwapInterval(1);  // 60 FPS locked
```

### ❌ Problema: "Memory leaks"
```bash
# Valgrind
valgrind --leak-check=full --show-leak-kinds=all ./Simulador_avion

# Agregar al código:
// delete de todos los recursos
glDeleteProgram(shader.getProgramID());
glDeleteVertexArrays(...);
glfwTerminate();
```

---

## CHECKLIST FINAL DE DEPLOYMENT

- [ ] Compilación en Windows, Linux y macOS exitosa
- [ ] Todas las dependencias instaladas
- [ ] Shaders copiados al directorio de build
- [ ] Modelos accesibles desde config.h
- [ ] FPS >= 60 en hardware objetivo
- [ ] Sin memory leaks después de 5+ minutos
- [ ] Sin OpenGL errors en consola
- [ ] Renderizado visualmente correcto (colores, iluminación)
- [ ] Todos los controles funcionan
- [ ] Exit limpio (sin crashes)
- [ ] LayerManager compatible
- [ ] Geometría procedural fallback funciona
- [ ] README actualizado con instrucciones de build
- [ ] Código comentado en funciones críticas
- [ ] Git ignore configurado (build/, obj/, *.exe)

