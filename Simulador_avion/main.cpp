// ============================================================
// ARCHIVO: main.cpp - 3D AIRPLANE VIEWER
// DESCRIPCION: Visor 3D interactivo con selección de modelos Assimp
// ============================================================

#include <GL/glut.h>
#include <iostream>
#include <cmath>
#include <string>
#include "model_loader.h"
#include "model_config.h"

// Variables globales
float viewX = 0.0f;
float viewY = 0.0f;
float viewZoom = 1.0f;
float viewRotation = 0.0f;
std::string loadedModelName;

// ============================================================
// SELECCIONAR MODELO - Menú de selección interactivo
// ============================================================
int seleccionarModelo() {
    std::cout << "\n========================================\n"
        << "3D Airplane Viewer\n"
        << "========================================\n\n";

    std::cout << "SELECCIONA UN MODELO:\n";

    // Mostrar solo los 4 modelos de aviones principales
    const int MAX_MODELS = 4;
    for (int i = 0; i < MAX_MODELS && i < ModelConfig::AVAILABLE_MODELS.size(); ++i) {
        const auto& modelInfo = ModelConfig::AVAILABLE_MODELS[i];
        std::cout << "  " << (i + 1) << " - " << modelInfo.description << "\n";
    }

    std::cout << "\nOpcion (1-" << MAX_MODELS << "): ";

    int choice;
    std::cin >> choice;

    if (choice < 1 || choice > MAX_MODELS) {
        std::cerr << "Opcion invalida. Usando modelo por defecto (Boeing 737).\n";
        return 0;  // Índice 0 = Boeing 737
    }

    return choice - 1;  // Convertir a índice (0-based)
}

// ============================================================
// RESHAPE - Configurar proyección
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
// DISPLAY - Renderizar modelo
// ============================================================
void display() {
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
    glMatrixMode(GL_MODELVIEW);
    glLoadIdentity();

    // Aplicar transformaciones
    glPushMatrix();
    {
        glTranslatef(viewX, viewY, 0.0f);
        glRotatef(viewRotation, 0.0f, 0.0f, 1.0f);
        glScalef(viewZoom, viewZoom, 1.0f);

        // Renderizar modelo seleccionado
        Model* model = ModelManager::getInstance().getModel(loadedModelName);
        if (model && !model->isEmpty()) {
            glColor3f(0.85f, 0.85f, 0.85f);

            glEnableClientState(GL_VERTEX_ARRAY);
            glVertexPointer(3, GL_FLOAT, 0, &model->vertices[0].x);
            glDrawElements(GL_TRIANGLES, model->indices.size(), GL_UNSIGNED_INT, &model->indices[0]);
            glDisableClientState(GL_VERTEX_ARRAY);
        }
        else {
            std::cerr << "Modelo " << loadedModelName << " no encontrado\n";
        }
    }
    glPopMatrix();

    glutSwapBuffers();
}

// ============================================================
// KEYBOARD - Controles simples
// ============================================================
void keyboard(unsigned char key, int x, int y) {
    switch (key) {
    case 'w': case 'W': viewY += 20.0f; break;
    case 's': case 'S': viewY -= 20.0f; break;
    case 'a': case 'A': viewX -= 20.0f; break;
    case 'd': case 'D': viewX += 20.0f; break;
    case 'q': case 'Q': viewZoom *= 1.1f; std::cout << "Zoom: " << viewZoom << "x\n"; break;
    case 'e': case 'E': viewZoom /= 1.1f; if (viewZoom < 0.1f) viewZoom = 0.1f; std::cout << "Zoom: " << viewZoom << "x\n"; break;
    case 'r': case 'R': viewRotation -= 15.0f; if (viewRotation < 0) viewRotation += 360; std::cout << "Rotacion: " << viewRotation << " deg\n"; break;
    case 't': case 'T': viewRotation += 15.0f; if (viewRotation >= 360) viewRotation -= 360; std::cout << "Rotacion: " << viewRotation << " deg\n"; break;
    case ' ': viewX = viewY = viewRotation = 0.0f; viewZoom = 1.0f; std::cout << "Reset\n"; break;
    case 27: exit(0); break;
    }
    glutPostRedisplay();
}

// ============================================================
// TIMER - Callback para actualización
// ============================================================
void timer(int value) {
    glutPostRedisplay();
    glutTimerFunc(16, timer, 0);
}

// ============================================================
// MAIN - Punto de entrada
// ============================================================
int main(int argc, char** argv) {
    // Solicitar selección de modelo
    int modelIndex = seleccionarModelo();

    glutInit(&argc, argv);
    glutInitDisplayMode(GLUT_DOUBLE | GLUT_RGB | GLUT_DEPTH);
    glutInitWindowSize(800, 600);
    glutInitWindowPosition(100, 100);

    const auto& selectedModelInfo = ModelConfig::AVAILABLE_MODELS[modelIndex];
    std::string windowTitle = "3D Viewer - " + selectedModelInfo.description;
    glutCreateWindow(windowTitle.c_str());

    glClearColor(0.05f, 0.05f, 0.05f, 1.0f);
    glEnable(GL_DEPTH_TEST);
    glEnable(GL_LINE_SMOOTH);

    glMatrixMode(GL_PROJECTION);
    glLoadIdentity();
    gluOrtho2D(-400.0, 400.0, -300.0, 300.0);
    glMatrixMode(GL_MODELVIEW);

    // Cargar modelo seleccionado
    std::cout << "Cargando " << selectedModelInfo.description << "...\n";
    ModelManager& mgr = ModelManager::getInstance();

    std::string fullPath = ModelConfig::getFullPath(selectedModelInfo.filename);
    loadedModelName = selectedModelInfo.name;

    if (mgr.loadModel(fullPath, loadedModelName)) {
        std::cout << "Exito!\n"
            << "  Vertices: " << mgr.getModelVertexCount(loadedModelName) << "\n"
            << "  Triangulos: " << mgr.getModelTriangleCount(loadedModelName) << "\n\n";
    }
    else {
        std::cerr << "Error cargando modelo\n";
        return 1;
    }

    std::cout << "Controles:\n"
        << "  W/A/S/D - Mover\n"
        << "  Q/E     - Zoom\n"
        << "  R/T     - Rotar\n"
        << "  SPACE   - Reset\n"
        << "  ESC     - Salir\n\n";

    glutDisplayFunc(display);
    glutReshapeFunc(reshape);
    glutKeyboardFunc(keyboard);
    glutTimerFunc(16, timer, 0);

    glutMainLoop();
    return 0;
}