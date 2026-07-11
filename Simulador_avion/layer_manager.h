#pragma once
#ifndef LAYER_MANAGER_H
#define LAYER_MANAGER_H

class LayerManager
{
private:
    int currentLayer;
    bool layerVisible[5];

public:
    LayerManager();

    void init();

    void setActiveLayer(int layer);

    int getActiveLayer() const;

    void toggleVisibility(int layer);

    bool isVisible(int layer) const;
};

#endif