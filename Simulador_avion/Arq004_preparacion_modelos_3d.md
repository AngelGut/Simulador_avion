# ARQ004: Preparación e Integración de Modelos 3D

## Objetivo
Documentar el sistema de carga de modelos 3D para soportar múltiples formatos y preparar la infraestructura para visualizar los 5 modelos proporcionados.

## Modelos a Soportar

| # | Nombre | Archivo | Formato | Estado | Notas |
|---|--------|---------|---------|--------|-------|
| 1 | Boeing 737-800 (AA) | American Airlines Boeing 737-800.blend | BLEND | ⚠️ Conversión requerida | Necesita exportar a OBJ desde Blender |
| 2 | Kawasaki Ki-61 | kawasaki-ki-61.blend | BLEND | ⚠️ Conversión requerida | Necesita exportar a OBJ desde Blender |
| 3 | MS-406 (RAR) | uploads_files_5625074_MS-406.rar | RAR | ⚠️ Descompresión | Requiere 'unrar', contiene modelo interno |
| 4 | MYSTERE IV Provence | MYSTERE IV N 117 Provence Nancy.fbx | FBX | ⚠️ Conversión requerida | Requiere FBX SDK o Assimp |
| 5 | Millennium Falcon | Millennium Falcon star wars.obj | OBJ | ✅ Directo | Soportado nativamente |

## Arquitectura del Sistema

### Componentes Creados

#### 1. **model_loader.h / model_loader.cpp**
Sistema modular de carga de modelos 3D.

**Clases principales:**
- `Vec2`, `Vec3` - Estructuras matemáticas sin dependencias
- `Model` - Contenedor de datos de geometría
- `ModelLoader` - Clase base abstracta
- `OBJLoader` - Implementado (Wavefront OBJ)
- `FBXLoader` - Stub (requiere SDK)
- `BLENDLoader` - Stub (requiere Blender API)
- `RARLoader` - Implementado (descompresión + detección de modelo)
- `ModelManager` - Gestor central (singleton)

**Características:**
```cpp
// Uso básico:
ModelManager& mgr = ModelManager::getInstance();
mgr.loadModel("path/to/model.obj", "my_model");

Model* model = mgr.getModel("my_model");
std::vector<std::string> loaded = mgr.getLoadedModels();
```

#### 2. **model_config.h**
Configuración centralizada de rutas y metadatos de los 5 modelos.

**Estructura:**
```cpp
namespace ModelConfig {
    const char* MODELS_BASE_PATH = "./models/";
    
    struct ModelInfo {
        std::string name;
        std::string filename;
        std::string format;
        std::string description;
        bool requiresConversion;
        std::string conversionHint;
    };
    
    const std::vector<ModelInfo> AVAILABLE_MODELS = { ... };
}
```

### Flujo de Carga

```
Archivo Input
    ↓
ModelManager::loadModel(path, name)
    ↓
getLoaderForFormat(path) ← Detecta formato
    ↓
Loader específico (OBJLoader, RARLoader, etc.)
    ↓
Model (vertices, normals, texCoords, indices)
    ↓
ModelManager::models[name] = model
    ↓
Renderer: drawModel(model)
```

## Preparación de los Modelos

### Formato OBJ ✅ (Soportado Nativamente)
**Archivo:** `Millennium Falcon star wars.obj`
- ✅ Directamente compatible
- No requiere preparación
- Límites: Sin soporte para animaciones

### Formato BLEND ⚠️ (Requiere Conversión)
**Archivos:** 
- `American Airlines Boeing 737-800.blend`
- `kawasaki-ki-61.blend`

#### Opción A: Exportar desde Blender UI
```bash
# En Blender:
1. File → Export As
2. Seleccionar: Wavefront (.obj)
3. Guardar con mismo nombre (reemplazar .blend por .obj)
4. Opción: Export as OBJ
```

#### Opción B: Automatizar con Blender Headless
```bash
# Script Python (export_blend.py):
import bpy
import sys

# Parámetro: archivo de entrada
input_file = sys.argv[-1]
output_file = input_file.replace(".blend", ".obj")

bpy.ops.wm.open_mainfile(filepath=input_file)

# Opcionalmente limpiar geometría:
# bpy.ops.object.delete(use_global=False)

# Exportar OBJ
bpy.ops.wm.obj_export(
    filepath=output_file,
    export_selected=False
)
```

**Uso:**
```bash
blender -b archivo.blend -P export_blend.py --no-window
```

### Formato RAR ⚠️ (Deshabilitado por Seguridad)
**Archivo:** `uploads_files_5625074_MS-406.rar`

**NOTA:** Descompresión automática de RAR deshabilitada por razones de seguridad (vulnerabilidades en `system()` y `popen()`).

#### Solución:
Descomprime manualmente el RAR y carga el archivo `.obj` o `.fbx` resultante:

```bash
unrar x uploads_files_5625074_MS-406.rar
# Esto extrae el contenido a la carpeta actual
# Luego sube los archivos .obj/.fbx a ./models/
```

#### Alternativa Online:
- Usa herramientas online para descomprimir RAR
- Descarga el modelo interior
- Carga el .obj o .fbx

### Formato FBX ⚠️ (Requiere SDK o Conversión)
**Archivo:** `MYSTERE IV N 117 Provence Nancy.fbx`

#### Opción A: Usar FBX SDK de Autodesk
- Descargar de: https://www.autodesk.com/developer/downloads/fbx-sdk
- Compilar contra FBX SDK
- Actualizar CMakeLists.txt / proyecto

#### Opción B: Usar Assimp (Open Source)
```bash
# Instalar Assimp
sudo apt-get install libassimp-dev

# O compilar desde fuente
git clone https://github.com/assimp/assimp.git
cd assimp && cmake . && make && sudo make install
```

**Integración en model_loader.cpp:**
```cpp
// Con Assimp:
#include <assimp/Importer.hpp>
#include <assimp/scene.h>
#include <assimp/postprocess.h>

bool FBXLoader::parseFBXFile(const std::string& filePath, Model& model) {
    Assimp::Importer importer;
    const aiScene* scene = importer.ReadFile(
        filePath,
        aiProcess_Triangulate | aiProcess_FlipWindingOrder
    );
    // ... procesar scene
}
```

#### Opción C: Convertir FBX a OBJ Previamente
```bash
# Con Blender
blender -b MYSTERE.fbx -P export_to_obj.py --no-window

# O con Assimp CLI (si está disponible)
assimp export MYSTERE.fbx MYSTERE.obj
```

## Estructura de Directorios

```
Simulador_avion/
├── models/                    # Nuevo directorio
│   ├── American Airlines Boeing 737-800.blend
│   ├── American Airlines Boeing 737-800.obj  (después conversión)
│   ├── kawasaki-ki-61.blend
│   ├── kawasaki-ki-61.obj                     (después conversión)
│   ├── uploads_files_5625074_MS-406.rar
│   ├── MYSTERE IV N 117 Provence Nancy.fbx
│   ├── MYSTERE IV N 117 Provence Nancy.obj    (después conversión)
│   └── Millennium Falcon star wars.obj
│
└── Simulador_avion/
    ├── model_loader.h
    ├── model_loader.cpp
    ├── model_config.h
    ├── Arq004_preparacion_modelos_3d.md       (este archivo)
    └── ...
```

## Integración con Renderer

### Paso 1: Actualizar renderer.h
```cpp
namespace Renderer {
    // Nueva función para dibujar modelos
    void drawModel(const Model& model);
    void drawModelWireframe(const Model& model);
};
```

### Paso 2: Implementar en renderer.cpp
```cpp
void Renderer::drawModel(const Model& model) {
    if (model.isEmpty()) return;
    
    glPushMatrix();
    glTranslatef(model.position.x, model.position.y, model.position.z);
    glRotatef(model.rotation, 0.0f, 1.0f, 0.0f);
    glScalef(model.scale.x, model.scale.y, model.scale.z);
    
    glBegin(GL_TRIANGLES);
    for (unsigned int idx : model.indices) {
        if (idx < model.vertices.size()) {
            const Vec3& v = model.vertices[idx];
            glVertex3f(v.x, v.y, v.z);
        }
    }
    glEnd();
    
    glPopMatrix();
}
```

### Paso 3: Integrar en main.cpp
```cpp
#include "model_loader.h"
#include "model_config.h"

int main(int argc, char** argv) {
    // ... inicialización ...
    
    // Cargar modelos
    ModelManager& mgr = ModelManager::getInstance();
    
    // Cargar Millennium Falcon (OBJ - directo)
    mgr.loadModel(
        ModelConfig::getFullPath("Millennium Falcon star wars.obj"),
        "falcon"
    );
    
    // Cargar MS-406 desde RAR
    mgr.loadModel(
        ModelConfig::getFullPath("uploads_files_5625074_MS-406.rar"),
        "ms406"
    );
    
    // ... resto de main ...
    
    return 0;
}
```

## Utilidades de Desarrollo

### Script de Conversión Batch (bash)
```bash
#!/bin/bash
# convert_models.sh

MODELS_DIR="./models"

# Convertir archivos BLEND a OBJ
for blend_file in $MODELS_DIR/*.blend; do
    obj_file="${blend_file%.blend}.obj"
    if [ ! -f "$obj_file" ]; then
        echo "Convirtiendo $blend_file..."
        blender -b "$blend_file" -P convert_blend.py --no-window
    fi
done

# Descomprimir RAR (manual)
# unrar x $MODELS_DIR/*.rar $MODELS_DIR/

echo "Conversión completada"
```

### Script Python para Blender
```python
# export_blend.py (usar con blender -P)
import bpy
import os
import sys

# Obtener ruta del archivo
blend_file = bpy.data.filepath
if not blend_file:
    print("Error: No se especificó archivo")
    sys.exit(1)

# Generar ruta de salida
output_file = blend_file.replace(".blend", ".obj")

print(f"Exportando {blend_file} → {output_file}")

# Exportar
try:
    bpy.ops.wm.obj_export(filepath=output_file)
    print("✅ Exportación exitosa")
except Exception as e:
    print(f"❌ Error: {e}")
    sys.exit(1)
```

## Checklist de Preparación

- [ ] Crear directorio `./models/`
- [ ] Copiar los 5 archivos de modelo a `./models/`
- [ ] Verificar que `unrar` esté instalado (para RAR)
- [ ] Convertir archivos BLEND a OBJ:
  - [ ] American Airlines Boeing 737-800.blend → .obj
  - [ ] kawasaki-ki-61.blend → .obj
- [ ] Convertir FBX a OBJ:
  - [ ] MYSTERE IV N 117 Provence Nancy.fbx → .obj
- [ ] Compilar proyecto con model_loader.h/cpp incluidos
- [ ] Agregar includes a main.cpp
- [ ] Probar carga de cada modelo

## Testing

```cpp
// test_models.cpp (crear para verificar carga)
#include "model_loader.h"
#include "model_config.h"
#include <iostream>

int main() {
    ModelManager& mgr = ModelManager::getInstance();
    
    // Test: OBJ directo
    if (mgr.loadModel("./models/Millennium Falcon star wars.obj", "falcon")) {
        std::cout << "✅ Falcon: " << mgr.getModelVertexCount("falcon") 
                  << " vertices\n";
    }
    
    // Test: BLEND (si fue convertido)
    if (mgr.loadModel("./models/American Airlines Boeing 737-800.obj", "b737")) {
        std::cout << "✅ B737: " << mgr.getModelVertexCount("b737") 
                  << " vertices\n";
    }
    
    // Test: RAR
    if (mgr.loadModel("./models/uploads_files_5625074_MS-406.rar", "ms406")) {
        std::cout << "✅ MS-406: " << mgr.getModelVertexCount("ms406") 
                  << " vertices\n";
    }
    
    return 0;
}
```

## Limitaciones y Consideraciones

### Formato OBJ
- ✅ Totalmente soportado
- ✅ Sin animaciones (estático)
- ✅ Soporta vértices, normales, coordenadas UV

### Formato BLEND
- ❌ No hay loader nativo (formato binario Blender)
- ⚠️ Requiere conversión previa a OBJ
- 💡 Alternativa: usar Blender como servidor de conversión

### Formato FBX
- ❌ No hay loader sin SDK
- ⚠️ Requiere FBX SDK o librería Assimp
- 💡 Recomendación: convertir a OBJ previamente

### Formato RAR
- ✅ Descompresión automática
- ⚠️ Requiere `unrar` en el sistema
- 💡 Se auto-detecta modelo interno

### Limitaciones OpenGL
- 2D actual vs 3D (modelos son 3D)
- Actualmente la aplicación es 2D (ortho)
- Necesitaría cambio a proyección 3D (perspective) para visualizar correctamente

## Próximos Pasos

1. Crear directorio `./models/` y copiar archivos
2. Convertir BLEND y FBX a OBJ
3. Compilar con model_loader.h/cpp
4. Integrar carga en main.cpp
5. Cambiar proyección a 3D si es necesario
6. Implementar renderizado de modelos 3D
7. Agregar controles de cámara 3D (orbita, zoom, pan)

---
**Documento:** Arq004_preparacion_modelos_3d.md
**Fecha:** 2025-07-23
**Responsable:** Sistema de Carga de Modelos 3D
