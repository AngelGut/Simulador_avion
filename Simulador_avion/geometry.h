#pragma once
#ifndef GEOMETRY_H
#define GEOMETRY_H

#include <vector>
#include <string>

// ============================================================
// Boeing737Layer
// ------------------------------------------------------------
// Estructura de soporte para almacenar datos de geometria
// cuando se necesite guardar vertices en lugar de dibujar
// directo (ej. resultado de Bresenham antes de conectarlo
// con drawLine). No todas las funciones la usan: las que
// dibujan directo con glBegin/glEnd no la necesitan.
// ============================================================
struct Boeing737Layer {
    std::vector<float> vertices;
    std::vector<unsigned int> indices;
    float r, g, b;      // Color RGB de la capa/componente
    std::string name;
};

namespace GeometryBuilder {

    // --------------------------------------------------------
    // generateFuselage()
    // Tecnica: Algoritmo de Bresenham (circulo) para las
    // secciones transversales del cilindro del fuselaje.
    // Justificacion: es la unica pieza con curva real, por eso
    // Bresenham tiene sentido aqui y no en piezas rectangulares.
    // --------------------------------------------------------
    void generateFuselage(float centerX, float centerY, float radius, float length);

    // --------------------------------------------------------
    // generateWings()
    // Tecnica: Transformaciones matriciales (glTranslatef +
    // glScalef) para posicionar ala izq/der a partir de UNA
    // definicion base, en vez de hardcodear 2 sets de coordenadas.
    // Nota: no usa Bresenham (son polígonos rectos) ni Bezier
    // (no hay curvas en el borde del ala en este alcance).
    // --------------------------------------------------------
    void generateWings();

    // --------------------------------------------------------
    // generateStructure() -> Capa 2 (costillas, largueros, marcos)
    // Tecnica: DDA para los largueros (lineas rectas longitudinales),
    // en contraste con Bresenham usado en el fuselaje. Permite
    // justificar en la documentacion por que se compararon ambos
    // algoritmos de rasterizacion de lineas.
    // Los marcos (frames) reutilizan bresenhamCircle() del fuselaje.
    // --------------------------------------------------------
    void generateStructure();

    // --------------------------------------------------------
    // generateSystems() -> Capa 3 (hidraulica, electrica, A/C)
    // Tecnica: Curvas de Bezier para tuberias hidraulicas y ductos
    // de aire acondicionado (en vez de zig-zag de lineas rectas),
    // ya que representan mejor un tubo/cable real.
    // Justificacion: no se usa Bezier en fuselaje/alas porque esas
    // piezas ya tienen su tecnica asignada (Bresenham/poligonos)
    // y anadir Bezier ahi seria redundante.
    // --------------------------------------------------------
    void generateSystems();

    // --------------------------------------------------------
    // generateCabin() -> Capa 4 (asientos, pasillos, cockpit)
    // Tecnica: ninguna especial de rasterizacion; son rectangulos
    // simples via glVertex2f. Se usan transformaciones (traslacion)
    // para ubicar cada asiento a partir de una plantilla base.
    // --------------------------------------------------------
    void generateCabin();

    // --------------------------------------------------------
    // generateMotors() -> parte de Capa 1 y Capa 5
    // Tecnica: Bresenham (circulo) para el contorno cilindrico
    // del motor, igual que el fuselaje pero de menor radio.
    // --------------------------------------------------------
    void generateMotors();

    // --------------------------------------------------------
    // generateLandingGear() -> Capa 5
    // Tecnica: Bresenham (circulo) para las ruedas; lineas rectas
    // simples para los puntales (no requieren DDA ya que son
    // verticales puras, sin pendiente que discretizar).
    // --------------------------------------------------------
    void generateLandingGear();

    // --------------------------------------------------------
    // Funciones de apoyo (algoritmos de rasterizacion)
    // --------------------------------------------------------

    // Bresenham para circulos (fuselaje, motores, ruedas, marcos)
    void bresenhamCircle(float cx, float cy, float radius);

    // Bresenham para semicirculos (costillas)
    void bresenhamSemicircle(float cx, float cy, float radius);

    // DDA para lineas rectas (largueros / spars)
    void ddaLine(float x0, float y0, float x1, float y1);

    // Curva de Bezier cubica (tuberias, ductos) - 4 puntos de control
    void bezierCurve(float x0, float y0, float x1, float y1,
        float x2, float y2, float x3, float y3, int segments);

    // Clipping simple de linea contra el viewport del avion
    // Justificacion de uso: se aplica solo si algun componente
    // generado dinamicamente (ej. cables) pudiera salirse del
    // area visible; si no ocurre, se documenta como "no aplica"
    // en ARQ00X con la razon de por que no fue necesario.
    bool clipLineToViewport(float& x0, float& y0, float& x1, float& y1,
        float xmin, float ymin, float xmax, float ymax);

} // namespace GeometryBuilder

#endif // GEOMETRY_H