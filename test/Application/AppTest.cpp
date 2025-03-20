// third party
#include <gtest/gtest.h>
// ClayEngine
#include <clay/utils/desktop/UtilsDesktop.h>
#include <clay/gui/desktop/WindowDesktop.h>
#include <clay/application/desktop/AppDesktop.h>
#include <clay/utils/common/Logger.h>


class DemoApp : public clay::AppDesktop {
public:
    DemoApp() : clay::AppDesktop() {}

    void loadResources() override {
        // load the shaders
        // Shaders
        mResources_.loadResource<clay::ShaderProgram>(
            {
                (clay::Resources::RESOURCE_PATH / "shaders/AssimpLight.vert").string() + ":VERTEX",
                (clay::Resources::RESOURCE_PATH / "shaders/AssimpLight.frag").string() + ":FRAGMENT"
            },
            "AssimpLight"
        );
        mResources_.loadResource<clay::ShaderProgram>(
            {
                (clay::Resources::RESOURCE_PATH / "shaders/Assimp.vert").string() + ":VERTEX",
                (clay::Resources::RESOURCE_PATH / "shaders/Assimp.frag").string() + ":FRAGMENT"
            },
            "Assimp"
        );
        mResources_.loadResource<clay::ShaderProgram>(
            {
                (clay::Resources::RESOURCE_PATH / "shaders/MVPTexShader.vert").string() + ":VERTEX",
                (clay::Resources::RESOURCE_PATH / "shaders/MVPTexShader.frag").string() + ":FRAGMENT"
            },
            "MVPTexShader"
        );
        mResources_.loadResource<clay::ShaderProgram>(
            {
                (clay::Resources::RESOURCE_PATH / "shaders/Text.vert").string() + ":VERTEX",
                (clay::Resources::RESOURCE_PATH / "shaders/Text.frag").string() + ":FRAGMENT"
            },
            "Text"
        );
        mResources_.loadResource<clay::ShaderProgram>(
            {
                (clay::Resources::RESOURCE_PATH / "shaders/MVPShader.vert").string() + ":VERTEX",
                (clay::Resources::RESOURCE_PATH / "shaders/MVPShader.frag").string() + ":FRAGMENT"
            },
            "MVPShader"
        );
        mResources_.loadResource<clay::ShaderProgram>(
            {
                (clay::Resources::RESOURCE_PATH / "shaders/TextureSurface.vert").string() + ":VERTEX",
                (clay::Resources::RESOURCE_PATH / "shaders/TextureSurface.frag").string() + ":FRAGMENT"
            },
            "TextureSurface"
        );
        mResources_.loadResource<clay::ShaderProgram>(
            {
                (clay::Resources::RESOURCE_PATH / "shaders/Blur.vert").string() + ":VERTEX",
                (clay::Resources::RESOURCE_PATH / "shaders/Blur.frag").string() + ":FRAGMENT"
            },
            "Blur"
        );
        mResources_.loadResource<clay::ShaderProgram>(
            {
                (clay::Resources::RESOURCE_PATH / "shaders/BloomFinal.vert").string() + ":VERTEX",
                (clay::Resources::RESOURCE_PATH / "shaders/BloomFinal.frag").string() + ":FRAGMENT"
            },
            "BloomFinal"
        );
        // Textures
        mResources_.loadResource<clay::Texture>({(clay::Resources::RESOURCE_PATH / "textures" /"Sprites.png").string()}, "SpriteSheet");
        mResources_.loadResource<clay::Texture>({(clay::Resources::RESOURCE_PATH / "textures" / "V.png").string()}, "SampleTexture");
        // Single white pixel
        std::vector<unsigned char> whitePixel{0xFF, 0xFF, 0xFF};
        mResources_.addResource(std::move(std::make_unique<clay::Texture>(*mGraphicsAPI_, whitePixel.data(), 1, 1, 3)), "Blank");

        // Mesh
        mResources_.loadResource<clay::Mesh>(
            { (clay::Resources::RESOURCE_PATH / "models" / "Cube.obj").string() },
            "Cube"
        );
        mResources_.addResource(
            std::move(std::make_unique<clay::Mesh>(
                *mGraphicsAPI_,
                std::vector<clay::Mesh::Vertex>{
                    // Position, normal, UV, tangent, bitangent
                    {{-.5f, -.5f, 0.0f}, {0.0f,  0.0f, -1.0f},  {0.0f,  0.0f},{0,0,0},{0,0,0}},
                    {{.5f, -.5f, 0.0f}, {0.0f,  0.0f, -1.0f},  {1.0f,  0.0f},{0,0,0},{0,0,0}},
                    {{.5f,  .5f, 0.0f}, {0.0f,  0.0f, -1.0f},  {1.0f,  1.0f},{0,0,0},{0,0,0}},
                    {{.5f,  .5f, 0.0f}, {0.0f,  0.0f, -1.0f},  {1.0f,  1.0f},{0,0,0},{0,0,0}},
                    {{-.5f,  .5f, 0.0f}, {0.0f,  0.0f, -1.0f},  {0.0f,  1.0f},{0,0,0},{0,0,0}},
                    {{-.5f, -.5f, 0.0f}, {0.0f,  0.0f, -1.0f},  {0.0f,  0.0f},{0,0,0},{0,0,0}},
                },
                std::vector<unsigned int>{
                    0,1,2,
                    3,4,5,
                }
            )),
            "RectPlane"
        );
        mResources_.loadResource<clay::Mesh>(
            { (clay::Resources::RESOURCE_PATH / "models" / "Sphere.obj").string() },
            "Sphere"
        );
        // Circle plane
        {
            int segments = 16; // Edges on Circle
            std::vector<clay::Mesh::Vertex> vertices;
            std::vector<unsigned int> indices;

            float radius = .5f;
            float x, y, z;
            for (int i = 0; i < segments; ++i) {
                float theta = 2.0f * 3.14159265358979323846264338327f * float(i) / float(segments);
                x = radius * cosf(theta);
                y = radius * sinf(theta);
                z = 0.0f;

                // UV coordinates
                float u = 0.5f * (x / radius + 1.0f);
                float v = 0.5f * (y / radius + 1.0f);

                // normal is always (0, 0, 1) for a plane
                vertices.push_back({{x, y, z}, {0.0f, 0.0f, 1.0f}, {u, v}, {0, 0, 0}, {0, 0, 0}});
            }

            // Triangulating the circle
            for (int i = 0; i < segments - 2; ++i) {
                indices.push_back(0);
                indices.push_back(i + 1);
                indices.push_back(i + 2);
            }

            // Closing the circle
            indices.push_back(0);
            indices.push_back(segments - 1);
            indices.push_back(1);

            mResources_.addResource(
                std::move(std::make_unique<clay::Mesh>(
                    *mGraphicsAPI_,
                    vertices,
                    indices
                )),
                "CircularPlane"
            );
        }
        // Audio
        if (mAudioManager_.isInitialized()) {
            mResources_.loadResource<clay::Audio>({ (clay::Resources::RESOURCE_PATH / "audio/beep_deep_1.wav").string() }, "Blip_Deep");
            mResources_.loadResource<clay::Audio>({ (clay::Resources::RESOURCE_PATH / "audio/Blip_1.wav").string() }, "Blip1");
            mResources_.loadResource<clay::Audio>({ (clay::Resources::RESOURCE_PATH / "audio/button_click_1.wav").string() }, "Button_click");
            mResources_.loadResource<clay::Audio>({ (clay::Resources::RESOURCE_PATH / "audio/PatakasWorld.wav").string() }, "PatakasWorld");
        }
        // Fonts
        mResources_.loadResource<clay::Font>({ (clay::Resources::RESOURCE_PATH / "fonts/Consolas.ttf").string() }, "Consolas");
        mResources_.loadResource<clay::Font>({ (clay::Resources::RESOURCE_PATH / "fonts/runescape_uf.ttf").string() }, "Runescape");

        // SpriteSheet
        mResources_.addResource(
            std::move(std::make_unique<clay::SpriteSheet>(
                *mResources_.getResource<clay::Texture>("SpriteSheet"),
                glm::ivec2{16,16}
            )),
            "SpriteSheet1"
        );
    }
};

class AppTest : public ::testing::Test {
protected:

    // Runs once before all test cases in this suite
    static void SetUpTestSuite() {}

    // Runs once after all test cases in this suite
    static void TearDownTestSuite() {}

    // Run once before each test 
    void SetUp() override {}

    // Run once after each test
    void TearDown() override {}
};

TEST(AppTest, Initialization) {
    // Confirm an instance of app can be made 
    LOG_I("Running test");
    clay::Resources::RESOURCE_PATH = TEST_RESOURCE_PATH;
    clay::Resources::loadFileToMemory = clay::utils::loadFileToMemory_desktop; 
    clay::Resources::loadImageFileToMemory = clay::utils::loadImageFileToMemory_desktop;
    LOG_I("Resource: %s", clay::Resources::RESOURCE_PATH.string().c_str());
    DemoApp theApp;

    std::unique_ptr<clay::WindowDesktop> pWindow = std::make_unique<clay::WindowDesktop>("ClayEngine Test", 800, 600);
    theApp.setWindow(std::move(pWindow));
    theApp.initialize();

    EXPECT_EQ(theApp.isRunning(), true) << "App should not be in a close state";
}