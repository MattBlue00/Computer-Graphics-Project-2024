// HEADERS AND DATA STRUCTURES

// professor headers
#include "modules/Starter.hpp"          // vulkan starter header
#include "modules/TextMaker.hpp"        // text header

// our headers
#include "modules/WVP.hpp"              // world view projections
#include "modules/Interaction.hpp"      // responses to input
#include "modules/Camera.hpp"           // handles camera data and changes

struct UniformBufferObject {
    alignas(16) glm::mat4 mvpMat;
    alignas(16) glm::mat4 mMat;
    alignas(16) glm::mat4 nMat;
};

struct GlobalUniformBufferObject {
    alignas(16) glm::vec3 lightDir;
    alignas(16) glm::vec4 lightColor;
    alignas(16) glm::vec3 eyePos;
    alignas(16) glm::vec4 eyeDir;
};

// imported here because it needs to see UBO and GUBO
#include "modules/Scene.hpp"            // scene header (from professor)

struct Vertex {
    glm::vec3 pos;
    glm::vec2 UV;
    glm::vec3 norm;
};

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

    Scene SC;
    glm::vec3 **deltaP;
    float *deltaA;
    float *usePitch;

    TextMaker txt;
    
    // Other application parameters
    int currScene = 0;
    float Ar;
    glm::vec3 Pos;
    float Yaw;
    glm::vec3 InitialPos;
    
    std::vector<std::string> car = {"car"};
    std::vector<std::string> world = {"world"};

    // Here you set the main application parameters
    void setWindowParameters() {
        // window size, titile and initial background
        windowWidth = 800;
        windowHeight = 600;
        windowTitle = "A04 - World View Projection";
        windowResizable = GLFW_TRUE;
        initialBackgroundColor = {0.0f, 0.85f, 1.0f, 1.0f};
        
        // Descriptor pool sizes
        uniformBlocksInPool = 19 * 2 + 2; // FIXME
        texturesInPool = 19 + 1; // FIXME
        setsInPool = 19 + 1; // FIXME

        Ar = 4.0f / 3.0f;
    }
    
    // What to do when the window changes size
    void onWindowResize(int w, int h) {
        std::cout << "Window resized to: " << w << " x " << h << "\n";
        Ar = (float)w / (float)h;
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
        P.init(this, &VD, "shaders/PhongVert.spv", "shaders/PhongFrag.spv", {&DSL});
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
        
        deltaP = (glm::vec3 **)calloc(SC.InstanceCount, sizeof(glm::vec3 *));
        deltaA = (float *)calloc(SC.InstanceCount, sizeof(float));
        usePitch = (float *)calloc(SC.InstanceCount, sizeof(float));
        for(int i=0; i < SC.InstanceCount; i++) {
            deltaP[i] = new glm::vec3(SC.I[i].Wm[3]);
            deltaA[i] = 0.0f;
            usePitch[i] = 0.0f;
        }
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
        for(int i=0; i < SC.InstanceCount; i++) {
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
        static bool debounce = false;
        static int curDebounce = 0;

        float deltaT;
        glm::vec3 m = glm::vec3(0.0f), r = glm::vec3(0.0f);
        bool fire = false;
        getSixAxis(deltaT, m, r, fire);
        
        static CameraData cameraData = {};
        cameraData.CamPitch = glm::radians(20.0f);
        cameraData.CamYaw   = M_PI;
        cameraData.CamDist  = 10.0f;
        cameraData.CamRoll  = 0.0f;
        
        const glm::vec3 CamTargetDelta = glm::vec3(0,2,0);
        const glm::vec3 Cam1stPos = glm::vec3(0.49061f, 2.07f, 2.7445f);
        
        static float SteeringAng = 0.0f;
        static float wheelRoll = 0.0f;
        static float dampedVel = 0.0f;
        
        const float STEERING_SPEED = glm::radians(30.0f);
        const float ROT_SPEED = glm::radians(120.0f);
        const float MOVE_SPEED = 2.5f;

        SteeringAng += -m.x * STEERING_SPEED * deltaT;
        SteeringAng = (SteeringAng < glm::radians(-35.0f) ? glm::radians(-35.0f) :
                      (SteeringAng > glm::radians(35.0f)  ? glm::radians(35.0f)  : SteeringAng));
        
        
        const float trailerL = 4.5f;
        static float tx = Pos.x - trailerL * sin(Yaw);
        static float tz = Pos.z - trailerL * cos(Yaw);

        glm::mat4 M;
        glm::vec3 CamPos = Pos;
        cameraData.dampedCamPos = CamPos;

        double lambdaVel = 8.0f;
        double dampedVelEpsilon = 0.001f;
        dampedVel =  MOVE_SPEED * deltaT * m.z * (1 - exp(-lambdaVel * deltaT)) +
                     dampedVel * exp(-lambdaVel * deltaT);
        dampedVel = ((fabs(dampedVel) < dampedVelEpsilon) ? 0.0f : dampedVel);
        wheelRoll -= dampedVel / 0.4;
        wheelRoll = (wheelRoll < 0.0 ? wheelRoll + 2*M_PI : (wheelRoll > 2*M_PI ? wheelRoll - 2*M_PI : wheelRoll));

        if(dampedVel != 0.0f) {
            glm::vec3 trailerPos = glm::vec3(tx, 0.0f, tz);
            glm::vec3 oldPos = Pos;
            
            if(SteeringAng != 0.0f) {
                const float l = 2.78f;
                float r = l / tan(SteeringAng);
                float cx = Pos.x + r * cos(Yaw);
                float cz = Pos.z - r * sin(Yaw);
                float Dbeta = dampedVel / r;
                Yaw = Yaw - Dbeta;
                Pos.x = cx - r * cos(Yaw);
                Pos.z = cz + r * sin(Yaw);
            } else {
                Pos.x = Pos.x - sin(Yaw) * dampedVel;
                Pos.z = Pos.z - cos(Yaw) * dampedVel;
            }
            if(m.x == 0) {
                if(SteeringAng > STEERING_SPEED * deltaT) {
                    SteeringAng -= STEERING_SPEED * deltaT;
                } else if(SteeringAng < -STEERING_SPEED * deltaT) {
                    SteeringAng += STEERING_SPEED * deltaT;
                } else {
                    SteeringAng = 0.0f;
                }
            }

            glm::vec3 deltaPos = Pos - oldPos;
            glm::vec3 trailerDir = glm::normalize(Pos - trailerPos);
            glm::vec3 trailerMove = glm::dot(deltaPos, trailerDir) * trailerDir;

            glm::vec3 preTrailerPos = trailerPos + trailerMove;
            glm::vec3 newTrailerDir = glm::normalize(preTrailerPos - Pos);
            trailerPos = Pos + trailerL * newTrailerDir;
            tx = trailerPos.x;
            tz = trailerPos.z;
        }
        
        // checks if space was pressed
        bool shouldRebuildPipeline = shouldChangeScene(window, &cameraData, &currScene, &debounce, &curDebounce, Pos);
        // if so, rebuilds pipeline
        if(shouldRebuildPipeline){
            RebuildPipeline();
        }

        // checks if esc was pressed
        shouldQuit(window);
        
        if(glfwGetKey(window, GLFW_KEY_V)) {
            if(!debounce) {
                debounce = true;
                curDebounce = GLFW_KEY_V;

                printVec3("Pos = ", Pos);
                std::cout << "Yaw         = " << Yaw         << ";\n";
                std::cout << "CamPitch    = " << cameraData.CamPitch    << ";\n";
                std::cout << "CamYaw      = " << cameraData.CamYaw      << ";\n";
                std::cout << "CamRoll     = " << cameraData.CamRoll     << ";\n";
                std::cout << "CamDist     = " << cameraData.CamDist     << ";\n";
                std::cout << "SteeringAng = " << SteeringAng << ";\n";
                std::cout << "wheelRoll   = " << wheelRoll   << ";\n";
                std::cout << "tx   = " << tx   << ";\n";
                std::cout << "tz   = " << tz   << ";\n\n";
            }
        } else {
            if((curDebounce == GLFW_KEY_V) && debounce) {
                debounce = false;
                curDebounce = 0;
            }
        }
        
        if(currScene == 0) {
            cameraData.CamYaw += ROT_SPEED * deltaT * r.y;
            cameraData.CamPitch -= ROT_SPEED * deltaT * r.x;
            cameraData.CamRoll -= ROT_SPEED * deltaT * r.z;
            cameraData.CamDist -= MOVE_SPEED * deltaT * m.y;
        
            cameraData.CamYaw = (cameraData.CamYaw < 0.0f ? 0.0f : (cameraData.CamYaw > 2*M_PI ? 2*M_PI : cameraData.CamYaw));
            cameraData.CamPitch = (cameraData.CamPitch < 0.0f ? 0.0f : (cameraData.CamPitch > M_PI_2-0.01f ? M_PI_2-0.01f : cameraData.CamPitch));
            cameraData.CamRoll = (cameraData.CamRoll < -M_PI ? -M_PI : (cameraData.CamRoll > M_PI ? M_PI : cameraData.CamRoll));
            cameraData.CamDist = (cameraData.CamDist < 7.0f ? 7.0f : (cameraData.CamDist > 15.0f ? 15.0f : cameraData.CamDist));
                
            glm::vec3 CamTarget = Pos + glm::vec3(glm::rotate(glm::mat4(1), Yaw, Y_AXIS) *
                             glm::vec4(CamTargetDelta,1));
            CamPos = CamTarget + glm::vec3(glm::rotate(glm::mat4(1), Yaw + cameraData.CamYaw, Y_AXIS) * glm::rotate(glm::mat4(1), -cameraData.CamPitch, X_AXIS) *
                             glm::vec4(0,0,cameraData.CamDist,1));

            const float lambdaCam = 10.0f;
            cameraData.dampedCamPos = CamPos * (1 - exp(-lambdaCam * deltaT)) +
                cameraData.dampedCamPos * exp(-lambdaCam * deltaT);
            M = MakeViewProjectionLookAt(cameraData.dampedCamPos, CamTarget, Y_AXIS, cameraData.CamRoll, glm::radians(90.0f), Ar, 0.1f, 500.0f);
        } else {
            cameraData.CamYaw -= ROT_SPEED * deltaT * r.y;
            cameraData.CamPitch -= ROT_SPEED * deltaT * r.x;
            cameraData.CamRoll -= ROT_SPEED * deltaT * r.z;
        
            cameraData.CamYaw = (cameraData.CamYaw < M_PI_2 ? M_PI_2 : (cameraData.CamYaw > 1.5*M_PI ? 1.5*M_PI : cameraData.CamYaw));
            cameraData.CamPitch = (cameraData.CamPitch < -0.25*M_PI ? -0.25*M_PI : (cameraData.CamPitch > 0.25*M_PI ? 0.25*M_PI : cameraData.CamPitch));
            cameraData.CamRoll = (cameraData.CamRoll < -M_PI ? -M_PI : (cameraData.CamRoll > M_PI ? M_PI : cameraData.CamRoll));
                
            glm::vec3 Cam1Pos = Pos + glm::vec3(glm::rotate(glm::mat4(1), Yaw, Y_AXIS) *
                             glm::vec4(Cam1stPos,1));
            M = MakeViewProjectionLookInDirection(Cam1Pos, Yaw + cameraData.CamYaw, cameraData.CamPitch, cameraData.CamRoll, glm::radians(90.0f), Ar, 0.1f, 500.0f);
        }

        glm::mat4 ViewPrj = M;
        UniformBufferObject ubo{};
        glm::mat4 baseTr = glm::mat4(1.0f);
        // Here is where you actually update your uniforms

        // updates global uniforms
        GlobalUniformBufferObject gubo{};
        gubo.lightDir = glm::vec3(cos(glm::radians(135.0f)), sin(glm::radians(135.0f)), 0.0f);
        gubo.lightColor = glm::vec4(1.0f, 1.0f, 1.0f, 1.0f);
        gubo.eyePos = cameraData.dampedCamPos;
        gubo.eyeDir = glm::vec4(0);
        gubo.eyeDir.w = 1.0;

        // Draw the car
        for (std::vector<std::string>::iterator it = car.begin(); it != car.end(); it++) {
            int i = SC.InstanceIds[it->c_str()];
            glm::vec3 dP = glm::vec3(glm::rotate(glm::mat4(1), Yaw, glm::vec3(0,1,0)) *
                                     glm::vec4(*deltaP[i],1));
            ubo.mMat = MakeWorld(Pos + dP, Yaw + deltaA[i], usePitch[i] * wheelRoll, 0) * baseTr;
            ubo.mvpMat = ViewPrj * ubo.mMat;
            ubo.nMat = glm::inverse(glm::transpose(ubo.mMat));

            SC.DS[i]->map(currentImage, &ubo, sizeof(ubo), 0);
            SC.DS[i]->map(currentImage, &gubo, sizeof(gubo), 2);
        }
        
        // Draw the landscape
        for (std::vector<std::string>::iterator it = world.begin(); it != world.end(); it++) {
            int i = SC.InstanceIds[it->c_str()];
            
            ubo.mMat = SC.I[i].Wm * baseTr;
            ubo.mvpMat = ViewPrj * ubo.mMat;
            ubo.nMat = glm::inverse(glm::transpose(ubo.mMat));

            SC.DS[i]->map(currentImage, &ubo, sizeof(ubo), 0);
            SC.DS[i]->map(currentImage, &gubo, sizeof(gubo), 2);
        }
        
    }
};

// This is the main: probably you do not need to touch this!
int main() {
    App app;

    try {
        app.run();
    } catch (const std::exception& e) {
        std::cerr << e.what() << std::endl;
        return EXIT_FAILURE;
    }

    return EXIT_SUCCESS;
}
