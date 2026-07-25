# 🚀 Boeing 737 Visualizer 3D - Simulador de Aviones

**Estado:** ✅ Listo para Pre-Entrega (Fase A)

Una aplicación 3D interactiva que carga y visualiza 5 modelos de aeronaves diferentes con controles de rotación, zoom y navegación.

---

## 📋 Requisitos Previos

- **Visual Studio 2022** (o similar)
- **vcpkg** (gestor de paquetes de C++)
- **C++20** o superior

---

## ⚡ Instalación Rápida (5 minutos)

### 1️⃣ Clonar el repositorio

```bash
git clone https://github.com/AngelGut/Simulador_avion.git
cd Simulador_avion
```

### 2️⃣ Instalar Assimp con vcpkg

```bash
# Si ya tienes vcpkg:
vcpkg install assimp:x64-windows

# Si NO tienes vcpkg, primero clonalo:
git clone https://github.com/Microsoft/vcpkg.git
cd vcpkg
.\bootstrap-vcpkg.bat
.\vcpkg integrate install
cd ..
vcpkg install assimp:x64-windows
```

### 3️⃣ Abrir en Visual Studio

```
1. Abre: Simulador_avion/Simulador_avion.sln
2. Esperá que Visual Studio indexe (5-10 seg)
3. Menú: Build → Rebuild Solution (Ctrl+Shift+B)
4. Debug → Start Debugging (F5) o presiona F5
```

### 4️⃣ ¡Listo! 🎉

El programa abrirá una consola pidiendo que selecciones un modelo (1-5).

---

## 🎮 Controles

| Acción | Teclas |
|--------|--------|
| **Rotar arriba/abajo** | I / K |
| **Rotar izq/der** | J / L |
| **Rotar Z (roll)** | R / T |
| **Zoom in/out** | Q / E |
| **Mover vista** | W / A / S / D |
| **Cambiar modelo** | 1, 2, 3, 4, 5 |
| **Reset vista** | ESPACIO |
| **Ver ayuda** | H |
| **Salir** | ESC |

---

## ✈️ Modelos Disponibles

```
1 → American Airlines Boeing 737-800 (BLEND)
2 → Kawasaki Ki-61 (BLEND)
3 → MS-406 (FBX)
4 → MYSTERE IV N 117 (FBX)
5 → ??? Sorpresa - Millennium Falcon (OBJ)
```

---

## 🔧 Solución de Problemas

### Error: "assimp.lib not found"
```
→ Ejecutá: vcpkg integrate install
→ Reconstruye: Build → Clean Solution → Rebuild Solution
```

### El programa no compila
```
→ Build → Clean Solution
→ Build → Rebuild Solution
→ Si persiste, verifica que vcpkg esté en C:\vcpkg (o tu ruta)
```

### Modelo no aparece o muy pequeño
```
→ Verificá que los archivos .blend/.fbx/.obj estén en:
   Simulador_avion/Simulador_avion/source/
→ El programa ajusta automáticamente zoom y escala
```

### El modelo se ve desarmado
```
→ Presioná Q/E para zoom in/out
→ Probá cambiar de modelo (1-5)
→ Los FBX a veces necesitan Q presionado 2-3 veces
```

---

## 📁 Estructura del Proyecto

```
Simulador_avion/
├── Simulador_avion/
│   ├── main.cpp              ← Punto de entrada, manejo de eventos
│   ├── renderer.cpp/h        ← Renderizado OpenGL
│   ├── model_loader.cpp/h    ← Cargador Assimp (OBJ/FBX/BLEND)
│   ├── config.h              ← Rutas de modelos
│   ├── geometry.cpp/h        ← Geometría procedural (fallback)
│   ├── layer_manager.cpp/h   ← Gestor de capas
│   ├── source/               ← Archivos de modelos 3D
│   └── Simulador_avion.vcxproj
├── README.md                 ← Este archivo
├── ROADMAP_3D.md             ← Plan Fase A vs Fase B
└── INSTALACION_OPCION_A.md   ← Detalles técnicos
```

---

## 🎯 ¿Qué se implementó en esta sesión?

### Fase A: Cargador de Modelos 3D

✅ **Assimp Integration**
- Soporte para OBJ, FBX, BLEND, GLTF
- Carga automática de vértices y normales
- Procesamiento de jerarquías de nodos

✅ **Normalización Automática**
- Cálculo de bounding box
- Escalado dinámico (1.5x)
- Centrado de modelos

✅ **Zoom Adaptativo**
- Cálculo automático de zoom por modelo
- Rango: -1.5 a -10.0
- Se ajusta según tamaño real

✅ **Transformaciones de Nodos**
- Aplicación de matrices de transformación
- Soporte para modelos FBX complejos
- Normales transformadas correctamente

✅ **Interfaz Interactiva**
- Menú de selección al inicio
- Cambio de modelo en tiempo real (1-5)
- Controles 3D completos

---

## 🚀 Próximos Pasos: Fase B (Después)

Para agregar **texturas, colores y shaders modernos:**

→ Ver archivo: `ROADMAP_3D.md`

Cambios principales:
- Migrar de GLUT → GLFW
- Agregar shaders GLSL
- Soportar texturas desde Assimp
- Usar VAO/VBO para mejor rendimiento

---

## 📞 Soporte

Si algo no funciona:

1. **Verificá vcpkg:** `vcpkg list | grep assimp`
2. **Limpia todo:** `Build → Clean Solution`
3. **Reconstruye:** `Build → Rebuild Solution`
4. **Reinicia VS:** Cierra y vuelve a abrir Visual Studio

---

## 📄 Licencia

Proyecto académico - Universidad

**Versión:** 2.0 (Fase A - Modelos 3D)  
**Última actualización:** Julio 2025
