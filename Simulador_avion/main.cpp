#include <iostream>
#include <GL/glut.h>
#include <cmath>

int currentLayer = 1;
float rotationAngle = 0.0f;

void display() {
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
    glClearColor(0.1f, 0.1f, 0.1f, 1.0f);

    glPushMatrix();
    glRotatef(rotationAngle, 0.0f, 0.0f, 1.0f);  // Rota sobre eje Z

    switch (currentLayer) {
    case 1:
        glColor3f(1.0f, 0.0f, 0.0f);
        glutWireTeapot(0.5);
        break;
    case 2:
        glColor3f(0.0f, 1.0f, 0.0f);
        glutWireTeapot(0.4);
        break;
    }

    glPopMatrix();
    glFlush();
    glutSwapBuffers();
}

void timer(int value) {
    rotationAngle += 2.0f;  // Incrementa 2 grados
    if (rotationAngle >= 360.0f) rotationAngle = 0.0f;
    glutPostRedisplay();
    glutTimerFunc(16, timer, 0);  // ~60 FPS
}

void keyboard(unsigned char key, int x, int y) {
    if (key == '1') currentLayer = 1;
    if (key == '2') currentLayer = 2;
    if (key == 27) exit(0);
    glutPostRedisplay();
}

void reshape(int w, int h) {
    glViewport(0, 0, w, h);
    glMatrixMode(GL_PROJECTION);
    glLoadIdentity();
    gluOrtho2D(-1, 1, -1, 1);
    glMatrixMode(GL_MODELVIEW);
}

int main(int argc, char** argv) {
    glutInit(&argc, argv);
    glutInitDisplayMode(GLUT_DOUBLE | GLUT_RGB | GLUT_DEPTH);
    glutInitWindowSize(800, 600);
    glutCreateWindow("Boeing 737 - Capas");

    glClearColor(0.1f, 0.1f, 0.1f, 1.0f);
    glMatrixMode(GL_PROJECTION);
    glLoadIdentity();
    gluOrtho2D(-1, 1, -1, 1);

    glutDisplayFunc(display);
    glutReshapeFunc(reshape);
    glutKeyboardFunc(keyboard);
    glutTimerFunc(16, timer, 0);  // Inicia el timer

    glutMainLoop();
    return 0;
}