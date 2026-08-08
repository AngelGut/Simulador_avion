# 🚀 Simulador de Aviones - Visor 3D Interactivo (Fase B)

**Estado:** ✅ Fase B Completada & Optimizada  
**Tecnología:** OpenGL 3.3 Core Profile | GLFW | GLAD | Assimp | GLM

Una aplicación interactiva 3D en C++ moderna que permite la visualización, exploración técnica y desensamblado interactivo de 4 modelos de aeronaves detallados en un hangar digital con iluminación y una interfaz HUD avanzada.

---

## ✈️ Aeronaves y Piezas Disponibles

El simulador cuenta con 4 aviones icónicos. Cada uno puede visualizarse en su estado ensamblado completo o desensamblarse en piezas individuales para analizar sus detalles internos:

1. **A-10 Thunderbolt II (10 piezas):** Cañón rotatorio GAU-8, Cabina de titanio, Motores Turbofan TF34, Alas principales, Tren de aterrizaje, Flaps de control, Misil pesado (AGM-65 Maverick), Misil mediano (GBU-12), Cohetes ligeros (FFAR) y Anclajes de carga.
2. **B-24 Liberator (7 piezas):** Cabina de mando, Bahía de bombas interna, Motores radiales Pratt & Whitney, Ala de alta eficiencia Davis, Tren retráctil (con corrección de orientación YZ), Estructura interna de aluminio y Flaps traseros.
3. **Boeing 787 Dreamliner (5 piezas):** Fuselaje composite de fibra de carbono, Turbofans GEnx de alta eficiencia, Tren de aterrizaje principal, Bodega de carga inferior y Cabina interior de pasajeros (`boing_inter`).
4. **MiG-29 Fulcrum (5 piezas):** Cabina de burbuja, Turbofans Klimov RD-33, Misil aire-aire de corto alcance R-73, Alas de flecha y LERX, Tren de aterrizaje rústico y Fuselaje de sustentación integrada.

---

## 🎮 Controles de Navegación e Interacción

| Acción | Control de Teclado / Ratón |
|--------|----------------------------|
| **Rotación de Cámara** | Arrastrar con **Click Izquierdo** del Ratón (o teclas `I` / `K` / `J` / `L`) |
| **Mover / Panear Vista** | Arrastrar con **Click Derecho** del Ratón (o teclas `W` / `A` / `S` / `D`) |
| **Zoom In / Out** | Rueda del Ratón o teclas `Q` / `E` |
| **Modo Piezas (Toggle)** | Tecla `P` o tecla `Retroceso (Backspace)` |
| **Navegar Piezas** | Teclas de flecha **Izquierda (←)** / **Derecha (→)** (solo en Modo Piezas) |
| **Restablecer Cámara** | Tecla **Espacio** (reajusta posición, rotación y aplica el zoom ideal del modelo) |
| **Guía de Ayuda (Toggle)**| Tecla `H` (muestra/oculta el overlay de controles en pantalla) |
| **Salir** | Tecla **ESC** |

---

## 💎 Características Avanzadas de Interfaz (HUD)

El HUD ha sido modernizado para ofrecer una presentación visual de nivel premium y máxima legibilidad:

### 1. Panel de Diagnóstico Lateral (Modo Avión Completo)
Al visualizar la aeronave armada completa, se ocultan los hotspots de detalles y se despliega una ficha técnica en el lateral izquierdo:
* **Ficha Técnica:** Título en escala grande (`2.4f`) y color amarillo de alto impacto, junto a especificaciones clave (velocidad, autonomía y capacidad).
* **Barras de Rendimiento:** Indicadores gráficos de barras segmentadas (estilo HUD militar en cian neón) que cuantifican blindaje, fuego, agilidad, eficiencia y confort de la aeronave.

### 2. Hotspots / Hologramas 3D (Modo Piezas)
Cuando se activa el desensamblado de piezas, el visor se enfoca en el componente activo y despliega esferas de luz animadas que representan puntos de interés (hotspots):
* **Activación por Proximidad:** Al posicionar el cursor sobre la esfera, esta despliega un menú flotante holográfico.
* **Auto-Envoltura (Word Wrap):** Un motor dinámico formatea el texto en líneas de hasta **36 caracteres**, soportando descripciones de hasta 4 líneas sin desbordar el cuadro de información (`420x200`).
* **Legibilidad Mejorada:** El texto utiliza sobre-dibujado cruzado de píxeles (efecto negrita/bold) y una sombra negra proyectada con opacidad (`0.75f`) para contrastar perfectamente sobre cualquier geometría tridimensional.

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
├── Simulador_avion/
│   ├── main.cpp              ← Loop principal, callbacks GLFW y estados de UI
│   ├── renderer.cpp/h        ← Lógica de renderizado y toggle de piezas
│   ├── model_loader.cpp/h    ← Cargador Assimp 3D con corrección de rotación
│   ├── model_renderer.cpp/h  ← Upload de buffers a la GPU (VAO/VBO)
│   ├── ui_renderer.cpp/h     ← Renderizado de texto, fondos y botones 2D
│   ├── shader.cpp/h          ← Compilador y gestor de programas shader GLSL
│   ├── app_state.h           ← Estructura del contexto y estados globales
│   ├── shaders/              
│   │   ├── vertex.glsl       ← Shader de vértices (iluminación Phong + matrices)
│   │   └── fragment.glsl     ← Shader de fragmentos (fusión de color y luz)
│   └── copy_dlls.bat         ← Script de automatización de librerías en build
└── Simulador_avion.slnx       ← Solución moderna de Visual Studio 2022
```

---

## 🛠️ Compilación y Ejecución en Visual Studio 2022

1. Abre `Simulador_avion.slnx` en Visual Studio 2022.
2. Asegúrate de configurar la solución en **Debug/Release** y plataforma **x64**.
3. Compila presionando **Ctrl + Shift + B**. Las dependencias (`glfw3.dll`, `assimp-vc145-mt.dll`, etc.) se copiarán automáticamente a la carpeta de salida gracias al post-build script.
4. Presiona **F5** para ejecutar.
