// ============================================================
// ARCHIVO: main.cpp - VERSIÓN MEJORADA
// RESPONSABLE: Luis (Aplicación)
// PROYECTO: Boeing 737 Visualizer - Julio 2026
// DESCRIPCION: Versión con Push/Pop Matrix, transformaciones,
//              y navegación interactiva (pan, zoom, rotación).
//              Permite ver el modelo desde diferentes puntos.
// ============================================================

#include <GL/glut.h>
#include <iostream>
#include <cmath>
#include "renderer.h"

// Declaración adelantada de función
void printHelp();

// ============================================================
// VARIABLES GLOBALES - Estado de la vista
// ============================================================

// Capa activa (1-5)
int currentLayer = 1;

// Transformación de vista: posición del modelo en pantalla
float viewX = 0.0f;      // Traslación X (pan horizontal)
float viewY = 0.0f;      // Traslación Y (pan vertical)
float viewZoom = 1.0f;   // Escala (zoom in/out)
float viewRotation = 0.0f; // Rotación en eje Z (grados)

// Variables de control de cámara con teclado
bool keyStates[256] = { false };  // Almacenar estado de todas las teclas

// ============================================================
// DISPLAY - Callback de dibujado con transformaciones
// ============================================================
void display() {
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

    glMatrixMode(GL_MODELVIEW);
    glLoadIdentity();

    // AVIÓN con transformaciones
    glPushMatrix();
    {
        glTranslatef(viewX, viewY, 0.0f);
        glRotatef(viewRotation, 0.0f, 0.0f, 1.0f);
        glScalef(viewZoom, viewZoom, 1.0f);

        Renderer::drawLayer(currentLayer);
    }
    glPopMatrix();

    // HUD ESTÁTICO (sin transformaciones)
    glLoadIdentity();  // Limpiar matriz nuevamente
    Renderer::drawLayerLabel(currentLayer);

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

    // Proyección ortogonal 2D
    // Ampliado para poder hacer pan sin salir del viewport
    gluOrtho2D(-400.0, 400.0, -300.0, 300.0);

    glMatrixMode(GL_MODELVIEW);
}

// ============================================================
// KEYBOARD - Manejo de eventos de teclado
// ============================================================
void keyboard(unsigned char key, int x, int y) {

    // ========================================
    // CAPAS: Teclas 1-5
    // ========================================
    if (key >= '1' && key <= '5') {
        currentLayer = key - '0';
        std::cout << "Capa " << currentLayer << " activada\n";
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
        std::cout << "Zoom: " << viewZoom << "\n";
        glutPostRedisplay();
        return;
    }
    if (key == 'e' || key == 'E') {
        viewZoom /= 1.1f;  // Zoom out (-10%)
        if (viewZoom < 0.1f) viewZoom = 0.1f;  // Mínimo
        std::cout << "Zoom: " << viewZoom << "\n";
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
        << "╔════════════════════════════════════════╗\n"
        << "║       CONTROLES - Boeing 737           ║\n"
        << "╠════════════════════════════════════════╣\n"
        << "║ CAPAS:                                 ║\n"
        << "║   1-5        Cambiar capas             ║\n"
        << "║                                        ║\n"
        << "║ NAVEGACIÓN (PAN):                      ║\n"
        << "║   W/A/S/D    Mover arriba/izq/abajo/der║\n"
        << "║                                        ║\n"
        << "║ ZOOM:                                  ║\n"
        << "║   Q          Zoom in (+10%)            ║\n"
        << "║   E          Zoom out (-10%)           ║\n"
        << "║                                        ║\n"
        << "║ ROTACIÓN (eje Z):                      ║\n"
        << "║   R          Rotar izquierda (-15°)    ║\n"
        << "║   T          Rotar derecha (+15°)      ║\n"
        << "║                                        ║\n"
        << "║ OTROS:                                 ║\n"
        << "║   ESPACIO    Reset vista               ║\n"
        << "║   H          Mostrar esta ayuda        ║\n"
        << "║   ESC        Salir                     ║\n"
        << "╚════════════════════════════════════════╝\n"
        << "\n";
}

// ============================================================
// MAIN - Punto de entrada
// ============================================================
int main(int argc, char** argv) {
    std::cout << "\n"
        << "╔═══════════════════════════════════════════╗\n"
        << "║  Boeing 737 Visualizer v2.0              ║\n"
        << "║  Con navegación interactiva (Push/Pop)   ║\n"
        << "║  Presiona H para ver controles           ║\n"
        << "╚═══════════════════════════════════════════╝\n"
        << "\n";

    // Inicializar GLUT
    glutInit(&argc, argv);
    glutInitDisplayMode(GLUT_DOUBLE | GLUT_RGB | GLUT_DEPTH);
    glutInitWindowSize(800, 600);
    glutInitWindowPosition(100, 100);
    glutCreateWindow("Boeing 737 Visualizer - Navegación Interactiva");

    // Setup OpenGL
    Renderer::setupOpenGL();

    // Registrar callbacks
    glutDisplayFunc(display);
    glutReshapeFunc(reshape);
    glutKeyboardFunc(keyboard);

    // Mostrar ayuda inicial
    printHelp();

    // Loop principal
    glutMainLoop();

    return 0;
}

/*
========================================
EXPLICACIÓN: PUSH/POP Y TRANSFORMACIONES
========================================

MATRIZ DE TRANSFORMACIÓN:
┌─────────────────────────────────────┐
│ glLoadIdentity()                    │ Resetear matriz
├─────────────────────────────────────┤
│ glTranslatef(x, y, z)               │ Mover objeto
│ glRotatef(angle, rx, ry, rz)        │ Rotar objeto
│ glScalef(sx, sy, sz)                │ Escalar objeto
└─────────────────────────────────────┘

ORDEN IMPORTA:
1. Translate primero → posiciona
2. Rotate después → gira alrededor del nuevo origen
3. Scale último → escala en la posición final

PUSH/POP:
glPushMatrix()   → Guardar estado actual de matriz
 [dibujar]
glPopMatrix()    → Restaurar matriz anterior

EJEMPLO:
glPushMatrix();
{
    glTranslatef(100, 0, 0);    // Mover 100 a derecha
    glRotatef(45, 0, 0, 1);     // Rotar 45° eje Z
    drawComponent();             // Dibujar componente
}
glPopMatrix();                   // Volver a matriz original

VENTAJA:
- Sin push/pop → transformaciones se acumulan
- Con push/pop → cada bloque es independiente
- Código limpio y predecible


NAVEGACIÓN EN 2D ORTHO:
============================

PAN (Translate):
- W/A/S/D mueve la vista
- glTranslatef(viewX, viewY, 0)

ZOOM (Scale):
- Q/E escala todo el modelo
- glScalef(viewZoom, viewZoom, 1)

ROTACIÓN (Rotate):
- R/T gira el modelo alrededor de Z
- glRotatef(viewRotation, 0, 0, 1)

RESET (Space):
- Vuelve a posición inicial (0, 0, zoom=1, rotación=0)
*/