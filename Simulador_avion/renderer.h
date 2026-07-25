#pragma once
#ifndef RENDERER_H
#define RENDERER_H

#include "geometry.h"
#include "model_loader.h"

namespace Renderer {

    // ========== INICIALIZACIÓN ==========
    void setupOpenGL();
    void initModel(const char* modelPath);
    void loadModelByNumber(int modelNumber);

    // ========== ILUMINACIÓN ==========
    void applyLighting();
    void applyColor(float r, float g, float b);

    // ========== DIBUJADO ==========
    void drawGrid();
    void drawLayer(int layerNumber);
    void drawLayerLabel(int layerNumber);

    // ========== ACCESO AL MODELO ==========
    Model* getLoadedModel();
    int getCurrentModelNumber();

} // namespace Renderer

#endif // RENDERER_H