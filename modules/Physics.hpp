#ifndef PHYSICS_HPP
#define PHYSICS_HPP

#include <btBulletDynamicsCommon.h>
#include <assimp/Importer.hpp>
#include <assimp/scene.h>
#include <assimp/postprocess.h>
#include <vector>

// physics global properties
btBroadphaseInterface* broadphase;
btDefaultCollisionConfiguration* collisionConfiguration;
btCollisionDispatcher* dispatcher;
btSequentialImpulseConstraintSolver* solver;
btDiscreteDynamicsWorld* dynamicsWorld;
btRigidBody* carRigidBody;
btRigidBody* circuitRigidBody;

// This will hold the static collision shapes
std::vector<btCollisionShape*> collisionShapes;

// This function uses the library assimp to take the xxx.obj file and convert it to a mesh
btTriangleMesh* loadMesh(const std::string& filePath) {
    Assimp::Importer importer;
    const aiScene* scene = importer.ReadFile(filePath, aiProcess_Triangulate | aiProcess_JoinIdenticalVertices);
    if (!scene || !scene->HasMeshes()) {
        throw std::runtime_error("Failed to load mesh: " + filePath);
    }

    btTriangleMesh* mesh = new btTriangleMesh();
    for (unsigned int i = 0; i < scene->mNumMeshes; ++i) {
        aiMesh* aiMesh = scene->mMeshes[i];
        if (!aiMesh->HasFaces()) {
            continue;
        }
        for (unsigned int j = 0; j < aiMesh->mNumFaces; ++j) {
            aiFace& face = aiMesh->mFaces[j];
            if (face.mNumIndices != 3) continue; // Ensure the face is a triangle

            btVector3 vertices[3];
            for (unsigned int k = 0; k < 3; ++k) {
                aiVector3D& vertex = aiMesh->mVertices[face.mIndices[k]];
                vertices[k] = btVector3(vertex.x, vertex.y, vertex.z);
            }

            mesh->addTriangle(vertices[0], vertices[1], vertices[2]);
        }
    }

    std::cout << "Loaded mesh with " << mesh->getNumTriangles() << " triangles" << std::endl;
    return mesh;
}

void initPhysics() {
    broadphase = new btDbvtBroadphase();
    collisionConfiguration = new btDefaultCollisionConfiguration();
    dispatcher = new btCollisionDispatcher(collisionConfiguration);
    solver = new btSequentialImpulseConstraintSolver();

    dynamicsWorld = new btDiscreteDynamicsWorld(dispatcher, broadphase, solver, collisionConfiguration);
    dynamicsWorld->setGravity(btVector3(0, -9.81, 0));

    // Load the circuit mesh
    btTriangleMesh* circuitMesh = loadMesh("models/Track.obj");
    btBvhTriangleMeshShape* circuitShape = new btBvhTriangleMeshShape(circuitMesh, true);
    //circuitShape->setMargin(0.1); // Adding collision margin
    collisionShapes.push_back(circuitShape);

    // Create circuit motion state
    btDefaultMotionState* circuitMotionState = new btDefaultMotionState(btTransform(btQuaternion(0, 0, 0, 1), btVector3(0, 0, 0)));
    btRigidBody::btRigidBodyConstructionInfo circuitRigidBodyCI(0, circuitMotionState, circuitShape, btVector3(0, 0, 0));
    circuitRigidBody = new btRigidBody(circuitRigidBodyCI);
    circuitRigidBody->setFriction(0.9); // Valore di attrito per asfalto (0.8 - 1.0)
    dynamicsWorld->addRigidBody(circuitRigidBody);

    // Load the barrier mesh
    btTriangleMesh* barrierMesh = loadMesh("models/Barrier.obj");
    btBvhTriangleMeshShape* barrierShape = new btBvhTriangleMeshShape(barrierMesh, true);
    //barrierShape->setMargin(0.1); // Ensure margin is set
    collisionShapes.push_back(barrierShape);

    // Create barrier motion state
    btDefaultMotionState* barrierMotionState = new btDefaultMotionState(btTransform(btQuaternion(0, 0, 0, 1), btVector3(0, 0, 0)));
    btRigidBody::btRigidBodyConstructionInfo barrierRigidBodyCI(0, barrierMotionState, barrierShape, btVector3(0, 0, 0));
    btRigidBody* barrierRigidBody = new btRigidBody(barrierRigidBodyCI);
    barrierRigidBody->setFriction(0.8);
    barrierRigidBody->setRestitution(0.1f); // Reduce bounciness
    dynamicsWorld->addRigidBody(barrierRigidBody);

    // Load the ramp mesh
    btTriangleMesh* rampMesh = loadMesh("models/SmallRamps.obj");
    btBvhTriangleMeshShape* rampShape = new btBvhTriangleMeshShape(rampMesh, true);
    //rampShape->setMargin(0.1); // Ensure margin is set
    collisionShapes.push_back(rampShape);

    // Create ramp motion state
    btDefaultMotionState* rampMotionState = new btDefaultMotionState(btTransform(btQuaternion(0, 0, 0, 1), btVector3(0, 0, 0)));
    btRigidBody::btRigidBodyConstructionInfo rampRigidBodyCI(0, rampMotionState, rampShape, btVector3(0, 0, 0));
    btRigidBody* rampRigidBody = new btRigidBody(rampRigidBodyCI);
    rampRigidBody->setFriction(0.8); // Valore di attrito per asfalto (0.8 - 1.0)
    dynamicsWorld->addRigidBody(rampRigidBody);

    // Car initialization with btBoxShape
    btBoxShape* carBoxShape = new btBoxShape(btVector3(1.0, 0.5, 2.0));
    carBoxShape->setMargin(0.3f); // Adding collision margin
    collisionShapes.push_back(carBoxShape);

    btDefaultMotionState* carMotionState = new btDefaultMotionState(btTransform(btQuaternion(0, 0, 0, 1), btVector3(0, 2, -10)));
    btScalar mass = 800.0f;
    btVector3 carInertia(0, 0, 0);
    carBoxShape->calculateLocalInertia(mass, carInertia);
    btRigidBody::btRigidBodyConstructionInfo carRigidBodyCI(mass, carMotionState, carBoxShape, carInertia);
    carRigidBody = new btRigidBody(carRigidBodyCI);
    carRigidBody->setFriction(0.5f);
    //carRigidBody->setRollingFriction(0.1f);
    carRigidBody->setDamping(0.9f, 0.9f);
    carRigidBody->setActivationState(DISABLE_DEACTIVATION); // Ensure car stays active
    //carRigidBody->setAngularFactor(btVector3(0, 1, 0)); // Limita la rotazione solo sull'asse y
    //carRigidBody->setRestitution(0.1f); // Reduce bounciness
    dynamicsWorld->addRigidBody(carRigidBody);

}


void cleanupPhysics() {
    for (int i = dynamicsWorld->getNumCollisionObjects() - 1; i >= 0; i--) {
        btCollisionObject* obj = dynamicsWorld->getCollisionObjectArray()[i];
        btRigidBody* body = btRigidBody::upcast(obj);
        if (body && body->getMotionState()) {
            delete body->getMotionState();
        }
        dynamicsWorld->removeCollisionObject(obj);
        delete obj;
    }

    for (size_t j = 0; j < collisionShapes.size(); j++) {
        delete collisionShapes[j];
    }
    collisionShapes.clear();

    delete dynamicsWorld;
    delete solver;
    delete dispatcher;
    delete collisionConfiguration;
    delete broadphase;
}

void applyTiltForces(btRigidBody* carBody, const btVector3& hitNormal) {
    // Estrai la matrice di trasformazione dal corpo rigido
    btTransform carTransform;
    carBody->getMotionState()->getWorldTransform(carTransform);

    // Calcola i punti di applicazione delle forze (davanti e dietro la macchina)
    btVector3 carPosition = carTransform.getOrigin();
    btVector3 forwardDir = carTransform.getBasis().getColumn(2);
    btVector3 upDir = carTransform.getBasis().getColumn(1);

    btVector3 rearPosition = carPosition - forwardDir * 2.0f;  // Punto dietro la macchina
    btVector3 frontPosition = carPosition + forwardDir * 2.0f; // Punto davanti la macchina

    // Calcola la forza da applicare in base all'angolo di inclinazione del terreno
    btScalar tiltForceMagnitude = 500.0f; // Modifica questo valore in base alle necessità

    // Applicare forza dietro in salita (hitNormal con componente Y < 1)
    if (hitNormal.getY() < 1.0f) {
        btVector3 tiltForce = upDir * tiltForceMagnitude * (1.0f - hitNormal.getY());
        carBody->applyForce(tiltForce, rearPosition - carPosition);
    }

    // Applicare forza davanti in discesa (hitNormal con componente Y < 1)
    if (hitNormal.getY() > 0.0f && hitNormal.getY() < 1.0f) {
        btVector3 tiltForce = -upDir * tiltForceMagnitude * (1.0f - hitNormal.getY());
        carBody->applyForce(tiltForce, frontPosition - carPosition);
    }
}

void updateCarOrientation() {
    // Esegui un raycast dalla posizione del veicolo verso il basso
    btTransform carTransform;
    carRigidBody->getMotionState()->getWorldTransform(carTransform);
    btVector3 carPosition = carTransform.getOrigin();

    btVector3 rayStart = carPosition;
    btVector3 rayEnd = carPosition - btVector3(0, 10.0f, 0); // Lancia il raggio 10 unità verso il basso

    btCollisionWorld::ClosestRayResultCallback rayCallback(rayStart, rayEnd);
    dynamicsWorld->rayTest(rayStart, rayEnd, rayCallback);

    btVector3 hitNormal(0, 1, 0); // Default normal in caso di mancato hit
    if (rayCallback.hasHit()) {
        hitNormal = rayCallback.m_hitNormalWorld;
        hitNormal.normalize();

        // Debug: Stampa dettagli sull'oggetto colpito
        const btCollisionObject* hitObject = rayCallback.m_collisionObject;
        const btRigidBody* hitBody = btRigidBody::upcast(hitObject);
        if (hitBody == circuitRigidBody) {
            //std::cout << "Hit the circuit!" << std::endl;
        }
        else {
            //std::cout << "Hit another object!" << std::endl;
        }
    } else {
        //std::cout << "Raycast missed!" << std::endl;
    }

    // Debug: Stampa la normale colpita
    //std::cout << "Hit Normal: " << hitNormal.getX() << ", " << hitNormal.getY() << ", " << hitNormal.getZ() << std::endl;

    // Applica le forze di inclinazione in base alla normale colpita
    applyTiltForces(carRigidBody, hitNormal);
}

void updatePhysics(float deltaT) {

    dynamicsWorld->stepSimulation(deltaT, 60);
    
    btTransform trans;
    carRigidBody->getMotionState()->getWorldTransform(trans);
    btVector3 pos = trans.getOrigin();
}

#endif


