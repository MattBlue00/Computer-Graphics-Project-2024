#ifndef GAME_OBJECT_HPP
#define GAME_OBJECT_HPP

class GameObject {
    
public:
    
    std::string getId() const { return id; }
    Model* getModel() const { return model; }
    Texture* getTexture() const { return texture; }
    DescriptorSet* getDescriptorSet() const { return descriptorSet; }
    bool isEnabled() const { return enabled; }
    
    glm::mat4 worldMatrix;
    
    GameObject(std::string id, Model* m, Texture* t, glm::mat4 wm, DescriptorSet* ds)
    : id(id), model(m), texture(t), worldMatrix(wm), descriptorSet(ds) {
        enabled = true;
        _oldWorldMatrix = worldMatrix;
    };
    
    virtual void init() {};
    
    void descriptorSetInit(DescriptorSetLayout &dsl){
        descriptorSet->init(EngineBaseProject, &dsl, {
            {0, UNIFORM, sizeof(UniformBufferObject), nullptr},
            {1, TEXTURE, 0, texture},
            {2, UNIFORM, sizeof(GlobalUniformBufferObject), nullptr}
        });
    }
    
    virtual void update() {};
    
    void descriptorSetCleanup() {
        descriptorSet->cleanup();
    }
    
    void localCleanup() {
        texture->cleanup();
        model->cleanup();
    }
    
    virtual ~GameObject() = default;
    
    void populateCommandBuffer(VkCommandBuffer commandBuffer, int currentImage, Pipeline &P) {
        model->bind(commandBuffer);
        descriptorSet->bind(commandBuffer, P, 0, currentImage);
                    
        vkCmdDrawIndexed(commandBuffer,
                static_cast<uint32_t>(model->indices.size()), 1, 0, 0, 0);
    }
    
    void mapMemory(int currentImage, GlobalUniformBufferObject* gubo, UniformBufferObject* ubo){
        descriptorSet->map(currentImage, ubo, sizeof(*ubo), 0);
        descriptorSet->map(currentImage, gubo, sizeof(*gubo), 2);
    }
    
    void disable(){
        _oldWorldMatrix = worldMatrix;
        worldMatrix = glm::scale(glm::mat4(1.0f), glm::vec3(0.0f, 0.0f, 0.0f));
        enabled = false;
    }
    
    void enable(){
        worldMatrix = _oldWorldMatrix;
        enabled = true;
    }
    
protected:
    
    std::string id;
    Model* model;
    Texture* texture;
    DescriptorSet* descriptorSet;
    
    bool enabled;
    
    // world matrix before the object is disabled
    glm::mat4 _oldWorldMatrix;
    
};

#endif
