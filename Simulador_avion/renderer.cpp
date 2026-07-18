// ============================================================
// ARCHIVO: renderer.cpp - VERSIÓN CORREGIDA
// RESPONSABLE: Ronald (Rendering)
// DESCRIPCION: Luces pequeñas a lo largo pista + HUD visible
// ============================================================

#include "renderer.h"
#include "geometry.h"
#include <GL/glut.h>
#include <cstdio>
#include <cmath>

namespace Renderer {

    // ============================================================
    // setupOpenGL() - Configuración general
    // ============================================================
    void setupOpenGL() {
        glClearColor(0.05f, 0.05f, 0.05f, 1.0f);
        glEnable(GL_LINE_SMOOTH);
        glHint(GL_LINE_SMOOTH_HINT, GL_NICEST);
        glLineWidth(1.0f);

        // NO HABILITAR ILUMINACIÓN - queremos colores planos
        glDisable(GL_LIGHTING);
    }

    // ============================================================
    // applyLighting() - Deshabilitada intencionalmente
    // ============================================================
    void applyLighting() {
        // Las luces están en el piso, no en el avión
        glDisable(GL_LIGHTING);
    }

    // ============================================================
    // applyColor() - Wrapper de color
    // ============================================================
    void applyColor(float r, float g, float b) {
        glColor3f(r, g, b);
    }

    // ============================================================
    // drawStreetLights() - Luces pequeñas a lo largo de la pista
    // ============================================================
    void drawStreetLights() {
        float groundLevel = -200.0f;

        // Puntos de luz pequeños a lo largo de la pista (izquierda)
        glPointSize(4.0f);
        glBegin(GL_POINTS);
        glColor3f(0.9f, 0.9f, 0.6f);  // Amarillo cálido

        for (float y = -140.0f; y <= 140.0f; y += 20.0f) {
            glVertex3f(-55.0f, y, -0.5f);  // Izquierda
            glVertex3f(55.0f, y, -0.5f);   // Derecha
        }

        glEnd();
        glPointSize(1.0f);

        // ========================================
        // HALOS suave alrededor de luces
        // ========================================
        glColor4f(0.8f, 0.8f, 0.5f, 0.15f);
        glEnable(GL_BLEND);
        glBlendFunc(GL_SRC_ALPHA, GL_ONE);

        for (float y = -140.0f; y <= 140.0f; y += 20.0f) {
            // Halo izquierda
            glBegin(GL_POLYGON);
            for (int i = 0; i < 16; i++) {
                float angle = (i / 16.0f) * 2.0f * 3.14159f;
                float x = -55.0f + 8.0f * cos(angle);
                float py = y + 8.0f * sin(angle);
                glVertex3f(x, py, -0.4f);
            }
            glEnd();

            // Halo derecha
            glBegin(GL_POLYGON);
            for (int i = 0; i < 16; i++) {
                float angle = (i / 16.0f) * 2.0f * 3.14159f;
                float x = 55.0f + 8.0f * cos(angle);
                float py = y + 8.0f * sin(angle);
                glVertex3f(x, py, -0.4f);
            }
            glEnd();
        }

        glDisable(GL_BLEND);
    }

    // ============================================================
    // drawGroundShadow() - Sombra del avión en piso
    // ============================================================
    void drawGroundShadow() {
        glColor4f(0.0f, 0.0f, 0.0f, 0.2f);
        glEnable(GL_BLEND);
        glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);

        glBegin(GL_POLYGON);
        glVertex3f(-100.0f, -190.0f, -0.9f);
        glVertex3f(-100.0f, -200.0f, -0.9f);
        glVertex3f(100.0f, -200.0f, -0.9f);
        glVertex3f(100.0f, -190.0f, -0.9f);
        glEnd();

        glDisable(GL_BLEND);
    }

    // ============================================================
    // drawAirportRunway() - Pista de aterrizaje
    // ============================================================
    void drawAirportRunway() {
        glDisable(GL_LIGHTING);

        // Asfalto de la pista (gris oscuro)
        glColor3f(0.25f, 0.25f, 0.25f);
        glBegin(GL_QUADS);
        glVertex3f(-60.0f, -150.0f, -0.9f);
        glVertex3f(60.0f, -150.0f, -0.9f);
        glVertex3f(60.0f, 150.0f, -0.9f);
        glVertex3f(-60.0f, 150.0f, -0.9f);
        glEnd();

        // Líneas laterales blancas
        glColor3f(1.0f, 1.0f, 1.0f);
        glLineWidth(2.0f);
        glBegin(GL_LINES);
        glVertex3f(-60.0f, -150.0f, -0.8f);
        glVertex3f(-60.0f, 150.0f, -0.8f);
        glVertex3f(60.0f, -150.0f, -0.8f);
        glVertex3f(60.0f, 150.0f, -0.8f);
        glEnd();

        // Línea discontinua central (amarilla)
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

    // ============================================================
    // drawLayerLabel() - Etiqueta de capa (HUD) - EN ESQUINA
    // ============================================================
    void drawLayerLabel(int layerNumber) {
        struct LayerInfo {
            const char* title;
            const char* subtitle;
            float r, g, b;
        };

        LayerInfo layers[6] = {
            { "", "", 0.0f, 0.0f, 0.0f },
            { "CAPA 1", "EXTERIOR", 0.9f, 0.9f, 0.9f },
            { "CAPA 2", "ESTRUCTURA", 0.0f, 1.0f, 0.0f },
            { "CAPA 3", "SISTEMAS", 0.0f, 0.4f, 1.0f },
            { "CAPA 4", "CABINA", 0.9f, 0.2f, 0.2f },
            { "CAPA 5", "PROPULSION", 0.5f, 0.5f, 0.5f }
        };

        if (layerNumber < 1 || layerNumber > 5) return;
        LayerInfo info = layers[layerNumber];

        // POSICIÓN EN ESQUINA (arriba izquierda)
        float posX = -380.0f;
        float posY = 260.0f;
        float boxWidth = 100.0f;
        float boxHeight = 25.0f;

        // Caja de fondo
        glColor3f(0.08f, 0.08f, 0.08f);
        glBegin(GL_QUADS);
        glVertex3f(posX, posY - boxHeight, 0.0f);
        glVertex3f(posX + boxWidth, posY - boxHeight, 0.0f);
        glVertex3f(posX + boxWidth, posY, 0.0f);
        glVertex3f(posX, posY, 0.0f);
        glEnd();

        // Borde
        glColor3f(0.4f, 0.4f, 0.4f);
        glLineWidth(1.5f);
        glBegin(GL_LINE_LOOP);
        glVertex3f(posX, posY - boxHeight, 0.0f);
        glVertex3f(posX + boxWidth, posY - boxHeight, 0.0f);
        glVertex3f(posX + boxWidth, posY, 0.0f);
        glVertex3f(posX, posY, 0.0f);
        glEnd();

        // Indicador de color
        glColor3f(info.r, info.g, info.b);
        glBegin(GL_QUADS);
        glVertex3f(posX + 2.0f, posY - boxHeight + 3.0f, 0.0f);
        glVertex3f(posX + 7.0f, posY - boxHeight + 3.0f, 0.0f);
        glVertex3f(posX + 7.0f, posY - 3.0f, 0.0f);
        glVertex3f(posX + 2.0f, posY - 3.0f, 0.0f);
        glEnd();

        // Título
        glColor3f(1.0f, 1.0f, 1.0f);
        glRasterPos2f(posX + 12.0f, posY - 7.0f);
        for (const char* c = info.title; *c != '\0'; c++) {
            glutBitmapCharacter(GLUT_BITMAP_HELVETICA_10, *c);
        }

        // Subtítulo
        glColor3f(0.75f, 0.75f, 0.75f);
        glRasterPos2f(posX + 12.0f, posY - 17.0f);
        for (const char* c = info.subtitle; *c != '\0'; c++) {
            glutBitmapCharacter(GLUT_BITMAP_HELVETICA_10, *c);
        }

        glLineWidth(1.0f);
    }

    // ============================================================
    // drawLayer() - Renderizar capa seleccionada
    // ============================================================
    void drawLayer(int layerNumber) {
        drawAirportRunway();

        // Dibujar luces pequeñas en la pista
        drawStreetLights();

        // Dibujar sombra del avión
        drawGroundShadow();

        // DIBUJAR HUD EN CADA FRAME
        drawLayerLabel(layerNumber);

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