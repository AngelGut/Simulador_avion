# 📦 Preparación de Modelos 3D - Guía Rápida

## 🎯 Objetivo
Preparar e integrar los 5 modelos 3D al simulador de avión.

## 📋 Modelos a Preparar

```
1. American Airlines Boeing 737-800.blend      → Convertir a OBJ
2. kawasaki-ki-61.blend                        → Convertir a OBJ
3. uploads_files_5625074_MS-406.rar            → Descomprimir
4. MYSTERE IV N 117 Provence Nancy.fbx         → Convertir a OBJ
5. Millennium Falcon star wars.obj             → Listo (no requiere conversión)
```

## 🚀 Quick Start

### Paso 1: Crear directorio de modelos
```bash
cd Simulador_avion
mkdir -p models
cp /ruta/a/modelos/* models/
```

### Paso 2: Ejecutar script de conversión
```bash
cd Simulador_avion
chmod +x convert_models.sh
./convert_models.sh
```

Este script:
- ✅ Convierte BLEND → OBJ (requiere Blender)
- ✅ Descomprime RAR (requiere unrar)
- ⚠️ Detecta FBX (requiere conversión manual)

### Paso 3: Verificar archivos
```bash
ls -la models/
# Deberías ver:
# - Millennium Falcon star wars.obj ✅
# - American Airlines Boeing 737-800.obj (después conversión)
# - kawasaki-ki-61.obj (después conversión)
# - ... y más
```

## 🔧 Conversión Manual por Formato

### OBJ (Millennium Falcon) ✅
No requiere conversión, ya está en formato soportado.

### BLEND → OBJ (Boeing 737, Kawasaki)

#### Opción A: Desde Blender UI
```
1. Abre el archivo .blend en Blender
2. File → Export As → Wavefront (.obj)
3. Guarda con mismo nombre
```

#### Opción B: Script Automatizado (recomendado)
```bash
# Convierte un archivo
blender -b models/American\ Airlines\ Boeing\ 737-800.blend \
        -P Simulador_avion/export_blend.py --no-window

# Convierte todos (si están en models/)
for file in models/*.blend; do
    blender -b "$file" -P Simulador_avion/export_blend.py --no-window
done
```

### RAR → Descomprimir (MS-406)

#### Instalar unrar
```bash
# Ubuntu/Debian
sudo apt-get install unrar

# macOS
brew install unrar

# RedHat/CentOS
sudo yum install unrar
```

#### Descomprimir
```bash
# Manual
unrar x models/uploads_files_5625074_MS-406.rar models/

# Script automático
./convert_models.sh
```

**Nota:** El archivo RAR contendrá otro modelo 3D internamente, que se cargará automáticamente.

### FBX → OBJ (MYSTERE IV)

FBX es un formato binario que requiere SDK especial. Opciones:

#### Opción A: Convertir con Blender
```bash
# Abre en Blender y exporta como OBJ
blender -b models/MYSTERE\ IV\ N\ 117\ Provence\ Nancy.fbx \
        -P Simulador_avion/export_blend.py --no-window
```

#### Opción B: Usar Assimp
```bash
# Instalar Assimp
sudo apt-get install libassimp-dev

# Convertir
assimp export models/MYSTERE.fbx models/MYSTERE.obj
```

#### Opción C: FBX Online Converter
- Visita: https://www.aspose.app/3d/conversion/fbx-to-obj
- Sube el archivo FBX
- Descarga como OBJ

## ✅ Checklist de Preparación

- [ ] Directorio `./models/` creado
- [ ] 5 archivos copiados a `./models/`
- [ ] Blender instalado (si tienes BLEND)
- [ ] `unrar` instalado (para RAR)
- [ ] Script ejecutado: `./convert_models.sh`
- [ ] Verificar archivos `.obj` en `./models/`

## 📊 Estructura Final Esperada

```
Simulador_avion/
├── models/
│   ├── American Airlines Boeing 737-800.obj         ✅
│   ├── American Airlines Boeing 737-800.blend       (original)
│   ├── kawasaki-ki-61.obj                           ✅
│   ├── kawasaki-ki-61.blend                         (original)
│   ├── MYSTERE IV N 117 Provence Nancy.obj          ✅
│   ├── MYSTERE IV N 117 Provence Nancy.fbx          (original)
│   ├── Millennium Falcon star wars.obj              ✅
│   ├── uploads_files_5625074_MS-406.rar             (original)
│   └── ... (archivos extraídos del RAR)
│
└── Simulador_avion/
    ├── model_loader.h                   (nuevo)
    ├── model_loader.cpp                 (nuevo)
    ├── model_config.h                   (nuevo)
    ├── export_blend.py                  (nuevo)
    ├── Arq004_preparacion_modelos_3d.md (nuevo)
    └── ...
```

## 🔌 Integración en el Código

Los archivos crean un sistema completo para cargar modelos:

```cpp
#include "model_loader.h"
#include "model_config.h"

// Cargar un modelo OBJ
ModelManager& mgr = ModelManager::getInstance();
mgr.loadModel("models/Millennium Falcon star wars.obj", "falcon");

// Obtener el modelo
Model* model = mgr.getModel("falcon");

// Info
std::cout << "Vértices: " << model->vertices.size();
std::cout << "Triángulos: " << model->indices.size() / 3;
```

## 📝 Archivos Generados

| Archivo | Propósito |
|---------|-----------|
| `model_loader.h` | Interfaz de carga (OBJ, FBX, BLEND, RAR) |
| `model_loader.cpp` | Implementación de loaders |
| `model_config.h` | Configuración de rutas y metadatos |
| `export_blend.py` | Script para exportar desde Blender |
| `convert_models.sh` | Automatización de conversiones |
| `Arq004_preparacion_modelos_3d.md` | Documentación detallada |

## ⚠️ Troubleshooting

### "unrar: command not found"
```bash
sudo apt-get install unrar
```

### "blender: command not found"
Instala Blender desde: https://www.blender.org/download/

### El OBJ generado está vacío
- Verifica que el archivo BLEND/FBX tenga meshes
- Intenta abrir en Blender UI y re-exportar manualmente

### FBX no se convierte
- Usa Assimp: `sudo apt-get install libassimp-dev`
- O convertir manualmente en Blender

### El RAR no se descomprime
- Instala `unrar` (ver arriba)
- O descomprime manualmente y verifica contenido

## 📚 Documentación Completa

Para detalles técnicos, ver: `Simulador_avion/Arq004_preparacion_modelos_3d.md`

---

**Próximo paso:** Una vez preparados los modelos, compilar el proyecto incluyendo los nuevos archivos y crear la integración con el renderer.
