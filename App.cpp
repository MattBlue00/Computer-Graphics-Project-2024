// HEADERS

#include "modules/engine/vulkan/Starter.hpp"                // vulkan starter header
#include "modules/engine/vulkan/TextMaker.hpp"              // text header
#include "Utils.hpp"                                        // constants and structs
#include "modules/managers/InputManager.hpp"                // responds to input
#include "modules/managers/SceneManager.hpp"                // updates scene
#include "modules/managers/CameraManager.hpp"               // handles camera movement
#include "modules/managers/CarManager.hpp"                  // handles car movement
#include "modules/managers/DrawManager.hpp"                 // draws the objects
#include "modules/managers/PhysicsManager.hpp"              // adds physics
#include "modules/managers/AudioManager.hpp"                // adds audio management
#include "modules/managers/LightsManager.hpp"               // adds lights management
#include "modules/engine/main/Scene.hpp"                    // scene header (from professor)
#include "modules/scenes/MainScene.hpp"                     // main scene
#include "modules/managers/UIManager.hpp"                   // manages UI
#include "modules/managers/GameManager.hpp"                 // manages game logic
#include "modules/data/EngineData.hpp"                      // global data that my custom engine uses
#include "modules/data/WorldData.hpp"                       // global data that my game modules use
#include "modules/engine/pattern/Receiver.hpp"              // class that receives signals and processes data
#include "modules/engine/pattern/Signal.hpp"                // signal that emits data
#include "modules/data/SignalTypes.hpp"                     // signal types

// MAIN APP

class App : public BaseProject {

protected:

    // Descriptor Layouts ["classes" of what will be passed to the shaders]
    DescriptorSetLayout DSL;

    // Vertex formats
    VertexDescriptor vertexDescriptor;

    // Pipelines [Shader couples]
    Pipeline phongPipeline;
    Pipeline cookTorrancePipeline;
    Pipeline toonPipeline;

    // Scene
    MainScene mainScene;

    // Managers
    UIManager uiManager;
    GameManager gameManager;
    LightsManager lightsManager;
    AudioManager audioManager;
    InputManager inputManager;
    SceneManager sceneManager;
    CameraManager cameraManager;
    DrawManager drawManager;
    PhysicsManager physicsManager;
    CarManager carManager;
    

    // Here you set the main application parameters
    void setWindowParameters() {
        // window size, titile and initial background
        windowWidth = 800;
        windowHeight = 600;
        windowTitle = "Rainbow Stadium: Time Attack!";
        windowResizable = GLFW_TRUE;
        initialBackgroundColor = {0.01f, 0.01f, 0.08f, 1.0f}; // dark blue
        
        // Descriptor pool sizes
        uniformBlocksInPool = 766;
        texturesInPool = 387;
        setsInPool = 387;

        EngineAspectRatio = 4.0f / 3.0f;
    }

    // What to do when the window changes size
    void onWindowResize(int w, int h) {
        std::cout << "Window resized to: " << w << " x " << h << "\n";
        EngineAspectRatio = (float)w / (float)h;
    }

    // Here you load and setup all your Vulkan Models and Textures.
    // Here you also create your Descriptor set layouts and load the shaders for the pipelines
    void localInit() {
        // Initialize ENGINE parameters
        EngineBaseProject = this;
        EngineWindow = window;
        
        // Descriptor Set Layout
        DSL.init(this, {
            {0, VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER, VK_SHADER_STAGE_ALL_GRAPHICS},
            {1, VK_DESCRIPTOR_TYPE_COMBINED_IMAGE_SAMPLER, VK_SHADER_STAGE_FRAGMENT_BIT},
            {2, VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER, VK_SHADER_STAGE_ALL_GRAPHICS}
        });
        
        // Vertex descriptors
        vertexDescriptor.init(this, {
                  {0, sizeof(Vertex), VK_VERTEX_INPUT_RATE_VERTEX}
            }, {
              {0, 0, VK_FORMAT_R32G32B32_SFLOAT, offsetof(Vertex, pos),
                     sizeof(glm::vec3), POSITION},
              {0, 1, VK_FORMAT_R32G32B32_SFLOAT, offsetof(Vertex, norm),
                     sizeof(glm::vec3), NORMAL},
              {0, 2, VK_FORMAT_R32G32_SFLOAT, offsetof(Vertex, UV),
                     sizeof(glm::vec2), UV}
            });
        
        // Pipelines init
        initPhongPipeline();
        initCookTorrancePipeline();
        initToonPipeline();

        // Load Scene
        mainScene.load("models/scene.json", &vertexDescriptor);
        mainScene.init();
        
        // init audio data from config file's path
        json config = parseConfigFile();
        audioData = config["audio"];
        
        // managers init
        inputManager.init();
        sceneManager.init();
        cameraManager.init();
        gameManager.init();
        uiManager.init();
        lightsManager.init();
        audioManager.init();
        drawManager.init();
        
        // add listeners
        
        quitSignal.addListener([this](std::string id, std::any data) {
            this->sceneManager.onSignal(quitSignal.getId(), data);
        });
        
        std::vector<Signal*> cameraManagerSignals = { &resetViewSignal, &changeCameraSignal, &updateDebounceSignal };
        for (Signal* signal : cameraManagerSignals) {
            signal->addListener([this, signal](std::string id, std::any data) {
                this->cameraManager.onSignal(signal->getId(), data);
            });
        }

        // creates the physics world
        physicsManager.init();
        carManager.init();
        
        std::vector<Signal*> gameManagerSignals = { &lapsSignal, &coinCollectedSignal };
        for (Signal* signal : gameManagerSignals) {
            signal->addListener([this, signal](std::string id, std::any data) {
                this->gameManager.onSignal(signal->getId(), data);
            });
        }
        
        std::vector<Signal*> uiManagerSignals = { &speedSignal, &timeSignal, &coinsSignal, &lapsSignal, &scoreSignal };
        for (Signal* signal : uiManagerSignals) {
            signal->addListener([this, signal](std::string id, std::any data) {
                this->uiManager.onSignal(signal->getId(), data);
            });
        }
        
        std::vector<Signal*> lightsManagerSignals = { &countdownSignal, &brakeSignal, &headlightsChangeSignal, &reverseSignal };
        for (Signal* signal : lightsManagerSignals) {
            signal->addListener([this, signal](std::string id, std::any data) {
                this->lightsManager.onSignal(signal->getId(), data);
            });
        }
        
        std::vector<Signal*> audioManagerSignals = { &countdownSignal, &coinsSignal, &lapsSignal };
        for (Signal* signal : audioManagerSignals) {
            signal->addListener([this, signal](std::string id, std::any data) {
                this->audioManager.onSignal(signal->getId(), data);
            });
        }
        
        countdownSignal.addListener([this](std::string id, std::any data) {
            this->carManager.onSignal(countdownSignal.getId(), data);
        });
        
        rebuildPipelineSignal.addListener([this](std::string id, std::any data) {
            this->RebuildPipeline();
        });
        
        std::cout << "Initialization completed!\n";
    }
    
    void initPhongPipeline(){
        // Pipeline [Shader couples]
        phongPipeline.init(this, &vertexDescriptor, "shaders/phong/PhongVert.spv", "shaders/phong/PhongFrag.spv", { &DSL });
        phongPipeline.setAdvancedFeatures(VK_COMPARE_OP_LESS_OR_EQUAL, VK_POLYGON_MODE_FILL,
            VK_CULL_MODE_NONE, false);
    }
    
    void initCookTorrancePipeline(){
        // Pipeline [Shader couples]
        cookTorrancePipeline.init(this, &vertexDescriptor, "shaders/cook_torrance/CookTorranceVert.spv", "shaders/cook_torrance/CookTorranceFrag.spv", { &DSL });
        cookTorrancePipeline.setAdvancedFeatures(VK_COMPARE_OP_LESS_OR_EQUAL, VK_POLYGON_MODE_FILL,
            VK_CULL_MODE_NONE, false);
    }
    
    void initToonPipeline(){
        // Pipeline [Shader couples]
        toonPipeline.init(this, &vertexDescriptor, "shaders/toon/ToonVert.spv", "shaders/toon/ToonFrag.spv", { &DSL });
        toonPipeline.setAdvancedFeatures(VK_COMPARE_OP_LESS_OR_EQUAL, VK_POLYGON_MODE_FILL,
            VK_CULL_MODE_NONE, false);
    }

    // Here you create your pipelines and Descriptor Sets!
    void pipelinesAndDescriptorSetsInit() {
        // This creates a new pipeline (with the current surface), using its shaders
        phongPipeline.create();
        cookTorrancePipeline.create();
        toonPipeline.create();

        // Here you define the data set
        mainScene.descriptorSetsInit(&DSL);
        uiManager.pipelinesAndDescriptorSetsInit();
    }

    // Here you destroy your pipelines and Descriptor Sets!
    // All the object classes defined in Starter.hpp have a method .cleanup() for this purpose
    void pipelinesAndDescriptorSetsCleanup() {
        std::cout << "Starting pipelines and descriptor sets cleanup.\n";
        // Cleanup pipelines
        phongPipeline.cleanup();
        cookTorrancePipeline.cleanup();
        toonPipeline.cleanup();

        mainScene.pipelinesAndDescriptorSetsCleanup();
        uiManager.pipelinesAndDescriptorSetsCleanup();
        std::cout << "Pipelines and descriptor sets cleanup completed.\n";
    }

    // Here you destroy all the Models, Texture and Desc. Set Layouts you created!
    // All the object classes defined in Starter.hpp have a method .cleanup() for this purpose
    // You also have to destroy the pipelines: since they need to be rebuilt, they have two different
    // methods: .cleanup() recreates them, while .destroy() delete them completely
    void localCleanup() {
        std::cout << "Starting local cleanup.\n";
        // Cleanup descriptor set layout
        DSL.cleanup();
        
        std::cout << "DSL cleanup completed.\n";

        // Destroys the pipelines
        phongPipeline.destroy();
        cookTorrancePipeline.destroy();
        toonPipeline.destroy();
        
        std::cout << "Pipelines destruction completed.\n";
        
        mainScene.localCleanup();
        
        std::cout << "Main scene cleanup completed.\n";
        
        gameManager.cleanup();
        uiManager.cleanup();
        physicsManager.cleanup();
        audioManager.cleanup();
        std::cout << "Local cleanup completed.\n";
    }

    // Here it is the creation of the command buffer:
    // You send to the GPU all the objects you want to draw,
    // with their buffers and textures

    void populateCommandBuffer(VkCommandBuffer commandBuffer, int currentImage) {
        // binds the pipeline
        phongPipeline.bind(commandBuffer);
        cookTorrancePipeline.bind(commandBuffer);
        toonPipeline.bind(commandBuffer);
        
        mainScene.populateCommandBuffer(commandBuffer, currentImage, {
            {PHONG, &phongPipeline},
            {COOK_TORRANCE, &cookTorrancePipeline},
            {TOON, &toonPipeline}
        });
    }
    
    void populateDynamicCommandBuffer(VkCommandBuffer commandBuffer, int currentImage) {
        uiManager.populateCommandBuffer(commandBuffer, currentImage);
    }

    // Here is where you update the uniforms.
    // Very likely this will be where you will be writing the logic of your application.
    void updateUniformBuffer(uint32_t currentImage) {
        EngineCurrentImage = currentImage;

        // inits WASD and arrows user input
        carMovementInput = ZERO_VEC3;
        cameraRotationInput = ZERO_VEC3;
        
        // gets WASD and arrows input from user and sets deltaT
        getSixAxis(EngineDeltaTime, carMovementInput, cameraRotationInput);

        carManager.update();
        physicsManager.update();
        
        vehicleTextureWorldMatrix = getCarTextureWorldMatrix(carWorldData);
        
        sceneManager.update();
        lightsManager.update();
        inputManager.update();
        cameraManager.update();
        gameManager.update();
        uiManager.update();
        audioManager.update();
        drawManager.update();
    }
    
};

// This is the main: probably you do not need to touch this!
int main() {
    App app;

    try {
        app.run();
    }
    catch (const std::exception& e) {
        std::cerr << e.what() << std::endl;
        return EXIT_FAILURE;
    }

    return EXIT_SUCCESS;
}
