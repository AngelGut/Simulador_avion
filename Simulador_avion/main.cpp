
#include <GL/glut.h>
#include <iostream>
#include "renderer.h"

// Variable global para almacenar el identificador de la capa activa en la prueba
int currentLayer = 1;

// ------------------------------------------------------------
// display()
// Callback de dibujo ejecutado por el bucle principal de GLUT.
// Redibuja el escenario y la geometría del avión en cada frame.
// ------------------------------------------------------------
void display() {
    // CORRECCIÓN 1: Se limpia tanto el buffer de color como el de profundidad (Z-Buffer)
    // Esto evita que la pantalla se quede congelada en verde y permite pasar la prueba de visibilidad.
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

    // Reiniciar la matriz de modelado/vista para evitar la acumulación de transformaciones
    glMatrixMode(GL_MODELVIEW);
    glLoadIdentity();

    // Invocar al motor gráfico para renderizar el aeropuerto y la capa seleccionada
    Renderer::drawLayer(currentLayer);

    // Intercambiar buffers para lograr un refresco fluido sin parpadeos (Doble Buffer)
    glutSwapBuffers();
}

// ------------------------------------------------------------
// reshape()
// Callback encargado de ajustar el Viewport y establecer la proyección ortográfica.
// ------------------------------------------------------------
void reshape(int w, int h) {
    if (h == 0) h = 1;

    glViewport(0, 0, w, h);

    glMatrixMode(GL_PROJECTION);
    glLoadIdentity();

    // CORRECCIÓN: Ampliamos los rangos para encuadrar todo el avión y la pista
    // X va de -250 a 250 (Ancho total 500) | Y va de -200 a 200 (Alto total 400)
    gluOrtho2D(-250.0, 250.0, -200.0, 200.0);

    glMatrixMode(GL_MODELVIEW);
}

// ------------------------------------------------------------
// keyboard()
// Manejo de eventos del teclado para la conmutación interactiva de capas.
// ------------------------------------------------------------
void keyboard(unsigned char key, int x, int y) {
    // Control de selección de capas mediante las teclas 1 a 5
    if (key >= '1' && key <= '5') {
        currentLayer = key - '0'; // Conversión de char numérico a int de capa
        std::cout << "Capa " << currentLayer << " activada de forma interactiva.\n"; //
    }

    // Tecla ESC para cerrar de forma segura el visualizador
    if (key == 27) {
        exit(0);
    }

    // Forzar a GLUT a volver a ejecutar el display() para reflejar el cambio de capa
    glutPostRedisplay();
}

// ------------------------------------------------------------
// main()
// Punto de entrada de la aplicación de pruebas.
// ------------------------------------------------------------
int main(int argc, char** argv) {
    // Inicialización del entorno GLUT
    glutInit(&argc, argv);

    // CORRECCIÓN 2: Se añade el flag GLUT_DEPTH en el Display Mode
    // Sin este flag, Visual Studio abre la ventana sin soporte de hardware para profundidad.
    glutInitDisplayMode(GLUT_DOUBLE | GLUT_RGB | GLUT_DEPTH); //

    // Configurar dimensiones físicas iniciales de la ventana (800x600 píxeles)
    glutInitWindowSize(800, 600);
    glutCreateWindow("PRUEBA - Boeing 737 (temporal)");

    // Ejecutar inicialización central del pipeline gráfico (Culling, Antialiasing y Luces)
    Renderer::setupOpenGL();

    // Registro de las funciones de devolución de llamadas (Callbacks) de la aplicación
    glutDisplayFunc(display);
    glutReshapeFunc(reshape);
    glutKeyboardFunc(keyboard);

    // Ceder el control del flujo al bucle de renderizado perpetuo de GLUT
    glutMainLoop();
    return 0;
}