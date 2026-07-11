// ============================================================
// ARCHIVO: main.cpp
// RESPONSABLE: Luis (Aplicación) + Cambios de navegación
// PROYECTO: Boeing 737 Visualizer - Julio 2026
// DESCRIPCION: Punto de entrada. Integra LayerManager,
//              Push/Pop Matrix, transformaciones (Translate,
//              Rotate, Scale) y navegación interactiva.
// ============================================================

#include <GL/glut.h>
#include <iostream>
#include <cmath>
#include "renderer.h"
#include "layer_manager.h"

// Declaración adelantada
void printHelp();

// ============================================================
// VARIABLES GLOBALES - Estado de aplicación
// ============================================================

// Gestión de capas (Luis)
LayerManager layerManager;

// Transformación de vista: navegación interactiva
float viewX = 0.0f;        // Traslación X (pan horizontal)
float viewY = 0.0f;        // Traslación Y (pan vertical)
float viewZoom = 1.0f;     // Escala (zoom in/out)
float viewRotation = 0.0f; // Rotación en eje Z (grados)

// ============================================================
// DISPLAY - Callback de dibujado con Push/Pop y transformaciones
// ============================================================
void display() {
    // Limpiar buffers
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

    glMatrixMode(GL_MODELVIEW);
    glLoadIdentity();

    // ========================================
    // BLOQUE 1: AVIÓN CON TRANSFORMACIONES
    // ========================================
    // Aplicar: Translate → Rotate → Scale
    // Todo dentro de Push/Pop para no afectar HUD
    glPushMatrix();
    {
        glTranslatef(viewX, viewY, 0.0f);      // Pan
        glRotatef(viewRotation, 0.0f, 0.0f, 1.0f);  // Rotación eje Z
        glScalef(viewZoom, viewZoom, 1.0f);    // Zoom

        // Dibujar geometría de la capa activa
        Renderer::drawLayer(layerManager.getActiveLayer());
    }
    glPopMatrix();  // Restaurar matriz - HUD no será transformado

    // ========================================
    // BLOQUE 2: HUD ESTÁTICO (sin transformaciones)
    // ========================================
    glLoadIdentity();
    glTranslatef(-200.0f, 0.0f, 0.0f);  // Mover HUD a izquierda
    Renderer::drawLayerLabel(layerManager.getActiveLayer());

    // Intercambiar buffers (double buffering)
    glutSwapBuffers();
}

// ============================================================
// RESHAPE - Callback de redimensión de ventana
// ============================================================
void reshape(int w, int h) {
    if (h == 0) h = 1;

    glViewport(0, 0, w, h);

    glMatrixMode(GL_PROJECTION);
    glLoadIdentity();

    // Proyección ortogonal 2D ampliada para permitir pan sin límites
    gluOrtho2D(-400.0, 400.0, -300.0, 300.0);

    glMatrixMode(GL_MODELVIEW);
}

// ============================================================
// TIMER - Callback para actualización a ~60 FPS
// ============================================================
void timer(int value)
{
    glutPostRedisplay();
    glutTimerFunc(16, timer, 0);  // 16ms ≈ 60 FPS
}

// ============================================================
// KEYBOARD - Manejo de eventos de teclado
// ============================================================
void keyboard(unsigned char key, int x, int y) {

    // ========================================
    // CAPAS: Teclas 1-5
    // ========================================
    if (key >= '1' && key <= '5') {
        layerManager.setActiveLayer(key - '0');
        glutPostRedisplay();
        return;
    }

    // ========================================
    // NAVEGACIÓN: W/A/S/D para PAN
    // ========================================
    if (key == 'w' || key == 'W') {
        viewY += 20.0f;  // Mover arriba
        glutPostRedisplay();
        return;
    }
    if (key == 's' || key == 'S') {
        viewY -= 20.0f;  // Mover abajo
        glutPostRedisplay();
        return;
    }
    if (key == 'a' || key == 'A') {
        viewX -= 20.0f;  // Mover izquierda
        glutPostRedisplay();
        return;
    }
    if (key == 'd' || key == 'D') {
        viewX += 20.0f;  // Mover derecha
        glutPostRedisplay();
        return;
    }

    // ========================================
    // ZOOM: Q para aumentar, E para disminuir
    // ========================================
    if (key == 'q' || key == 'Q') {
        viewZoom *= 1.1f;  // Zoom in (+10%)
        std::cout << "Zoom: " << viewZoom << "x\n";
        glutPostRedisplay();
        return;
    }
    if (key == 'e' || key == 'E') {
        viewZoom /= 1.1f;  // Zoom out (-10%)
        if (viewZoom < 0.1f) viewZoom = 0.1f;
        std::cout << "Zoom: " << viewZoom << "x\n";
        glutPostRedisplay();
        return;
    }

    // ========================================
    // ROTACIÓN: R para rotar izq, T para rotar der
    // ========================================
    if (key == 'r' || key == 'R') {
        viewRotation -= 15.0f;  // Girar 15° CCW
        if (viewRotation < 0.0f) viewRotation += 360.0f;
        std::cout << "Rotación: " << viewRotation << "°\n";
        glutPostRedisplay();
        return;
    }
    if (key == 't' || key == 'T') {
        viewRotation += 15.0f;  // Girar 15° CW
        if (viewRotation >= 360.0f) viewRotation -= 360.0f;
        std::cout << "Rotación: " << viewRotation << "°\n";
        glutPostRedisplay();
        return;
    }

    // ========================================
    // RESET: ESPACIO para volver a posición inicial
    // ========================================
    if (key == ' ') {
        viewX = 0.0f;
        viewY = 0.0f;
        viewZoom = 1.0f;
        viewRotation = 0.0f;
        std::cout << "Vista reseteada\n";
        glutPostRedisplay();
        return;
    }

    // ========================================
    // AYUDA: H para mostrar controles
    // ========================================
    if (key == 'h' || key == 'H') {
        printHelp();
        return;
    }

    // ========================================
    // SALIDA: ESC para cerrar
    // ========================================
    if (key == 27) {
        std::cout << "Cerrando aplicación...\n";
        exit(0);
    }
}

// ============================================================
// PRINTHELP - Mostrar controles disponibles
// ============================================================
void printHelp() {
    std::cout << "\n"
        << "╔═══════════════════════════════════════════════╗\n"
        << "║       CONTROLES - Boeing 737 Visualizer      ║\n"
        << "╠═══════════════════════════════════════════════╣\n"
        << "║ CAPAS:                                        ║\n"
        << "║   1-5        Cambiar entre capas              ║\n"
        << "║                                               ║\n"
        << "║ NAVEGACIÓN (PAN):                             ║\n"
        << "║   W          Mover arriba                     ║\n"
        << "║   S          Mover abajo                      ║\n"
        << "║   A          Mover izquierda                  ║\n"
        << "║   D          Mover derecha                    ║\n"
        << "║                                               ║\n"
        << "║ ZOOM:                                         ║\n"
        << "║   Q          Zoom in (+10%)                   ║\n"
        << "║   E          Zoom out (-10%)                  ║\n"
        << "║                                               ║\n"
        << "║ ROTACIÓN (eje Z):                             ║\n"
        << "║   R          Rotar izquierda (-15°)           ║\n"
        << "║   T          Rotar derecha (+15°)             ║\n"
        << "║                                               ║\n"
        << "║ OTROS:                                        ║\n"
        << "║   ESPACIO    Reset vista                      ║\n"
        << "║   H          Mostrar esta ayuda               ║\n"
        << "║   ESC        Salir                            ║\n"
        << "╚═══════════════════════════════════════════════╝\n"
        << "\n";
}

// ============================================================
// MAIN - Punto de entrada
// ============================================================
int main(int argc, char** argv) {
    std::cout << "\n"
        << "╔════════════════════════════════════════════════╗\n"
        << "║   Boeing 737 Visualizer v3.0                  ║\n"
        << "║   Con Push/Pop Matrix y navegación            ║\n"
        << "║   Presiona H para ver controles               ║\n"
        << "╚════════════════════════════════════════════════╝\n"
        << "\n";

    // Inicializar GLUT
    glutInit(&argc, argv);
    glutInitDisplayMode(GLUT_DOUBLE | GLUT_RGB | GLUT_DEPTH);
    glutInitWindowSize(800, 600);
    glutInitWindowPosition(100, 100);
    glutCreateWindow("Boeing 737 Visualizer - Con Push/Pop Matrix");

    // Inicializar OpenGL
    Renderer::setupOpenGL();

    // Inicializar LayerManager
    layerManager.init();

    // Registrar callbacks
    glutDisplayFunc(display);
    glutReshapeFunc(reshape);
    glutKeyboardFunc(keyboard);
    glutTimerFunc(16, timer, 0);

    // Mostrar ayuda inicial
    printHelp();

    // Loop principal
    glutMainLoop();

    return 0;
}

/*
========================================
RESUMEN DE CAMBIOS IMPLEMENTADOS
========================================

✅ PUSH/POP MATRIX:
   - glPushMatrix() antes de transformaciones
   - glPopMatrix() después
   - Permite aislamiento de transformaciones

✅ TRANSFORMACIONES:
   - glTranslatef(viewX, viewY, 0) para PAN
   - glRotatef(viewRotation, 0, 0, 1) para rotación eje Z
   - glScalef(viewZoom, viewZoom, 1) para ZOOM

✅ NAVEGACIÓN INTERACTIVA:
   - W/A/S/D: mover modelo
   - Q/E: zoom in/out
   - R/T: rotar
   - ESPACIO: reset vista

✅ LAYERMANAGER INTEGRADO:
   - layerManager.setActiveLayer() en keyboard
   - layerManager.getActiveLayer() en display
   - layerManager.init() en main

✅ HUD ESTÁTICO:
   - Fuera de Push/Pop
   - glTranslatef(-200, 0, 0) para posición
   - No se transforma con modelo
*/