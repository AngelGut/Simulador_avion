# 🔧 Guía de Compilación - Fase B Moderna

## ⚠️ Cambios de Dependencias

La Fase B requiere nuevas librerías. Actualiza tu `vcpkg`:

### Instalación de Dependencias (Windows + vcpkg)

```bash
# Navega a tu directorio vcpkg
cd C:\path\to\vcpkg

# Instala las dependencias necesarias
vcpkg install glfw3:x64-windows assimp:x64-windows glm:x64-windows

# Integra con Visual Studio (si no lo hizo aún)
vcpkg integrate install
```

*(Nota: Esta versión utiliza **GLAD** en lugar de GLEW. GLAD se compila directamente desde el código fuente incluido en el proyecto (`glad.c`), por lo que no es necesario instalarlo externamente).*

### Instalación de Dependencias (Linux)

```bash
sudo apt-get update
sudo apt-get install libglfw3-dev libassimp-dev libglm-dev
```

### Instalación de Dependencias (macOS)

```bash
brew install glfw3 assimp glm
```

## 📋 Configuración de Visual Studio (`.vcxproj`)

Si usas Visual Studio, la configuración portable ya viene lista en el archivo del proyecto:

### 1. **Directorios de Include**
Establecido en Propiedades → VC++ Directories → Include Directories:
```
$(SolutionDir)Simulador_avion/include
$(VCPKG_ROOT)/installed/x64-windows/include
```

### 2. **Directorios de Librería**
Establecido en Propiedades → VC++ Directories → Library Directories:
```
$(VCPKG_ROOT)/installed/x64-windows/lib
```

### 3. **Vinculación de Librerías**
Establecido en Propiedades → Linker → Input → Additional Dependencies:
```
glfw3.lib
opengl32.lib
assimp-vc145-mt.lib
```

---

## 📁 Estructura de Carpetas (Esperada)

```
Simulador_avion/
├── Simulador_avion/
│   ├── main.cpp              (Punto de entrada GLFW)
│   ├── shader.h/cpp          (Gestor de shaders GLSL)
│   ├── model_loader.h/cpp    (Cargador Assimp moderno y VAO/VBO)
│   ├── model_renderer.h/cpp  (Administrador de buffers de GPU y VAO)
│   ├── ui_renderer.h/cpp     (Renderizado 2D de textos y HUD)
│   ├── renderer.h/cpp        (Manejo del hangar y modos de piezas)
│   ├── geometry.h/cpp        (Geometría procedural fallback)
│   ├── app_state.h           (Variables globales de estado)
│   ├── glad.c                (Cargador OpenGL Core)
│   └── shaders/              
│       ├── vertex.glsl       (Vertex shader)
│       └── fragment.glsl     (Fragment shader)
├── models/                   (Modelos 3D GLB optimizados)
├── lib/                      (Librerías compiladas locales)
└── include/                  (Headers del proyecto)
```

## ✅ Verificación Pre-Compilación

Antes de compilar, verifica:
- [ ] GLFW3 instalado y accesible.
- [ ] Assimp instalado y accesible.
- [ ] GLM instalado y accesible.
- [ ] La carpeta `shaders/` existe junto al código fuente y contiene los archivos `.glsl`.

## 🚀 Ejecución

Una vez compilado:
* **Desde Visual Studio:** Presiona **F5** (Debug) o **Ctrl + F5** (Sin depurar).
* **Directorio de salida:** El ejecutable requiere las librerías dinámicas (`glfw3.dll`, `assimp-vc145-mt.dll`) y las carpetas `models/` y `shaders/` en su mismo nivel de ejecución. La copia de DLLs se automatiza con el archivo `copy_dlls.bat` en la fase de post-build.

---

## 🔍 Debugging

### Error: "glfw3.h: No such file or directory"
→ Verifica que GLFW está instalado en vcpkg (`vcpkg list`).
→ Confirma que ejecutaste `vcpkg integrate install`.

### Error: "Failed to load shaders"
→ Verifica que la carpeta `shaders/` con `vertex.glsl` y `fragment.glsl` esté en la misma carpeta que el ejecutable compilado.

### Error: "OpenGL 3.3 not supported"
→ Actualiza los drivers de tu GPU. La aplicación requiere soporte para OpenGL 3.3 Core Profile.
