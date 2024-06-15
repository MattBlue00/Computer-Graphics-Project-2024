#include "modules/Starter.hpp"

struct Vertex {
    glm::vec3 pos;
    glm::vec2 UV;
    glm::vec3 norm;
};

class App : public BaseProject {
    protected:
    
    // aspect ratio
    float Ar;
    
    DescriptorSetLayout DSL;
    VertexDescriptor VD;
    Pipeline P;
    
    // Here you set the main application parameters
    void setWindowParameters() override {
        // window size, titile and initial background
        windowWidth = 800;
        windowHeight = 600;
        windowTitle = "Rainbow Stadium";
        windowResizable = GLFW_TRUE;
        initialBackgroundColor = {0.0f, 0.85f, 1.0f, 1.0f};
        
        // Descriptor pool sizes
        uniformBlocksInPool = 19 * 2 + 2;
        texturesInPool = 19 + 1;
        setsInPool = 19 + 1;

        Ar = 4.0f / 3.0f;
    }
    
    // What to do when the window changes size
    void onWindowResize(int w, int h) override {
        std::cout << "Window resized to: " << w << " x " << h << "\n";
        Ar = (float)w / (float)h;
    }
    
    void localInit() override {
        
        // Descriptor Layouts [what will be passed to the shaders]
        DSL.init(this, {
                    {0, VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER, VK_SHADER_STAGE_ALL_GRAPHICS}
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
        P.init(this, &VD, "shaders/ToonVert.spv", "shaders/ToonFrag.spv", {&DSL});
        P.setAdvancedFeatures(VK_COMPARE_OP_LESS_OR_EQUAL, VK_POLYGON_MODE_FILL,
                                     VK_CULL_MODE_NONE, false);
        
    }
    
    void pipelinesAndDescriptorSetsInit() override {
        // creates a new pipeline with the current surface
        P.create();
    }
    
    void populateCommandBuffer(VkCommandBuffer commandBuffer, int i) override {
        // binds the pipeline
        P.bind(commandBuffer);
    }
    
    void updateUniformBuffer(uint32_t currentImage) override {
        
    }

    void pipelinesAndDescriptorSetsCleanup() override {
        P.cleanup();
    }
    
    void localCleanup() override {
        DSL.cleanup();
        P.cleanup();
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
