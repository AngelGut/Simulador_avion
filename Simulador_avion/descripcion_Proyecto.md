# Descripción del Proyecto — Visor 3D Interactivo de Aeronaves

**Proyecto:** Simulador y Explorador Técnico de Aeronaves 3D  
**Asignatura:** Computación Gráfica   
**Tecnología:** C++ | OpenGL 3.3 Core Profile | GLFW | GLAD | Assimp | GLM  
**Plataforma:** Windows (Visual Studio 2022)  

---

## 1. ¿Qué es?

El **Visor 3D de Aeronaves** es una aplicación gráfica tridimensional interactiva que permite explorar la estructura, componentes y características técnicas de 4 aeronaves icónicas dentro de un hangar virtual iluminado en tiempo real. 

La aplicación permite desensamblar los aviones en sus piezas constitutivas principales, revelando detalles mecánicos y estructurales internos acompañados de una interfaz holográfica HUD.

---

## 2. ¿Cómo funciona?

### Interfaz y Modos de Visualización
El visor opera bajo dos modos principales que se alternan con la tecla `P` o `Retroceso`:

1. **Modo Avión Completo (Hangar General):**
   * Muestra la aeronave completamente armada sobre la pista del hangar.
   * Despliega un panel lateral de diagnóstico técnico con la ficha técnica y barras gráficas de desempeño (blindaje, fuego, agilidad, eficiencia y confort).
2. **Modo Piezas (Desensamblado / Vista Explosiva):**
   * El avión se divide en sus piezas individuales flotantes.
   * Se enfoca la cámara en la pieza activa y se ocultan las demás piezas para mayor claridad.
   * Se puede navegar de forma secuencial entre componentes usando las teclas de flecha `Izquierda (←)` y `Derecha (→)`.
   * Despliega esferas de luz animadas (Hotspots 3D). Al posicionar el cursor sobre ellas, se abre una ventana holográfica con la explicación detallada de la pieza.

### Controles Principales
* **Navegación:** Click izquierdo y arrastrar para orbitar; Click derecho y arrastrar para trasladar la vista. Rueda del ratón para Zoom.
* **Cámara de Emergencia:** Tecla `Espacio` para reiniciar la cámara a la posición por defecto adaptada a la escala del modelo.
* **Información:** Tecla `H` para activar/desactivar la superposición de ayuda en pantalla.

---

## 3. ¿Para qué sirve?

### Objetivos Pedagógicos y Prácticos
1. **Comprensión de Ingeniería:** Estudiar de forma interactiva la disposición y el propósito de cada pieza aeronáutica (motores, flaps, cabina de titanio, bodegas, armamento pesado).
2. **Interactividad Avanzada:** Aplicar transformaciones tridimensionales en tiempo real y coordinar la proyección de cámaras con interacción de puntero sobre geometría 3D.
3. **Presentación de Producto:** Diseñado con un acabado estético de nivel premium para exhibir modelos de ingeniería a compradores o espectadores teóricos.

---

## 4. Características Técnicas del Motor Gráfico

### Renderizado y Gráficos
* **Buffer de Datos:** Carga geometría compleja mediante Assimp y genera buffers dinámicos de vértices (VBO, EBO) mapeados en objetos de matriz de vértices (VAO) en la GPU.
* **Shaders GLSL (Modern OpenGL):**
  * *Vertex Shader:* Procesa transformaciones matriciales de cámara (MVP) y normalización de normales de iluminación.
  * *Fragment Shader:* Modelo de iluminación de Phong (Ambiente + Difuso + Especular) con soporte de colores de materiales por vértice.
* **Motor de Texto 2D HUD:** Renderiza cadenas de caracteres escalables directamente sobre la proyección ortogonal, aplicando sobre-dibujado cruzado de píxeles (negrita) y sombras proyectadas (`drop-shadow`) para asegurar legibilidad.

### Estructura de Componentes de Software
* **`main.cpp`:** Inicialización de ventana (GLFW/GLAD), loop principal y despacho de eventos de teclado/ratón.
* **`renderer.cpp`:** Renderizado de la escena 3D, hangar, modelo y control del modo de piezas.
* **`model_loader.cpp`:** Carga y normalización de modelos `.glb` vía Assimp, aplicando correcciones de rotación (como el tren del B-24).
* **`ui_renderer.cpp`:** Sistema gráfico 2D para dibujo de menús, botones, textos y HUD.
* **`shader.cpp`:** Carga, compilación y vinculación de archivos shader `.glsl`.