#pragma once
// standard lib
#include <stdexcept>
// third party
#include <GLFW/glfw3.h>
#include "imgui.h"
#include "imgui_impl_glfw.h"
#include "imgui_impl_opengl3.h"
// project
#include "Clay/Application/Logger.h"

namespace clay {

class ImGuiComponent {
public:
    /** Constructor */
    ImGuiComponent();

    /** Virtual destructor */
    virtual ~ImGuiComponent() = 0;

    /**
     * Initialize Imgui
     * @param window The Window this GUI is inside
     */
    static void initializeImGui(GLFWwindow* window);

    /** Deinitialize imgui. Used upon application closure */
    static void deinitialize();

    /** Begin the Imgui render protocol */
    static void startRender();

    /** Build the imgui component for every frame*/
    virtual void buildImGui();

    /**End the Imgui draw protocol */
    static void endRender();

    /** Call start-build-end Render*/
    void render();

    /**
     * Set the removal status of this component
     * @param window The Window this GUI is inside
     */
    void setRemove(const bool remove);

    /** Get if this component is marked for removal*/
    bool isRemove() const;

    /** If the mouse is currently on a GUI*/
    static bool mouseOnGUI();

    /** If the keyboard is focusing on a GUI component */
    static bool keyboardGUIFocus();

protected:
    /** If this component should be deleted */
    bool mIsRemove_ = false;

private:
    /** If imgui is already initalized */
    static bool sImguiInitialized_;
};

} // namespace clay