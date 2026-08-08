# ARQ000 — Decisiones Arquitectónicas Generales

**Proyecto:** Visor 3D Interactivo de Aeronaves  
**Fecha:** Julio/Agosto 2026  

---

## 1. Migración de Stack: OpenGL 1.1 + GLUT → OpenGL 3.3 Core + GLFW + GLAD

### Decisión
Migrar toda la base gráfica del simulador de funciones en desuso de OpenGL 1.1 (pipeline fijo, `glBegin`/`glEnd`, transformaciones inmediatas) hacia **OpenGL 3.3 Core Profile** utilizando **GLFW** para la gestión de ventanas y contextos, y **GLAD** como cargador de extensiones modernas.

### Justificación
* **Uso de Shaders GLSL:** Permite programar directamente en la GPU la física de la luz (iluminación Phong) en lugar de depender del hardware de función fija.
* **VAO y VBO (Rendimiento):** Los datos tridimensionales de los modelos se cargan una única vez en la memoria de la tarjeta de video, reduciendo los tiempos de transferencia de datos CPU-GPU en cada cuadro.
* **Multiplataforma y Control Moderno:** GLFW proporciona un control de eventos de ratón y teclado mucho más robusto que la interfaz anticuada de callbacks de GLUT.

---

## 2. Formato de Modelos: Integración con Assimp (GLB / glTF)

### Decisión
Utilizar la librería **Assimp (Open Asset Import Library)** para leer y deserializar archivos en formato **GLB** (glTF binario).

### Justificación
* **Soporte de Estructuras Complejas:** glTF es el estándar moderno de la industria web y móvil para assets 3D eficientes.
* **Materiales Embebidos:** Permite extraer de forma directa los nombres de las piezas, colores de material e iluminación definidos en el software de modelado (Blender).
* **Parche de Compatibilidad para Morph Targets:** Assimp tiene una limitación conocida que causa fallas con accessors vacíos de morph targets. Decidimos procesar los GLB mediante un script de parches en Python para remover animaciones redundantes y morphs vacíos, garantizando compatibilidad 100% nativa con Assimp.

---

## 3. Iluminación Basada en Fragment Shaders (Phong Model)

### Decisión
Desarrollar shaders de vértices y fragmentos personalizados en GLSL para procesar la iluminación de la escena en espacio de vista mundial.

### Justificación
* El modelo de sombreado Phong (Ambiente + Difuso + Especular) produce reflexiones dinámicas realistas sobre la superficie de los aviones.
* Calcular la iluminación por píxel (Fragment Shader) en lugar de por vértice (Gouraud shading) elimina artefactos visuales y genera gradientes de luz suaves y naturales.