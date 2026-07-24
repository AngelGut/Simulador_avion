// ============================================================
// ARCHIVO: main.cpp - VERSIÓN SIMPLIFICADA TEST
// DESCRIPCION: Test simple de carga y renderizado de modelos Assimp
// ============================================================

#include <GL/glut.h>
#include <iostream>
#include <cmath>
#include "model_loader.h"

// Variables globales
float viewX = 0.0f;
float viewY = 0.0f;
float viewZoom = 1.0f;
float viewRotation = 0.0f;

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

        // Renderizar modelo Boeing 737
        Model* model = ModelManager::getInstance().getModel("boeing737");
        if (model && !model->isEmpty()) {
            glColor3f(0.85f, 0.85f, 0.85f);

            glEnableClientState(GL_VERTEX_ARRAY);
            glVertexPointer(3, GL_FLOAT, 0, &model->vertices[0].x);
            glDrawElements(GL_TRIANGLES, model->indices.size(), GL_UNSIGNED_INT, &model->indices[0]);
            glDisableClientState(GL_VERTEX_ARRAY);
        } else {
            std::cerr << "Modelo boeing737 no encontrado\n";
        }
    }
    glPopMatrix();

    glutSwapBuffers();
}

// ============================================================
// KEYBOARD - Controles simples
// ============================================================
void keyboard(unsigned char key, int x, int y) {
    switch(key) {
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
    std::cout << "\n========================================\n"
              << "Boeing 737 - Test Assimp + GLM\n"
              << "========================================\n\n";

    glutInit(&argc, argv);
    glutInitDisplayMode(GLUT_DOUBLE | GLUT_RGB | GLUT_DEPTH);
    glutInitWindowSize(800, 600);
    glutInitWindowPosition(100, 100);
    glutCreateWindow("Boeing 737 - Assimp Test");

    glClearColor(0.05f, 0.05f, 0.05f, 1.0f);
    glEnable(GL_DEPTH_TEST);
    glEnable(GL_LINE_SMOOTH);

    glMatrixMode(GL_PROJECTION);
    glLoadIdentity();
    gluOrtho2D(-400.0, 400.0, -300.0, 300.0);
    glMatrixMode(GL_MODELVIEW);

    // Cargar Boeing 737
    std::cout << "Cargando Boeing 737...\n";
    ModelManager& mgr = ModelManager::getInstance();
    if (mgr.loadModel("./source/American Airlines Boeing 737-800.blend", "boeing737")) {
        std::cout << "Exito!\n"
                  << "  Vertices: " << mgr.getModelVertexCount("boeing737") << "\n"
                  << "  Triangulos: " << mgr.getModelTriangleCount("boeing737") << "\n\n";
    } else {
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
