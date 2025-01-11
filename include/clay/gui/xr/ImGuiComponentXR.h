#pragma once
#ifdef CLAY_PLATFORM_VR
// third party
#include "imgui.h"
#include "imgui_impl_android.h"
#include "imgui_impl_opengl3.h"

namespace clay {

class ImGuiComponentXR {
public:
    ImGuiComponentXR();

    virtual ~ImGuiComponentXR() = default;

    static void initialize(ANativeWindow* pWindow);

    static void deinitialize();

    static void startRender();
    virtual void buildImGui();
    static void endRender();

    void render();
};

} // namespace clay
#endif