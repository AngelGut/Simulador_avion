# ARQ001 — Estructura de Capas a Vista Explosiva 3D

**Proyecto:** Visor 3D Interactivo de Aeronaves  
**Fecha:** Julio/Agosto 2026  

---

## 1. Evolución: Capas 2D → Modo Piezas 3D (Exploded View)

### Contexto Inicial
El proyecto original estaba planeado como un visor 2D que alternaba capas superpuestas (Exterior, Sistemas, Estructura).

### Decisión de Rediseño
Transformar el concepto de capas en un **sistema de vista de despiece tridimensional (Modo Piezas)**. En lugar de dibujos planos de líneas, las piezas del avión son modelos tridimensionales individuales en formato GLB que se cargan y controlan de forma independiente.

---

## 2. Organización de Piezas en Disco y Memoria

### Carga del Hangar Completo (Modo Armado)
Se carga el modelo del avión completo desde la ruta raíz del hangar (ej: `models/boeing-787-_dreamliner.glb`).

### Carga de Piezas Individuales (Modo Despiece)
Cuando se presiona la tecla `P` o `Backspace`, el sistema oculta el modelo principal y carga las piezas individuales correspondientes desde su directorio de despiece:
* **Estructura de Carpetas:**
  * `models/Desarmados/a10/` (10 archivos `.glb`)
  * `models/Desarmados/b24/` (7 archivos `.glb`)
  * `models/Desarmados/boing/` (5 archivos `.glb`)
  * `models/Desarmados/mig29/` (5 archivos `.glb`)
* En memoria, estas piezas se almacenan en un vector dinámico de modelos (`partModels`) administrado por la clase `Renderer`.

---

## 3. Navegación e Integración de Hotspots HUD

* El usuario utiliza las flechas **Izquierda (←)** y **Derecha (→)** para navegar secuencialmente entre las piezas cargadas en memoria.
* **Cámara Dinámica:** Al cambiar de pieza activa, el sistema traslada el punto focal y ajusta el zoom automáticamente según el radio de la pieza, aislando visualmente el componente seleccionado.
* **Base de Datos de Hotspots:** Se definieron puntos de interés tridimensionales en `main.cpp` asociados por palabras clave a los nombres de las piezas, desplegando los hologramas de información correspondientes únicamente cuando esa pieza está en pantalla.