#include <gtest/gtest.h>
// ClayEngine
#include <Clay/Application/App.h>

TEST(AppTest, Initialization) {
    // Confirm an instance of app can be made 
    LOG_I("Resource: %s", clay::Resource::RESOURCE_PATH);
    clay::App app;

    app.isRunning();
}