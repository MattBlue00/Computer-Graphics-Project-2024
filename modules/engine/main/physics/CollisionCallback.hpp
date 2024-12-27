#ifndef COLLISION_CALLBACK_HPP
#define COLLISION_CALLBACK_HPP

class CollisionCallback : public btCollisionWorld::ContactResultCallback {
    
public:
    std::vector<Collider*>& collidersToProcess;
    const btCollisionObject* vehicle;

    CollisionCallback(std::vector<Collider*>& collidersToProcess, const btCollisionObject* vehicle)
        : collidersToProcess(collidersToProcess), vehicle(vehicle) {}

    btScalar addSingleResult(btManifoldPoint& cp,
                             const btCollisionObjectWrapper* colObj0Wrap, int partId0, int index0,
                             const btCollisionObjectWrapper* colObj1Wrap, int partId1, int index1) override {
        const btCollisionObject* obj0 = colObj0Wrap->getCollisionObject();
        const btCollisionObject* obj1 = colObj1Wrap->getCollisionObject();

        // Identifica l'altro oggetto rispetto al veicolo
        const btCollisionObject* other = (obj0 == vehicle) ? obj1 : obj0;

        // Controlla se l'altro oggetto è un Collider
        
        // Prima castiamo il void* al tipo giusto
        Collider* colliderPtr = static_cast<Collider*>(other->getUserPointer());

        // Ora puoi fare il dynamic_cast
        Collider* collider = dynamic_cast<Collider*>(colliderPtr);
        
        if (collider) {
            collidersToProcess.push_back(collider);
        }
        return 0;
    }
};

#endif
