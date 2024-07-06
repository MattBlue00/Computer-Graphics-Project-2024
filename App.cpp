// HEADERS AND FUNCTION PROTOTYPES

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

// imported here because it needs to see UBO and GUBO (which are in Utils.hpp)
#include "modules/Scene.hpp"            // scene header (from professor)
#include <glm/gtc/type_ptr.hpp>
#include <glm/gtx/string_cast.hpp>
#include <glm/gtx/compatibility.hpp>

// used to set lights, camera position and direction
void updateGUBO(GlobalUniformBufferObject* gubo, glm::vec3 dampedCamPos);

// TEXT TO SHOW

std::vector<SingleText> outText = {
    {1, {"Third Person", "Press SPACE to change view", "", ""}, 0, 0},
    {1, {"First Person", "Press SPACE to change view", "", ""}, 0, 0},
};

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

    // Text Maker
    TextMaker txt;

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
        initialBackgroundColor = { 0.145f, 0.157f, 0.314f, 1.0f }; // dark blue

        // Descriptor pool sizes
        uniformBlocksInPool = 100; // FIXME
        texturesInPool = 100; // FIXME
        setsInPool = 100; // FIXME

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
            P.init(this, &VD, "shaders/PhongVert.spv", "shaders/PhongFrag.spv", { &DSL });
            P.setAdvancedFeatures(VK_COMPARE_OP_LESS_OR_EQUAL, VK_POLYGON_MODE_FILL,
                VK_CULL_MODE_NONE, false);

            // Load Scene
            SC.init(this, &VD, DSL, P, "models/scene.json");

            // updates the text
            txt.init(this, &outText);

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

            // creates the physics world
            initPhysics();
    }

    // Here you create your pipelines and Descriptor Sets!
    void pipelinesAndDescriptorSetsInit() {
        // This creates a new pipeline (with the current surface), using its shaders
        P.create();

        // Here you define the data set
        SC.pipelinesAndDescriptorSetsInit(DSL);
        txt.pipelinesAndDescriptorSetsInit();
    }

    // Here you destroy your pipelines and Descriptor Sets!
    // All the object classes defined in Starter.hpp have a method .cleanup() for this purpose
    void pipelinesAndDescriptorSetsCleanup() {
        // Cleanup pipelines
        P.cleanup();

        SC.pipelinesAndDescriptorSetsCleanup();
        txt.pipelinesAndDescriptorSetsCleanup();
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
        txt.localCleanup();
        cleanupPhysics;
    }

    // Here it is the creation of the command buffer:
    // You send to the GPU all the objects you want to draw,
    // with their buffers and textures

    void populateCommandBuffer(VkCommandBuffer commandBuffer, int currentImage) {
        // binds the pipeline
        P.bind(commandBuffer);

        SC.populateCommandBuffer(commandBuffer, currentImage, P);
        txt.populateCommandBuffer(commandBuffer, currentImage, currScene);
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

        float turningSpeed = 1.0f;

        // inits WASD and arrows user input
        glm::vec3 carMovementInput = ZERO_VEC3;
        glm::vec3 cameraRotationInput = ZERO_VEC3;

        // ???
        bool fire = false;

        // gets WASD and arrows input from user, and sets deltaT and fire
        getSixAxis(deltaT, carMovementInput, cameraRotationInput, fire);

        // accelerates or decelerates car according to user input
        updateCarMovement(carRigidBody, carMovementInput, deltaT, dynamicsWorld);

        updatePhysics(deltaT);

        btTransform transform;
        carRigidBody->getMotionState()->getWorldTransform(transform);
        glm::vec3 bodyPosition = glm::vec3(transform.getOrigin().getX(), transform.getOrigin().getY(), transform.getOrigin().getZ());
        btQuaternion rotation = transform.getRotation();
        float bodyYaw = atan2(2.0 * (rotation.getY() * rotation.getW() + rotation.getX() * rotation.getZ()),
            1.0 - 2.0 * (rotation.getY() * rotation.getY() + rotation.getX() * rotation.getX()));

        // inits the camera to third position view
        static CameraData cameraData = {};
        switchToThirdPersonCamera(&cameraData);


        // camera variables definition
        glm::mat4 M; // will be used as a return result when building view matrix
        glm::vec3 CamPos = Pos;
        static glm::vec3 dampedCamPos = CamPos; // MUST stay here

        

        // checks if space was pressed
        bool shouldRebuildPipeline = shouldChangeScene(window, &cameraData, &currScene, &debounce, &curDebounce, &dampedCamPos, Pos);
        // if so, rebuilds pipeline
        if (shouldRebuildPipeline) {
            RebuildPipeline();
        }

        // checks if esc was pressed
        shouldQuit(window);

        // checks if v was pressed
        shouldPrintDebugVariables(window, Pos, Yaw, cameraData, SteeringAng, &debounce, &curDebounce, std::bind(&App::printVec3, this, std::placeholders::_1, std::placeholders::_2));

        // updates camera position
        if (currScene == THIRD_PERSON_SCENE) {
            updateThirdPersonCamera(&cameraData, &CamPos, &dampedCamPos, &M, bodyYaw, AspectRatio, ROT_SPEED, deltaT, cameraRotationInput, carMovementInput, bodyPosition);
        }
        else {
            updateFirstPersonCamera(&cameraData, &M, bodyYaw, AspectRatio, ROT_SPEED, deltaT, cameraRotationInput, carMovementInput, bodyPosition);
        }

        glm::mat4 ViewPrj = M;
        UniformBufferObject ubo{};
        glm::mat4 baseCar = ONE_MAT4;

        // Here is where you actually update your uniforms

        GlobalUniformBufferObject gubo{};
        // sets lights, camera position and direction;
        updateGUBO(&gubo, dampedCamPos);

        // draws the car
        drawCar(&SC, &gubo, &ubo, currentImage, bodyYaw, bodyPosition, baseCar, ViewPrj, deltaP, deltaA, usePitch);

        // draws the circuit
        drawWorld(&SC, &gubo, &ubo, currentImage, bodyYaw, bodyPosition, baseCar, ViewPrj, deltaP, deltaA, usePitch);

    }
};

void updateGUBO(GlobalUniformBufferObject* gubo, glm::vec3 dampedCamPos) {
    // updates global uniforms
    gubo->lightDir = glm::vec3(cos(DEG_135), sin(DEG_135), 0.0f);
    gubo->lightColor = ONE_VEC4;
    gubo->eyePos = dampedCamPos;
    gubo->eyeDir = ZERO_VEC4;
    gubo->eyeDir.w = 1.0;
}

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
