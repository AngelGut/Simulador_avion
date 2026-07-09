// ============================================================
// ARCHIVO: geometry.cpp
// RESPONSABLE: Ronald (Geometría y Motores)
// PROYECTO: Boeing 737 Visualizer - Julio 2026
// DESCRIPTION: Implementación final corregida de las mallas 
//              sólidas 2D paramétricas y algoritmos de soporte.
// ============================================================

#include "geometry.h"
#include <GL/glut.h>
#include <cmath>
#include <vector>
#include <algorithm>

namespace GeometryBuilder {

    // ============================================================
    // FUNCIONES DE APOYO (Algoritmos de rasterización entera y curvas)
    // ============================================================

    void bresenhamCircle(float cx, float cy, float radius) {
        int x = 0;
        int y = (int)radius;
        int d = 3 - 2 * (int)radius;

        std::vector<std::pair<float, float>> points;

        while (x <= y) {
            points.push_back({ cx + x, cy + y });
            points.push_back({ cx - x, cy + y });
            points.push_back({ cx + x, cy - y });
            points.push_back({ cx - x, cy - y });
            points.push_back({ cx + y, cy + x });
            points.push_back({ cx - y, cy + x });
            points.push_back({ cx + y, cy - x });
            points.push_back({ cx - y, cy - x });

            if (d < 0) {
                d = d + 4 * x + 6;
            }
            else {
                d = d + 4 * (x - y) + 10;
                y--;
            }
            x++;
        }

        std::sort(points.begin(), points.end(),
            [cx, cy](const std::pair<float, float>& a, const std::pair<float, float>& b) {
                return atan2(a.second - cy, a.first - cx) < atan2(b.second - cy, b.first - cx);
            });

        glBegin(GL_LINE_LOOP);
        for (auto& p : points) {
            glNormal3f(0.0f, 0.0f, 1.0f);
            glVertex3f(p.first, p.second, 0.0f);
        }
        glEnd();
    }

    void bresenhamSemicircle(float cx, float cy, float radius) {
        int x = 0;
        int y = (int)radius;
        int d = 3 - 2 * (int)radius;

        std::vector<std::pair<float, float>> points;

        while (x <= y) {
            points.push_back({ cx + x, cy + y });
            points.push_back({ cx - x, cy + y });
            points.push_back({ cx + y, cy + x });
            points.push_back({ cx - y, cy + x });

            if (d < 0) {
                d = d + 4 * x + 6;
            }
            else {
                d = d + 4 * (x - y) + 10;
                y--;
            }
            x++;
        }

        std::sort(points.begin(), points.end(),
            [cx, cy](const std::pair<float, float>& a, const std::pair<float, float>& b) {
                return atan2(a.second - cy, a.first - cx) < atan2(b.second - cy, b.first - cx);
            });

        glBegin(GL_LINE_STRIP);
        for (auto& p : points) {
            glNormal3f(0.0f, 0.0f, 1.0f);
            glVertex3f(p.first, p.second, 0.0f);
        }
        glEnd();
    }

    void ddaLine(float x0, float y0, float x1, float y1) {
        float dx = x1 - x0;
        float dy = y1 - y0;
        int steps = (int)fmax(fabs(dx), fabs(dy));
        if (steps == 0) return;

        float xInc = dx / (float)steps;
        float yInc = dy / (float)steps;
        float x = x0, y = y0;

        glBegin(GL_LINE_STRIP);
        for (int i = 0; i <= steps; i++) {
            glNormal3f(0.0f, 0.0f, 1.0f);
            glVertex3f(x, y, 0.0f);
            x += xInc; y += yInc;
        }
        glEnd();
    }

    void bezierCurve(float x0, float y0, float x1, float y1,
                      float x2, float y2, float x3, float y3, int segments) {
        glBegin(GL_LINE_STRIP);
        for (int i = 0; i <= segments; i++) {
            float t = (float)i / (float)segments;
            float u = 1.0f - t;
            float bx = u * u * u * x0 + 3 * u * u * t * x1 + 3 * u * t * t * x2 + t * t * t * x3;
            float by = u * u * u * y0 + 3 * u * u * t * y1 + 3 * u * t * t * y2 + t * t * t * y3;
            glNormal3f(0.0f, 0.0f, 1.0f);
            glVertex3f(bx, by, 0.0f);
        }
        glEnd();
    }

    bool clipLineToViewport(float& x0, float& y0, float& x1, float& y1,
                            float xmin, float ymin, float xmax, float ymax) {
        return true;
    }

    // ============================================================
    // GENERADORES DE COMPONENTES MEJORADOS (SOLUCIÓN DE MALLAS MESH)
    // ============================================================

    // ------------------------------------------------------------
    // generateFuselage() - Capa 1: Exterior (SOLUCIÓN AL FILAMENTO)
    // CORRECCIÓN: Se inyectan de forma pareada los vértices izquierdo y derecho
    // en el bucle longitudinal para que GL_QUAD_STRIP construya masa sólida real.
    // ------------------------------------------------------------
    void generateFuselage(float centerX, float centerY, float radius, float length) {
        int slices = 32;
        float rMax = 15.0f; // Escala refinada idéntica a tu plano de referencia
        
        float yInicio = -110.0f;
        float yFinal = 110.0f;
        int numPasos = 40;
        float altoPaso = (yFinal - yInicio) / numPasos;

        // Iteración longitudinal por secciones
        for (int i = 0; i < numPasos; i++) {
            float y0 = yInicio + i * altoPaso;
            float y1 = yInicio + (i + 1) * altoPaso;

            // Calcular dinámicamente el perfil del radio para afilar los extremos (Nariz/Cola)
            float r0 = rMax;
            if (y0 > 60.0f)  r0 = rMax * (1.0f - pow((y0 - 60.0f) / (yFinal - 60.0f), 2));
            else if (y0 < -50.0f) r0 = rMax * (1.0f - 0.7f * ((y0 - (-50.0f)) / (yInicio - (-50.0f))));

            float r1 = rMax;
            if (y1 > 60.0f)  r1 = rMax * (1.0f - pow((y1 - 60.0f) / (yFinal - 60.0f), 2));
            else if (y1 < -50.0f) r1 = rMax * (1.0f - 0.7f * ((y1 - (-50.0f)) / (yInicio - (-50.0f))));

            // Tira de cuadriláteros con inyección de vértices pareados (¡Solución al bug!)
            glBegin(GL_QUAD_STRIP);
            for (int j = 0; j <= slices; j++) {
                float angle = (j / (float)slices) * 2.0f * 3.141592f;
                float factorLuz = 0.4f + 0.6f * cos(angle); // Sombreado Gouraud fijo
                
                // Color material base blanco degradado
                glColor3f(factorLuz, factorLuz, factorLuz);
                glNormal3f(cos(angle), 0.0f, sin(angle));

                // Vértice 1: Base de la rebanada (Sección i)
                glVertex3f(centerX + r0 * cos(angle), y0, 0.0f);
                
                // Vértice 2: Techo de la rebanada (Sección i+1)
                glVertex3f(centerX + r1 * cos(angle), y1, 0.0f);
            }
            glEnd();
        }

        // --- ESTABILIZADORES DE COLA (ALERONES TRASEROS REALISTAS) ---
        glDisable(GL_LIGHTING);
        glColor3f(0.85f, 0.85f, 0.85f);
        
        // Alerón Derecho (Sentido CCW)
        glBegin(GL_QUADS);
            glVertex3f(centerX + 5.0f, -80.0f, 0.0f);
            glVertex3f(centerX + 42.0f, -105.0f, 0.0f); 
            glVertex3f(centerX + 39.0f, -112.0f, 0.0f);
            glVertex3f(centerX + 0.0f, -102.0f, 0.0f);
        glEnd();

        // Alerón Izquierdo (Sentido CCW corregido)
        glBegin(GL_QUADS);
            glVertex3f(centerX - 5.0f, -80.0f, 0.0f);
            glVertex3f(centerX - 0.0f, -102.0f, 0.0f);
            glVertex3f(centerX - 39.0f, -112.0f, 0.0f);
            glVertex3f(centerX - 42.0f, -105.0f, 0.0f);
        glEnd();

        glEnable(GL_LIGHTING);
    }

    // ------------------------------------------------------------
    // generateWings() - Capa 1: Exterior
    // Construcción acoplada de las superficies alares de flujo en flecha.
    // ------------------------------------------------------------
    void generateWings() {
        glDisable(GL_LIGHTING);
        glColor3f(1.0f, 0.0f, 0.0f); // Rojo

        // Ala Derecha (CCW)
        glBegin(GL_QUADS);
            glVertex3f(15.0f, -25.0f, 0.0f);  
            glVertex3f(125.0f, -15.0f, 0.0f); 
            glVertex3f(120.0f, -2.0f, 0.0f);  
            glVertex3f(15.0f, 2.0f, 0.0f);    
        glEnd();

        // Ala Izquierda (CCW corregido)
        glBegin(GL_QUADS);
            glVertex3f(-15.0f, -25.0f, 0.0f);
            glVertex3f(-15.0f, 2.0f, 0.0f);    
            glVertex3f(-120.0f, -2.0f, 0.0f);  
            glVertex3f(-125.0f, -15.0f, 0.0f);
        glEnd();

        glEnable(GL_LIGHTING);
    }

    // ------------------------------------------------------------
    // generateMotors() - Capa 1 / Capa 5: Propulsión
    // Integra los dos turbofans CFM56 acoplados sólidamente bajo el extradós alar.
    // ------------------------------------------------------------
    void generateMotors() {
        int slices = 24;
        float rMotor = 6.5f;
        float lenMotor = 22.0f;
        float xPositions[2] = { -65.0f, 65.0f }; 
        float yPosicion = -8.0f;

        for (int m = 0; m < 2; m++) {
            float cx = xPositions[m];

            // Tira de cuadriláteros sólidos con iluminación lateral Gouraud
            glBegin(GL_QUAD_STRIP);
            for (int i = 0; i <= slices; i++) {
                float angle = (i / (float)slices) * 2.0f * 3.141592f;
                float factor = 0.3f + 0.5f * cos(angle);

                glColor3f(0.4f * factor, 0.4f * factor, 0.4f * factor);
                glNormal3f(cos(angle), 0.0f, sin(angle));

                // Par de vértices longitudinales del motor
                glVertex3f(cx + rMotor * cos(angle), yPosicion - lenMotor / 2.0f, 0.0f);
                glVertex3f(cx + rMotor * cos(angle), yPosicion + lenMotor / 2.0f, 0.0f);
            }
            glEnd();

            // Tapa frontal de admisión (Intake)
            glColor3f(0.05f, 0.05f, 0.05f);
            glBegin(GL_TRIANGLE_FAN);
                glNormal3f(0.0f, 1.0f, 0.0f);
                glVertex3f(cx, yPosicion + lenMotor / 2.0f, 0.0f);
                for (int i = 0; i <= slices; i++) {
                    float angle = (i / (float)slices) * 2.0f * 3.141592f;
                    glVertex3f(cx + rMotor * cos(angle), yPosicion + lenMotor / 2.0f, 0.0f);
                }
            glEnd();
        }
    }

    void generateLandingGear() {
        glDisable(GL_LIGHTING);
        glColor3f(0.0f, 0.0f, 0.0f);
        glLineWidth(2.5f);
        glBegin(GL_LINES);
            glVertex3f(-45.0f, -20.0f, 0.0f); glVertex3f(-45.0f, -50.0f, 0.0f);
            glVertex3f(45.0f, -20.0f, 0.0f);  glVertex3f(45.0f, -50.0f, 0.0f);
            glVertex3f(0.0f, 85.0f, 0.0f);    glVertex3f(0.0f, 105.0f, 0.0f);
        glEnd();
        glLineWidth(1.0f);
        bresenhamCircle(-45.0f, -53.0f, 4.0f);
        bresenhamCircle(45.0f, -53.0f, 4.0f);
        bresenhamCircle(0.0f, 108.0f, 3.0f);
        glEnable(GL_LIGHTING);
    }

    void generateStructure() {
        glDisable(GL_LIGHTING);
        glColor3f(0.0f, 1.0f, 0.0f);
        glLineWidth(1.5f);
        for (int i = 0; i < 7; i++) {
            float yPos = -85.0f + i * 28.0f;
            bresenhamSemicircle(0.0f, yPos, 14.5f);
        }
        glColor3f(0.2f, 1.0f, 0.2f);
        ddaLine(0.0f, -110.0f, 0.0f, 110.0f);
        ddaLine(-10.0f, -90.0f, -10.0f, 80.0f);
        ddaLine(10.0f, -90.0f, 10.0f, 80.0f);
        glLineWidth(1.0f);
        glEnable(GL_LIGHTING);
    }

    void generateSystems() {
        glDisable(GL_LIGHTING);
        glColor3f(0.0f, 0.0f, 1.0f);
        glLineWidth(1.5f);
        bezierCurve(-110.0f, -15.0f, -40.0f, 20.0f, 40.0f, -20.0f, 110.0f, -5.0f, 30);
        glColor3f(1.0f, 1.0f, 0.0f);
        for (int i = 0; i < 6; i++) {
            float yPos = -40.0f + i * 15.0f;
            glBegin(GL_LINE_STRIP);
                glVertex3f(-110.0f, yPos, 0.0f); glVertex3f(110.0f, yPos, 0.0f);
            glEnd();
        }
        glLineWidth(1.0f);
        glEnable(GL_LIGHTING);
    }

    void generateCabin() {
        glDisable(GL_LIGHTING);
        glColor3f(0.3f, 0.3f, 0.3f);
        glBegin(GL_QUADS);
            glVertex3f(-13.0f, -65.0f, 0.0f); glVertex3f(14.0f, -65.0f, 0.0f);
            glVertex3f(14.0f, 70.0f, 0.0f);   glVertex3f(-13.0f, 70.0f, 0.0f);
        glEnd();

        int rows = 10;
        int seatsPerRow[3] = { 2, 3, 2 };
        for (int row = 0; row < rows; row++) {
            float yPos = 60.0f - (row * 11.0f);
            float xStart = -12.0f;
            for (int col = 0; col < 3; col++) {
                float offsetColumnas = col * 8.5f;
                for (int seat = 0; seat < seatsPerRow[col]; seat++) {
                    float xPos = xStart + offsetColumnas + seat * 2.8f;
                    glColor3f(0.65f, 0.16f, 0.16f);
                    glPushMatrix(); glTranslatef(xPos, yPos, 0.0f);
                    glBegin(GL_QUADS);
                        glVertex3f(-0.9f, -1.3f, 0.0f); glVertex3f(0.9f, -1.3f, 0.0f);
                        glVertex3f(0.9f, 1.3f, 0.0f);   glVertex3f(-0.9f, 1.3f, 0.0f);
                    glEnd();
                    glColor3f(0.85f, 0.35f, 0.35f);
                    glBegin(GL_LINE_LOOP);
                        glVertex3f(-0.9f, -1.3f, 0.0f); glVertex3f(0.9f, -1.3f, 0.0f);
                        glVertex3f(0.9f, 1.3f, 0.0f);   glVertex3f(-0.9f, 1.3f, 0.0f);
                    glEnd();
                    glPopMatrix();
                }
            }
        }
        glEnable(GL_LIGHTING);
    }

} // namespace GeometryBuilder