#include "renderer.h"
#include "geometry.h"
#include <GL/glut.h>
#include <cstdio>

namespace Renderer {

    // --------------------------------------------------------
    // setupOpenGL()
    // Inicializacion general antes de glutMainLoop(). Configura
    // color de fondo, antialiasing (GL_LINE_SMOOTH).
    // --------------------------------------------------------
    void setupOpenGL() {
        glClearColor(0.05f, 0.05f, 0.05f, 1.0f); // Fondo casi negro

        glEnable(GL_LINE_SMOOTH);
        glHint(GL_LINE_SMOOTH_HINT, GL_NICEST);

        glLineWidth(1.0f);

        applyLighting();
    }

    // --------------------------------------------------------
    // applyLighting()
    // Luz ambiente + una luz direccional basica (GL_LIGHT0), pero
    // DESACTIVADA permanentemente. El proyecto es 2D con colores
    // planos (glColor3f); GL_LIGHTING calcularia reflejos sobre
    // normales que no representan superficies reales, causando
    // colores incorrectos. Se documenta la configuracion por si
    // se usa en una extension futura 3D, pero no se activa aqui.
    // --------------------------------------------------------
    void applyLighting() {
        glEnable(GL_LIGHT0);
        glShadeModel(GL_FLAT);

        GLfloat ambientLight[] = { 0.3f, 0.3f, 0.3f, 1.0f };
        GLfloat diffuseLight[] = { 0.7f, 0.7f, 0.7f, 1.0f };
        GLfloat lightPosition[] = { 0.0f, 0.0f, 1.0f, 0.0f };

        glLightfv(GL_LIGHT0, GL_AMBIENT, ambientLight);
        glLightfv(GL_LIGHT0, GL_DIFFUSE, diffuseLight);
        glLightfv(GL_LIGHT0, GL_POSITION, lightPosition);

        glDisable(GL_LIGHTING); // Nunca se activa durante el dibujo
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
    // Escenario de fondo: pista de aterrizaje con bordes y linea
    // central. Se dibuja antes de la geometria del avion.
    // --------------------------------------------------------
    void drawAirportRunway() {
        glDisable(GL_LIGHTING);

        // Asfalto de la pista
        glColor3f(0.25f, 0.25f, 0.25f);
        glBegin(GL_QUADS);
        glVertex3f(-60.0f, -150.0f, -0.9f);
        glVertex3f(60.0f, -150.0f, -0.9f);
        glVertex3f(60.0f, 150.0f, -0.9f);
        glVertex3f(-60.0f, 150.0f, -0.9f);
        glEnd();

        // Lineas laterales blancas
        glColor3f(1.0f, 1.0f, 1.0f);
        glLineWidth(2.0f);
        glBegin(GL_LINES);
        glVertex3f(-60.0f, -150.0f, -0.8f); glVertex3f(-60.0f, 150.0f, -0.8f);
        glVertex3f(60.0f, -150.0f, -0.8f);  glVertex3f(60.0f, 150.0f, -0.8f);
        glEnd();

        // Linea discontinua central
        glColor3f(1.0f, 1.0f, 0.0f);
        glLineWidth(1.5f);
        glBegin(GL_LINES);
        for (float y = -150.0f; y < 150.0f; y += 30.0f) {
            glVertex3f(0.0f, y, -0.8f);
            glVertex3f(0.0f, y + 15.0f, -0.8f);
        }
        glEnd();

        glLineWidth(1.0f);
    }

    // ------------------------------------------------------------
    // drawLayerLabel()
    // HUD tipo simulador: caja de fondo, indicador de color, titulo
    // y subtitulo de la capa activa, contador "X/5". Se declara e
    // implementa ANTES de drawLayer() porque esta la invoca.
    // ------------------------------------------------------------
    void drawLayerLabel(int layerNumber) {
        struct LayerInfo {
            const char* title;
            const char* subtitle;
            float r, g, b;
        };

        LayerInfo layers[6] = {
            { "", "", 0.0f, 0.0f, 0.0f },
            { "CAPA 1", "EXTERIOR",   0.9f, 0.9f, 0.9f },
            { "CAPA 2", "ESTRUCTURA", 0.0f, 1.0f, 0.0f },
            { "CAPA 3", "SISTEMAS",   0.0f, 0.4f, 1.0f },
            { "CAPA 4", "CABINA",     0.9f, 0.2f, 0.2f },
            { "CAPA 5", "PROPULSION", 0.5f, 0.5f, 0.5f }
        };

        if (layerNumber < 1 || layerNumber > 5) return;
        LayerInfo info = layers[layerNumber];

        // Caja de fondo semi-solida
        glColor3f(0.08f, 0.08f, 0.08f);
        glBegin(GL_QUADS);
        glVertex3f(-195.0f, 128.0f, 0.0f);
        glVertex3f(-90.0f, 128.0f, 0.0f);
        glVertex3f(-90.0f, 145.0f, 0.0f);
        glVertex3f(-195.0f, 145.0f, 0.0f);
        glEnd();

        // Borde de la caja
        glColor3f(0.4f, 0.4f, 0.4f);
        glLineWidth(1.0f);
        glBegin(GL_LINE_LOOP);
        glVertex3f(-195.0f, 128.0f, 0.0f);
        glVertex3f(-90.0f, 128.0f, 0.0f);
        glVertex3f(-90.0f, 145.0f, 0.0f);
        glVertex3f(-195.0f, 145.0f, 0.0f);
        glEnd();

        // Indicador de color
        glColor3f(info.r, info.g, info.b);
        glBegin(GL_QUADS);
        glVertex3f(-192.0f, 132.0f, 0.0f);
        glVertex3f(-187.0f, 132.0f, 0.0f);
        glVertex3f(-187.0f, 141.0f, 0.0f);
        glVertex3f(-192.0f, 141.0f, 0.0f);
        glEnd();

        // Titulo
        glColor3f(1.0f, 1.0f, 1.0f);
        glRasterPos2f(-183.0f, 138.0f);
        for (const char* c = info.title; *c != '\0'; c++) {
            glutBitmapCharacter(GLUT_BITMAP_HELVETICA_12, *c);
        }

        // Subtitulo
        glColor3f(0.75f, 0.75f, 0.75f);
        glRasterPos2f(-183.0f, 131.0f);
        for (const char* c = info.subtitle; *c != '\0'; c++) {
            glutBitmapCharacter(GLUT_BITMAP_HELVETICA_10, *c);
        }

        // Contador "X/5"
        char counter[8];
        sprintf(counter, "%d/5", layerNumber);
        glColor3f(0.6f, 0.6f, 0.6f);
        glRasterPos2f(175.0f, 138.0f);
        for (const char* c = counter; *c != '\0'; c++) {
            glutBitmapCharacter(GLUT_BITMAP_HELVETICA_12, *c);
        }
    }

    // --------------------------------------------------------
    // drawLayer()
    // Llama a los generadores de geometry.cpp segun la capa activa.
    // --------------------------------------------------------
    void drawLayer(int layerNumber) {
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

        drawLayerLabel(layerNumber); // HUD tipo simulador
    }

} // namespace Renderer