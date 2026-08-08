# ARQ004 — Preparación y Optimización de Modelos 3D

**Proyecto:** Visor 3D Interactivo de Aeronaves  
**Fecha:** Julio/Agosto 2026  

---

## 1. Flujo de Exportación de Modelos desde Blender (glTF / GLB)

### Formato Seleccionado
Se seleccionó **GLB (glTF Binario)** como el formato exclusivo para todas las aeronaves. Este formato empaqueta la malla poligonal, materiales y coordenadas de texturas en un único archivo consolidado de alto rendimiento.

### Guía de Configuración en Blender para Exportar:
Para evitar incompatibilidades de lectura con el motor gráfico, utiliza los siguientes ajustes en la ventana de exportación de Blender (`File -> Export -> glTF 2.0`):
1. **Include:**
   * `Limit to: Selected Objects` (opcional, para evitar exportar cámaras o luces de Blender).
2. **Transform:**
   * `+Y Up` checked (estándar de glTF).
3. **Geometry:**
   * `Apply Modifiers` checked (importante para aplicar subdivisiones y espejos en la geometría).
   * `Materials` set to `Export` (para incluir colores difusos de materiales).
4. **Animation (Crítico):**
   * **Desmarcar completamente la casilla "Animation"** y todas sus sub-casillas (incluyendo Shape Keys y Skins).
   * *Razón:* Si se exportan animaciones vacías o morph targets, Blender genera *accessors* dispersos (sparse) sin `bufferView` que provocan errores de puntero nulo (`GLTF2: data is null`) en el parser de Assimp.

---

## 2. Parcheador Automático de Modelos en Python (`fix_glb.py`)

Para solucionar problemas de incompatibilidad de modelos sin tener que volver a modelar o re-exportar constantemente, se creó un script automatizado en Python (`fix_glb.py`):

```python
# Características clave del parcheador:
# 1. Remueve la sección 'animations' y 'skins' del bloque JSON del glTF.
# 2. Remueve las referencias a morph targets ('targets') de cada malla.
# 3. Detecta cualquier accessor inválido que carezca de la propiedad 'bufferView'
#    y lo reemplaza con una copia de un accessor de geometría válido.
# 4. Re-serializa el archivo GLB manteniendo alineación de 4 bytes en los bloques.
```

Este script se ejecuta sobre las nuevas exportaciones de Blender para asegurar que Assimp las cargue de manera inmediata y sin caídas por violación de acceso.

---

## 3. Correcciones de Orientación en Tiempo de Carga (C++)

Debido a que algunos assets tridimensionales compartidos (como el tren de aterrizaje del B-24, `b24_tren.glb`) fueron exportados con ejes cambiados, el motor incluye una corrección geométrica en memoria dentro de `model_loader.cpp`:

```cpp
// Si el archivo en carga es el tren de aterrizaje del B-24
if (pathStr.find("b24_tren") != std::string::npos) {
    // Aplicar una matriz de rotación de 90 grados sobre el eje X
    // a todos los vértices y normales leídos por Assimp antes de
    // generar los buffers de la GPU.
}
```

Esto evita tener que alterar la jerarquía de nodos en Blender y garantiza que la pieza se dibuje parada y orientada correctamente de forma transparente para el resto del motor.
