#pragma once
#include <string>
#include <vector>
#include <atomic>
#include "model.h"
#include "model_renderer.h"

enum class AppState {
    LOADING, WELCOME, MENU, VIEWER
};

struct PlaneOption {
    std::string name;
    std::string filePath;
};

struct AppContext {
    AppState state = AppState::LOADING;

    float loadingProgress = 0.0f;
    bool assetsLoaded = false;

    float cameraYaw = 0.0f;
    float cameraDistance = 5.0f;
    glm::vec3 cameraTarget{ 0.0f, 0.0f, 0.0f };

    float cameraDefaultYaw = 0.0f;
    float cameraDefaultDistance = 5.0f;
    glm::vec3 cameraDefaultTarget{ 0.0f, 0.0f, 0.0f };

    bool showHelp = false;

    std::vector<PlaneOption> planes = {
        { "A-10 Thunderbolt II",   "../models/a-10_thunderbolt_ii.glb" },
        { "Boeing 787 Dreamliner", "../models/boeing-787-_dreamliner.glb" },
        { "B-24 Liberator",                "../models/b-24_liberator.glb" },
        { "MiG-29",                "../models/mig_29_9-13.glb" }
    };

    int selectedPlane = -1;

    std::vector<GLModel> gpuModels;
    bool modelsUploaded = false;
    float totalTime = 0.0f;

    // --- Carga en segundo plano ---
    std::vector<Model> loadedModels;
    std::atomic<int> modelsLoadedCount{ 0 };
    std::atomic<bool> loadingDone{ false };

    int windowWidth = 1280;
    int windowHeight = 720;

    double mouseX = 0.0, mouseY = 0.0;
    bool mousePressed = false;

    float deltaTime = 0.0f;
    float lastFrameTime = 0.0f;
};