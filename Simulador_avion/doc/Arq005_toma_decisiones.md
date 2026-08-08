# ARQ005 — Toma de Decisiones Técnicas y Librerías

**Proyecto:** Visor 3D Interactivo de Aeronaves  
**Fecha:** Agosto 2026  

---

## 1. Librerías Seleccionadas y Justificación (Tech Stack)

Para construir el motor gráfico y la interfaz interactiva sobre OpenGL 3.3 Core Profile, se seleccionaron las siguientes librerías de soporte:

### 1.1 GLFW (Graphics Library Framework)
* **Función:** Creación de ventana, inicialización del contexto OpenGL y capturador de eventos de periféricos (teclado/ratón).
* **Por qué se eligió (vs GLUT/FreeGLUT):** 
  * GLUT es una librería obsoleta (inactiva desde hace más de 20 años) que bloquea el hilo de ejecución principal en su propio loop. 
  * GLFW ofrece un control total y explícito sobre el bucle de renderizado (`while`), soporte multimonitor nativo, y callbacks modernos de alta precisión para capturar la rueda del ratón (zoom) y arrastres analógicos del cursor.

### 1.2 GLAD (OpenGL Loader Generator)
* **Función:** Carga los punteros de las funciones modernas de OpenGL 3.3+ en tiempo de ejecución.
* **Por qué se eligió (vs GLEW):**
  * GLEW requiere vincular librerías estáticas adicionales (`glew32.lib`) y arrastrar archivos binarios externos (`glew32.dll`), lo que suele generar conflictos de versiones en compiladores de Visual Studio.
  * GLAD genera código fuente en C totalmente limpio (`glad.c` / `glad.h`) que se compila directamente junto al proyecto. Esto reduce las dependencias externas a cero y garantiza portabilidad total entre equipos de desarrollo.

### 1.3 Assimp (Open Asset Import Library)
* **Función:** Cargador e importador universal de modelos tridimensionales complejos.
* **Por qué se eligió:**
  * Escribir un parser propio para formatos complejos como FBX o GLTF/GLB tomaría cientos de horas. 
  * Assimp unifica la lectura de múltiples formatos bajo una única estructura jerárquica de nodos y mallas, permitiendo además extraer información detallada de los materiales (como colores difusos) y normales de geometría.

### 1.4 GLM (OpenGL Mathematics)
* **Función:** Librería matemática de vectores y matrices optimizada para gráficos 3D.
* **Por qué se eligió:**
  * Al migrar a OpenGL moderno, el programador debe encargarse del cálculo de transformaciones (Model, View, Projection).
  * GLM está diseñado con la misma sintaxis que el lenguaje de shaders de OpenGL (GLSL), facilitando la multiplicación de matrices, operaciones con cuaterniones para cámaras orbitales, y transformaciones de normales.

### 1.5 stb_image (por Sean Barrett)
* **Función:** Carga de imágenes en memoria para texturizado de superficies (PNG, JPG, BMP).
* **Por qué se eligió:**
  * Es una librería de cabecera única, ultra-rápida y de dominio público. Evita tener que incluir SDKs pesados e inestables como `libpng` o `libjpeg` en el entorno de compilación de Visual Studio.

---

## 2. Decisiones Clave de Arquitectura y Resolución de Problemas

### 2.1 Estandarización a Formato GLB (glTF Binario)
* **Decisión:** Usar exclusivamente archivos `.glb` para almacenar aviones y piezas, en lugar de OBJ o FBX.
* **Justificación:** 
  * El formato OBJ no soporta jerarquías de mallas ni empaqueta texturas internamente.
  * El formato FBX es propietario y su parser suele dar problemas de escala e importación de materiales entre Blender y Assimp. 
  * GLB es un estándar abierto y eficiente que optimiza el tamaño en disco y preserva intactas las estructuras de mallas complejas y colores del material definidos en Blender.

### 2.2 Parche de Compatibilidad para Morph Targets en Assimp
* **Decisión:** Resolver el error fatal de carga en Assimp (`GLTF2: data is null`) a través de un script en Python (`fix_glb.py`) que pre-procesa el modelo, en lugar de modificar la librería de Assimp o reconstruir los modelos 3D.
* **Justificación:** Assimp falla al intentar leer *accessors* de animación o morph targets que carecen de `bufferView` (una característica común al exportar desde Blender). El script en Python elimina de manera limpia las animaciones vacías redundantes y sustituye los accessors vacíos por referencias válidas antes de que el motor de C++ intente leerlos.

### 2.3 Rotación del Tren de Aterrizaje del B-24 en Memoria
* **Decisión:** Corregir la orientación acostada de las ruedas del B-24 aplicando una rotación matemática de 90° sobre el eje X durante el proceso de importación del archivo `b24_tren.glb`, en lugar de re-importar y re-alinear el modelo en Blender.
* **Justificación:** Evita alterar los archivos fuentes de Blender del equipo de arte y soluciona de forma inmediata el desalineamiento mediante una transformación matricial simple en memoria de la CPU durante la carga.

### 2.4 Legibilidad del HUD y Envoltura de Texto (Word Wrap)
* **Decisión:** Diseñar un algoritmo manual de word-wrap limitado a 36 caracteres y aplicar un doble renderizado de texto con desfases (negrita simulada + sombra negra).
* **Justificación:** Las fuentes bitmap simples en OpenGL pueden perderse fácilmente visualmente sobre fondos de modelos poligonales oscuros o texturizados. La sombra de fondo garantiza legibilidad total (contraste) y el word-wrap evita que las descripciones detalladas de los hotspots se corten en los bordes del HUD flotante.
