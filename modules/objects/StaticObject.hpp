#ifndef STATIC_OBJECT_HPP
#define STATIC_OBJECT_HPP

#include "Debug.hpp"

class StaticObject: public GameObject {
    
public:
    
    StaticObject(std::string id, Model* m, Texture* t, glm::mat4 wm, DescriptorSet* ds, PipelineType pt, std::unordered_map<std::string, float> props)
    : GameObject(id, m, t, wm, ds, pt, props) {}
    
};

#endif
