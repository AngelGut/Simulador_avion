// ============================================================
// ARCHIVO: main_with_models_example.cpp
// PROPÓSITO: Ejemplo de integración de carga automática de modelos
//
// Este archivo muestra cómo integrar ModelManagerAuto en main.cpp
// Para usar: reemplaza el código de inicialización en main.cpp
// ============================================================

#include <GL/glut.h>
#include <iostream>
#include <cmath>
#include "renderer.h"
#include "layer_manager.h"
#include "model_manager_auto.h"  // ← NUEVO

// Declaración adelantada
void printHelp();
void drawHelpOverlay();

// ============================================================
// VARIABLES GLOBALES
// ============================================================

LayerManager layerManager;
ModelManagerAuto& modelMgr = ModelManagerAuto::getInstance();  // ← NUEVO

// Estado de vista
float viewX = 0.0f;
float viewY = 0.0f;
float viewZoom = 1.0f;
float viewRotation = 0.0f;

bool showHelp = false;
bool showModels = true;  // ← NUEVO: Toggle para mostrar modelos

// ============================================================
// DISPLAY - Con renderizado de modelos
// ============================================================
void display() {
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

    glMatrixMode(GL_MODELVIEW);
    glLoadIdentity();

    // ========================================
    // BLOQUE 1: CAPA ACTIVA (con transformaciones)
    // ========================================
    glPushMatrix();
    {
        glTranslatef(viewX, viewY, 0.0f);
        glRotatef(viewRotation, 0.0f, 0.0f, 1.0f);
        glScalef(viewZoom, viewZoom, 1.0f);

        // Dibujar capa
        Renderer::drawLayer(layerManager.getActiveLayer());

        // ← NUEVO: Dibujar modelos cargados
        if (showModels) {
            auto models = modelMgr.getAvailableModels();
            for (size_t i = 0; i < models.size(); ++i) {
                Model* model = modelMgr.getModel(models[i]);
                if (model && !model->isEmpty()) {
                    // Dibujar modelo (requiere implementar Renderer::drawModel)
                    // Renderer::drawModel(*model);

                    // Por ahora, solo mostrar info en consola
                    if (i == 0) {
                        std::cout << "Renderizando: " << models[i] << std::endl;
                    }
                }
            }
        }
    }
    glPopMatrix();

    // ========================================
    // BLOQUE 2: HUD ESTÁTICO
    // ========================================
    glLoadIdentity();
    glTranslatef(-200.0f, 0.0f, 0.0f);
    Renderer::drawLayerLabel(layerManager.getActiveLayer());

    // ← NUEVO: Mostrar información de modelos cargados
    if (showModels) {
        glLoadIdentity();
        glTranslatef(-150.0f, 250.0f, 0.0f);
        glColor3f(0.2f, 0.8f, 0.2f);

        auto models = modelMgr.getAvailableModels();
        std::string modelInfo = "Modelos: " + std::to_string(models.size());

        glRasterPos2f(0.0f, 0.0f);
        for (char c : modelInfo) {
            glutBitmapCharacter(GLUT_BITMAP_HELVETICA_12, c);
        }
    }

    // ========================================
    // BLOQUE 3: PANEL DE AYUDA
    // ========================================
    glLoadIdentity();
    if (showHelp) {
        drawHelpOverlay();
    }

    glutSwapBuffers();
}

// ============================================================
// RESHAPE
// ============================================================
void reshape(int w, int h) {
    if (h == 0) h = 1;
    glViewport(0, 0, w, h);
    glMatrixMode(GL_PROJECTION);
    glLoadIdentity();
    gluOrtho2D(-400.0, 400.0, -300.0, 300.0);
    glMatrixMode(GL_MODELVIEW);
}

// ============================================================
// TIMER
// ============================================================
void timer(int value) {
    glutPostRedisplay();
    glutTimerFunc(16, timer, 0);
}

// ============================================================
// KEYBOARD - CON NUEVAS TECLAS
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
    // NAVEGACIÓN: W/A/S/D
    // ========================================
    if (key == 'w' || key == 'W') {
        viewY += 20.0f;
        glutPostRedisplay();
        return;
    }
    if (key == 's' || key == 'S') {
        viewY -= 20.0f;
        glutPostRedisplay();
        return;
    }
    if (key == 'a' || key == 'A') {
        viewX -= 20.0f;
        glutPostRedisplay();
        return;
    }
    if (key == 'd' || key == 'D') {
        viewX += 20.0f;
        glutPostRedisplay();
        return;
    }

    // ========================================
    // ZOOM: Q/E
    // ========================================
    if (key == 'q' || key == 'Q') {
        viewZoom *= 1.1f;
        std::cout << "Zoom: " << viewZoom << "x\n";
        glutPostRedisplay();
        return;
    }
    if (key == 'e' || key == 'E') {
        viewZoom /= 1.1f;
        if (viewZoom < 0.1f) viewZoom = 0.1f;
        std::cout << "Zoom: " << viewZoom << "x\n";
        glutPostRedisplay();
        return;
    }

    // ========================================
    // ROTACIÓN: R/T
    // ========================================
    if (key == 'r' || key == 'R') {
        viewRotation -= 15.0f;
        if (viewRotation < 0.0f) viewRotation += 360.0f;
        std::cout << "Rotación: " << viewRotation << "°\n";
        glutPostRedisplay();
        return;
    }
    if (key == 't' || key == 'T') {
        viewRotation += 15.0f;
        if (viewRotation >= 360.0f) viewRotation -= 360.0f;
        std::cout << "Rotación: " << viewRotation << "°\n";
        glutPostRedisplay();
        return;
    }

    // ========================================
    // RESET: ESPACIO
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
    // ← NUEVO: M para toggle de modelos
    // ========================================
    if (key == 'm' || key == 'M') {
        showModels = !showModels;
        std::cout << (showModels ? "✅ Modelos visibles\n" : "❌ Modelos ocultos\n");
        modelMgr.printModelsInfo();  // Mostrar info
        glutPostRedisplay();
        return;
    }

    // ========================================
    // AYUDA: H
    // ========================================
    if (key == 'h' || key == 'H') {
        showHelp = !showHelp;
        printHelp();
        glutPostRedisplay();
        return;
    }

    // ========================================
    // SALIDA: ESC
    // ========================================
    if (key == 27) {
        std::cout << "Cerrando aplicación...\n";
        exit(0);
    }
}

// ============================================================
// PRINTHELP - ACTUALIZADO
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
        << " MODELOS 3D:\n"
        << "   M          Toggle mostrar/ocultar modelos\n"  // ← NUEVO
        << "\n"
        << " OTROS:\n"
        << "   ESPACIO    Reset vista\n"
        << "   H          Mostrar/ocultar esta ayuda\n"
        << "   ESC        Salir\n"
        << "==================================================\n"
        << "\n";
}

// ============================================================
// DRAWHELPOVERLAY
// ============================================================
void drawHelpOverlay() {
    const char* lines[] = {
        "CONTROLES",
        "1-5  Cambiar capa",
        "W A S D  Mover vista",
        "Q / E  Zoom +/-",
        "R / T  Rotar izq/der",
        "M  Mostrar/ocultar modelos",  // ← NUEVO
        "ESPACIO  Reset vista",
        "H  Cerrar ayuda",
        "ESC  Salir"
    };
    const int numLines = 9;  // ← Actualizado

    float boxRight = 390.0f, boxLeft = 200.0f;
    float boxTop = -170.0f, boxBottom = boxTop - (numLines * 14.0f) - 12.0f;

    glColor3f(0.08f, 0.08f, 0.08f);
    glBegin(GL_QUADS);
    glVertex3f(boxLeft, boxBottom, 0.0f);
    glVertex3f(boxRight, boxBottom, 0.0f);
    glVertex3f(boxRight, boxTop, 0.0f);
    glVertex3f(boxLeft, boxTop, 0.0f);
    glEnd();

    glColor3f(0.45f, 0.45f, 0.45f);
    glLineWidth(1.0f);
    glBegin(GL_LINE_LOOP);
    glVertex3f(boxLeft, boxBottom, 0.0f);
    glVertex3f(boxRight, boxBottom, 0.0f);
    glVertex3f(boxRight, boxTop, 0.0f);
    glVertex3f(boxLeft, boxTop, 0.0f);
    glEnd();

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
// MAIN - CON CARGA AUTOMÁTICA DE MODELOS
// ============================================================
int main(int argc, char** argv) {
    std::cout << "\n"
        << "====================================================\n"
        << "   Boeing 737 Visualizer v4.0\n"
        << "   Con Push/Pop Matrix, navegacion y modelos 3D\n"
        << "   Presiona H para ver controles\n"
        << "====================================================\n"
        << "\n";

    // Inicializar GLUT
    glutInit(&argc, argv);
    glutInitDisplayMode(GLUT_DOUBLE | GLUT_RGB | GLUT_DEPTH);
    glutInitWindowSize(800, 600);
    glutInitWindowPosition(100, 100);
    glutCreateWindow("Boeing 737 Visualizer - Con Modelos 3D");

    // Inicializar OpenGL
    Renderer::setupOpenGL();

    // Inicializar LayerManager
    layerManager.init();

    // ← NUEVO: Cargar automáticamente todos los modelos de ./models/
    std::cout << "\n[INICIO] Cargando modelos 3D...\n";
    int loadedModels = modelMgr.loadAllModelsFromFolder("./models/");

    // Mostrar información de modelos cargados
    modelMgr.printModelsInfo();

    if (loadedModels > 0) {
        std::cout << "✅ " << loadedModels << " modelo(s) listo(s) para renderizar\n\n";
    } else {
        std::cout << "ℹ️  Sin modelos. Copia archivos .obj o .fbx a ./models/\n\n";
    }

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
