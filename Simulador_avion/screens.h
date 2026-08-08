#pragma once
#include "app_state.h"
struct GLFWwindow; 

namespace Screens {
    void renderLoading(AppContext& ctx);
    void renderWelcome(AppContext& ctx);
    void renderMenu(AppContext& ctx);
    void renderViewer(AppContext& ctx, GLFWwindow* window);
}
