#include "geometry.h"
#include <GL/glut.h>
#include <cmath>
#include <vector>
#include <algorithm>

namespace GeometryBuilder {

    // FUNCIONES DE APOYO (Algoritmos de rasterización entera y curvas)
 

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

    void bezierTube(float x0, float y0, float x1, float y1,
        float x2, float y2, float x3, float y3,
        int segments, float thickness) {
        glBegin(GL_QUAD_STRIP);
        for (int i = 0; i <= segments; i++) {
            float t = (float)i / (float)segments;
            float u = 1.0f - t;

            float bx = u * u * u * x0 + 3 * u * u * t * x1 + 3 * u * t * t * x2 + t * t * t * x3;
            float by = u * u * u * y0 + 3 * u * u * t * y1 + 3 * u * t * t * y2 + t * t * t * y3;

            glVertex3f(bx, by + thickness / 2.0f, 0.0f); // Borde superior
            glVertex3f(bx, by - thickness / 2.0f, 0.0f); // Borde inferior
        }
        glEnd();
    }

    bool clipLineToViewport(float& x0, float& y0, float& x1, float& y1,
                            float xmin, float ymin, float xmax, float ymax) {
        return true;
    }






    // generateFuselage() - Capa 1: Exterior (SOLUCIÓN AL FILAMENTO)
    // Se inyectan de forma pareada los vértices izquierdo y derecho
    // en el bucle longitudinal para que GL_QUAD_STRIP construya masa sólida real.

    void generateFuselage(float centerX, float centerY, float radius, float length) {
        float rMax = 15.0f;
        float yInicio = -110.0f;
        float yFinal = 110.0f;

        // Cuerpo del fuselaje: poligono plano de un solo color, con
        // perfil afilado en nariz y cola (mismos puntos de quiebre
        // que ya tenias: nariz desde y=60, cola desde y=-50).
        glColor3f(0.85f, 0.85f, 0.85f); // Gris claro, color plano

        glBegin(GL_POLYGON);
        glVertex3f(centerX, yFinal, 0.0f);                    // Punta de nariz
        glVertex3f(centerX + rMax, 60.0f, 0.0f);               // Inicio afilado nariz
        glVertex3f(centerX + rMax, -50.0f, 0.0f);              // Lado derecho recto
        glVertex3f(centerX + rMax * 0.3f, yInicio, 0.0f);      // Cola (afilado parcial)
        glVertex3f(centerX - rMax * 0.3f, yInicio, 0.0f);      // Cola (afilado parcial)
        glVertex3f(centerX - rMax, -50.0f, 0.0f);              // Lado izquierdo recto
        glVertex3f(centerX - rMax, 60.0f, 0.0f);               // Inicio afilado nariz
        glEnd();

        // --- ESTABILIZADORES DE COLA (se quedan igual, ya estan bien) ---
        glColor3f(0.85f, 0.85f, 0.85f);

            // Triangulo negro en la punta de la nariz (mismo estilo que el
            // intake de los motores), simulando ventana/detalle frontal
        glColor3f(0.05f, 0.05f, 0.05f);
        glBegin(GL_TRIANGLES);
        glVertex3f(centerX, yFinal, 0.0f);           // Punta de la nariz
        glVertex3f(centerX - 5.0f, yFinal - 15.0f, 0.0f);
        glVertex3f(centerX + 5.0f, yFinal - 15.0f, 0.0f);
        glEnd();

        glBegin(GL_QUADS);
        glVertex3f(centerX + 5.0f, -80.0f, 0.0f);
        glVertex3f(centerX + 42.0f, -105.0f, 0.0f);
        glVertex3f(centerX + 39.0f, -112.0f, 0.0f);
        glVertex3f(centerX + 0.0f, -102.0f, 0.0f);
        glEnd();

        glBegin(GL_QUADS);
        glVertex3f(centerX - 5.0f, -80.0f, 0.0f);
        glVertex3f(centerX - 0.0f, -102.0f, 0.0f);
        glVertex3f(centerX - 39.0f, -112.0f, 0.0f);
        glVertex3f(centerX - 42.0f, -105.0f, 0.0f);
        glEnd();
    }

    // ------------------------------------------------------------
    // generateWings() - Capa 1: Exterior
    // Construcción acoplada de las superficies alares de flujo en flecha.
    // ------------------------------------------------------------
    void generateWings() {
       
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

  
    }

    // ------------------------------------------------------------
    // generateMotors() - Capa 1 / Capa 5: Propulsión
    // Integra los dos turbofans CFM56 acoplados sólidamente bajo el extradós alar.
    // ------------------------------------------------------------
    void generateMotors() {
        float positions[2] = { -65.0f, 65.0f };
        for (int m = 0; m < 2; m++) {
            float cx = positions[m];
            float cy = -20.0f;

            // Cuerpo del motor - circulo aproximado, color plano
            glColor3f(0.35f, 0.35f, 0.35f);
            glBegin(GL_POLYGON);
            for (int i = 0; i < 20; i++) {
                float angle = (i / 20.0f) * 2.0f * 3.14159f;
                glVertex3f(cx + 10.0f * cos(angle), cy + 18.0f * sin(angle), 0.0f);
            }
            glEnd();

            // Intake (frente del motor) - triangulo negro, mas grande
            // para que se note bien en ambas capas (1 y 5)
            glColor3f(0.02f, 0.02f, 0.02f);
            glBegin(GL_TRIANGLES);
            glVertex3f(cx, cy + 18.0f, 0.0f);
            glVertex3f(cx - 8.0f, cy + 4.0f, 0.0f);
            glVertex3f(cx + 8.0f, cy + 4.0f, 0.0f);
            glEnd();
        }
    }
    void generateLandingGear() {
        drawFuselageOutline(); // Contexto visual, igual que en capas 2-4

        // Patas del tren (puntales) - negro, gruesas
        glColor3f(0.05f, 0.05f, 0.05f);
        glLineWidth(3.0f);
        glBegin(GL_LINES);
        glVertex3f(-11.0f, -20.0f, 0.0f); glVertex3f(-11.0f, -42.0f, 0.0f);
        glVertex3f(11.0f, -20.0f, 0.0f);  glVertex3f(11.0f, -42.0f, 0.0f);
        glVertex3f(0.0f, 55.0f, 0.0f);    glVertex3f(0.0f, 75.0f, 0.0f);
        glEnd();
        glLineWidth(1.0f);

        // Ruedas - rellenas (poligono circular), no solo contorno
        glColor3f(0.05f, 0.05f, 0.05f);
        float wheelPositions[3][2] = {
            { -11.0f, -46.0f }, // Rueda principal izquierda
            {  11.0f, -46.0f }, // Rueda principal derecha
            {  0.0f,  79.0f }   // Rueda de nariz
        };
        float wheelRadii[3] = { 4.5f, 4.5f, 3.0f };

        for (int w = 0; w < 3; w++) {
            glBegin(GL_POLYGON);
            for (int i = 0; i < 16; i++) {
                float angle = (i / 16.0f) * 2.0f * 3.14159f;
                glVertex3f(wheelPositions[w][0] + wheelRadii[w] * cos(angle),
                    wheelPositions[w][1] + wheelRadii[w] * sin(angle), 0.0f);
            }
            glEnd();
        }

        // Aro claro sobre cada rueda (detalle tipo llanta)
        glColor3f(0.5f, 0.5f, 0.5f);
        glLineWidth(1.0f);
        for (int w = 0; w < 3; w++) {
            bresenhamCircle(wheelPositions[w][0], wheelPositions[w][1], wheelRadii[w] * 0.5f);
        }
    }

void generateStructure() {
    // Costillas (ribs) - semicirculos, Verde, mas espaciadas para
    // evitar solapamiento tipo "resorte"
    glColor3f(0.0f, 1.0f, 0.0f);
    glLineWidth(1.5f);
    for (int i = 0; i < 6; i++) {
        float yPos = -100.0f + i * 40.0f;
        bresenhamSemicircle(0.0f, yPos, 12.0f);
    }

    // Largueros (spars) - lineas rectas via DDA, Verde claro
    glColor3f(0.2f, 1.0f, 0.2f);
    ddaLine(0.0f, -110.0f, 0.0f, 110.0f);
    ddaLine(-10.0f, -100.0f, -10.0f, 100.0f);
    ddaLine(10.0f, -100.0f, 10.0f, 100.0f);

    // Marcos (frames) - circulos completos, Verde oscuro,
    // intercalados entre las costillas
    glColor3f(0.0f, 0.6f, 0.0f);
    for (int i = 0; i < 5; i++) {
        float yPos = -80.0f + i * 40.0f;
        bresenhamCircle(0.0f, yPos, 12.0f);
    }

    glLineWidth(1.0f);
}
void generateSystems() {
    drawFuselageOutline();

    // Tuberia hidraulica - tubo MAS ANCHO para que se vea con cuerpo
    glColor3f(0.0f, 0.3f, 0.9f);
    bezierTube(-13.0f, -80.0f, -8.0f, -20.0f, 8.0f, 20.0f, 13.0f, 80.0f, 30, 10.0f);

    glLineWidth(1.5f);
    glColor3f(0.0f, 0.1f, 0.5f);
    bezierCurve(-13.0f, -80.0f, -8.0f, -20.0f, 8.0f, 20.0f, 13.0f, 80.0f, 30);

    glColor3f(0.85f, 0.85f, 0.85f);
    bresenhamCircle(-13.0f, -80.0f, 4.0f);
    bresenhamCircle(13.0f, 80.0f, 4.0f);

    // Cables electricos
    glColor3f(1.0f, 1.0f, 0.0f);
    glLineWidth(1.0f);
    for (int i = 0; i < 6; i++) {
        float yPos = -60.0f + i * 20.0f;
        glBegin(GL_LINE_STRIP);
        glVertex3f(-13.0f, yPos, 0.0f);
        glVertex3f(13.0f, yPos, 0.0f);
        glEnd();
    }

    // Sistema de aire acondicionado - ducto MAS ANCHO
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
void generateCabin() {
    drawFuselageOutline();

    // Asientos de piloto y copiloto - mas pequenos, ubicados en la
    // zona donde el fuselaje aun tiene ancho suficiente (y=75, el
    // radio ahi es ~10.5, por eso el ancho/posicion es menor que
    // los asientos de pasajeros)
    glColor3f(0.85f, 0.2f, 0.2f);
    float pilotY = 75.0f;
    float pilotX[2] = { -4.0f, 4.0f };
    for (int i = 0; i < 2; i++) {
        float x = pilotX[i];
        glBegin(GL_QUADS);
        glVertex3f(x - 2.0f, pilotY - 3.0f, 0.0f);
        glVertex3f(x + 2.0f, pilotY - 3.0f, 0.0f);
        glVertex3f(x + 2.0f, pilotY + 3.0f, 0.0f);
        glVertex3f(x - 2.0f, pilotY + 3.0f, 0.0f);
        glEnd();
    }

    // Cabina de pasajeros - 4 columnas (2 izq, 2 der), desplazada
    // hacia atras para quedar dentro de la zona recta del fuselaje
    // (y entre 55 y -45, donde el radio es constante = 15)
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
            glVertex3f(x - seatW / 2, y - seatH / 2, 0.0f);
            glVertex3f(x + seatW / 2, y - seatH / 2, 0.0f);
            glVertex3f(x + seatW / 2, y + seatH / 2, 0.0f);
            glVertex3f(x - seatW / 2, y + seatH / 2, 0.0f);
            glEnd();
        }
    }

    // Pasillo central (linea gris tenue)
    glColor3f(0.4f, 0.4f, 0.4f);
    glLineWidth(1.0f);
    glBegin(GL_LINES);
    glVertex3f(0.0f, yStart + 5.0f, 0.0f);
    glVertex3f(0.0f, yStart - rows * rowSpacing, 0.0f);
    glEnd();
}

// drawFuselageOutline()
// Silueta tenue (solo contorno, gris oscuro) del fuselaje, usada
// como referencia de fondo en capas internas. No es la Capa 1
// real, solo guia visual para dar contexto espacial.

    void drawFuselageOutline() {
        glColor3f(0.6f, 0.6f, 0.6f); // Gris claro, contrasta con pista (0.25)
        glLineWidth(1.5f);
        glBegin(GL_LINE_LOOP);
        glVertex3f(0.0f, 110.0f, 0.0f);
        glVertex3f(15.0f, 60.0f, 0.0f);
        glVertex3f(15.0f, -50.0f, 0.0f);
        glVertex3f(4.5f, -110.0f, 0.0f);
        glVertex3f(-4.5f, -110.0f, 0.0f);
        glVertex3f(-15.0f, -50.0f, 0.0f);
        glVertex3f(-15.0f, 60.0f, 0.0f);
        glEnd();
        glLineWidth(1.0f);
    }

} // namespace GeometryBuilder