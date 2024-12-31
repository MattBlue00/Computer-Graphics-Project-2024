#ifndef GAME_OBJECT_HPP
#define GAME_OBJECT_HPP

#include "graphics/PipelineTypes.hpp"

class GameObject {
    
public:
    
    std::string getId() const { return id; }
    Model* getModel() const { return model; }
    Texture* getTexture() const { return texture; }
    DescriptorSet* getDescriptorSet() const { return descriptorSet; }
    PipelineType getPipelineType() const { return pipelineType; }
    float getProperty(std::string key) { return properties[key]; }
    bool isEnabled() const { return enabled; }
    
    glm::mat4 worldMatrix;
    
    GameObject(std::string id, Model* m, Texture* t, glm::mat4 wm, DescriptorSet* ds, PipelineType pt, std::unordered_map<std::string, float> props)
    : id(id), model(m), texture(t), worldMatrix(wm), descriptorSet(ds), pipelineType(pt), properties(props) {
        enabled = true;
        _oldWorldMatrix = worldMatrix;
    };
    
    virtual void init() {};
    
    void descriptorSetInit(DescriptorSetLayout &dsl){
        descriptorSet->init(EngineBaseProject, &dsl, {
            {0, UNIFORM, sizeof(CookTorranceUniformBufferObject), nullptr},
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
    
    void populateCommandBuffer(VkCommandBuffer commandBuffer, int currentImage, Pipeline* pipeline) {
        model->bind(commandBuffer);
        descriptorSet->bind(commandBuffer, *pipeline, 0, currentImage);
                    
        vkCmdDrawIndexed(commandBuffer,
                static_cast<uint32_t>(model->indices.size()), 1, 0, 0, 0);
    }
    
    void mapMemory(int currentImage, GlobalUniformBufferObject* gubo, void* ubo, int uboSize){
        descriptorSet->map(currentImage, ubo, uboSize, 0);
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
    PipelineType pipelineType;
    std::unordered_map<std::string, float> properties;
    
    bool enabled;
    
    // world matrix before the object is disabled
    glm::mat4 _oldWorldMatrix;
    
};

#endif
