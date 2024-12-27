#ifndef COLLIDER_HPP
#define COLLIDER_HPP

#include <btBulletDynamicsCommon.h>

class Collider : public btCollisionObject {
    
public:
    
    Collider() {
        setUserPointer(this);
    }

    virtual void onCollision(Collider* other) {};

};

#endif
