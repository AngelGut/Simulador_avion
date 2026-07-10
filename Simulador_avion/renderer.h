#pragma once
#ifndef RENDERER_H
#define RENDERER_H

#include "geometry.h"

namespace Renderer {

    // --------------------------------------------------------
    // setupOpenGL()
    // Inicializacion general: color de fondo, modo de linea,
    // antialiasing (GL_LINE_SMOOTH) y llamada a applyLighting().
    // Se invoca una sola vez, antes de glutMainLoop().
    // --------------------------------------------------------
    void setupOpenGL();

    // --------------------------------------------------------
    // applyLighting()
    // Configura luz ambiente + una luz direccional basica
    // (GL_LIGHT0). No se usan texturas ni materiales complejos,
    // solo GL_AMBIENT y GL_DIFFUSE por requerimiento del curso.
    // --------------------------------------------------------
    void applyLighting();

    // --------------------------------------------------------
    // applyColor()
    // Wrapper sobre glColor3f. Existe como funcion propia (en
    // vez de llamar glColor3f directo desde geometry.cpp) para
    // centralizar en un solo lugar cualquier logica futura de
    // material/color (ej. si se agregara glMaterialfv despues).
    // --------------------------------------------------------
    void applyColor(float r, float g, float b);

    // --------------------------------------------------------
    // drawGrid()
    // Dibuja una grilla de referencia de fondo (lineas grises
    // tenues) para dar contexto espacial a cada capa. Se dibuja
    // antes que la geometria del avion en cada frame.
    // --------------------------------------------------------
    void drawGrid();

    // --------------------------------------------------------
    // drawLayer()
    // Punto de entrada que llama al generador de geometry.cpp
    // correspondiente segun el numero de capa activa (1-5).
    // Es la funcion que display() [main.cpp, Luis] invoca cada
    // frame con LayerManager.getActiveLayer().
    // --------------------------------------------------------
    void drawLayer(int layerNumber);

    void drawLayerLabel(int layerNumber);
} // namespace Renderer

#endif // RENDERER_H