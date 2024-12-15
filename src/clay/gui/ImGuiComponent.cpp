// class
#include "clay/gui/ImGuiComponent.h"

namespace clay {

bool ImGuiComponent::sImguiInitialized_ = false;

ImGuiComponent::ImGuiComponent() {}

ImGuiComponent::~ImGuiComponent() {}

void ImGuiComponent::initializeImGui(GLFWwindow* window) {
    if (!sImguiInitialized_) {
        const char* glsl_version = "#version 330";

        IMGUI_CHECKVERSION();
        ImGui::CreateContext();
        ImGuiIO& io = ImGui::GetIO(); (void)io;
        //io.ConfigFlags |= ImGuiConfigFlags_NavEnableKeyboard;     // Enable Keyboard Controls
        //io.ConfigFlags |= ImGuiConfigFlags_NavEnableGamepad;      // Enable Gamepad Controls
        //disable .ini file generations
        io.IniFilename = nullptr;

        // Setup Platform/Renderer backends
        if (!ImGui_ImplGlfw_InitForOpenGL(window, true)) {
            LOG_E("ImGui_ImplGlfw_InitForOpenGL failed");
            throw std::runtime_error("ImGui_ImplGlfw_InitForOpenGL error");
        }
        if (!ImGui_ImplOpenGL3_Init(glsl_version)) {
            LOG_E("ImGui_ImplOpenGL3_Init failed");
            throw std::runtime_error("ImGui_ImplOpenGL3_Init error");
        }
        sImguiInitialized_ = true;
    }
}

void ImGuiComponent::deinitialize() {
    ImGui_ImplOpenGL3_Shutdown();
    ImGui_ImplGlfw_Shutdown();
    ImGui::DestroyContext();
    sImguiInitialized_ = false;
}

void ImGuiComponent::render() {
    startRender();
    buildImGui();
    endRender();
}

void ImGuiComponent::startRender() {
    ImGui_ImplOpenGL3_NewFrame();
    ImGui_ImplGlfw_NewFrame();
    ImGui::NewFrame();
}

void ImGuiComponent::buildImGui() {}

void ImGuiComponent::endRender() {
    ImGui::Render();
    ImGui_ImplOpenGL3_RenderDrawData(ImGui::GetDrawData());
}

void ImGuiComponent::setRemove(const bool remove) {
    mIsRemove_ = remove;
}

bool ImGuiComponent::isRemove() const {
    return mIsRemove_;
}

bool ImGuiComponent::mouseOnGUI() {
    return ImGui::GetIO().WantCaptureMouse;
}

bool ImGuiComponent::keyboardGUIFocus() {
    return ImGui::GetIO().WantCaptureKeyboard;
}

} // namespace clay