#pragma once
#include <string>
#include <vector>

enum class AppState {
    LOADING,
    WELCOME,
    MENU,
    VIEWER
};

struct PlaneOption {
    std::string name;
    std::string filePath;
};

struct AppContext {
    AppState state = AppState::LOADING;

    // --- Loading screen ---
    float loadingProgress = 0.0f; // 0.0 a 1.0
    bool assetsLoaded = false;

    // --- Datos de aviones ---
    std::vector<PlaneOption> planes = {
        { "A-10 Thunderbolt II",     "models/a-10_thunderbolt_ii.glb" },
        { "Boeing 787 Dreamliner",   "models/boeing-787-_dreamliner.glb" },
        { "MiG-29",                  "models/mig_29_9-13.glb" }
    };
    int selectedPlane = -1;

    // --- Ventana ---
    int windowWidth = 1280;
    int windowHeight = 720;

    // --- Input ---
    double mouseX = 0.0, mouseY = 0.0;
    bool mousePressed = false; // true solo en el frame del click

    // --- Tiempo (para animaciones/progreso) ---
    float deltaTime = 0.0f;
    float lastFrameTime = 0.0f;
};