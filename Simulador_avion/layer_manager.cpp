// ============================================================
// ARCHIVO: layer_manager.cpp
// RESPONSABLE: Luis(ese tipo es vagisimo) (Gestión de capas)
// DESCRIPCION: Implementación de LayerManager - controla qué
//              capa (1-5) está activa y visible.
// ============================================================

#include "layer_manager.h"
#include <iostream>

// ============================================================
// Constructor - Inicializar valores por defecto
// ============================================================
LayerManager::LayerManager()
    : currentLayer(1)  // Capa 1 por defecto (Exterior)
{
    // Inicializar todas las capas como invisibles
    for (int i = 0; i < 5; i++) {
        layerVisible[i] = false;
    }
    // Capa 1 visible por defecto
    layerVisible[0] = true;
}

// ============================================================
// init() - Inicialización (puede llamarse en main si es necesario)
// ============================================================
void LayerManager::init()
{
    // Puede usarse para reinicializar estado
    currentLayer = 1;
    for (int i = 0; i < 5; i++) {
        layerVisible[i] = false;
    }
    layerVisible[0] = true;

    std::cout << "[LayerManager] Inicializado. Capa activa: " << currentLayer << "\n";
}

// ============================================================
// setActiveLayer() - Cambiar capa activa
// ============================================================
// Parámetros: layer (1-5)
// Efecto: Desactiva capa anterior, activa nueva capa
// ============================================================
void LayerManager::setActiveLayer(int layer)
{
    // Validar rango
    if (layer < 1 || layer > 5) {
        std::cerr << "[LayerManager ERROR] Capa inválida: " << layer << "\n";
        return;
    }

    // Desactivar capa anterior
    layerVisible[currentLayer - 1] = false;

    // Activar nueva capa
    currentLayer = layer;
    layerVisible[layer - 1] = true;

    std::cout << "[LayerManager] Capa " << currentLayer << " activada\n";
}

// ============================================================
// getActiveLayer() - Obtener capa activa actual
// ============================================================
// Retorna: número de capa (1-5)
// ============================================================
int LayerManager::getActiveLayer() const
{
    return currentLayer;
}

// ============================================================
// toggleVisibility() - Mostrar/ocultar una capa específica
// ============================================================
// Nota: Método adicional para controle fino (opcional)
// ============================================================
void LayerManager::toggleVisibility(int layer)
{
    if (layer < 1 || layer > 5) {
        std::cerr << "[LayerManager ERROR] Capa inválida: " << layer << "\n";
        return;
    }

    layerVisible[layer - 1] = !layerVisible[layer - 1];

    std::cout << "[LayerManager] Capa " << layer
        << (layerVisible[layer - 1] ? " visible" : " oculta") << "\n";
}

// ============================================================
// isVisible() - Consultar si una capa está visible
// ============================================================
bool LayerManager::isVisible(int layer) const
{
    if (layer < 1 || layer > 5) {
        return false;
    }
    return layerVisible[layer - 1];
}