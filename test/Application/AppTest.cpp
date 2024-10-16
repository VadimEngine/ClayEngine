#include <gtest/gtest.h>
// ClayEngine
#include <Clay/Application/App.h>

TEST(AppTest, Initialization) {
    // Confirm an instance of app can be made
    clay::Resource::SRC_PATH = std::filesystem::current_path() / "res";
    clay::Resource::RESOURCE_PATH = std::filesystem::current_path() / "res";

    clay::App app;

    app.isRunning();
}