# ARQ002 — Renderizado, Luces y Materiales

**Proyecto:** Boeing 737 Visualizer  
**Fecha:** Julio 2026  
**Estado:** Aprobado  

---

## 1. Pipeline Gráfico: Fixed Pipeline vs Shaders

### Decisión
Usar **Fixed Pipeline de OpenGL 1.1** en lugar de shaders modernos.

### Contexto
- GLUT soporta nativamente OpenGL 1.1 (fixed pipeline)
- Proyecto de 4 semanas (scope limitado)
- No se requiere calidad cinematográfica
- Estudiantes en gráfica básica

### Alternativas Consideradas

| Aproximación | Pros | Contras | Veredicto |
|-------------|------|---------|----------|
| **Fixed Pipeline (GL 1.1)** ✅ | Trivial de usar, GLUT nativo | Menos control, legacy | ✅ Seleccionado |
| **Shaders GLSL (GL 3.0+)** | Moderno, flexible | Requiere GLFW, curva aprendizaje | ❌ Rechazado |
| **Deferred Shading** | Profesional | Overkill para 2D | ❌ Rechazado |

### Justificación
1. **GLUT 1.1** viene integrado
2. **glLight(), glMaterial()** es suficiente
3. **Sin overhead** de compilar shaders
4. **Enfocarse en geometría** vs configuración de shaders

### Implementación Básica

```cpp
// renderer.cpp - Setup del pipeline
void Renderer::setupOpenGL() {
    // Habilitar profundidad
    glEnable(GL_DEPTH_TEST);
    
    // Habilitar iluminación
    glEnable(GL_LIGHTING);
    glEnable(GL_LIGHT0);  // Luz principal
    glEnable(GL_LIGHT1);  // Luz ambiente
    
    // Habilitar color tracking (usar glColor para material)
    glEnable(GL_COLOR_MATERIAL);
    glColorMaterial(GL_FRONT_AND_BACK, GL_AMBIENT_DIFFUSE);
    
    // Modelo de iluminación Gouraud (defecto)
    glShadeModel(GL_SMOOTH);
}

// Render loop
void display() {
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
    glClearColor(0.1f, 0.1f, 0.1f, 1.0f);
    
    setupLighting();
    renderer.drawLayer(layerManager.getActiveLayer());
    
    glFlush();
    glutSwapBuffers();
}
```

---

## 2. Iluminación: Modelo Phong Simplificado

### Decisión
Implementar **iluminación Phong básica** con 2 luces: direccional (sol) + ambiente.

### Componentes de Iluminación

```
Fórmula Phong simplificada:
Color = Ambiente + (Difusa × max(0, N·L)) + (Especular × max(0, R·V)^shine)

Donde:
- Ambiente: luz constante (~0.3)
- Difusa: color base con ángulo hacia luz
- Especular: brillo puntual
- N = normal de superficie
- L = dirección luz
- R = reflexión
- V = vista
```

### Configuración de Luces

```cpp
// renderer.cpp - Configurar luces GLUT
void Renderer::setupLighting() {
    // Luz 0: Direccional (simulando sol)
    GLfloat light0_position[] = {1.0f, 1.0f, 1.0f, 0.0f};  // Infinita
    GLfloat light0_ambient[]  = {0.2f, 0.2f, 0.2f, 1.0f};
    GLfloat light0_diffuse[]  = {0.8f, 0.8f, 0.8f, 1.0f};
    GLfloat light0_specular[] = {1.0f, 1.0f, 1.0f, 1.0f};
    
    glLightfv(GL_LIGHT0, GL_POSITION, light0_position);
    glLightfv(GL_LIGHT0, GL_AMBIENT, light0_ambient);
    glLightfv(GL_LIGHT0, GL_DIFFUSE, light0_diffuse);
    glLightfv(GL_LIGHT0, GL_SPECULAR, light0_specular);
    
    // Luz 1: Ambiente global (garantiza mínima visibilidad)
    GLfloat light1_ambient[] = {0.3f, 0.3f, 0.3f, 1.0f};
    glLightfv(GL_LIGHT1, GL_AMBIENT, light1_ambient);
    
    // Parámetros globales
    GLfloat global_ambient[] = {0.1f, 0.1f, 0.1f, 1.0f};
    glLightModelfv(GL_LIGHT_MODEL_AMBIENT, global_ambient);
}
```

### Alternativas Descartadas

| Modelo | Pros | Contras | Veredicto |
|--------|------|---------|----------|
| **Sin iluminación (color plano)** | Trivial | Monótono, sin profundidad | ❌ Rechazado |
| **Phong simplificado** ✅ | Balance realismo/complejidad | Más cálculo | ✅ Seleccionado |
| **Ray tracing** | Foto-realista | Imposible en tiempo real 2D | ❌ Rechazado |

---

## 3. Materiales: Propiedades de Superficie

### Decisión
Usar **propiedades de material RGB** por componente sin texturas.

### Propiedades de Material

Cada componente tiene:
- **Ambient**: Refleja luz ambiente (0.1-0.3)
- **Diffuse**: Refleja luz difusa (color principal)
- **Specular**: Refleja especular (brillo)
- **Shininess**: Suavidad de brillo (0-128)

### Implementación por Capa

```cpp
// geometry.h - Estructura de material
struct Material {
    GLfloat ambient[4];   // Luz ambiente
    GLfloat diffuse[4];   // Luz difusa (color principal)
    GLfloat specular[4];  // Brillo
    GLfloat shininess;    // Exponente (0-128)
};

// renderer.cpp - Aplicar material
void Renderer::applyMaterial(const Material& mat) {
    glMaterialfv(GL_FRONT_AND_BACK, GL_AMBIENT, mat.ambient);
    glMaterialfv(GL_FRONT_AND_BACK, GL_DIFFUSE, mat.diffuse);
    glMaterialfv(GL_FRONT_AND_BACK, GL_SPECULAR, mat.specular);
    glMaterialf(GL_FRONT_AND_BACK, GL_SHININESS, mat.shininess);
}
```

### Materiales por Componente

```cpp
// Material: Fuselaje (exterior)
const Material MATERIAL_FUSELAGE = {
    .ambient  = {0.3f, 0.3f, 0.3f, 1.0f},  // Gris claro
    .diffuse  = {1.0f, 1.0f, 1.0f, 1.0f},  // Blanco (color principal)
    .specular = {0.8f, 0.8f, 0.8f, 1.0f},  // Algo de brillo
    .shininess = 50.0f                     // Brillo moderado (aluminio)
};

// Material: Estructura (acero)
const Material MATERIAL_STRUCTURE = {
    .ambient  = {0.2f, 0.2f, 0.2f, 1.0f},
    .diffuse  = {0.4f, 0.8f, 0.4f, 1.0f},  // Verde (metal pintado)
    .specular = {0.6f, 0.6f, 0.6f, 1.0f},
    .shininess = 80.0f                     // Más metálico
};

// Material: Sistemas (plástico/caucho)
const Material MATERIAL_SYSTEMS = {
    .ambient  = {0.1f, 0.1f, 0.1f, 1.0f},
    .diffuse  = {0.5f, 0.5f, 1.0f, 1.0f},  // Azul (tuberías)
    .specular = {0.2f, 0.2f, 0.2f, 1.0f},  // Poco brillo
    .shininess = 10.0f                     // Mate
};

// Material: Asientos (tela/cuero)
const Material MATERIAL_SEATS = {
    .ambient  = {0.3f, 0.1f, 0.1f, 1.0f},
    .diffuse  = {0.65f, 0.16f, 0.16f, 1.0f},  // Marrón
    .specular = {0.1f, 0.1f, 0.1f, 1.0f},     // Muy mate
    .shininess = 5.0f                         // Casi nada de brillo
};

// Material: Motores (metal oscuro)
const Material MATERIAL_ENGINES = {
    .ambient  = {0.2f, 0.2f, 0.2f, 1.0f},
    .diffuse  = {0.4f, 0.4f, 0.4f, 1.0f},     // Gris oscuro
    .specular = {0.7f, 0.7f, 0.7f, 1.0f},     // Muy reflectivo
    .shininess = 120.0f                       // Muy brillante (metal pulido)
};
```

---

## 4. Colores RGB vs Texturas

### Decisión
Usar **solo colores RGB** (sin texturas).

### Justificación
1. **Requerimiento:** "Sin texturas"
2. **Claridad visual:** Colores distinguen capas
3. **Rendimiento:** Sin overhead de texturas
4. **Pedagogía:** Enfoque en geometría + iluminación

### Alternativas Descartadas

| Opción | Pros | Contras | Veredicto |
|--------|------|---------|----------|
| **Colores planos (RGB)** ✅ | Simple, claro, requerido | Menos realista | ✅ Seleccionado |
| **Texturas 2D mapeadas** | Realismo | Requiere UV mapping, archivos | ❌ Rechazado |
| **Procedurales** | Dinámicas | Complejo, overhead | ❌ Rechazado |

### Implementación

```cpp
// renderer.cpp - Dibujar con color
void Renderer::drawComponent(const Component& comp) {
    applyMaterial(comp.material);
    
    glColor3fv(comp.color.rgb);  // Usar color RGB
    
    glBegin(comp.primitiveType);  // GL_LINES, GL_TRIANGLES, etc.
    for (const auto& vertex : comp.vertices) {
        glVertex2f(vertex.x, vertex.y);
    }
    glEnd();
}
```

---

## 5. Transformaciones Matriciales

### Decisión
Usar **matrices de transformación OpenGL estándar** (glTranslatef, glRotatef, glScalef).

### Transformaciones Soportadas

```cpp
void Renderer::applyTransform(const Transform& t) {
    glPushMatrix();
    
    // Traslación
    glTranslatef(t.position.x, t.position.y, t.position.z);
    
    // Rotación
    glRotatef(t.rotation.z, 0.0f, 0.0f, 1.0f);  // Eje Z (2D)
    
    // Escala
    glScalef(t.scale.x, t.scale.y, t.scale.z);
    
    // Dibujar geometría...
    
    glPopMatrix();
}
```

### Matrices por Capa

```cpp
// Layer 1: Sin transformación (identidad)
glPushMatrix();
glLoadIdentity();
drawExterior();
glPopMatrix();

// Layer 2: Rotación leve para enfoque
glPushMatrix();
glTranslatef(0, 0, 0);
glRotatef(0.5f, 0, 0, 1);  // Rotación Z mínima
drawStructure();
glPopMatrix();

// Layer 3-5: Sin transformación extra
```

### Alternativas

| Método | Pros | Contras | Veredicto |
|--------|------|---------|----------|
| **Stack de matrices GL** ✅ | Estándar, eficiente | Orden importa | ✅ Seleccionado |
| **Cálculo manual (GLM)** | Más control | Overhead adicional | ❌ Rechazado |
| **sin transformación** | Trivial | Limitado | ❌ Rechazado |

---

## 6. Proyección: Ortho 2D

### Decisión
Usar **proyección ortogonal 2D** con `gluOrtho2D()`.

### Especificaciones

```cpp
// renderer.cpp - Configurar proyección
void Renderer::setupProjection(int width, int height) {
    glMatrixMode(GL_PROJECTION);
    glLoadIdentity();
    
    // Proyección ortho 2D
    // Coordenadas: X [-200, 200], Y [-150, 150]
    gluOrtho2D(-200, 200, -150, 150);
    
    glMatrixMode(GL_MODELVIEW);
    glLoadIdentity();
}

// En reshape callback
void reshape(int w, int h) {
    if (h == 0) h = 1;
    float aspect = (float)w / (float)h;
    
    glViewport(0, 0, w, h);
    glMatrixMode(GL_PROJECTION);
    glLoadIdentity();
    
    // Ajustar para mantener aspect ratio
    gluOrtho2D(-200 * aspect, 200 * aspect, -150, 150);
    
    glMatrixMode(GL_MODELVIEW);
}
```

### Ventajas de Ortho

1. **Sin perspectiva** → líneas paralelas permanecen paralelas
2. **Fácil medir** → píxel = unidad exacta
3. **Para 2D** → ideal visualización top-down
4. **No clip artifacts** → sin problemas de profundidad relativa

### Alternativas Rechazadas

| Proyección | Pros | Contras | Veredicto |
|-----------|------|---------|----------|
| **Ortho 2D** ✅ | Simple, perfecto 2D | No perspectiva | ✅ Seleccionado |
| **Perspectiva** | Realista 3D | Overkill para 2D | ❌ Rechazado |
| **Isométrica** | Pseudo-3D | Más matrices | ❌ Rechazado |

---

## 7. Rasterización: Líneas vs Triángulos

### Decisión
Usar principalmente **líneas (GL_LINES, GL_LINE_LOOP)** para fuselaje; triángulos rellenos para componentes internos.

### Primitivas Utilizadas

```cpp
// Líneas (fuselaje exterior, estructura)
glBegin(GL_LINES);
glVertex2f(x1, y1);
glVertex2f(x2, y2);
glEnd();

// Polígonos cerrados (alas, asientos)
glBegin(GL_LINE_LOOP);
glVertex2f(x1, y1);
glVertex2f(x2, y2);
glVertex2f(x3, y3);
glEnd();

// Triángulos rellenos (componentes sólidos)
glBegin(GL_TRIANGLES);
glVertex2f(x1, y1);
glVertex2f(x2, y2);
glVertex2f(x3, y3);
glEnd();

// Puntos (detalles finos)
glBegin(GL_POINTS);
glVertex2f(x, y);
glEnd();
```

### Configuración de Líneas

```cpp
// renderer.cpp - Configurar estilos de línea
glLineWidth(1.0f);      // Ancho estándar
glEnable(GL_LINE_SMOOTH);
glHint(GL_LINE_SMOOTH_HINT, GL_NICEST);

// Para costillas rellenas
glPolygonMode(GL_FRONT_AND_BACK, GL_LINE);  // Wireframe
// o
glPolygonMode(GL_FRONT_AND_BACK, GL_FILL);  // Relleno
```

---

## 8. Blending (Transparencia)

### Decisión
**NO usar transparencia** (cada capa es opaca).

### Justificación
1. Capas son exclusivas (una a la vez)
2. No hay superposición necesaria
3. Evita complejidad de depth sorting
4. Rendimiento mejor sin blending

### Configuración (deshabilitada)

```cpp
glDisable(GL_BLEND);  // Sin mezcla de colores
```

### Si se necesitara (future):

```cpp
glEnable(GL_BLEND);
glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
glColor4f(r, g, b, alpha);  // Usar alpha canal
```

---

## 9. Back-Face Culling (Eliminación de Caras Traseras)

### Decisión
**Habilitar back-face culling** para optimización.

### Justificación
1. Vista superior → nunca ve caras traseras
2. Reduce cálculos de iluminación 50%
3. Previene artefactos de normal invertida

### Implementación

```cpp
void Renderer::setupCulling() {
    glEnable(GL_CULL_FACE);
    glCullFace(GL_BACK);          // Descartar caras traseras
    glFrontFace(GL_CCW);           // Orden: counter-clockwise frontal
}
```

---

## 10. Anti-aliasing

### Decisión
**Habilitado pero no crítico** para 2D.

### Configuración

```cpp
void Renderer::setupAntialiasing() {
    // Suavizado de líneas
    glEnable(GL_LINE_SMOOTH);
    glHint(GL_LINE_SMOOTH_HINT, GL_NICEST);
    
    // Suavizado de polígonos
    glEnable(GL_POLYGON_SMOOTH);
    glHint(GL_POLYGON_SMOOTH_HINT, GL_NICEST);
}
```

---

## 11. Pipeline Completo de Render

```
┌─────────────────────────────────┐
│    display() callback (GLUT)    │
└────────────┬────────────────────┘
             ↓
┌─────────────────────────────────┐
│ glClear(COLOR_BUFFER | DEPTH)   │
└────────────┬────────────────────┘
             ↓
┌─────────────────────────────────┐
│    setupLighting()              │
│ - Posiciona luces               │
│ - Configura ambiente            │
└────────────┬────────────────────┘
             ↓
┌─────────────────────────────────┐
│    getActiveLayer()             │
│ - Obtiene Layer ID              │
└────────────┬────────────────────┘
             ↓
┌─────────────────────────────────┐
│    drawLayer(layer)             │
│ ├─ Para cada componente:        │
│ │  ├─ applyMaterial()           │
│ │  ├─ glColor3f()               │
│ │  ├─ applyTransform()          │
│ │  ├─ glBegin/glVertex/glEnd()  │
│ └─ Fin loop                     │
└────────────┬────────────────────┘
             ↓
┌─────────────────────────────────┐
│    glFlush()                    │
│    glutSwapBuffers()            │
└─────────────────────────────────┘
```

---

## 12. Trade-offs de Rendering

| Decisión | Ganancia | Pérdida |
|----------|----------|---------|
| **Fixed Pipeline** | Simpleza | Control fino |
| **Phong simplificado** | Realismo básico | No especular avanzado |
| **Solo RGB** | Velocidad | Menos detalle visual |
| **Ortho 2D** | Claridad | Sin perspectiva |
| **Sin transparencia** | Rendimiento | No overlays |

---

## 13. Checklist de Implementación

- [ ] `setupOpenGL()` — configurar GLUT + depth + lighting
- [ ] `setupLighting()` — 2 luces (direccional + ambiente)
- [ ] `applyMaterial()` — 5 materiales predefinidos
- [ ] `drawLayer()` — dibujar componentes con material
- [ ] `setupProjection()` — ortho 2D en reshape
- [ ] `setupCulling()` — back-face culling
- [ ] `setupAntialiasing()` — suavizado (opcional)
- [ ] Callbacks integrados en display/reshape

---

**Versión:** 1.0  
**Última actualización:** Julio 2026  
**Aprobado por:** Ronald (rendering) + Usuario (documentador)  
**Estado:** Final