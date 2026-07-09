# ARQ001 — Estructura de Capas y Geometría

**Proyecto:** Boeing 737 Visualizer  
**Fecha:** Julio 2026  
**Estado:** Aprobado  

---

## 1. Arquitectura de Capas

### Decisión
Implementar un sistema de **5 capas independientes** donde cada una es un componente visual completo y mutuamente excluyente.

### Estructura Jerárquica

```
LayerManager
├── Layer 1: Exterior (activa por defecto)
│   ├── Fuselaje (cilindro principal)
│   ├── Alas (superficies alares)
│   ├── Estabilizadores (cola)
│   └── Motores (visibles)
│
├── Layer 2: Estructura Interna
│   ├── Costillas (ribs) transversales
│   ├── Largueros (spars) longitudinales
│   ├── Marcos (frames) circulares
│   └── Refuerzos
│
├── Layer 3: Sistemas
│   ├── Hidráulica (tuberías)
│   ├── Eléctrica (cables)
│   ├── Aire acondicionado (ducts)
│   └── Presurización
│
├── Layer 4: Cabina
│   ├── Asientos (pasajeros)
│   ├── Pasillos (aisles)
│   ├── Puertas (doors)
│   └── Cockpit
│
└── Layer 5: Propulsión
    ├── Motores principales (2x)
    ├── Tren de aterrizaje
    ├── Combustible (tanques)
    └── Sistemas de aire
```

### Implementación en Código

```cpp
// ========== HEADER: layer_manager.h ==========

class LayerManager {
private:
    // Estructura que representa UNA capa
    struct Layer {
        int id;                                    // ID 1-5
        std::string name;                          // "Exterior", "Estructura", etc.
        std::vector<GeometryComponent> components; // Triángulos, líneas, etc.
        bool visible;                              // ¿Se dibuja esta capa?
    };
    
    Layer layers[5];        // Array de las 5 capas
    int currentLayer;       // Índice de capa activa (0-4)
    
public:
    LayerManager();                          // Constructor
    void setActiveLayer(int layer);          // Cambiar a capa N
    int getActiveLayer() const;              // Obtener capa actual
    void render();                           // Dibujar capa activa
};

// ========== SOURCE: layer_manager.cpp ==========

// Constructor: inicializar las 5 capas
LayerManager::LayerManager() {
    // Capa 0: Exterior
    layers[0] = {
        1,                    // id = 1
        "Exterior",          // name
        {},                  // components = vacío (se llena después)
        false                // visible = falso (todavía no)
    };
    
    // Capa 1: Estructura
    layers[1] = {2, "Estructura", {}, false};
    
    // Capa 2: Sistemas
    layers[2] = {3, "Sistemas", {}, false};
    
    // Capa 3: Cabina
    layers[3] = {4, "Cabina", {}, false};
    
    // Capa 4: Propulsión
    layers[4] = {5, "Propulsión", {}, false};
    
    currentLayer = 0;              // Capa 0 (Exterior) es la inicial
    layers[0].visible = true;      // Hacerla visible
}

// Función: cambiar capa activa
void LayerManager::setActiveLayer(int layer) {
    // Validar que el índice sea válido (0-4)
    if (layer >= 0 && layer < 5) {
        
        // Desactivar capa anterior
        layers[currentLayer].visible = false;
        
        // Cambiar a capa nueva
        currentLayer = layer;
        
        // Activar capa nueva
        layers[currentLayer].visible = true;
    }
}

// Función: obtener índice de capa activa
int LayerManager::getActiveLayer() const {
    return currentLayer;  // Devuelve 0-4
}

// Función: dibujar capa activa
void LayerManager::render() {
    // Verificar que la capa actual esté visible
    if (layers[currentLayer].visible) {
        
        // Recorrer TODOS los componentes de esta capa
        for (auto& component : layers[currentLayer].components) {
            // Dibujar cada componente (triángulo, línea, etc.)
            renderer.drawComponent(component);
        }
    }
}
```

---

## 2. Geometría del Fuselaje

### Decisión
Modelar fuselaje como **cilindro discretizado** usando algoritmo de Bresenham.

### Especificaciones

```
Fuselaje (Capa 1 - Exterior)
├── Tipo: Cilindro circular
├── Largo: ~300 píxeles (39.5 m en realidad)
├── Diámetro: ~60 píxeles (3.76 m en realidad)
├── Secciones transversales: 20 (para suavidad)
├── Color: Blanco (RGB: 1.0, 1.0, 1.0)
└── Espesor línea: 1 píxel
```

### Algoritmo Bresenham para Cilindro

```cpp
// geometry.cpp - Generar fuselaje con Bresenham
void GeometryBuilder::generateFuselage() {
    float centerX = 0.0f;
    float centerY = 0.0f;
    float radius = 30.0f;      // Diámetro 60 píxeles
    float length = 300.0f;     // Largo fuselaje
    
    int numSections = 20;      // Resolución
    int pointsPerSection = 32; // Puntos en círculo
    
    // Generar secciones transversales (círculos)
    for (int i = 0; i < numSections; i++) {
        float z = (i / (float)numSections) * length - length/2;
        
        // Usar Bresenham para círculo perfecto
        bresenhamCircle(centerX, centerY, radius, pointsPerSection);
        
        // Conectar con sección anterior
        if (i > 0) {
            drawLine(prevSection[j], currentSection[j]);
        }
    }
}

// Algoritmo Bresenham para círculo discretizado
void GeometryBuilder::bresenhamCircle(float cx, float cy, float r, int points) {
    std::vector<Point> circle;
    int x = 0;
    int y = (int)r;
    int d = 3 - 2 * (int)r;
    
    while (x <= y) {
        // Dibujar 8 puntos simétricos
        circle.push_back({cx + x, cy + y});
        circle.push_back({cx - x, cy + y});
        circle.push_back({cx + x, cy - y});
        circle.push_back({cx - x, cy - y});
        circle.push_back({cx + y, cy + x});
        circle.push_back({cx - y, cy + x});
        circle.push_back({cx + y, cy - x});
        circle.push_back({cx - y, cy - x});
        
        if (d < 0) {
            d = d + 4*x + 6;
        } else {
            d = d + 4*(x-y) + 10;
            y--;
        }
        x++;
    }
    
    return circle;
}
```

### Alternativas Descartadas

| Aproximación | Pros | Contras | Veredicto |
|-------------|------|---------|----------|
| **glutWireSphere()** | Trivial | No discretización propia, "trampa" | ❌ Rechazado |
| **Bresenham circular** ✅ | Correcta discretización | Más código | ✅ Seleccionado |
| **Tabla precalculada** | Rápido | Inflexible, overhead memoria | ❌ Rechazado |

---

## 3. Alas

### Especificaciones

```
Alas (Capa 1 - Exterior)
├── Tipo: 2 rectángulos (ala derecha + izquierda)
├── Largo cada una: ~150 píxeles
├── Ancho cada una: ~50 píxeles
├── Posición: A ±40 píxeles del fuselaje
├── Color: Rojo (RGB: 1.0, 0.0, 0.0)
├── Ángulo: 0° (horizontal)
└── Espesor línea: 1 píxel
```

### Implementación

```cpp
void GeometryBuilder::generateWings() {
    // Ala izquierda
    glColor3f(1.0f, 0.0f, 0.0f);  // Rojo
    glBegin(GL_LINE_LOOP);
    glVertex2f(-75, -50);   // Esquina frontal izquierda
    glVertex2f(-75, 50);    // Esquina trasera izquierda
    glVertex2f(75, 50);     // Esquina trasera derecha
    glVertex2f(75, -50);    // Esquina frontal derecha
    glEnd();
    
    // Ala derecha (espejo)
    glBegin(GL_LINE_LOOP);
    glVertex2f(-75, 40);
    glVertex2f(-75, 90);
    glVertex2f(75, 90);
    glVertex2f(75, 40);
    glEnd();
}
```

---

## 4. Estructura Interna (Capa 2)

### Especificaciones

```
Costillas (Ribs) - Líneas transversales
├── Cantidad: 12
├── Separación: ~25 píxeles (longitudinal)
├── Forma: Semicírculos
├── Color: Verde (RGB: 0.0, 1.0, 0.0)
└── Espesor: 1 píxel

Largueros (Spars) - Líneas longitudinales
├── Cantidad: 3 (frontal, central, trasera)
├── Posición: Distribuidas en diámetro fuselaje
├── Largo: ~300 píxeles (todo fuselaje)
├── Color: Verde claro (RGB: 0.2, 1.0, 0.2)
└── Espesor: 1 píxel

Marcos (Frames) - Círculos de refuerzo
├── Cantidad: 8
├── Diámetro: Similar al fuselaje (~60)
├── Posición: Distribuidas a lo largo
├── Color: Verde oscuro (RGB: 0.0, 0.7, 0.0)
└── Espesor: 1.5 píxeles
```

### Implementación

```cpp
void GeometryBuilder::generateStructure() {
    // Costillas (ribs) transversales - semicírculos
    glColor3f(0.0f, 1.0f, 0.0f);  // Verde
    for (int i = 0; i < 12; i++) {
        float z = (i / 12.0f) * 300 - 150;
        drawBresenhamSemicircle(0, z, 30);  // Radio 30
    }
    
    // Largueros (spars) longitudinales - líneas
    glColor3f(0.2f, 1.0f, 0.2f);  // Verde claro
    for (int i = 0; i < 3; i++) {
        float angle = (i / 3.0f) * 360;
        drawRadialLine(angle, 300);
    }
    
    // Marcos (frames) de refuerzo
    glColor3f(0.0f, 0.7f, 0.0f);  // Verde oscuro
    for (int i = 0; i < 8; i++) {
        float z = (i / 8.0f) * 300 - 150;
        drawBresenhamCircle(0, z, 30);
    }
}
```

---

## 5. Sistemas Internos (Capa 3)

### Especificaciones

```
Tuberías Hidráulicas
├── Cantidad: 4-6 líneas
├── Trazado: Longitudinal (zig-zag)
├── Color: Azul (RGB: 0.0, 0.0, 1.0)
├── Ancho: 1 píxel
└── Puntos de conexión: Motores, actuadores

Cables Eléctricos
├── Cantidad: 8-10 líneas
├── Trazado: Distribuido por todo fuselaje
├── Color: Amarillo (RGB: 1.0, 1.0, 0.0)
├── Ancho: 0.5 píxeles
└── Fuente: Batería (frente) → distribución

Sistema de Aire Acondicionado
├── Ducts: Líneas principales
├── Color: Cian (RGB: 0.0, 1.0, 1.0)
├── Ancho: 1.5 píxeles
└── Conexión: Cockpit y cabina
```

### Implementación

```cpp
void GeometryBuilder::generateSystems() {
    // Tuberías hidráulicas - Azul
    glColor3f(0.0f, 0.0f, 1.0f);
    glBegin(GL_LINE_STRIP);
    for (int i = 0; i < 6; i++) {
        float x = -80 + (i * 30);
        float y = -20 + (i % 2) * 40;
        glVertex2f(x, y);
    }
    glEnd();
    
    // Cables eléctricos - Amarillo (múltiples)
    glColor3f(1.0f, 1.0f, 0.0f);
    for (int cable = 0; cable < 8; cable++) {
        glBegin(GL_LINE_STRIP);
        for (int i = 0; i < 20; i++) {
            float x = -150 + (i * 15);
            float y = -60 + cable * 15;
            glVertex2f(x, y);
        }
        glEnd();
    }
    
    // Sistema de aire - Cian
    glColor3f(0.0f, 1.0f, 1.0f);
    drawACSystem();
}
```

---

## 6. Cabina de Pasajeros (Capa 4)

### Especificaciones

```
Asientos
├── Cantidad: 30 (3 columnas × 10 filas)
├── Forma: Pequeños rectángulos (~4×6 píxeles)
├── Separación: ~8 píxeles
├── Color: Marrón (RGB: 0.65, 0.16, 0.16)
├── Disposición: 2-3-2 (clase económica típica)
└── Área: Centro del fuselaje

Pasillos
├── Cantidad: 2 (izquierda y central)
├── Ancho: ~10 píxeles
├── Largo: ~250 píxeles
├── Color: Gris (RGB: 0.5, 0.5, 0.5)
└── Separación: Pasillos vacíos entre filas

Cockpit
├── Posición: Frente del fuselaje
├── Tamaño: ~40×30 píxeles
├── Color: Gris oscuro (RGB: 0.2, 0.2, 0.2)
├── Elementos: 2 asientos (piloto + copiloto)
└── Cristales: Líneas blancas para ventanas
```

### Implementación

```cpp
void GeometryBuilder::generateCabin() {
    // Asientos - disposición 2-3-2
    glColor3f(0.65f, 0.16f, 0.16f);  // Marrón
    int rows = 10;
    int seatsPerRow[] = {2, 3, 2};    // Izquierda, Centro, Derecha
    
    for (int row = 0; row < rows; row++) {
        float y = 70 - (row * 8);  // De adelante hacia atrás
        int seatIndex = 0;
        
        for (int col = 0; col < 3; col++) {
            for (int seat = 0; seat < seatsPerRow[col]; seat++) {
                float x = -50 + col * 30 + seat * 8;
                drawSeat(x, y, 4, 6);  // Ancho 4, alto 6
            }
            if (col < 2) seatIndex++;  // Espacio para pasillo
        }
    }
    
    // Pasillos - líneas grises
    glColor3f(0.5f, 0.5f, 0.5f);
    glBegin(GL_LINES);
    glVertex2f(-15, 80);
    glVertex2f(-15, -80);
    glVertex2f(15, 80);
    glVertex2f(15, -80);
    glEnd();
    
    // Cockpit - frente
    glColor3f(0.2f, 0.2f, 0.2f);  // Gris oscuro
    glBegin(GL_LINE_LOOP);
    glVertex2f(-30, 140);
    glVertex2f(30, 140);
    glVertex2f(30, 110);
    glVertex2f(-30, 110);
    glEnd();
}
```

---

## 7. Propulsión (Capa 5)

### Especificaciones

```
Motores (2x CFM56-7B)
├── Cantidad: 2 (uno por ala)
├── Posición: Ala izquierda (x=-100, y=0)
│            Ala derecha (x=100, y=0)
├── Tipo: Cilindro horizontal
├── Diámetro: ~25 píxeles
├── Largo: ~40 píxeles
├── Color: Gris (RGB: 0.4, 0.4, 0.4)
└── Cores: Triángulo negro en frente

Tren de Aterrizaje
├── Tren Principal: 2x (bajo alas)
│   ├── Posición: x=±50, y=-60
│   ├── Forma: Líneas verticales + ruedas
│   └── Color: Negro (RGB: 0.0, 0.0, 0.0)
│
└── Tren Nariz: 1x
    ├── Posición: x=0, y=140
    ├── Forma: Línea vertical + rueda frontal
    └── Color: Negro

Tanques de Combustible
├── Ubicación: Dentro de alas y fuselaje central
├── Cantidad: 3 (2 en alas + 1 central)
├── Color: Cian claro (RGB: 0.5, 0.8, 1.0)
└── Forma: Rectángulos rellenos
```

### Implementación

```cpp
void GeometryBuilder::generateEngines() {
    // Motor izquierdo
    glColor3f(0.4f, 0.4f, 0.4f);  // Gris
    drawCylinder(-100, 0, 12.5, 40);
    
    // Motor derecho
    drawCylinder(100, 0, 12.5, 40);
    
    // Intake (frente motor) - triángulo negro
    glColor3f(0.0f, 0.0f, 0.0f);
    glBegin(GL_TRIANGLES);
    glVertex2f(-100, 0);
    glVertex2f(-95, -5);
    glVertex2f(-95, 5);
    glEnd();
}

void GeometryBuilder::generateLandingGear() {
    glColor3f(0.0f, 0.0f, 0.0f);  // Negro
    
    // Tren principal izquierdo
    glBegin(GL_LINES);
    glVertex2f(-50, -30);
    glVertex2f(-50, -70);
    glEnd();
    drawCircle(-50, -75, 5);  // Rueda
    
    // Tren principal derecho
    glBegin(GL_LINES);
    glVertex2f(50, -30);
    glVertex2f(50, -70);
    glEnd();
    drawCircle(50, -75, 5);
    
    // Tren nariz
    glBegin(GL_LINES);
    glVertex2f(0, 130);
    glVertex2f(0, 160);
    glEnd();
    drawCircle(0, 165, 4);  // Rueda más pequeña
}
```

---

## 8. Paleta de Colores

| Capa | Componente | RGB | Hex |
|------|-----------|-----|-----|
| 1 | Fuselaje | (1.0, 1.0, 1.0) | #FFFFFF |
| 1 | Alas | (1.0, 0.0, 0.0) | #FF0000 |
| 1 | Cola | (0.8, 0.8, 0.0) | #CCCC00 |
| 1 | Motores visibles | (0.4, 0.4, 0.4) | #666666 |
| 2 | Costillas | (0.0, 1.0, 0.0) | #00FF00 |
| 2 | Largueros | (0.2, 1.0, 0.2) | #33FF33 |
| 2 | Marcos | (0.0, 0.7, 0.0) | #00B300 |
| 3 | Hidráulica | (0.0, 0.0, 1.0) | #0000FF |
| 3 | Eléctrica | (1.0, 1.0, 0.0) | #FFFF00 |
| 3 | Aire Acond. | (0.0, 1.0, 1.0) | #00FFFF |
| 4 | Asientos | (0.65, 0.16, 0.16) | #A52A2A |
| 4 | Pasillos | (0.5, 0.5, 0.5) | #808080 |
| 4 | Cockpit | (0.2, 0.2, 0.2) | #333333 |
| 5 | Motores | (0.4, 0.4, 0.4) | #666666 |
| 5 | Tren aterrizaje | (0.0, 0.0, 0.0) | #000000 |
| 5 | Combustible | (0.5, 0.8, 1.0) | #80CCFF |

---

## 9. Trade-offs de Diseño

| Aspecto | Decisión | Ganancia | Pérdida |
|--------|----------|----------|---------|
| **Resolución Bresenham** | 20 secciones | Smoothness | ~1000 líneas dibujadas |
| **Colores vs Texturas** | RGB plano | Rápido, simple | Menos realista |
| **Separación de capas** | 1 a la vez | Claridad | No ver superposición |
| **Detalles internos** | Simplificados | Implementable | No ultra-realista |

---

## 10. Evoluciones Futuras (Out of Scope)

- [ ] Adicionar detalles secundarios (ventanas, puertas)
- [ ] Implementar texturas para materiales reales
- [ ] Agregar animación de sistemas (flujo hidráulico)
- [ ] Vista 3D isométrica de capas superpuestas
- [ ] Exportar geometría a formato OBJ
- [ ] Simulación básica de vuelo

---

**Versión:** 1.0  
**Última actualización:** Julio 2026  
**Aprobado por:** Usuario (documentador) + Ronald (geometría)  
**Estado:** Final