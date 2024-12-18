#ifndef DIRECTION_BARRIER_HPP
#define DIRECTION_BARRIER_HPP

class DirectionBarrier: public GameObject {
  
public:
    
    DirectionBarrier(std::string id, Model* m, Texture* t, glm::mat4 wm, DescriptorSet* ds)
    : GameObject(id, m, t, wm, ds) {}
    
    void update(PositionData positionData) override {
        // Condizioni per disegnare o rescalare le barriere
        bool drawOval = firstLap && !secondLap && id == "dir_barrier_oval";
        bool drawInner = !firstLap && secondLap && id == "dir_barrier_inner";
        bool rescale = (!firstLap && !secondLap) || (firstLap && !secondLap && id == "dir_barrier_inner") || (!firstLap && secondLap && id == "dir_barrier_oval");

        if (drawOval || drawInner) {}
        else if (rescale) {
            // Rescale other barriers to zero
            worldMatrix = glm::scale(worldMatrix, glm::vec3(0.0f, 0.0f, 0.0f));
        }
    }
    
};

#endif
