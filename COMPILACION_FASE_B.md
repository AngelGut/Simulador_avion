# 🔧 Guía de Compilación - Fase B Moderna

## ⚠️ Cambios de Dependencias

La Fase B requiere nuevas librerías. Actualiza tu `vcpkg`:

### Instalación de Dependencias (Windows + vcpkg)

```bash
# Navega a tu directorio vcpkg
cd C:\path\to\vcpkg

# Instala las dependencias necesarias
vcpkg install glfw3:x64-windows glew:x64-windows assimp:x64-windows glm:x64-windows

# Integra con Visual Studio (si no lo hizo aún)
vcpkg integrate install
```

### Instalación de Dependencias (Linux)

```bash
sudo apt-get update
sudo apt-get install libglfw3-dev libglew-dev libassimp-dev libglm-dev
```

### Instalación de Dependencias (macOS)

```bash
brew install glfw3 glew assimp glm
```

## 📋 Configuración de Visual Studio (`.vcxproj`)

Si usas Visual Studio, actualiza tu archivo de proyecto:

### 1. **Directorios de Include**

Agrega en Propiedades → VC++ Directories → Include Directories:

```
$(SolutionDir)Simulador_avion/include
$(VCPKG_ROOT)/installed/x64-windows/include
```

### 2. **Directorios de Librería**

Agrega en Propiedades → VC++ Directories → Library Directories:

```
$(VCPKG_ROOT)/installed/x64-windows/lib
```

### 3. **Vinculación de Librerías**

Agrega en Propiedades → Linker → Input → Additional Dependencies:

```
glfw3.lib
glew32.lib
opengl32.lib
assimp-vc145-mt.lib
```

### 4. **Directorios de Ejecución**

Asegúrate que las DLL estén en el PATH:

```
$(VCPKG_ROOT)/installed/x64-windows/bin
```

O copia las DLL al directorio de salida (Debug/Release).

## 🎯 CMakeLists.txt (Alternativa)

Si prefieres usar CMake, crea este archivo en la raíz:

```cmake
cmake_minimum_required(VERSION 3.10)
project(Simulador_avion)

set(CMAKE_CXX_STANDARD 17)

# Buscar paquetes
find_package(glfw3 REQUIRED)
find_package(GLEW REQUIRED)
find_package(assimp REQUIRED)
find_package(OpenGL REQUIRED)
find_package(glm REQUIRED)

# Crear ejecutable
add_executable(Simulador_avion
    Simulador_avion/main.cpp
    Simulador_avion/renderer.cpp
    Simulador_avion/model_loader.cpp
    Simulador_avion/shader.cpp
    Simulador_avion/geometry.cpp
    Simulador_avion/layer_manager.cpp
    Simulador_avion/utils.cpp
)

# Vincular librerías
target_link_libraries(Simulador_avion
    glfw
    GLEW::GLEW
    assimp::assimp
    OpenGL::OpenGL
    glm::glm
)

# Directorios de include
target_include_directories(Simulador_avion PRIVATE
    ${CMAKE_CURRENT_SOURCE_DIR}/Simulador_avion/include
)

# Post-build: copiar shaders
add_custom_command(TARGET Simulador_avion POST_BUILD
    COMMAND ${CMAKE_COMMAND} -E copy_directory
    ${CMAKE_CURRENT_SOURCE_DIR}/Simulador_avion/shaders
    $<TARGET_FILE_DIR:Simulador_avion>/shaders
)
```

Compilar con CMake:

```bash
mkdir build
cd build
cmake ..
cmake --build . --config Release
```

## 📁 Estructura de Carpetas (Esperada)

```
Simulador_avion/
├── Simulador_avion/
│   ├── main.cpp              (← NUEVO: GLFW)
│   ├── shader.h              (← NUEVO)
│   ├── shader.cpp            (← NUEVO)
│   ├── model_loader.h        (← MODIFICADO: VAO/VBO)
│   ├── model_loader.cpp      (← MODIFICADO)
│   ├── renderer.h
│   ├── renderer.cpp
│   ├── geometry.h
│   ├── geometry.cpp
│   ├── config.h
│   ├── shaders/              (← NUEVA CARPETA)
│   │   ├── vertex.glsl       (← NUEVO)
│   │   └── fragment.glsl     (← NUEVO)
│   └── ... (otros archivos)
├── models/                   (Modelos 3D)
├── lib/                      (Librerías)
└── include/                  (Headers)
```

## ✅ Verificación Pre-Compilación

Antes de compilar, verifica:

- [ ] GLFW3 instalado: `glfw3.h` accesible
- [ ] GLEW instalado: `GL/glew.h` accesible
- [ ] Assimp instalado: `assimp/Importer.hpp` accesible
- [ ] GLM instalado: `glm/glm.hpp` accesible
- [ ] Carpeta `shaders/` existe en `Simulador_avion/`
- [ ] Archivos `.glsl` están en `shaders/`

## 🚀 Ejecución

Una vez compilado:

```bash
# Windows
Debug/Simulador_avion.exe

# Linux/macOS
./Simulador_avion
```

### Requisitos en Tiempo de Ejecución

1. **Carpeta `shaders/`** en el mismo directorio del ejecutable
2. **Carpeta `models/`** con archivos OBJ/FBX
3. **GPU compatible con OpenGL 3.3+**

## 🔍 Debugging

Si ves errores al compilar:

### Error: "glfw3.h: No such file or directory"
→ Verifica que GLFW está instalado en vcpkg
→ Verifica el path en Include Directories

### Error: "glfwInit undefined reference"
→ Verifica que glfw3.lib está en Additional Dependencies
→ Verifica que vcpkg integrate install se ejecutó

### Error: "Failed to load shaders"
→ Verifica que `shaders/` está en el mismo directorio que el ejecutable
→ Verifica permisos de lectura en los archivos `.glsl`

### Error: "OpenGL 3.3 not supported"
→ Actualiza drivers de GPU
→ Verifica que la GPU soporta OpenGL 3.3+

## 📊 Versiones Testeadas

- GLFW: 3.3.x, 3.4.x
- GLEW: 2.1.x, 2.2.x
- Assimp: 5.0.x, 5.1.x
- GLM: 0.9.9.x, 1.0.x
- OpenGL: 3.3+, 4.x

---

**Si tienes problemas, verifica:**
1. Versión de Visual Studio (2017+)
2. Versión de cmake (3.10+)
3. Drivers de GPU actualizados
4. Permisos de carpetas

