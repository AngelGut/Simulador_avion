// ============================================================
// ARCHIVO: geometry.cpp
// RESPONSABLE: Ronald (Geometría y Motores)
// PROYECTO: Boeing 737 Visualizer - Julio 2026
// DESCRIPTION: Implementación de mallas 2D con NORMALES
//              para soporte de iluminación Phong.
// ============================================================

#include "geometry.h"
#include <GL/glut.h>
#include <cmath>
#include <vector>
#include <algorithm>

namespace GeometryBuilder {

    // ============================================================
    // FUNCIONES DE APOYO (Algoritmos de rasterización + normales)
    // ============================================================

    // Bresenham Circle - con normales para iluminación
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
            glNormal3f(0.0f, 0.0f, 1.0f);  // ← NORMAL
            glVertex3f(p.first, p.second, 0.0f);
        }
        glEnd();
    }

    // Bresenham Semicircle - con normales
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
            glNormal3f(0.0f, 0.0f, 1.0f);  // ← NORMAL
            glVertex3f(p.first, p.second, 0.0f);
        }
        glEnd();
    }

    // DDA Line - con normales
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
            glNormal3f(0.0f, 0.0f, 1.0f);  // ← NORMAL
            glVertex3f(x, y, 0.0f);
            x += xInc; y += yInc;
        }
        glEnd();
    }

    // Bezier Curve - con normales
    void bezierCurve(float x0, float y0, float x1, float y1,
                      float x2, float y2, float x3, float y3, int segments) {
        glBegin(GL_LINE_STRIP);
        for (int i = 0; i <= segments; i++) {
            float t = (float)i / (float)segments;
            float u = 1.0f - t;
            float bx = u * u * u * x0 + 3 * u * u * t * x1 + 3 * u * t * t * x2 + t * t * t * x3;
            float by = u * u * u * y0 + 3 * u * u * t * y1 + 3 * u * t * t * y2 + t * t * t * y3;
            glNormal3f(0.0f, 0.0f, 1.0f);  // ← NORMAL
            glVertex3f(bx, by, 0.0f);
        }
        glEnd();
    }

    // Bezier Tube - con normales
    void bezierTube(float x0, float y0, float x1, float y1,
        float x2, float y2, float x3, float y3,
        int segments, float thickness) {
        glBegin(GL_QUAD_STRIP);
        for (int i = 0; i <= segments; i++) {
            float t = (float)i / (float)segments;
            float u = 1.0f - t;

            float bx = u * u * u * x0 + 3 * u * u * t * x1 + 3 * u * t * t * x2 + t * t * t * x3;
            float by = u * u * u * y0 + 3 * u * u * t * y1 + 3 * u * t * t * y2 + t * t * t * y3;

            glNormal3f(0.0f, 0.0f, 1.0f);  // ← NORMAL
            glVertex3f(bx, by + thickness / 2.0f, 0.0f);
            glNormal3f(0.0f, 0.0f, 1.0f);  // ← NORMAL
            glVertex3f(bx, by - thickness / 2.0f, 0.0f);
        }
        glEnd();
    }

    bool clipLineToViewport(float& x0, float& y0, float& x1, float& y1,
                            float xmin, float ymin, float xmax, float ymax) {
        return true;
    }

    // ============================================================
    // GENERADORES DE COMPONENTES CON NORMALES
    // ============================================================

    // generateFuselage - con normales en todos los vértices
    void generateFuselage(float centerX, float centerY, float radius, float length) {
        float rMax = 15.0f;
        float yInicio = -110.0f;
        float yFinal = 110.0f;

        glColor3f(0.85f, 0.85f, 0.85f);

        glBegin(GL_POLYGON);
        glNormal3f(0.0f, 0.0f, 1.0f);  // ← NORMAL
        glVertex3f(centerX, yFinal, 0.0f);
        glNormal3f(0.0f, 0.0f, 1.0f);  // ← NORMAL
        glVertex3f(centerX + rMax, 60.0f, 0.0f);
        glNormal3f(0.0f, 0.0f, 1.0f);  // ← NORMAL
        glVertex3f(centerX + rMax, -50.0f, 0.0f);
        glNormal3f(0.0f, 0.0f, 1.0f);  // ← NORMAL
        glVertex3f(centerX + rMax * 0.3f, yInicio, 0.0f);
        glNormal3f(0.0f, 0.0f, 1.0f);  // ← NORMAL
        glVertex3f(centerX - rMax * 0.3f, yInicio, 0.0f);
        glNormal3f(0.0f, 0.0f, 1.0f);  // ← NORMAL
        glVertex3f(centerX - rMax, -50.0f, 0.0f);
        glNormal3f(0.0f, 0.0f, 1.0f);  // ← NORMAL
        glVertex3f(centerX - rMax, 60.0f, 0.0f);
        glEnd();

        // Triángulo punta de nariz
        glColor3f(0.05f, 0.05f, 0.05f);
        glBegin(GL_TRIANGLES);
        glNormal3f(0.0f, 0.0f, 1.0f);  // ← NORMAL
        glVertex3f(centerX, yFinal, 0.0f);
        glNormal3f(0.0f, 0.0f, 1.0f);  // ← NORMAL
        glVertex3f(centerX - 5.0f, yFinal - 15.0f, 0.0f);
        glNormal3f(0.0f, 0.0f, 1.0f);  // ← NORMAL
        glVertex3f(centerX + 5.0f, yFinal - 15.0f, 0.0f);
        glEnd();

        // Estabilizador cola derecho
        glBegin(GL_QUADS);
        glNormal3f(0.0f, 0.0f, 1.0f);  // ← NORMAL
        glVertex3f(centerX + 5.0f, -80.0f, 0.0f);
        glNormal3f(0.0f, 0.0f, 1.0f);  // ← NORMAL
        glVertex3f(centerX + 42.0f, -105.0f, 0.0f);
        glNormal3f(0.0f, 0.0f, 1.0f);  // ← NORMAL
        glVertex3f(centerX + 39.0f, -112.0f, 0.0f);
        glNormal3f(0.0f, 0.0f, 1.0f);  // ← NORMAL
        glVertex3f(centerX + 0.0f, -102.0f, 0.0f);
        glEnd();

        // Estabilizador cola izquierdo
        glBegin(GL_QUADS);
        glNormal3f(0.0f, 0.0f, 1.0f);  // ← NORMAL
        glVertex3f(centerX - 5.0f, -80.0f, 0.0f);
        glNormal3f(0.0f, 0.0f, 1.0f);  // ← NORMAL
        glVertex3f(centerX - 0.0f, -102.0f, 0.0f);
        glNormal3f(0.0f, 0.0f, 1.0f);  // ← NORMAL
        glVertex3f(centerX - 39.0f, -112.0f, 0.0f);
        glNormal3f(0.0f, 0.0f, 1.0f);  // ← NORMAL
        glVertex3f(centerX - 42.0f, -105.0f, 0.0f);
        glEnd();
    }

    // generateWings - con normales
    void generateWings() {
        glColor3f(1.0f, 0.0f, 0.0f);

        // Ala Derecha
        glBegin(GL_QUADS);
        glNormal3f(0.0f, 0.0f, 1.0f);  // ← NORMAL
        glVertex3f(15.0f, -25.0f, 0.0f);
        glNormal3f(0.0f, 0.0f, 1.0f);  // ← NORMAL
        glVertex3f(125.0f, -15.0f, 0.0f);
        glNormal3f(0.0f, 0.0f, 1.0f);  // ← NORMAL
        glVertex3f(120.0f, -2.0f, 0.0f);
        glNormal3f(0.0f, 0.0f, 1.0f);  // ← NORMAL
        glVertex3f(15.0f, 2.0f, 0.0f);
        glEnd();

        // Ala Izquierda
        glBegin(GL_QUADS);
        glNormal3f(0.0f, 0.0f, 1.0f);  // ← NORMAL
        glVertex3f(-15.0f, -25.0f, 0.0f);
        glNormal3f(0.0f, 0.0f, 1.0f);  // ← NORMAL
        glVertex3f(-15.0f, 2.0f, 0.0f);
        glNormal3f(0.0f, 0.0f, 1.0f);  // ← NORMAL
        glVertex3f(-120.0f, -2.0f, 0.0f);
        glNormal3f(0.0f, 0.0f, 1.0f);  // ← NORMAL
        glVertex3f(-125.0f, -15.0f, 0.0f);
        glEnd();
    }

    // generateMotors - con normales
    void generateMotors() {
        float positions[2] = { -65.0f, 65.0f };
        for (int m = 0; m < 2; m++) {
            float cx = positions[m];
            float cy = -20.0f;

            // Cuerpo motor
            glColor3f(0.35f, 0.35f, 0.35f);
            glBegin(GL_POLYGON);
            for (int i = 0; i < 20; i++) {
                float angle = (i / 20.0f) * 2.0f * 3.14159f;
                glNormal3f(0.0f, 0.0f, 1.0f);  // ← NORMAL
                glVertex3f(cx + 10.0f * cos(angle), cy + 18.0f * sin(angle), 0.0f);
            }
            glEnd();

            // Intake
            glColor3f(0.02f, 0.02f, 0.02f);
            glBegin(GL_TRIANGLES);
            glNormal3f(0.0f, 0.0f, 1.0f);  // ← NORMAL
            glVertex3f(cx, cy + 18.0f, 0.0f);
            glNormal3f(0.0f, 0.0f, 1.0f);  // ← NORMAL
            glVertex3f(cx - 8.0f, cy + 4.0f, 0.0f);
            glNormal3f(0.0f, 0.0f, 1.0f);  // ← NORMAL
            glVertex3f(cx + 8.0f, cy + 4.0f, 0.0f);
            glEnd();
        }
    }

    // generateLandingGear - con normales
    void generateLandingGear() {
        drawFuselageOutline();

        // Patas
        glColor3f(0.05f, 0.05f, 0.05f);
        glLineWidth(3.0f);
        glBegin(GL_LINES);
        glNormal3f(0.0f, 0.0f, 1.0f);  // ← NORMAL
        glVertex3f(-11.0f, -20.0f, 0.0f);
        glNormal3f(0.0f, 0.0f, 1.0f);  // ← NORMAL
        glVertex3f(-11.0f, -42.0f, 0.0f);
        glNormal3f(0.0f, 0.0f, 1.0f);  // ← NORMAL
        glVertex3f(11.0f, -20.0f, 0.0f);
        glNormal3f(0.0f, 0.0f, 1.0f);  // ← NORMAL
        glVertex3f(11.0f, -42.0f, 0.0f);
        glNormal3f(0.0f, 0.0f, 1.0f);  // ← NORMAL
        glVertex3f(0.0f, 55.0f, 0.0f);
        glNormal3f(0.0f, 0.0f, 1.0f);  // ← NORMAL
        glVertex3f(0.0f, 75.0f, 0.0f);
        glEnd();
        glLineWidth(1.0f);

        // Ruedas
        glColor3f(0.05f, 0.05f, 0.05f);
        float wheelPositions[3][2] = {
            { -11.0f, -46.0f },
            {  11.0f, -46.0f },
            {  0.0f,  79.0f }
        };
        float wheelRadii[3] = { 4.5f, 4.5f, 3.0f };

        for (int w = 0; w < 3; w++) {
            glBegin(GL_POLYGON);
            for (int i = 0; i < 16; i++) {
                float angle = (i / 16.0f) * 2.0f * 3.14159f;
                glNormal3f(0.0f, 0.0f, 1.0f);  // ← NORMAL
                glVertex3f(wheelPositions[w][0] + wheelRadii[w] * cos(angle),
                    wheelPositions[w][1] + wheelRadii[w] * sin(angle), 0.0f);
            }
            glEnd();
        }

        // Aros
        glColor3f(0.5f, 0.5f, 0.5f);
        glLineWidth(1.0f);
        for (int w = 0; w < 3; w++) {
            bresenhamCircle(wheelPositions[w][0], wheelPositions[w][1], wheelRadii[w] * 0.5f);
        }
    }

    // generateStructure - con normales
    void generateStructure() {
        glColor3f(0.0f, 1.0f, 0.0f);
        glLineWidth(1.5f);
        for (int i = 0; i < 6; i++) {
            float yPos = -100.0f + i * 40.0f;
            bresenhamSemicircle(0.0f, yPos, 12.0f);
        }

        glColor3f(0.2f, 1.0f, 0.2f);
        ddaLine(0.0f, -110.0f, 0.0f, 110.0f);
        ddaLine(-10.0f, -100.0f, -10.0f, 100.0f);
        ddaLine(10.0f, -100.0f, 10.0f, 100.0f);

        glColor3f(0.0f, 0.6f, 0.0f);
        for (int i = 0; i < 5; i++) {
            float yPos = -80.0f + i * 40.0f;
            bresenhamCircle(0.0f, yPos, 12.0f);
        }

        glLineWidth(1.0f);
    }

    // generateSystems - con normales
    void generateSystems() {
        drawFuselageOutline();

        // Tubería hidráulica
        glColor3f(0.0f, 0.3f, 0.9f);
        bezierTube(-13.0f, -80.0f, -8.0f, -20.0f, 8.0f, 20.0f, 13.0f, 80.0f, 30, 10.0f);

        glLineWidth(1.5f);
        glColor3f(0.0f, 0.1f, 0.5f);
        bezierCurve(-13.0f, -80.0f, -8.0f, -20.0f, 8.0f, 20.0f, 13.0f, 80.0f, 30);

        glColor3f(0.85f, 0.85f, 0.85f);
        bresenhamCircle(-13.0f, -80.0f, 4.0f);
        bresenhamCircle(13.0f, 80.0f, 4.0f);

        // Cables eléctricos
        glColor3f(1.0f, 1.0f, 0.0f);
        glLineWidth(1.0f);
        for (int i = 0; i < 6; i++) {
            float yPos = -60.0f + i * 20.0f;
            glBegin(GL_LINE_STRIP);
            glNormal3f(0.0f, 0.0f, 1.0f);  // ← NORMAL
            glVertex3f(-13.0f, yPos, 0.0f);
            glNormal3f(0.0f, 0.0f, 1.0f);  // ← NORMAL
            glVertex3f(13.0f, yPos, 0.0f);
            glEnd();
        }

        // Sistema A/C
        glColor3f(0.1f, 0.8f, 0.8f);
        bezierTube(-10.0f, 90.0f, -5.0f, 40.0f, 5.0f, 30.0f, 10.0f, 95.0f, 30, 10.0f);

        glLineWidth(1.5f);
        glColor3f(0.0f, 0.4f, 0.4f);
        bezierCurve(-10.0f, 90.0f, -5.0f, 40.0f, 5.0f, 30.0f, 10.0f, 95.0f, 30);

        glColor3f(0.85f, 0.85f, 0.85f);
        bresenhamCircle(-10.0f, 90.0f, 4.0f);
        bresenhamCircle(10.0f, 95.0f, 4.0f);

        glLineWidth(1.0f);
    }

    // generateCabin - con normales
    void generateCabin() {
        drawFuselageOutline();

        // Asientos piloto/copiloto
        glColor3f(0.85f, 0.2f, 0.2f);
        float pilotY = 75.0f;
        float pilotX[2] = { -4.0f, 4.0f };
        for (int i = 0; i < 2; i++) {
            float x = pilotX[i];
            glBegin(GL_QUADS);
            glNormal3f(0.0f, 0.0f, 1.0f);  // ← NORMAL
            glVertex3f(x - 2.0f, pilotY - 3.0f, 0.0f);
            glNormal3f(0.0f, 0.0f, 1.0f);  // ← NORMAL
            glVertex3f(x + 2.0f, pilotY - 3.0f, 0.0f);
            glNormal3f(0.0f, 0.0f, 1.0f);  // ← NORMAL
            glVertex3f(x + 2.0f, pilotY + 3.0f, 0.0f);
            glNormal3f(0.0f, 0.0f, 1.0f);  // ← NORMAL
            glVertex3f(x - 2.0f, pilotY + 3.0f, 0.0f);
            glEnd();
        }

        // Cabina pasajeros
        glColor3f(0.85f, 0.2f, 0.2f);
        int rows = 10;
        float yStart = 50.0f;
        float rowSpacing = 9.5f;
        float seatW = 4.0f, seatH = 7.0f;
        float colX[4] = { -10.0f, -4.5f, 4.5f, 10.0f };

        for (int row = 0; row < rows; row++) {
            float y = yStart - row * rowSpacing;
            for (int c = 0; c < 4; c++) {
                float x = colX[c];
                glBegin(GL_QUADS);
                glNormal3f(0.0f, 0.0f, 1.0f);  // ← NORMAL
                glVertex3f(x - seatW / 2, y - seatH / 2, 0.0f);
                glNormal3f(0.0f, 0.0f, 1.0f);  // ← NORMAL
                glVertex3f(x + seatW / 2, y - seatH / 2, 0.0f);
                glNormal3f(0.0f, 0.0f, 1.0f);  // ← NORMAL
                glVertex3f(x + seatW / 2, y + seatH / 2, 0.0f);
                glNormal3f(0.0f, 0.0f, 1.0f);  // ← NORMAL
                glVertex3f(x - seatW / 2, y + seatH / 2, 0.0f);
                glEnd();
            }
        }

        // Pasillo
        glColor3f(0.4f, 0.4f, 0.4f);
        glLineWidth(1.0f);
        glBegin(GL_LINES);
        glNormal3f(0.0f, 0.0f, 1.0f);  // ← NORMAL
        glVertex3f(0.0f, yStart + 5.0f, 0.0f);
        glNormal3f(0.0f, 0.0f, 1.0f);  // ← NORMAL
        glVertex3f(0.0f, yStart - rows * rowSpacing, 0.0f);
        glEnd();
    }

    // drawFuselageOutline - con normales
    void drawFuselageOutline() {
        glColor3f(0.6f, 0.6f, 0.6f);
        glLineWidth(1.5f);
        glBegin(GL_LINE_LOOP);
        glNormal3f(0.0f, 0.0f, 1.0f);  // ← NORMAL
        glVertex3f(0.0f, 110.0f, 0.0f);
        glNormal3f(0.0f, 0.0f, 1.0f);  // ← NORMAL
        glVertex3f(15.0f, 60.0f, 0.0f);
        glNormal3f(0.0f, 0.0f, 1.0f);  // ← NORMAL
        glVertex3f(15.0f, -50.0f, 0.0f);
        glNormal3f(0.0f, 0.0f, 1.0f);  // ← NORMAL
        glVertex3f(4.5f, -110.0f, 0.0f);
        glNormal3f(0.0f, 0.0f, 1.0f);  // ← NORMAL
        glVertex3f(-4.5f, -110.0f, 0.0f);
        glNormal3f(0.0f, 0.0f, 1.0f);  // ← NORMAL
        glVertex3f(-15.0f, -50.0f, 0.0f);
        glNormal3f(0.0f, 0.0f, 1.0f);  // ← NORMAL
        glVertex3f(-15.0f, 60.0f, 0.0f);
        glEnd();
        glLineWidth(1.0f);
    }

} // namespace GeometryBuilder