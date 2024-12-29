#ifndef COLLIDER_HPP
#define COLLIDER_HPP

#include <btBulletDynamicsCommon.h>

class Collider : public btCollisionObject {
    
public:
    
    Collider() {}
    
    void init(btRigidBody* rb) {
        rb->setUserPointer(this);
    }

    virtual void onCollision(Collider* other) {};

};

#endif
