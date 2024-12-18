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
#include "modules/engine/vulkan/Scene.hpp"                  // scene header (from professor)
#include "modules/scenes/MainScene.hpp"                     // main scene
#include "modules/managers/UIManager.hpp"                   // manages UI
#include "modules/managers/utils/ManagerInitData.hpp"       // utils
#include "modules/managers/utils/ManagerUpdateData.hpp"     // utils

// MAIN APP

class App : public BaseProject {

protected:

    // Descriptor Layouts ["classes" of what will be passed to the shaders]
    DescriptorSetLayout DSL;

    // Vertex formats
    VertexDescriptor VD;

    // Pipelines [Shader couples]
    Pipeline ambientPipeline;

    // Scene
    MainScene mainScene;

    // Managers
    UIManager uiManager;
    LightsManager lightsManager;
    AudioManager audioManager;
    InputManager inputManager;
    SceneManager sceneManager;
    CameraManager cameraManager;
    DrawManager drawManager;
    PhysicsManager physicsManager;
    CarManager carManager;

    // aspect ratio
    float aspectRatio;
    
    // global variables
    btDynamicsWorld* dynamicsWorld;
    LightsData lightsData;

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

        aspectRatio = 4.0f / 3.0f;
    }

    // What to do when the window changes size
    void onWindowResize(int w, int h) {
        std::cout << "Window resized to: " << w << " x " << h << "\n";
        aspectRatio = (float)w / (float)h;
    }

    // Here you load and setup all your Vulkan Models and Textures.
    // Here you also create your Descriptor set layouts and load the shaders for the pipelines
    void localInit() {
        // Descriptor Layouts [what will be passed to the shaders]
        DSL.init(this, {
                    {0, VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER, VK_SHADER_STAGE_ALL_GRAPHICS},
                    {1, VK_DESCRIPTOR_TYPE_COMBINED_IMAGE_SAMPLER, VK_SHADER_STAGE_FRAGMENT_BIT},
                    {2, VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER, VK_SHADER_STAGE_ALL_GRAPHICS}
            });

        // Vertex descriptors
        VD.init(this, {
                  {0, sizeof(Vertex), VK_VERTEX_INPUT_RATE_VERTEX}
            }, {
              {0, 0, VK_FORMAT_R32G32B32_SFLOAT, offsetof(Vertex, pos),
                     sizeof(glm::vec3), POSITION},
              {0, 1, VK_FORMAT_R32G32_SFLOAT, offsetof(Vertex, UV),
                     sizeof(glm::vec2), UV},
              {0, 2, VK_FORMAT_R32G32B32_SFLOAT, offsetof(Vertex, norm),
                     sizeof(glm::vec3), NORMAL}
            });

        // Pipelines [Shader couples]
        ambientPipeline.init(this, &VD, "shaders/AmbientVert.spv", "shaders/AmbientFrag.spv", { &DSL });
        ambientPipeline.setAdvancedFeatures(VK_COMPARE_OP_LESS_OR_EQUAL, VK_POLYGON_MODE_FILL,
            VK_CULL_MODE_NONE, false);

        // Load Scene
        mainScene.load(this, &VD, DSL, "models/scene.json");
        mainScene.init();
        
        json config = parseConfigFile();
        
        // managers init
        
        InputManagerInitData inputManagerInitData = InputManagerInitData(window);
        inputManager.init(&inputManagerInitData);
        
        SceneManagerInitData sceneManagerInitdata = SceneManagerInitData(window);
        sceneManager.init(&sceneManagerInitdata);
        
        CameraManagerInitData cameraManagerInitData = CameraManagerInitData();
        cameraManager.init(&cameraManagerInitData);
        
        UIManagerInitData uiManagerInitData = UIManagerInitData(this);
        uiManager.init(&uiManagerInitData);
        
        LightsManagerInitData lightsManagerInitData = LightsManagerInitData();
        lightsManager.init(&lightsManagerInitData);
        
        AudioManagerInitData audioManagerInitData = AudioManagerInitData(config["audio"]);
        audioManager.init(&audioManagerInitData);
        
        lightsData = lightsManager.getLightsData();
        DrawManagerInitData drawManagerInitData = DrawManagerInitData(mainScene.getGameObjects(), lightsData);
        drawManager.init(&drawManagerInitData);
        
        // add observers
        shouldQuitSubject.addObserver(&sceneManager);
        shouldChangeSceneSubject.addObserver(&sceneManager);
        shouldUpdateDebounce.addObserver(&sceneManager);
        
        shouldChangeView.addObserver(&cameraManager);
        resetViewSubject.addObserver(&cameraManager);

        // creates the physics world
        PhysicsManagerInitData physicsManagerInitData = PhysicsManagerInitData(mainScene.getSceneJson());
        physicsManager.init(&physicsManagerInitData);
        
        dynamicsWorld = physicsManager.getDynamicsWorld();
        CarManagerInitData carManagerInitData = CarManagerInitData(dynamicsWorld);
        carManager.init(&carManagerInitData);
        
        // register the UI observers
        speedSubject.addObserver(&uiManager);
        collectedCoinsSubject.addObserver(&uiManager);
        checkLapsSubject.addObserver(&uiManager);
        
        // register the Light Observers, the timers are in UiManager
        startTimerSubject.addObserver(&lightsManager);
        brakeSubject.addObserver(&lightsManager);
        shouldChangeHeadlightsStatus.addObserver(&lightsManager);
        
        // register the Audio Observers
        collectedCoinsSubject.addObserver(&audioManager);
        startTimerSubject.addObserver(&audioManager);
        checkLapsSubject.addObserver(&audioManager);
        
        changeCircuitSubject.addObserver(&physicsManager);
        
        startTimerSubject.addObserver(&carManager);
        
        std::cout << "Initialization completed!\n";
    }

    // Here you create your pipelines and Descriptor Sets!
    void pipelinesAndDescriptorSetsInit() {
        // This creates a new pipeline (with the current surface), using its shaders
        ambientPipeline.create();

        // Here you define the data set
        mainScene.pipelinesAndDescriptorSetsInit(DSL);
        uiManager.pipelinesAndDescriptorSetsInit();
    }

    // Here you destroy your pipelines and Descriptor Sets!
    // All the object classes defined in Starter.hpp have a method .cleanup() for this purpose
    void pipelinesAndDescriptorSetsCleanup() {
        // Cleanup pipelines
        ambientPipeline.cleanup();

        mainScene.pipelinesAndDescriptorSetsCleanup();
        uiManager.pipelinesAndDescriptorSetsCleanup();
    }

    // Here you destroy all the Models, Texture and Desc. Set Layouts you created!
    // All the object classes defined in Starter.hpp have a method .cleanup() for this purpose
    // You also have to destroy the pipelines: since they need to be rebuilt, they have two different
    // methods: .cleanup() recreates them, while .destroy() delete them completely
    void localCleanup() {
        // Cleanup descriptor set layouts
        DSL.cleanup();

        // Destroys the pipelines
        ambientPipeline.destroy();
        
        mainScene.localCleanup();
        
        uiManager.cleanup();
        physicsManager.cleanup();
        audioManager.cleanup();
    }

    // Here it is the creation of the command buffer:
    // You send to the GPU all the objects you want to draw,
    // with their buffers and textures

    void populateCommandBuffer(VkCommandBuffer commandBuffer, int currentImage) {
        // binds the pipeline
        ambientPipeline.bind(commandBuffer);
        mainScene.populateCommandBuffer(commandBuffer, currentImage, ambientPipeline);
    }
    
    void populateDynamicCommandBuffer(VkCommandBuffer commandBuffer, int currentImage) {
        uiManager.populateCommandBuffer(commandBuffer, currentImage, sceneManager.getCurrentScene());
    }

    // Here is where you update the uniforms.
    // Very likely this will be where you will be writing the logic of your application.
    void updateUniformBuffer(uint32_t currentImage) {

        // small amount of time that passes at every "update"
        float deltaT;

        // inits WASD and arrows user input
        glm::vec3 carMovementInput = ZERO_VEC3;
        glm::vec3 cameraRotationInput = ZERO_VEC3;

        // gamepad controls triggers
        bool sceneChanged = false;
        bool headlightsChanged = false;
        bool viewReset = false;
        
        // gets WASD and arrows input from user and sets deltaT
        getSixAxis(deltaT, carMovementInput, cameraRotationInput, &headlightsChanged, &sceneChanged, &viewReset);

        // updates vehicle movement
        CarManagerUpdateData carManagerUpdateData = CarManagerUpdateData(carMovementInput);
        carManager.update(&carManagerUpdateData);
        
        btRaycastVehicle* car = carManager.getVehicle();
        // applies a step in the physics simulation
        PhysicsManagerUpdateData physicsManagerUpdateData = PhysicsManagerUpdateData(deltaT, car);
        physicsManager.update(&physicsManagerUpdateData);
        
        // get position, yaw and pitch of car rigid body
        glm::vec3 carPosition = carManager.getVehiclePosition();
        float yaw = carManager.getVehicleYaw();
        float pitch = carManager.getVehiclePitch();
        float roll = carManager.getVehicleRoll();
        
        glm::mat4 textureWm = getCarTextureWorldMatrix(carPosition, pitch, yaw, roll);
        
        // checks if space was pressed
        bool shouldRebuildPipeline;
        
        // manager updates
        LightsManagerUpdateData lightsManagerUpdateData = LightsManagerUpdateData(textureWm);
        lightsManager.update(&lightsManagerUpdateData);
        
        InputManagerUpdateData inputManagerUpdateData = InputManagerUpdateData(&shouldRebuildPipeline, &headlightsChanged, &sceneChanged, &viewReset);
        inputManager.update(&inputManagerUpdateData);
        
        CameraManagerUpdateData cameraManagerUpdateData = CameraManagerUpdateData(aspectRatio, deltaT, cameraRotationInput, carMovementInput, PositionData(carPosition, yaw, pitch, roll));
        cameraManager.update(&cameraManagerUpdateData);
        
        UIManagerUpdateData uiManagerUpdateData = UIManagerUpdateData();
        uiManager.update(&uiManagerUpdateData);
        
        AudioManagerUpdateData audioManagerUpdateData = AudioManagerUpdateData();
        audioManager.update(&audioManagerUpdateData);

        // if so, rebuilds pipeline
        if(shouldRebuildPipeline){
            RebuildPipeline();
        }

        glm::vec3 cameraPosition = cameraManager.getCameraPosition();
        glm::mat4 viewProjection = cameraManager.getViewProjection();
        lightsData = lightsManager.getLightsData();
        PositionData positionData = PositionData(carPosition, yaw, pitch, roll);
        
        DrawManagerUpdateData drawManagerUpdateData = DrawManagerUpdateData(currentImage, positionData, cameraPosition, viewProjection, lightsData);
        drawManager.update(&drawManagerUpdateData);

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
