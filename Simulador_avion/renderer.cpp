#include "renderer.h"
#include "geometry.h"
#include <GL/glut.h>

namespace Renderer {

    // --------------------------------------------------------
    // setupOpenGL()
    // Inicializacion general antes de glutMainLoop(). Configura
    // color de fondo, antialiasing (GL_LINE_SMOOTH), Z-Buffer y Culling.
    // --------------------------------------------------------
    void setupOpenGL() {
        // 1. Color de fondo del entorno (Verde pasto oscuro para los laterales de la pista)
        glClearColor(0.1f, 0.35f, 0.1f, 1.0f);

        // 2. Antialiasing: suaviza las lineas dibujadas con GL_LINES,
        // GL_LINE_LOOP y GL_LINE_STRIP (circulos, largueros, Bezier).
        glEnable(GL_LINE_SMOOTH);
        glHint(GL_LINE_SMOOTH_HINT, GL_NICEST);
        glEnable(GL_BLEND);
        glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);

        glLineWidth(1.0f);

        // 3. Conceptos de Clase: Z-Buffer para control de profundidad correcta
        glEnable(GL_DEPTH_TEST);
        glDepthFunc(GL_LESS);

        // 4. Conceptos de Clase: Optimización mediante Back-Face Culling
        glEnable(GL_CULL_FACE);
        glCullFace(GL_BACK);
        glFrontFace(GL_CCW);

        applyLighting();
    }

    // --------------------------------------------------------
    // applyLighting()
    // Luz ambiente + una luz direccional basica (GL_LIGHT0).
    // --------------------------------------------------------
    void applyLighting() {
        glEnable(GL_LIGHTING);
        glEnable(GL_LIGHT0);
        glShadeModel(GL_SMOOTH); // Gouraud shading visto en clase

        GLfloat ambientLight[] = { 0.3f, 0.3f, 0.3f, 1.0f };
        GLfloat diffuseLight[] = { 0.7f, 0.7f, 0.7f, 1.0f };
        GLfloat lightPosition[] = { 0.0f, 0.0f, 1.0f, 0.0f }; // Direccional (w=0)

        glLightfv(GL_LIGHT0, GL_AMBIENT, ambientLight);
        glLightfv(GL_LIGHT0, GL_DIFFUSE, diffuseLight);
        glLightfv(GL_LIGHT0, GL_POSITION, lightPosition);

        // Activamos la vinculacion del color material para que funcione glColor3f
        glEnable(GL_COLOR_MATERIAL);
        glColorMaterial(GL_FRONT_AND_BACK, GL_AMBIENT_AND_DIFFUSE);
    }

    // --------------------------------------------------------
    // applyColor()
    // Wrapper sobre glColor3f para centralizar el manejo de color.
    // --------------------------------------------------------
    void applyColor(float r, float g, float b) {
        glColor3f(r, g, b);
    }

    // --------------------------------------------------------
    // drawAirportRunway()
    // Sustituye a drawGrid(). Dibuja una pista de aterrizaje técnica
    // usando primitivas puras e iteraciones geométricas.
    // --------------------------------------------------------
    void drawAirportRunway() {
        // Deshabilitar iluminación momentáneamente para el escenario plano de fondo
        glDisable(GL_LIGHTING);

        // 1. Asfalto de la pista (Gris oscuro centrado)
        glColor3f(0.25f, 0.25f, 0.25f);
        glBegin(GL_QUADS);
        glVertex3f(-60.0f, -150.0f, -0.9f); // Usamos Z ligeramente menor (-0.9)
        glVertex3f(60.0f, -150.0f, -0.9f); // para que quede al fondo en el Z-Buffer
        glVertex3f(60.0f, 150.0f, -0.9f);
        glVertex3f(-60.0f, 150.0f, -0.9f);
        glEnd();

        // 2. Líneas laterales blancas (Bordes de pista)
        glColor3f(1.0f, 1.0f, 1.0f);
        glLineWidth(2.0f);
        glBegin(GL_LINES);
        glVertex3f(-60.0f, -150.0f, -0.8f); glVertex3f(-60.0f, 150.0f, -0.8f);
        glVertex3f(60.0f, -150.0f, -0.8f); glVertex3f(60.0f, 150.0f, -0.8f);
        glEnd();

        // 3. Línea discontinua central (Eje de pista)
        glColor3f(1.0f, 1.0f, 0.0f); // Amarillo técnico para visibilidad
        glLineWidth(1.5f);
        glBegin(GL_LINES);
        for (float y = -150.0f; y < 150.0f; y += 30.0f) {
            glVertex3f(0.0f, y, -0.8f);
            glVertex3f(0.0f, y + 15.0f, -0.8f);
        }
        glEnd();

        glLineWidth(1.0f); // Resetear grosor
        glEnable(GL_LIGHTING); // Reactivar iluminación para el avión
    }

    // --------------------------------------------------------
    // drawLayer()
    // Llama a los generadores de geometry.cpp segun la capa activa.
    // --------------------------------------------------------
    void drawLayer(int layerNumber) {
        // Cambiamos drawGrid() por nuestro nuevo fondo aeroportuario
        drawAirportRunway();

        switch (layerNumber) {
        case 1: // Exterior
            GeometryBuilder::generateFuselage(0.0f, 0.0f, 30.0f, 300.0f);
            GeometryBuilder::generateWings();
            GeometryBuilder::generateMotors();
            break;

        case 2: // Estructura interna
            GeometryBuilder::generateStructure();
            break;

        case 3: // Sistemas
            GeometryBuilder::generateSystems();
            break;

        case 4: // Cabina
            GeometryBuilder::generateCabin();
            break;

        case 5: // Propulsion
            GeometryBuilder::generateMotors();
            GeometryBuilder::generateLandingGear();
            break;

        default:
            break;
        }
    }

} // namespace Renderer