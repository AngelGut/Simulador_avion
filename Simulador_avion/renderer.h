#pragma once
#ifndef RENDERER_H
#define RENDERER_H

#include "geometry.h"
#include "model_loader.h"
#include <string>

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

    // ========== MODO PIEZAS ==========
    void togglePartsMode();
    void nextPart();
    void prevPart();
    bool isPartsModeActive();
    std::string getCurrentPartName();
    int getCurrentPartIndex();
    int getNumParts();

    // ========== UTILIDADES ==========
    void printHelp();

} // namespace Renderer

#endif // RENDERER_H