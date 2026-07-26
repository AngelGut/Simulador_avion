# 🔧 Configuración Visual Studio para Fase B

## ⚠️ Problema Actual

Visual Studio no encuentra:
- `GL/glew.h` 
- `GLFW/glfw3.h`
- `glm/glm.hpp`
- `assimp/Importer.hpp`

## ✅ Solución Rápida

### Opción 1: Actualizar Include Directories en `.vcxproj`

1. **Abre el archivo** `Simulador_avion/Simulador_avion.vcxproj` con editor de texto

2. **Busca** la sección VC++ Directories:
```xml
<PropertyGroup>
  <IncludePath>...</IncludePath>
</PropertyGroup>
```

3. **Reemplaza** con las rutas correctas:
```xml
<PropertyGroup>
  <IncludePath>$(SolutionDir)include;$(VCPKG_ROOT)\installed\x64-windows\include;$(IncludePath)</IncludePath>
  <LibraryPath>$(SolutionDir)lib;$(VCPKG_ROOT)\installed\x64-windows\lib;$(LibraryPath)</LibraryPath>
</PropertyGroup>
```

### Opción 2: Usar CMakeLists.txt (Recomendado)

Si prefieres CMake (más moderno):

```bash
cd /home/user/Simulador_avion
mkdir build
cd build
cmake .. -DCMAKE_BUILD_TYPE=Release
cmake --build . --config Release
```

Asegúrate que vcpkg está integrado:
```bash
vcpkg integrate install
```

### Opción 3: Configurar Manualmente en Visual Studio

1. **Abre Visual Studio**
2. **Project → Properties**
3. **VC++ Directories**
4. **Include Directories**, agrega:
   - `C:\path\to\vcpkg\installed\x64-windows\include`
   - `C:\path\to\project\include`
5. **Library Directories**, agrega:
   - `C:\path\to\vcpkg\installed\x64-windows\lib`
   - `C:\path\to\project\lib`

6. **Linker → Input → Additional Dependencies**, agrega:
```
glfw3.lib
glew32.lib
opengl32.lib
assimp-vc145-mt.lib
```

## 📋 Verificación

Después de configurar, estos archivos deben compilar sin errores:

- ✅ main.cpp (GLFW)
- ✅ shader.cpp (GLEW, GLM)
- ✅ model_loader.cpp (Assimp, GLM, GLEW)
- ✅ renderer.cpp (GLEW)
- ✅ geometry.cpp (GLEW)

## 🚀 Compilar Después

```bash
# Si usas CMake
cd build
cmake --build . --config Release

# Si usas Visual Studio IDE
Build → Build Solution (F7)
```

## ⚡ Troubleshooting

**Error: "GL/glew.h not found"**
→ Verifica que VCPKG_ROOT está en el PATH
→ Verifica Include Directories en project properties

**Error: "glfw3.lib not found"**
→ Verifica Library Directories
→ Verifica que glfw3 está instalado: `vcpkg list | grep glfw`

**Error: "identifier ... is undefined"**
→ Asegúrate que GL/glew.h se incluye ANTES que cualquier macro GL
→ Orden correcto: `#include <GL/glew.h>` → `#include <GLFW/glfw3.h>`

---

**Una vez configurado, todo debería compilar sin errores.**
