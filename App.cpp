// HEADERS

// professor headers
#include "modules/engine/custom/Starter.hpp"          // vulkan starter header
#include "modules/engine/custom/TextMaker.hpp"        // text header

// our headers
#include "Utils.hpp"                    // constants and structs
#include "modules/managers/InputManager.hpp"      // responds to input
#include "modules/managers/SceneManager.hpp"      // updates scene
#include "modules/managers/CameraManager.hpp"           // handles camera movement
#include "modules/managers/CarManager.hpp"              // handles car movement
#include "modules/managers/DrawManager.hpp"           // draws the objects
#include "modules/managers/PhysicsManager.hpp"          // adds physics
#include "modules/managers/AudioManager.hpp"            // adds audio management
#include "modules/managers/LightsManager.hpp"           // adds lights management

// imported here because it needs to see UBO and GUBO (which are in Utils.hpp)
#include "modules/engine/custom/SceneLoader.hpp"            // scene header (from professor)
#include "modules/managers/UIManager.hpp"

// PROTOTYPES DECLARATION

// used to set lights, camera position and direction
void updateGUBO(GlobalUniformBufferObject* gubo, glm::vec3 dampedCamPos);

// MAIN APP

class App : public BaseProject {

protected:

    // Descriptor Layouts ["classes" of what will be passed to the shaders]
    DescriptorSetLayout DSL;

    // Vertex formats
    VertexDescriptor VD;

    // Pipelines [Shader couples]
    Pipeline P;

    // Scene
    Scene SC;

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
    json sceneJson;
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
        uniformBlocksInPool = 800; // FIXME
        texturesInPool = 800; // FIXME
        setsInPool = 800; // FIXME

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
        P.init(this, &VD, "shaders/AmbientVert.spv", "shaders/AmbientFrag.spv", { &DSL });
        P.setAdvancedFeatures(VK_COMPARE_OP_LESS_OR_EQUAL, VK_POLYGON_MODE_FILL,
            VK_CULL_MODE_NONE, false);

        // Load Scene
        SC.init(this, &VD, DSL, P, "models/scene.json");
        
        json config = parseConfigFile();
        
        // managers init
        inputManager.init({window});
        sceneManager.init({window});
        cameraManager.init({});
        uiManager.init({this});
        lightsManager.init({});
        audioManager.init({&config["audio"]});
        
        lightsData = lightsManager.getLightsData();
        drawManager.init({&SC, &lightsData});
        
        // add observers
        inputManager.addObserversForSceneEvents({&sceneManager});
        inputManager.addObserversForLightEvents({&lightsManager});
        sceneManager.addObservers({&cameraManager});

        // creates the physics world
        sceneJson = SC.sceneJson;
        physicsManager.init({&sceneJson});
        
        dynamicsWorld = physicsManager.getDynamicsWorld();
        carManager.init({dynamicsWorld});
        
        // register the UI observers
        speedSubject.addObserver(&uiManager);
        collectedCoinsSubject.addObserver(&uiManager);
        checkLapsSubject.addObserver(&uiManager);
        
        // register the Light Observers, the timers are in UiManager
        uiManager.startTimerSubject.addObserver(&lightsManager);
        brakeSubject.addObserver(&lightsManager);
        
        // register the Audio Observers
        collectedCoinsSubject.addObserver(&audioManager);
        uiManager.startTimerSubject.addObserver(&audioManager);
        checkLapsSubject.addObserver(&audioManager);
        
        changeCircuitSubject.addObserver(&physicsManager);
        
        std::cout << "Initialization completed!\n";
    }

    // Here you create your pipelines and Descriptor Sets!
    void pipelinesAndDescriptorSetsInit() {
        // This creates a new pipeline (with the current surface), using its shaders
        P.create();

        // Here you define the data set
        SC.pipelinesAndDescriptorSetsInit(DSL);
        uiManager.pipelinesAndDescriptorSetsInit();
    }

    // Here you destroy your pipelines and Descriptor Sets!
    // All the object classes defined in Starter.hpp have a method .cleanup() for this purpose
    void pipelinesAndDescriptorSetsCleanup() {
        // Cleanup pipelines
        P.cleanup();

        SC.pipelinesAndDescriptorSetsCleanup();
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
        P.destroy();

        SC.localCleanup();
        
        uiManager.cleanup();
        physicsManager.cleanup();
        audioManager.cleanup();
    }

    // Here it is the creation of the command buffer:
    // You send to the GPU all the objects you want to draw,
    // with their buffers and textures

    void populateCommandBuffer(VkCommandBuffer commandBuffer, int currentImage) {
        // binds the pipeline
        P.bind(commandBuffer);
        SC.populateCommandBuffer(commandBuffer, currentImage, P);
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

        // gets WASD and arrows input from user and sets deltaT
        getSixAxis(deltaT, carMovementInput, cameraRotationInput);

        // updates vehicle movement
        carManager.update({&carMovementInput, &deltaT});
        
        btRaycastVehicle* car = carManager.getVehicle();
        // applies a step in the physics simulation
        physicsManager.update({&deltaT, car});
        
        // get position, yaw and pitch of car rigid body
        glm::vec3 carPosition = carManager.getVehiclePosition();
        float yaw = carManager.getVehicleYaw();
        float pitch = carManager.getVehiclePitch();
        float roll = carManager.getVehicleRoll();
        
        glm::mat4 textureWm = getCarTextureWorldMatrix(carPosition, pitch, yaw, roll);
        
        // checks if space was pressed
        bool shouldRebuildPipeline;
        
        // manager updates
        lightsManager.update({&textureWm});
        inputManager.update({&shouldRebuildPipeline});
        cameraManager.update({&pitch, &yaw, &roll, &aspectRatio, &deltaT, &cameraRotationInput, &carMovementInput, &carPosition});
        uiManager.update({});
        audioManager.update({});

        // if so, rebuilds pipeline
        if(shouldRebuildPipeline){
            RebuildPipeline();
        }

        glm::vec3 cameraPosition = cameraManager.getCameraPosition();
        glm::mat4 viewProjection = cameraManager.getViewProjection();
        lightsData = lightsManager.getLightsData();
        drawManager.update({&currentImage, &pitch, &yaw, &roll, &carPosition, &cameraPosition, &viewProjection, &lightsData});

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
