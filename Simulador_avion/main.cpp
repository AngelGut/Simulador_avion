// ============================================================
// ARCHIVO: main.cpp
// RESPONSABLE: Luis(dique, ese vago) (Aplicación) + Cambios de navegación
// DESCRIPCION: Punto de entrada. Integra LayerManager,
//              Push/Pop Matrix, transformaciones (Translate,
//              Rotate, Scale) y navegación interactiva.
// si algo sale mal es culpa solamente de luis amir 
// ============================================================

#include <GL/glut.h>
#include <iostream>
#include <cmath>
#include "renderer.h"
#include "layer_manager.h"

// Declaración adelantada
void printHelp();
void drawHelpOverlay();

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

bool showHelp = false;     // Controla si el panel de ayuda se dibuja

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

    // ========================================
    // BLOQUE 3: PANEL DE AYUDA (esquina inferior derecha)
    // ========================================
    glLoadIdentity();
    if (showHelp) {
        drawHelpOverlay();
    }

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
    // AYUDA: H para mostrar/ocultar controles
    // ========================================
    if (key == 'h' || key == 'H') {
        showHelp = !showHelp;
        printHelp();
        glutPostRedisplay();
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
        << "==================================================\n"
        << "       CONTROLES - Boeing 737 Visualizer\n"
        << "==================================================\n"
        << " CAPAS:\n"
        << "   1-5        Cambiar entre capas\n"
        << "\n"
        << " NAVEGACION (PAN):\n"
        << "   W/A/S/D    Mover arriba/izq/abajo/der\n"
        << "\n"
        << " ZOOM:\n"
        << "   Q / E      Zoom in / Zoom out\n"
        << "\n"
        << " ROTACION (eje Z):\n"
        << "   R / T      Rotar izquierda / derecha\n"
        << "\n"
        << " OTROS:\n"
        << "   ESPACIO    Reset vista\n"
        << "   H          Mostrar/ocultar esta ayuda\n"
        << "   ESC        Salir\n"
        << "==================================================\n"
        << "\n";
}

// ============================================================
// DRAWHELPOVERLAY - Dibuja el panel de controles en pantalla,
// ubicado en la esquina inferior derecha del viewport.
// ============================================================
void drawHelpOverlay() {
    const char* lines[] = {
        "CONTROLES",
        "1-5  Cambiar capa",
        "W A S D  Mover vista",
        "Q / E  Zoom +/-",
        "R / T  Rotar izq/der",
        "ESPACIO  Reset vista",
        "H  Cerrar ayuda",
        "ESC  Salir"
    };
    const int numLines = 8;

    float boxRight = 390.0f, boxLeft = 200.0f;
    float boxTop = -170.0f, boxBottom = boxTop - (numLines * 14.0f) - 12.0f;

    // Fondo semi-solido
    glColor3f(0.08f, 0.08f, 0.08f);
    glBegin(GL_QUADS);
    glVertex3f(boxLeft, boxBottom, 0.0f);
    glVertex3f(boxRight, boxBottom, 0.0f);
    glVertex3f(boxRight, boxTop, 0.0f);
    glVertex3f(boxLeft, boxTop, 0.0f);
    glEnd();

    // Borde
    glColor3f(0.45f, 0.45f, 0.45f);
    glLineWidth(1.0f);
    glBegin(GL_LINE_LOOP);
    glVertex3f(boxLeft, boxBottom, 0.0f);
    glVertex3f(boxRight, boxBottom, 0.0f);
    glVertex3f(boxRight, boxTop, 0.0f);
    glVertex3f(boxLeft, boxTop, 0.0f);
    glEnd();

    // Titulo (primera linea) en color de acento, resto en blanco
    for (int i = 0; i < numLines; i++) {
        float y = boxTop - 16.0f - (i * 14.0f);

        if (i == 0) {
            glColor3f(0.4f, 0.75f, 1.0f);
            glRasterPos2f(boxLeft + 10.0f, y);
            for (const char* c = lines[i]; *c != '\0'; c++) {
                glutBitmapCharacter(GLUT_BITMAP_HELVETICA_12, *c);
            }
        }
        else {
            glColor3f(0.9f, 0.9f, 0.9f);
            glRasterPos2f(boxLeft + 10.0f, y);
            for (const char* c = lines[i]; *c != '\0'; c++) {
                glutBitmapCharacter(GLUT_BITMAP_HELVETICA_10, *c);
            }
        }
    }
}

// ============================================================
// MAIN - Punto de entrada
// ============================================================
int main(int argc, char** argv) {
    std::cout << "\n"
        << "====================================================\n"
        << "   Boeing 737 Visualizer v3.0\n"
        << "   Con Push/Pop Matrix y navegacion\n"
        << "   Presiona H para ver controles\n"
        << "====================================================\n"
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

✅ PANEL DE AYUDA:
   - Se activa/desactiva con H
   - Dibujado en esquina inferior derecha, fuera del Push/Pop
  - No se transforma con modelo
  arregle una mierda de amir en el boton h,
*/