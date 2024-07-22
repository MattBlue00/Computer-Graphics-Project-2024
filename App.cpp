// HEADERS

// professor headers
#include "modules/Starter.hpp"          // vulkan starter header
#include "modules/TextMaker.hpp"        // text header

// our headers
#include "Utils.hpp"                    // constants and structs
#include "modules/Interaction.hpp"      // responds to input
#include "modules/Camera.hpp"           // handles camera movement
#include "modules/Car.hpp"              // handles car movement
#include "modules/Drawer.hpp"           // draws the objects
#include "modules/Physics.hpp"          // adds physics
#include "modules/Audio.hpp"            // adds audio management
#include "modules/Lights.hpp"           // adds lights management

// imported here because it needs to see UBO and GUBO (which are in Utils.hpp)
#include "modules/Scene.hpp"            // scene header (from professor)
#include "modules/UIManager.hpp"

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

    // ???
    glm::vec3** deltaP;

    // ???
    float* deltaA;

    // ???
    float* usePitch;

    // Managers
    UIManager uiManager;
    LightManager lightManager;
    AudioManager audioManager;
    
    // current scene
    int currScene = THIRD_PERSON_SCENE;

    // aspect ratio
    float AspectRatio;

    // Position
    glm::vec3 Pos;

    // Yaw
    float Yaw;

    // Initial position
    glm::vec3 InitialPos;

    // Here you set the main application parameters
    void setWindowParameters() {
        // window size, titile and initial background
        windowWidth = 800;
        windowHeight = 600;
        windowTitle = "Rainbow Stadium: Time Attack!";
        windowResizable = GLFW_TRUE;
        initialBackgroundColor = {0.01f, 0.01f, 0.08f, 1.0f}; // dark blue
        
        // Descriptor pool sizes
        uniformBlocksInPool = 700; // FIXME
        texturesInPool = 700; // FIXME
        setsInPool = 700; // FIXME

        AspectRatio = 4.0f / 3.0f;
    }

    // What to do when the window changes size
    void onWindowResize(int w, int h) {
        std::cout << "Window resized to: " << w << " x " << h << "\n";
        AspectRatio = (float)w / (float)h;
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
        
        // init the UI
        uiManager.init(this);
        
        // Init local variables
        Pos = glm::vec3(SC.I[SC.InstanceIds["car"]].Wm[3]);
        InitialPos = Pos;
        Yaw = 0;

        deltaP = (glm::vec3**)calloc(SC.InstanceCount, sizeof(glm::vec3*));
        deltaA = (float*)calloc(SC.InstanceCount, sizeof(float));
        usePitch = (float*)calloc(SC.InstanceCount, sizeof(float));
        for (int i = 0; i < SC.InstanceCount; i++) {
            deltaP[i] = new glm::vec3(SC.I[i].Wm[3]);
            deltaA[i] = 0.0f;
            usePitch[i] = 0.0f;
        }
        
        json config = parseConfigFile();

        // creates the physics world
        initPhysics(SC.sceneJson);
        
        initCar();
        
        // register the UI observers
        speedSubject.addObserver(&uiManager);
        collectedCoinsSubject.addObserver(&uiManager);
        checkLapsSubject.addObserver(&uiManager);
        
        // register the Light Observers, the timers are in UiManager
        uiManager.startTimerSubject.addObserver(&lightManager);
        brakeSubject.addObserver(&lightManager);
        
        // register the Audio Observers
        collectedCoinsSubject.addObserver(&audioManager);
        uiManager.startTimerSubject.addObserver(&audioManager);
        checkLapsSubject.addObserver(&audioManager);
        
        // initializes the audio system and loads the sounds
        audioManager.initAudio(config["audio"]);
        
        lightManager.initLights();
        
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
        for (int i = 0; i < SC.InstanceCount; i++) {
            delete deltaP[i];
        }
        free(deltaP);
        free(deltaA);
        free(usePitch);

        // Cleanup descriptor set layouts
        DSL.cleanup();

        // Destroys the pipelines
        P.destroy();

        SC.localCleanup();
        uiManager.localCleanup();
        cleanupPhysics();
        audioManager.cleanupAudio();
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
        uiManager.populateCommandBuffer(commandBuffer, currentImage, currScene);
    }

    // Here is where you update the uniforms.
    // Very likely this will be where you will be writing the logic of your application.
    void updateUniformBuffer(uint32_t currentImage) {

        // true if the scene is transitioning to another scene
        static bool debounce = false;

        // holds the pressed input key
        static int curDebounce = 0;

        // small amount of time that passes at every "update"
        float deltaT;

        // inits WASD and arrows user input
        glm::vec3 carMovementInput = ZERO_VEC3;
        glm::vec3 cameraRotationInput = ZERO_VEC3;

        // ???
        bool fire = false;

        // gets WASD and arrows input from user, and sets deltaT and fire
        getSixAxis(deltaT, carMovementInput, cameraRotationInput, fire);

        // updates vehicle movement
        updateVehicle(vehicle, carMovementInput, deltaT);

        // applies a step in the physics simulation
        updatePhysics(deltaT);
        
        // update lights
        lightManager.updateLightPosition();
        
        checkCollisions(vehicle, SC.sceneJson);
        
        audioManager.updateAudioSystem();

        // get position, yaw and pitch of car rigid body
        glm::vec3 bodyPosition = getVehiclePosition(vehicle);
        float bodyYaw = getVehicleYaw(vehicle);
        float bodyPitch = getVehiclePitch(vehicle);
        float bodyRoll = getVehicleRoll(vehicle);

        // inits the camera to third position view
        static CameraData cameraData = {};
        switchToThirdPersonCamera(&cameraData);

        // camera variables definition
        glm::mat4 M; // will be used as a return result when building view matrix
        glm::vec3 CamPos = Pos;
        static glm::vec3 dampedCamPos = CamPos; // MUST stay here

        uiManager.updateUI();
        
        // checks if space was pressed
        bool shouldRebuildPipeline = shouldChangeScene(window, &cameraData, &currScene, &debounce, &curDebounce, &dampedCamPos, Pos);

        // if so, rebuilds pipeline
        if(shouldRebuildPipeline){
            RebuildPipeline();
        }

        // checks if esc was pressed
        shouldQuit(window);

        // checks if v was pressed
        //shouldPrintDebugVariables(window, Pos, Yaw, cameraData, SteeringAng, &debounce, &curDebounce, std::bind(&App::printVec3, this, std::placeholders::_1, std::placeholders::_2));

        // updates camera position
        if (currScene == THIRD_PERSON_SCENE) {
            updateThirdPersonCamera(&cameraData, &CamPos, &dampedCamPos, &M, bodyYaw, bodyPitch, bodyRoll, AspectRatio, ROT_SPEED, deltaT, cameraRotationInput, carMovementInput, bodyPosition);
        }
        else {
            updateFirstPersonCamera(&cameraData, &M, bodyYaw, bodyPitch, bodyRoll, AspectRatio, ROT_SPEED, deltaT, cameraRotationInput, carMovementInput, bodyPosition);
        }

        glm::mat4 ViewPrj = M;
        UniformBufferObject ubo{};
        glm::mat4 baseCar = ONE_MAT4;
                        
        // Here is where you actually update your uniforms

        GlobalUniformBufferObject gubo{};
        // sets lights, camera position and direction;
        updateGUBO(&gubo, dampedCamPos);
        


        // draws every object of this app
        drawAll(&SC, &gubo, &ubo, currentImage, bodyYaw, bodyPosition, baseCar, ViewPrj, deltaP, deltaA, usePitch, bodyPitch, bodyRoll);

    }
    
    void updateGUBO(GlobalUniformBufferObject* gubo, glm::vec3 dampedCamPos) {
        // updates global uniforms
        gubo->ambientLightDir = glm::vec3(cos(DEG_135), sin(DEG_135), 0.0f);
        gubo->ambientLightColor = ONE_VEC4;
        gubo->eyeDir = ZERO_VEC4;
        gubo->eyeDir.w = 1.0;

        for (int i = 0; i < LIGHTS_COUNT; i++) {
            gubo->lightColor[i] = glm::vec4(lightManager.LightColors[i], lightManager.LightIntensities[i]);
            gubo->lightDir[i].v = lightManager.LightWorldMatrices[i] * glm::vec4(0, 0, 1, 0);
            gubo->lightPos[i].v = lightManager.LightWorldMatrices[i] * glm::vec4(0, 0, 0, 1);
            gubo->lightOn[i].v = lightManager.LightOn[i];
        }

        gubo->eyePos = dampedCamPos;
    }
};



// This is the main: probably you do not need to touch this!
int main() {
    //prova
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
