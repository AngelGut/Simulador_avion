# 📦 Instalación - Fase A (Pre-Entrega)

## ¿Qué se implementó?

✅ Cargador de modelos 3D con **Assimp**  
✅ Soporte para archivos **OBJ, FBX, BLEND, GLTF**  
✅ Proyección 3D con perspectiva  
✅ Iluminación básica  
✅ Rotación en 3 ejes (I/K/J/L/R/T)  

## Pasos de Instalación

### 1️⃣ Instalar Assimp con vcpkg

```bash
vcpkg install assimp:x64-windows
```

Si no tienes vcpkg:
```bash
git clone https://github.com/Microsoft/vcpkg.git
cd vcpkg
.\bootstrap-vcpkg.bat
.\vcpkg integrate install
```

### 2️⃣ Configurar el Modelo a Cargar

Edita `config.h`:

```cpp
// config.h línea 10
#define MODEL_PATH "Millennium Falcon star wars.obj"
```

**Opciones disponibles:**
- `"Millennium Falcon star wars.obj"` ✅ (OBJ directo)
- `"American Airlines Boeing 737-800.blend"` (requiere exportar a OBJ)
- `"MS-406.fbx"` (Assimp lo convierte automáticamente)
- `"MYSTERE IV N 117 Provence Nancy.fbx"` (Assimp lo convierte)

### 3️⃣ Compilar en Visual Studio

1. Abre `Simulador_avion.sln`
2. Build → Build Solution (Ctrl+Shift+B)
3. Espera a que compile sin errores

### 4️⃣ Ejecutar

Debug → Start Debugging (F5)

O ejecuta directamente:
```bash
x64/Debug/Simulador_avion.exe
```

---

## 🎮 Controles

### Rotación 3D
- **I / K** → Rotar arriba/abajo (Pitch - eje X)
- **J / L** → Rotar izquierda/derecha (Yaw - eje Y)
- **R / T** → Rotar alrededor eje Z (Roll)

### Cámara
- **Q / E** → Zoom in/out (acercar/alejar)
- **W / A / S / D** → Pan (mover vista)

### Otros
- **ESPACIO** → Reset vista
- **H** → Mostrar/ocultar ayuda
- **ESC** → Salir

---

## 🔧 Solución de Problemas

### Error: "assimp.lib not found"
→ Verifica que vcpkg está integrado: `vcpkg integrate install`

### El modelo no aparece
→ Verifica que el archivo existe en el directorio de ejecución  
→ Comprueba la ruta en `config.h`

### Compilación fallida
→ Limpia: Build → Clean Solution  
→ Reconstruye: Build → Rebuild Solution

---

## 📊 Archivos Nuevos/Modificados

### Nuevos:
- ✅ `model_loader.h` - Header del cargador
- ✅ `model_loader.cpp` - Implementación de Assimp
- ✅ `config.h` - Configuración de rutas
- ✅ `ROADMAP_3D.md` - Plan de evolución
- ✅ `INSTALACION_OPCION_A.md` - Este archivo

### Modificados:
- ✏️ `renderer.h` - Nuevas funciones para modelos
- ✏️ `renderer.cpp` - Lógica de dibujado con modelos
- ✏️ `main.cpp` - Inicialización de modelos
- ✏️ `Simulador_avion.vcxproj` - Agrega model_loader.cpp y assimp.lib

---

## 🚀 Próximo Paso: Fase B

Cuando quieras evolucionar a shaders modernos y GLFW:
→ Consulta `ROADMAP_3D.md`

Comandos:
```bash
git add ROADMAP_3D.md INSTALACION_OPCION_A.md
git commit -m "Fase A: Cargador de modelos con Assimp"
git push origin angel
```

---

## ✅ Pre-Entrega Checklist

- [ ] Assimp instalado con vcpkg
- [ ] config.h apunta al modelo correcto
- [ ] Proyecto compila sin errores
- [ ] El modelo se visualiza 3D
- [ ] Controles de rotación funcionan
- [ ] Zoom y pan funcionan
- [ ] Se puede resent con ESPACIO
- [ ] Commit hecho a rama `angel`

¡Listo para pre-entrega! 🎉
