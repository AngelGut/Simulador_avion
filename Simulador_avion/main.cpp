// ============================================================
// ARCHIVO: main.cpp
// RESPONSABLE: Luis (Aplicación) + Cambios 3D con Modelos
// DESCRIPCION: Punto de entrada. Proyección 3D con carga de
//              modelos usando Assimp. Rotación en 3 ejes.
// ============================================================

#include <GL/glut.h>
#include <iostream>
#include <cmath>
#include "renderer.h"
#include "layer_manager.h"
#include "config.h"

// Declaración adelantada
void printHelp();

// ============================================================
// VARIABLES GLOBALES - Estado de aplicación 3D
// ============================================================

LayerManager layerManager;

// Transformación de vista: navegación interactiva 3D
float viewRotationX = 0.0f;  // Rotación eje X (pitch)
float viewRotationY = 0.0f;  // Rotación eje Y (yaw)
float viewRotationZ = 0.0f;  // Rotación eje Z (roll)
float viewZoom = -5.0f;      // Distancia de cámara en Z
float viewX = 0.0f;          // Pan horizontal
float viewY = 0.0f;          // Pan vertical

bool showHelp = false;

// ============================================================
// DISPLAY - Callback de dibujado 3D
// ============================================================
void display() {
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

    glMatrixMode(GL_MODELVIEW);
    glLoadIdentity();

    // Posición de cámara y vista
    gluLookAt(viewX, viewY, viewZoom,  // Posición cámara
        0.0f, 0.0f, 0.0f,        // Punto de mira
        0.0f, 1.0f, 0.0f);       // Vector "arriba"

    // Aplicar rotaciones 3D
    glRotatef(viewRotationX, 1.0f, 0.0f, 0.0f);  // Pitch
    glRotatef(viewRotationY, 0.0f, 1.0f, 0.0f);  // Yaw
    glRotatef(viewRotationZ, 0.0f, 0.0f, 1.0f);  // Roll

    // Dibujar avión
    Renderer::drawLayer(1);  // Solo capa exterior para prueba

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

    // Proyección 3D con perspectiva
    gluPerspective(45.0f, (float)w / (float)h, 0.1f, 500.0f);

    glMatrixMode(GL_MODELVIEW);
}

// ============================================================
// TIMER - Callback para actualización a ~60 FPS
// ============================================================
void timer(int value) {
    glutPostRedisplay();
    glutTimerFunc(16, timer, 0);  // 16ms ≈ 60 FPS
}

// ============================================================
// KEYBOARD - Manejo de eventos de teclado 3D
// ============================================================
void keyboard(unsigned char key, int x, int y) {

    // ROTACIÓN PITCH (Arriba/Abajo)
    if (key == 'i' || key == 'I') {
        viewRotationX += 10.0f;
        glutPostRedisplay();
        return;
    }
    if (key == 'k' || key == 'K') {
        viewRotationX -= 10.0f;
        glutPostRedisplay();
        return;
    }

    // ROTACIÓN YAW (Izquierda/Derecha)
    if (key == 'j' || key == 'J') {
        viewRotationY -= 10.0f;
        glutPostRedisplay();
        return;
    }
    if (key == 'l' || key == 'L') {
        viewRotationY += 10.0f;
        glutPostRedisplay();
        return;
    }

    // ROTACIÓN ROLL (Rotación alrededor eje Z)
    if (key == 'r' || key == 'R') {
        viewRotationZ -= 10.0f;
        glutPostRedisplay();
        return;
    }
    if (key == 't' || key == 'T') {
        viewRotationZ += 10.0f;
        glutPostRedisplay();
        return;
    }

    // ZOOM (Acercar/Alejar)
    if (key == 'q' || key == 'Q') {
        viewZoom += 0.5f;  // Alejar
        std::cout << "Zoom: " << -viewZoom << "\n";
        glutPostRedisplay();
        return;
    }
    if (key == 'e' || key == 'E') {
        viewZoom -= 0.5f;  // Acercar
        if (viewZoom > -0.5f) viewZoom = -0.5f;
        std::cout << "Zoom: " << -viewZoom << "\n";
        glutPostRedisplay();
        return;
    }

    // PAN (Mover arriba/abajo/izquierda/derecha)
    if (key == 'w' || key == 'W') {
        viewY += 0.5f;
        glutPostRedisplay();
        return;
    }
    if (key == 's' || key == 'S') {
        viewY -= 0.5f;
        glutPostRedisplay();
        return;
    }
    if (key == 'a' || key == 'A') {
        viewX -= 0.5f;
        glutPostRedisplay();
        return;
    }
    if (key == 'd' || key == 'D') {
        viewX += 0.5f;
        glutPostRedisplay();
        return;
    }

    // RESET
    if (key == ' ') {
        viewRotationX = 0.0f;
        viewRotationY = 0.0f;
        viewRotationZ = 0.0f;
        viewZoom = -5.0f;
        viewX = 0.0f;
        viewY = 0.0f;
        std::cout << "Vista reseteada\n";
        glutPostRedisplay();
        return;
    }

    // AYUDA
    if (key == 'h' || key == 'H') {
        showHelp = !showHelp;
        printHelp();
        glutPostRedisplay();
        return;
    }

    // SALIDA
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
        << "    Boeing 737 Visualizer 3D v1.0\n"
        << "==================================================\n"
        << " ROTACIÓN (Pitch/Yaw/Roll):\n"
        << "   I / K      Rotar arriba / abajo (Pitch)\n"
        << "   J / L      Rotar izquierda / derecha (Yaw)\n"
        << "   R / T      Rotar CW / CCW (Roll)\n"
        << "\n"
        << " ZOOM (Cámara):\n"
        << "   Q / E      Alejar / Acercar\n"
        << "\n"
        << " PAN (Mover vista):\n"
        << "   W / A / S / D    Arriba / Izq / Abajo / Der\n"
        << "\n"
        << " OTROS:\n"
        << "   ESPACIO    Reset vista\n"
        << "   H          Mostrar/ocultar esta ayuda\n"
        << "   ESC        Salir\n"
        << "==================================================\n"
        << "\n";
}

// ============================================================
// MAIN - Punto de entrada
// ============================================================
int main(int argc, char** argv) {
    std::cout << "\n"
        << "====================================================\n"
        << "   Boeing 737 Visualizer 3D v2.0\n"
        << "   Con carga de modelos 3D (Assimp)\n"
        << "   Presiona H para ver controles\n"
        << "====================================================\n"
        << "\n";

    // Inicializar GLUT
    glutInit(&argc, argv);
    glutInitDisplayMode(GLUT_DOUBLE | GLUT_RGB | GLUT_DEPTH);
    glutInitWindowSize(1024, 768);
    glutInitWindowPosition(100, 100);
    glutCreateWindow("Boeing 737 Visualizer 3D - Con Modelos");

    // Inicializar OpenGL
    Renderer::setupOpenGL();

    // Inicializar LayerManager
    layerManager.init();

    // ============================================================
    // CARGAR MODELO 3D
    // ============================================================
    std::cout << "\n--- Iniciando carga de modelos ---\n";
    Renderer::initModel(MODEL_PATH);
    std::cout << "--- Carga completada ---\n\n";

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