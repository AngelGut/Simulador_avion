# ARQ003 — Ciclo de Vida, Entrada y Control de Aplicación

**Proyecto:** Boeing 737 Visualizer  
**Fecha:** Julio 2026  
**Responsable:** Luis (aplicación) + Usuario (documentador)  
**Estado:** Aprobado  

---

## 1. Ciclo de Vida GLUT

### Decisión
Usar **ciclo de vida estándar GLUT** con callbacks registrados.

### Fases del Ciclo

```
┌─────────────────────────────────┐
│  1. Inicialización (main)       │
├─────────────────────────────────┤
│  glutInit()                     │
│  glutInitDisplayMode()          │
│  glutInitWindowSize()           │
│  glutCreateWindow()             │
│  glClearColor()                 │
│  setupOpenGL()                  │
└────────────┬────────────────────┘
             ↓
┌─────────────────────────────────┐
│  2. Registro de Callbacks       │
├─────────────────────────────────┤
│  glutDisplayFunc()              │
│  glutReshapeFunc()              │
│  glutKeyboardFunc()             │
│  glutTimerFunc()                │
└────────────┬────────────────────┘
             ↓
┌─────────────────────────────────┐
│  3. Loop Principal (GLUT)       │
├─────────────────────────────────┤
│  glutMainLoop()                 │
│  ├─ Espera eventos              │
│  ├─ Llama callbacks              │
│  └─ Repite hasta exit()         │
└────────────┬────────────────────┘
             ↓
┌─────────────────────────────────┐
│  4. Limpieza (exit)             │
├─────────────────────────────────┤
│  Liberar memoria                │
│  Cerrar ventana                 │
│  Terminar programa              │
└─────────────────────────────────┘
```

### Implementación Completa

```cpp
// main.cpp - Ciclo de vida GLUT

#include <iostream>
#include <GL/glut.h>
#include "layer_manager.h"
#include "renderer.h"

// Variables globales
LayerManager g_layerManager;
Renderer g_renderer;
int g_windowWidth = 800;
int g_windowHeight = 600;

// ============ FASE 1: CALLBACKS ============

void display() {
    // Callback display - llamado cuando necesita redibujarse
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
    glClearColor(0.1f, 0.1f, 0.1f, 1.0f);
    
    std::cout << "[DISPLAY] Redibujando capa " 
              << g_layerManager.getActiveLayer() + 1 << "\n";
    
    g_renderer.setupLighting();
    g_renderer.drawLayer(g_layerManager.getActiveLayer());
    
    glFlush();
    glutSwapBuffers();
}

void reshape(int width, int height) {
    // Callback reshape - llamado al cambiar tamaño ventana
    if (height == 0) height = 1;
    
    g_windowWidth = width;
    g_windowHeight = height;
    
    std::cout << "[RESHAPE] Nueva dimensión: " 
              << width << "x" << height << "\n";
    
    glViewport(0, 0, width, height);
    glMatrixMode(GL_PROJECTION);
    glLoadIdentity();
    gluOrtho2D(-200, 200, -150, 150);
    glMatrixMode(GL_MODELVIEW);
}

void keyboard(unsigned char key, int x, int y) {
    // Callback keyboard - llamado al presionar tecla
    std::cout << "[KEYBOARD] Tecla presionada: " 
              << key << " (ASCII " << (int)key << ")\n";
    
    if (key >= '1' && key <= '5') {
        int layer = key - '0' - 1;  // 0-4
        g_layerManager.setActiveLayer(layer);
        std::cout << "[KEYBOARD] → Activando capa " << layer + 1 << "\n";
    }
    else if (key == 27) {  // ESC
        std::cout << "[KEYBOARD] → Salida solicitada (ESC)\n";
        exit(0);
    }
    else if (key == 'h' || key == 'H') {
        std::cout << "\n=== AYUDA ===\n"
                  << "1-5: Cambiar capas\n"
                  << "ESC: Salir\n"
                  << "H: Esta ayuda\n"
                  << "=============\n";
    }
    
    glutPostRedisplay();  // Fuerza redibujado
}

void timer(int value) {
    // Callback timer - llamado periódicamente
    // Útil para animaciones (no usado en v1.0)
    
    // Redibujar
    glutPostRedisplay();
    
    // Registrar siguiente timer (16ms = ~60 FPS)
    glutTimerFunc(16, timer, 0);
}

// ============ FASE 2: INICIALIZACIÓN ============

void initGLUT(int argc, char** argv) {
    std::cout << "[INIT] Inicializando GLUT...\n";
    
    // Inicializar GLUT
    glutInit(&argc, argv);
    
    // Modo display
    glutInitDisplayMode(GLUT_DOUBLE | GLUT_RGB | GLUT_DEPTH);
    
    // Tamaño ventana
    glutInitWindowSize(g_windowWidth, g_windowHeight);
    glutInitWindowPosition(100, 100);
    
    // Crear ventana
    glutCreateWindow("Boeing 737 Visualizer - Capas");
    
    std::cout << "[INIT] Ventana GLUT creada (800x600)\n";
}

void initOpenGL() {
    std::cout << "[INIT] Inicializando OpenGL...\n";
    
    // Color de fondo
    glClearColor(0.1f, 0.1f, 0.1f, 1.0f);
    
    // Habilitar depth testing
    glEnable(GL_DEPTH_TEST);
    
    // Proyección inicial
    glMatrixMode(GL_PROJECTION);
    glLoadIdentity();
    gluOrtho2D(-200, 200, -150, 150);
    glMatrixMode(GL_MODELVIEW);
    glLoadIdentity();
    
    std::cout << "[INIT] OpenGL configurado\n";
}

void initLayers() {
    std::cout << "[INIT] Inicializando capas...\n";
    
    // LayerManager inicializa en constructor
    g_layerManager.setActiveLayer(0);  // Capa 1
    
    std::cout << "[INIT] Capa activa: 1 (Exterior)\n";
}

void initRenderer() {
    std::cout << "[INIT] Inicializando renderer...\n";
    
    g_renderer.setupOpenGL();
    
    std::cout << "[INIT] Renderer listo\n";
}

void registerCallbacks() {
    std::cout << "[INIT] Registrando callbacks...\n";
    
    glutDisplayFunc(display);
    glutReshapeFunc(reshape);
    glutKeyboardFunc(keyboard);
    glutTimerFunc(16, timer, 0);  // 16ms = ~60 FPS
    
    std::cout << "[INIT] Callbacks registrados\n";
}

// ============ MAIN ============

int main(int argc, char** argv) {
    std::cout << "\n╔═════════════════════════════════════╗\n"
              << "║   Boeing 737 Visualizer v1.0        ║\n"
              << "║   Computación Gráfica - 8vo Semestre║\n"
              << "╚═════════════════════════════════════╝\n\n";
    
    // Fase 1: GLUT
    initGLUT(argc, argv);
    
    // Fase 2: OpenGL
    initOpenGL();
    
    // Fase 3: LayerManager
    initLayers();
    
    // Fase 4: Renderer
    initRenderer();
    
    // Fase 5: Callbacks
    registerCallbacks();
    
    // Mostrar controles
    std::cout << "\n=== CONTROLES ===\n"
              << "Teclas 1-5: Cambiar capas\n"
              << "ESC: Salir\n"
              << "H: Ayuda\n"
              << "==================\n\n";
    
    std::cout << "[MAIN] Iniciando loop principal...\n";
    
    // Fase 6: LOOP PRINCIPAL
    glutMainLoop();
    
    // (Nunca llega aquí a menos que exit() sea llamado)
    std::cout << "[MAIN] Programa terminado normalmente\n";
    
    return 0;
}
```

---

## 2. Entrada por Teclado

### Decisión
Usar **glutKeyboardFunc() callback** para entrada de teclado.

### Mapa de Teclas

```
Tecla   | Función
--------|------------------------------------------
1       | Activar Capa 1 (Exterior)
2       | Activar Capa 2 (Estructura)
3       | Activar Capa 3 (Sistemas)
4       | Activar Capa 4 (Cabina)
5       | Activar Capa 5 (Propulsión)
ESC     | Salir del programa
H       | Mostrar ayuda
P       | Pausar/reanudar (future)
R       | Reset a capa 1 (future)
```

### Implementación del Handler

```cpp
void keyboard(unsigned char key, int x, int y) {
    // Parámetros:
    // - key: código ASCII de tecla
    // - x, y: posición del mouse cuando se presionó
    
    // Convertir a minúsculas si es letra
    if (key >= 'A' && key <= 'Z') {
        key = key - 'A' + 'a';
    }
    
    // Procesar teclas de capas
    if (key >= '1' && key <= '5') {
        int layer = key - '0' - 1;  // Convertir '1' → 0, '2' → 1, etc.
        
        if (g_layerManager.setActiveLayer(layer)) {
            std::cout << "✓ Capa " << layer + 1 << " activada\n";
        }
        glutPostRedisplay();  // Fuerza redibujado
        return;
    }
    
    // Salida
    if (key == 27) {  // ESC
        std::cout << "Cerrando aplicación...\n";
        exit(0);
    }
    
    // Ayuda
    if (key == 'h') {
        printHelp();
        return;
    }
    
    // Tecla no reconocida
    std::cerr << "Tecla no reconocida: '" << key << "' (ASCII " 
              << (int)key << ")\n";
}

void printHelp() {
    std::cout << "\n╔════════════════════════════════╗\n"
              << "║         CONTROLES              ║\n"
              << "╠════════════════════════════════╣\n"
              << "║ 1-5 : Cambiar entre capas      ║\n"
              << "║ H   : Ver esta ayuda           ║\n"
              << "║ ESC : Salir                    ║\n"
              << "╚════════════════════════════════╝\n\n";
}
```

### Alternativas Descartadas

| Método | Pros | Contras | Veredicto |
|--------|------|---------|----------|
| **Keyboard callback** ✅ | Estándar GLUT, simple | Solo teclas | ✅ Seleccionado |
| **Mouse callback** | Más intuitivo | Overhead UI | ❌ Rechazado |
| **Joystick** | Cool | Raro, overhead | ❌ Rechazado |

---

## 3. LayerManager: Control de Estado

### Decisión
Encapsular lógica de capas en clase `LayerManager`.

### Responsabilidades

```cpp
// layer_manager.h

class LayerManager {
private:
    int currentLayer;                    // 0-4
    std::vector<Layer> layers;           // 5 capas
    bool layerVisible[5];               // Flags de visibilidad
    
public:
    // Constructor
    LayerManager();
    
    // Control de capas
    bool setActiveLayer(int layerIndex);  // Cambiar capa
    int getActiveLayer() const;           // Obtener capa actual
    
    // Información
    std::string getLayerName(int idx) const;
    Layer& getLayer(int idx);
    
    // Utilidad
    void printLayerInfo() const;
};

// layer_manager.cpp

LayerManager::LayerManager() 
    : currentLayer(0) {
    
    // Inicializar 5 capas
    layers.resize(5);
    
    layers[0].id = 1;
    layers[0].name = "Exterior";
    layers[0].color = {1.0f, 0.0f, 0.0f};  // Rojo
    
    layers[1].id = 2;
    layers[1].name = "Estructura";
    layers[1].color = {0.0f, 1.0f, 0.0f};  // Verde
    
    layers[2].id = 3;
    layers[2].name = "Sistemas";
    layers[2].color = {0.0f, 0.0f, 1.0f};  // Azul
    
    layers[3].id = 4;
    layers[3].name = "Cabina";
    layers[3].color = {1.0f, 1.0f, 0.0f};  // Amarillo
    
    layers[4].id = 5;
    layers[4].name = "Propulsión";
    layers[4].color = {1.0f, 0.0f, 1.0f};  // Magenta
    
    setActiveLayer(0);  // Capa 1 por defecto
}

bool LayerManager::setActiveLayer(int layerIndex) {
    if (layerIndex < 0 || layerIndex >= 5) {
        std::cerr << "ERROR: Capa inválida (" << layerIndex << ")\n";
        return false;
    }
    
    // Desactivar anterior
    layerVisible[currentLayer] = false;
    
    // Activar nueva
    currentLayer = layerIndex;
    layerVisible[currentLayer] = true;
    
    return true;
}

int LayerManager::getActiveLayer() const {
    return currentLayer;
}

std::string LayerManager::getLayerName(int idx) const {
    if (idx >= 0 && idx < 5) {
        return layers[idx].name;
    }
    return "Unknown";
}
```

---

## 4. Renderizado: Display Loop

### Decisión
Usar **glutDisplayFunc()** callback para renderizar cada frame.

### Implementación

```cpp
void display() {
    // 1. Limpiar buffers
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
    glClearColor(0.1f, 0.1f, 0.1f, 1.0f);
    
    // 2. Setup iluminación
    g_renderer.setupLighting();
    
    // 3. Obtener capa activa
    int activeLayer = g_layerManager.getActiveLayer();
    
    // 4. Dibujar capa
    g_renderer.drawLayer(activeLayer);
    
    // 5. Mostrar buffer frontal
    glFlush();
    glutSwapBuffers();  // Double buffering
}
```

### Double Buffering

```
Frame N          Frame N+1
┌────────┐       ┌────────┐
│ Back   │ ←draw │ Back   │
│ Buffer │       │ Buffer │
└────────┘       └────────┘
    ↓ swap          ↓ swap
┌────────┐       ┌────────┐
│ Front  │ show  │ Front  │
│ Buffer │       │ Buffer │
└────────┘       └────────┘
    ↑                ↑
  Pantalla         Pantalla
  Frame N        Frame N+1
```

### Ventajas
- Sin parpadeo (flicker)
- Transiciones suaves
- 60 FPS posible

---

## 5. Reshape: Ajuste de Ventana

### Decisión
Usar **glutReshapeFunc()** para mantener proyección correcta.

### Implementación

```cpp
void reshape(int width, int height) {
    // Evitar división por cero
    if (height == 0) height = 1;
    
    // Guardar nuevas dimensiones
    g_windowWidth = width;
    g_windowHeight = height;
    
    // Actualizar viewport
    glViewport(0, 0, width, height);
    
    // Recalcular proyección (mantener aspect ratio)
    glMatrixMode(GL_PROJECTION);
    glLoadIdentity();
    
    float aspect = (float)width / (float)height;
    float left = -200 * aspect;
    float right = 200 * aspect;
    float bottom = -150;
    float top = 150;
    
    gluOrtho2D(left, right, bottom, top);
    
    glMatrixMode(GL_MODELVIEW);
}
```

---

## 6. Timer: Loop de Actualización

### Decisión
Usar **glutTimerFunc()** para frecuencia de actualización (~60 FPS).

### Implementación

```cpp
void timer(int value) {
    // Callback timer - se ejecuta cada 16ms
    
    // Actualizar lógica (animaciones, etc.)
    // (v1.0: sin animación)
    
    // Forzar redibujado
    glutPostRedisplay();
    
    // Registrar próxima ejecución
    glutTimerFunc(16, timer, 0);  // 16ms ≈ 60 FPS
}

// En main:
glutTimerFunc(16, timer, 0);  // Iniciar timer
```

### Cálculo de FPS

```
16 ms/frame = 1000 / 16 = 62.5 frames por segundo
```

---

## 7. Flujo de Control: Cambiar Capas

```
Usuario presiona '2'
    ↓
[KERNEL] → keyboard() callback
    ↓
key = '2'
    ↓
layer = 2 - 1 = 1
    ↓
g_layerManager.setActiveLayer(1)
    │
    ├─→ Desactivar capa anterior
    ├─→ Activar capa 1
    └─→ return true
    ↓
std::cout << "Capa 2 activada"
    ↓
glutPostRedisplay()  // Fuerza redibujado
    ↓
[GLUT scheduler]
    ↓
display() callback
    ↓
g_renderer.drawLayer(1)  // Dibuja estructura
    ↓
Pantalla actualiza
```

---

## 8. Manejo de Errores

### Error Handling Básico

```cpp
void keyboard(unsigned char key, int x, int y) {
    try {
        if (key >= '1' && key <= '5') {
            int layer = key - '0' - 1;
            
            if (!g_layerManager.setActiveLayer(layer)) {
                std::cerr << "ERROR: No se pudo cambiar a capa " 
                          << layer + 1 << "\n";
                return;
            }
            glutPostRedisplay();
        }
    }
    catch (const std::exception& e) {
        std::cerr << "EXCEPCIÓN en keyboard: " << e.what() << "\n";
    }
}

bool LayerManager::setActiveLayer(int layerIndex) {
    if (layerIndex < 0 || layerIndex >= 5) {
        std::cerr << "ERROR: Índice de capa inválido: " 
                  << layerIndex << "\n";
        return false;
    }
    
    // ... cambiar capa ...
    
    return true;
}
```

---

## 9. Logging y Debug

### Niveles de Log

```cpp
enum LogLevel {
    LOG_ERROR,    // Errores críticos
    LOG_WARN,     // Advertencias
    LOG_INFO,     // Información general
    LOG_DEBUG,    // Debug detallado
};

void log(LogLevel level, const std::string& msg) {
    const char* levelStr[] = {"[ERROR]", "[WARN]", "[INFO]", "[DEBUG]"};
    std::cout << levelStr[level] << " " << msg << "\n";
}

// Uso:
log(LOG_INFO, "Capa 2 activada");
log(LOG_DEBUG, "display() called");
log(LOG_ERROR, "Falló inicialización de layer");
```

---

## 10. Checklist del Ciclo de Vida

- [ ] `main()` — punto de entrada
- [ ] `initGLUT()` — glutInit + window
- [ ] `initOpenGL()` — GL setup
- [ ] `initLayers()` — LayerManager
- [ ] `initRenderer()` — Renderer setup
- [ ] `registerCallbacks()` — display, reshape, keyboard, timer
- [ ] `display()` — redibujado
- [ ] `reshape()` — viewport/proyección
- [ ] `keyboard()` — entrada 1-5, ESC
- [ ] `timer()` — loop ~60 FPS
- [ ] `glutMainLoop()` — loop principal
- [ ] Logging en cada fase
- [ ] Error handling basic

---

**Versión:** 1.0  
**Última actualización:** Julio 2026  
**Aprobado por:** Luis (aplicación) + Usuario (documentador)  
**Estado:** Final