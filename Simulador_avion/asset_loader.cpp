// asset_loader.cpp
#include "asset_loader.h"
#include "model.h"
#include <thread>

void startAssetLoading(AppContext& ctx) {
    ctx.loadedModels.resize(ctx.planes.size());

    std::thread([&ctx]() {
        for (size_t i = 0; i < ctx.planes.size(); i++) {
            CPU::loadModel(ctx.planes[i].filePath, ctx.loadedModels[i]);
            ctx.modelsLoadedCount++;
        }
        ctx.loadingDone = true;
        }).detach();
}