#include <gtest/gtest.h>
// ClayEngine
#include <clay/application/App.h>

TEST(AppTest, Initialization) {
    // Confirm an instance of app can be made 
    LOG_I("Resource: %s", clay::Resource::RESOURCE_PATH.string().c_str());
    clay::App app;

    app.isRunning();
}