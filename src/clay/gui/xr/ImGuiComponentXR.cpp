#ifdef CLAY_PLATFORM_VR

// class
#include "clay/gui/xr/ImGuiComponentXR.h"

namespace clay {

ImGuiComponentXR::ImGuiComponentXR() {}

void ImGuiComponentXR::initialize(ANativeWindow* pWindow) {
    // Setup Dear ImGui context
    IMGUI_CHECKVERSION();
    ImGui::CreateContext();
    ImGuiIO& io = ImGui::GetIO();
    //disable .ini file generations
    io.IniFilename = nullptr;

    // Setup Dear ImGui style
    ImGui::StyleColorsDark();
    //ImGui::StyleColorsLight();

    // Setup Platform/Renderer backends
    ImGui_ImplAndroid_Init(pWindow);
    ImGui_ImplOpenGL3_Init("#version 300 es"); // TODO 320?
    // g_Initialized = true;
}

void ImGuiComponentXR::deinitialize() {
    ImGui_ImplOpenGL3_Shutdown();
    ImGui_ImplAndroid_Shutdown();
    ImGui::DestroyContext();
}

void ImGuiComponentXR::startRender() {
    ImGui_ImplOpenGL3_NewFrame();
    ImGui_ImplAndroid_NewFrame();
    ImGui::NewFrame();
}

void ImGuiComponentXR::buildImGui() {}

void ImGuiComponentXR::endRender() {
    ImGui::Render();
    ImGui_ImplOpenGL3_RenderDrawData(ImGui::GetDrawData());
}

void ImGuiComponentXR::render() {
    startRender();
    buildImGui();
    endRender();
}

} // namespace clay

#endif