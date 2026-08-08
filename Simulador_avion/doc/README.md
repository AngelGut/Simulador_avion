# 🚀 Simulador de Aviones - Visor 3D Interactivo (Fase B + Optimizaciones)

**Estado:** ✅ Fase B Completada & Optimizada  
**Tecnología:** OpenGL 3.3 Core Profile | GLFW | GLAD | Assimp | GLM | Miniaudio  

Una aplicación interactiva 3D en C++ moderna que permite la visualización, exploración técnica y desensamblado interactivo de 4 modelos de aeronaves detallados en un 
hangar digital con iluminación, música de fondo interactiva y una interfaz HUD avanzada.

---

## ✈️ Aeronaves y Piezas Disponibles

El simulador cuenta con 4 aviones icónicos. Cada uno puede visualizarse en su estado ensamblado completo o desensamblarse en piezas individuales para analizar sus detalles internos:

1. **A-10 Thunderbolt II (10 piezas):** Cañón GAU-8, Cabina, Motores TF34, Alas principales, Tren de aterrizaje, Flaps, Misil Maverick, Misil GBU-12, Cohetes FFAR y Anclajes.
2. **B-24 Liberator (16 piezas):** Cabina, Bahía de bombas, Motores radiales, Ala Davis, Tren de aterrizaje, Flaps traseros, hélices y flaps delanteros.
3. **Boeing 787 Dreamliner (15 piezas):** Fuselaje composite de fibra de carbono, Turbofans GEnx, Tren de aterrizaje principal, Bodega de carga y Cabina de pasajeros.
4. **MiG-29 Fulcrum (20 piezas):** Cabina, Motores RD-33, Misil aire-aire R-73, Alas de flecha y LERX, Tren de aterrizaje y Fuselaje de sustentación.

---

## 🎮 Controles de Navegación e Interacción

| Acción | Control de Teclado / Ratón |
|--------|----------------------------|
| **Rotación de Cámara** | Arrastrar con **Click Izquierdo** del Ratón (o teclas `I` / `K` / `J` / `L`) |
| **Mover / Panear Vista** | Arrastrar con **Click Derecho** del Ratón (o teclas `W` / `A` / `S` / `D`) |
| **Zoom In / Out** | Rueda del Ratón o teclas `Q` / `E` |
| **Modo Piezas (Toggle)** | Tecla `P` o tecla `Retroceso (Backspace)` |
| **Navegar Piezas** | Teclas de flecha **Izquierda (←)** / **Derecha (→)** (solo en Modo Piezas) |
| **Restablecer Cámara** | Tecla **Espacio** (reajusta posición, rotación y aplica el zoom ideal) |
| **Guía de Ayuda (Toggle)**| Tecla `H` (muestra/oculta el overlay de controles en pantalla) |
| **Ocultar todo el HUD** | Tecla `F1` (modo inmersivo de visualización limpia) |
| **Silenciar Música** | Tecla `M` (habilita/deshabilita la música de fondo en el hangar) |
| **Salir** | Tecla **ESC** o Botón **Salir** (en la pantalla de bienvenida) |

---

## ⚡ Optimizaciones y Características Avanzadas

### 1. Precarga Diferida por Fotograma (Evita Congelamiento y Crasheos)
* **Antes:** La carga síncrona en disco congelaba el programa al cambiar de avión o activar el modo piezas, además de saturar la cola de comandos del driver de video.
* **Ahora:** Un sistema de cola secuencial carga un único archivo por frame durante la pantalla de inicio, llamando a `glFlush()` e interactuando con `glfwSwapBuffers`. Esto mantiene la cola de la GPU limpia, previene crasheos en controladores AMD/Intel y permite transiciones 100% instantáneas en el visor.

### 2. Audio Integrado con AudioManager
* Soporte nativo para archivos de audio `.mp3` para cada avión a través de la API `miniaudio`.
* Botón de encendido/silenciador del motor integrado en el panel del visor.
* Música ambiental para el hangar con atajos de teclado para silenciar (`M`).

### 3. Pantalla Completa Nativa
* El programa obtiene automáticamente la resolución nativa de tu monitor mediante GLFW e inicia directamente en pantalla completa para una inmersión total.

### 4. Botón Salir y Liberación Completa de Memoria
* Se añadió un botón de salida directa en el menú de bienvenida que ejecuta las funciones `cleanupPreloadedModels()` y `shutdown()` para vaciar la VRAM/RAM y apagar el motor de audio limpiamente antes de cerrar el proceso.

---

## 🔧 Resoluciones Técnicas & Parches

### 1. Parche de Compatibilidad de Modelos Assimp (GLTF/GLB)
Assimp presenta una limitación conocida al leer accessors sin `bufferView` (como los que exporta Blender para morph targets o animaciones vacías), lo que causaba el fallo fatal: `GLTF2: data is null`.
* **Solución Aplicada:** Se incluye un script de procesamiento en Python (`fix_glb.py`) que optimiza los modelos GLB. El script elimina las animaciones redundantes y los morph targets, y reasigna los accessors vacíos a una referencia válida de geometría. Esto resolvió los fallos de carga para el Boeing y su cabina interna (`boing_inter.glb`).

### 2. Corrección del Tren de Aterrizaje del B-24 (`b24_tren`)
Debido a una orientación de exportación incorrecta en el modelo original, el tren del B-24 aparecía acostado horizontalmente.
* **Solución Aplicada:** El cargador de modelos (`model_loader.cpp`) detecta el archivo `"b24_tren"` y aplica una transformación matemática de rotación de 90° en el eje X directamente en memoria, asegurando que se dibuje parado en el suelo del hangar.

---

## 📁 Estructura del Proyecto

```
Simulador_avion/
├── models/                   ← Modelos 3D optimizados en GLB (A10, B24, Boeing, MiG29)
├── audio/                    ← Efectos de sonido de motor y música del hangar
├── Simulador_avion/
│   ├── main.cpp              ← Loop principal, callbacks GLFW y estados de UI
│   ├── renderer.cpp/h        ← Lógica de renderizado, caché de precarga y toggle de piezas
│   ├── screens.cpp/h         ← Pantallas de carga con máquina de estados, menú y HUD
│   ├── model_loader.cpp/h    ← Cargador Assimp 3D con corrección de rotación
│   ├── model_renderer.cpp/h  ← Upload de buffers a la GPU (VAO/VBO)
│   ├── audio_manager.cpp/h   ← Gestión de hilos de audio para música y motores
│   ├── ui_renderer.cpp/h     ← Renderizado de texto, fondos y botones 2D
│   ├── shader.cpp/h          ← Compilador y gestor de programas shader GLSL
│   ├── app_state.h           ← Estructura del contexto y estados globales
│   └── copy_dlls.bat         ← Script de automatización de librerías en build
└── Simulador_avion.slnx       ← Solución moderna de Visual Studio 2022
```

---
