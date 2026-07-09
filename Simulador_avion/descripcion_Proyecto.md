# Descripción del Proyecto — Boeing 737 Visualizer

**Proyecto:** Visualizador 2D por capas de Boeing 737  
**Asignatura:** Computación Gráfica   
**Tecnología:** C++ + GLUT + OpenGL  
**Plataforma:** Windows (Visual Studio Community)  

---

## 1. ¿Qué es?

**Boeing 737 Visualizer** es una aplicación gráfica 2D interactiva que permite visualizar la estructura interna de un avión Boeing 737 desde una **vista superior (top-down)**. 

La aplicación divide el avión en **5 capas independientes**, cada una mostrando un aspecto diferente de la arquitectura interna del avión.

---

## 2. ¿Cómo funciona?

### Interfaz
- **Ventana gráfica:** 800x600 píxeles
- **Control:** Teclado (teclas 1-5 para cambiar capas)
- **Visualización:** Proyección ortogonal 2D

### Las 5 Capas

| Tecla | Capa | Contenido |
|-------|------|----------|
| **1** | Exterior | Fuselaje cilíndrico + alas + cola + motores visibles |
| **2** | Estructura Interna | Costillas, vigas, esqueleto del avión |
| **3** | Sistemas | Tuberías hidráulicas, cables eléctricos, sistemas de aire |
| **4** | Cabina | Asientos de pasajeros, pasillos, cockpit |
| **5** | Motores | Motores a reacción, tren de aterrizaje, detalles |

### Flujo de uso
```
Ejecutar aplicación
    ↓
Se muestra Capa 1 (Exterior)
    ↓
Usuario presiona tecla (1-5)
    ↓
Cambia a esa capa (visualización instantánea)
    ↓
Consola muestra mensaje: "Capa X activada"
    ↓
Tecla ESC = salir
```

---

## 3. ¿Para qué sirve?

### Objetivos Pedagógicos
1. **Enseñanza:** Estudiantes comprenden estructura compleja de avión
2. **Visualización:** Cómo se interconectan componentes internos
3. **Interactividad:** Usuario explora a su ritmo

### Casos de Uso
- 📚 **Educación:** Clases de ingeniería aeronáutica
- 🏢 **Presentaciones:** Ventas/marketing de aeronaves
- 🔧 **Mantenimiento:** Técnicos visualizan dónde están sistemas

---

## 4. Características Técnicas

### Gráficos
- ✅ Rasterización (GL_LINES, GL_TRIANGLES)
- ✅ Algoritmo de Bresenham (líneas curvas del fuselaje)
- ✅ Colores RGB (diferenciación de capas)
- ✅ Luces básicas (ambiente + direccional)
- ✅ Materiales (sin texturas)
- ✅ Transformaciones matriciales 2D (rotación, traslación)
- ✅ Proyección ortho (gluOrtho2D)

### Entrada
- ✅ Callbacks de GLUT (keyboard, reshape, display)
- ✅ Ciclo de vida GLUT (main loop ~60 FPS)
- ✅ Mensajes en consola (std::cout)

---

## 5. Arquitectura de Componentes

```
┌─────────────────────────────────┐
│      main.cpp (GLUT Loop)       │
│  Luis: Inicialización + Entrada │
└────────────┬────────────────────┘
             │
    ┌────────┴────────┐
    │                 │
┌───v────────────┐ ┌─v──────────────────┐
│  LayerManager  │ │ Renderer           │
│  - setLayer()  │ │  - drawLayer()     │
│  - getLayer()  │ │  - applyColor()    │
│                │ │  - applyLighting() │
└────────────────┘ └─v──────────────────┘
                     │
                ┌────v────────────┐
                │  GeometryBuilder│
                │                 │
                │ - Fuselaje      │
                │ - Alas          │
                │ - Motores       │
                │ - Cabina        │
                │ - Sistemas      │
                └─────────────────┘
```

---

## 6. Timeline

| Semana | Hito | Responsables |
|--------|------|--------------|
| **1** | Setup + Arquitectura | Todos |
| **2** | Geometría | Ronald + Usuario (docs) |
| **3** | Rendering + Integración | Ronald + Luis |
| **4** | Testing + Presentación | Todos |

---

**Versión:** 1.0  
**Última actualización:** Julio 2026  
**Estado:** En desarrollo