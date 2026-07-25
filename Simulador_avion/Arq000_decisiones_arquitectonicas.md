# ARQ000 — Decisiones Arquitectónicas Generales

**Proyecto:** Boeing 737 Visualizer  
**Fecha:** Julio 2026

---

## 1. Lenguaje: C++ con GLUT

### Decisión
Usar **C++ con GLUT (OpenGL Utility Toolkit)** como stack principal de desarrollo.

### Contexto
- Proyecto obligatorio de **Computación Gráfica** (8vo semestre)
- Laboratorio de la universidad con GLUT preinstalado
- Equipo con experiencia en C++

### Decisiones de Implementación
```cpp
// Estructura GLUT mínima
#include <GL/glut.h>
//Incluye librería GLUT (ya instalada en tu PC)

int main(int argc, char** argv) {
   glutInit(&argc, argv);
//Inicializa GLUT con los argumentos
//Necesario ANTES de cualquier otra llamada GLUT
//&argc = puntero a argc (GLUT puede modificarlo)
   glutInitDisplayMode(GLUT_DOUBLE | GLUT_RGB | GLUT_DEPTH);
// Configura modo de display
// GLUT_DOUBLE   = Double buffering (sin parpadeo)
// GLUT_RGB      = Colores RGB (no indexed)
// GLUT_DEPTH    = Buffer de profundidad (Z-buffer)
    glutInitWindowSize(800, 600);
    glutCreateWindow("Boeing 737");
// Crear ventana con título "Boeing 737"
// Sin esto, no hay window
    
    glutDisplayFunc(display);
    glutReshapeFunc(reshape);
// Registrar función reshape() para cambios de tamaño ventana
// Si usuario redimensiona → reshape() es llamado
    glutKeyboardFunc(keyboard);
    glutTimerFunc(16, timer, 0);
// Registrar función timer() cada 16 milisegundos
// 16ms ≈ 60 FPS
// Parámetros: (milisegundos, función, valor_a_pasar)
    
    glutMainLoop();
// LOOP INFINITO de GLUT
// Espera eventos (keyboard, mouse, etc.)
// Llama callbacks cuando eventos ocurren
// Solo sale si exit() es llamado
    return 0;
// (nunca llega aquí)
}
```

---

## 2. Avión: Boeing 737 (vs Otras Aeronaves)

### Decisión
Modelar un **Boeing 737** comercial como sujeto principal.

### Alternativas Consideradas

| Avión | Pros | Contras | Veredicto |
|-------|------|---------|----------|
| **Airbus A380** | Más grande, impresionante | Muy complejo, overhead de datos | ❌ Rechazado |
| **Cessna 172** | Muy simple | Poco educativo, menos interés | ❌ Rechazado |
| **Concorde** | Histórico, único | Geometría extraña, difícil | ❌ Rechazado |
| **Boeing 737** ✅ | Simple, documentado, comercial | — | ✅ Seleccionado |
| **F-16** | Militar, interesante | Complejo, sensible políticamente | ❌ Rechazado |

### Justificación
1. **Geometría simple** → fuselaje cilíndrico + alas rectangulares
2. **Bien documentado** → especificaciones públicas disponibles
3. **Comercial** → relevancia mundial, mercado LATAM importante
4. **Proporciones claras** → fácil de visualizar en capas
5. **Educativo** → caso de estudio real en ingeniería

### Especificaciones Adoptadas
```
Boeing 737-800 (referencia)
├── Largo: 39.5 m
├── Envergadura: 35.9 m
├── Altura: 12.5 m
├── Capacidad: 189 pasajeros
└── Motores: 2x CFM56-7B

Para el proyecto (escala 2D):
├── Fuselaje: cilindro de ~100 píxeles
├── Alas: rectángulos de ~150x50 píxeles
├── Motores: cilindros de ~40 píxeles
└── Cockpit: rectángulo frontal
```

### Trade-offs
| Aspecto | Ganancia | Pérdida |
|--------|----------|---------|
| **Complejidad** | Educativo | No super realista |
| **Relevancia** | Alto interés mercado |
| **Documentación** | Especificaciones públicas | Demasiada información (filtrar) |

---

## 3. Visualización: 2D Top-Down (vs 3D Isométrico)

### Decisión
Usar **vista superior 2D ortogonal** en lugar de 3D isométrico o perspectiva.

### Contexto
- 4 semanas de desarrollo
- Objetivo: visualizar capas internas

### Alternativas Consideradas

| Perspectiva | Pros | Contras | Veredicto |
|-------------|------|---------|----------|
| **3D Perspectiva** | Más realista, atractivo | Transformaciones complejas, larga curva | ❌ Rechazado |
| **3D Isométrico** | Buena visualización de capas | Aún requiere transformaciones 3D | ❌ Rechazado |
| **2D Lateral** | Fácil de entender | Difícil ver distribución de alas | ❌ Rechazado |
| **2D Superior** ✅ | Claro, simple, educativo | Menos "wow", requiere imaginación | ✅ Seleccionado |

### Justificación
1. **Proyección Ortho** → `gluOrtho2D()` es trivial
2. **Capas claras** → vista superior muestra bien distribución interna
3. **Rápido de implementar** → sin transformaciones 3D complejas
4. **Bajo overhead** → rasterización 2D simple
5. **Pedagogía** → estudiantes entienden mejor estructura

### Implementación
```cpp
void reshape(int w, int h) {
    glViewport(0, 0, w, h);
    glMatrixMode(GL_PROJECTION);
    glLoadIdentity();
    gluOrtho2D(-200, 200, -150, 150);  // Left, Right, Bottom, Top
    glMatrixMode(GL_MODELVIEW);
}
```

### Trade-offs
| Aspecto | Ganancia | Pérdida |
|--------|----------|---------|
| **Realismo** | Implementación simple | No es 3D "real" |
| **Tiempo dev** | Rápido | Menos impresionante visualmente |
| **Escalabilidad** | Fácil mantener 2D | Difícil migrar a 3D después |

---

## 4. Capas: 5 Niveles Independientes

### Decisión
Dividir la visualización en **5 capas independientes**, activadas por teclado (teclas 1-5).

### Capas Definidas

```
Capa 1: EXTERIOR
├── Fuselaje (outline cilíndrico con Bresenham)
├── Alas (color rojo)
├── Cola (estabilizador vertical)
└── Motores (visibles)

Capa 2: ESTRUCTURA
├── Costillas (ribs) horizontales
├── Vigas longitudinales
├── Marcos (frames) de fuselaje
└── Color: Verde

Capa 3: SISTEMAS
├── Tuberías hidráulicas (azul)
├── Cables eléctricos (amarillo)
├── Sistema de aire acondicionado (cian)
└── Color: Multi-color

Capa 4: CABINA
├── Asientos de pasajeros (filas)
├── Pasillos centrales
├── Cockpit (frente)
└── Color: Marrón/gris

Capa 5: MOTORES
├── Motores a reacción detallados
├── Tren de aterrizaje principal
├── Tren de nariz
└── Color: Gris oscuro
```

### Alternativas Consideradas

| Estructura | Pros | Contras | Veredicto |
|-----------|------|---------|----------|
| **Capas opacas superpuestas** | Bonito | Difícil ver capas intermedias | ❌ Rechazado |
| **Capas exclusivas (1 a la vez)** ✅ | Claro, enfocado | Requiere cambio manual | ✅ Seleccionado |
| **Capas con transparencia** | Educativo | OpenGL 1.1 sin alpha blending fácil | ❌ Rechazado |
| **Panel 3D con rotación** | Inmersivo | Overhead 3D, fuera de scope | ❌ Rechazado |

### Justificación
1. **Una a la vez** → usuario entiende cada componente
2. **Interactividad** → controla qué ver
3. **Educativo** → se enfoca en estructura específica
4. **Fácil de implementar** → solo cambiar flags de visibilidad
5. **Presentación** → cada capa es un "slide" independiente

### Implementación
```cpp
class LayerManager {
    bool layerVisible[5];
    int currentLayer;
public:
    void setActiveLayer(int layer) {
        for(int i = 0; i < 5; i++) 
            layerVisible[i] = (i == layer);
        currentLayer = layer;
    }
};
```

### Trade-offs
| Aspecto | Ganancia | Pérdida |
|--------|----------|---------|
| **Claridad** | Cada capa enfocada | No ver superposición simultánea |
| **Control** | Usuario decide qué ver | Menos automático |
| **Código** | Implementación simple | Gestión de visibilidad adicional |

---

## 5. Entrada: Teclado (Teclas 1-5)

### Decisión
Usar **teclado con teclas 1-5** para cambiar capas (sin mouse).

### Alternativas Consideradas

| Input | Pros | Contras | Veredicto |
|-------|------|---------|----------|
| **Mouse (click)** | Intuitivo | Requiere UI buttons, overhead | ❌ Rechazado |
| **Teclado (1-5)** ✅ | Simple, directo, rápido | Requiere documentación | ✅ Seleccionado |
| **Joystick** | Cool | Overengineering | ❌ Rechazado |
| **Touchscreen** | Moderno | No disponible en labs | ❌ Rechazado |

### Justificación
1. **GLUT keyboard callback** → fácil de implementar
2. **Rápido** → tecla = cambio instantáneo
3. **Sin UI overhead** → no necesita botones gráficos
4. **Documentable** → teclas en pantalla inicial

### Implementación
```cpp
void keyboard(unsigned char key, int x, int y) {
    if (key >= '1' && key <= '5') {
        int layer = key - '0';
        layerManager.setActiveLayer(layer);
        printf("Capa %d activada\n", layer);
    }
    if (key == 27) exit(0);  // ESC
    glutPostRedisplay();
}

glutKeyboardFunc(keyboard);
```

### Trade-offs
| Aspecto | Ganancia | Pérdida |
|--------|----------|---------|
| **Usabilidad** | Muy rápido | Menos intuitivo que mouse |
| **Implementación** | Trivial | Sin feedback visual directo |
| **Accesibilidad** | Teclado estándar | No funciona en touchscreen |

---

## 6. Algoritmos Gráficos: Bresenham para Fuselaje

### Decisión
Usar **algoritmo de Bresenham** para dibujar líneas curvas del fuselaje.

### Justificación
1. **Requerimiento del curso** → enseñanza de rasterización
2. **Precisión** → discretización correcta en grilla
3. **Eficiencia** → solo operaciones enteras (sin floats)
4. **Histórico** → algoritmo clásico, educativo

### Implementación
```cpp
// Pseudocódigo Bresenham para círculo
void bresenhamCircle(float cx, float cy, float r, float color[3]) {
    int x = 0;
    int y = r;
    int d = 3 - 2*r;
    
    while (x <= y) {
        // Dibujar 8 puntos simétricos
        drawPoint(cx + x, cy + y, color);
        drawPoint(cx - x, cy + y, color);
        drawPoint(cx + x, cy - y, color);
        drawPoint(cx - x, cy - y, color);
        drawPoint(cx + y, cy + x, color);
        drawPoint(cx - y, cy + x, color);
        drawPoint(cx + y, cy - x, color);
        drawPoint(cx - y, cy - x, color);
        
        if (d < 0) d = d + 4*x + 6;
        else { d = d + 4*(x-y) + 10; y--; }
        x++;
    }
}
```

### Trade-offs
| Aspecto | Ganancia | Pérdida |
|--------|----------|---------|
| **Corrección** | Algoritmo probado | Código puede ser lento sin optimizar |
| **Educación** | Enseña conceptos | Más código que usar `glutWireSphere()` |
| **Requerimientos** | Cumple requisitos curso | Overhead en desarrollo |

---

## 7. Resumen de Decisiones

| Decisión | Opción Seleccionada | Razón Clave |
|----------|-------------------|------------|
| Lenguaje | C++ + GLUT | Instalado, simple, OpenGL puro |
| Avión | Boeing 737 | Comercial, documentado, educativo |
| Perspectiva | 2D Top-Down | Rápido, claro, educativo |
| Entrada | Teclado 1-5 | Simple, directo, sin UI overhead |
| Capas | 5 exclusivas | Clara visualización de componentes |
| Algoritmos | Bresenham | Requerimiento curso + rasterización |

---

## 8. Cambios Futuros (No en Scope Actual)

Si se escala el proyecto:

- [ ] Migrar a OpenGL 3.0+ con shaders
- [ ] Agregar vista 3D isométrica
- [ ] Implementar zoom/pan con mouse
- [ ] Exportar geometría a OBJ/STL
- [ ] Agregar animación de componentes
- [ ] Multiplataforma (GLFW en lugar de GLUT)

---

**Versión:** 1.0  
**Última actualización:** Julio 2026  
**Aprobado por:** Usuario (documentador)  
**Estado:** Final