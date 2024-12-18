#ifndef STATIC_OBJECT_HPP
#define STATIC_OBJECT_HPP

class StaticObject: public GameObject {
    
public:
    
    StaticObject(std::string id, Model* m, Texture* t, glm::mat4 wm, DescriptorSet* ds)
    : GameObject(id, m, t, wm, ds) {}
    
    void update(PositionData positionData) override {}
};

#endif
