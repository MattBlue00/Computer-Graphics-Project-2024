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

// This will hold the static collision shapes
std::vector<btCollisionShape*> collisionShapes;



//This function uses the library assimp to take the xxx.obj file and convert it to a mesh
btTriangleMesh* loadMesh(const std::string& filePath) {
    Assimp::Importer importer;
    const aiScene* scene = importer.ReadFile(filePath, aiProcess_Triangulate |
        aiProcess_JoinIdenticalVertices |
        aiProcess_GenSmoothNormals |
        aiProcess_PreTransformVertices |
        aiProcess_ImproveCacheLocality |
        aiProcess_OptimizeGraph |
        aiProcess_OptimizeMeshes |
        aiProcess_FixInfacingNormals);

    if (!scene) {
        throw std::runtime_error("Failed to load mesh: " + filePath);
    }

    btTriangleMesh* mesh = new btTriangleMesh();

    for (unsigned int i = 0; i < scene->mNumMeshes; ++i) {
        aiMesh* aiMesh = scene->mMeshes[i];
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

    return mesh;
}

void initPhysics(){
    broadphase = new btDbvtBroadphase();
    collisionConfiguration = new btDefaultCollisionConfiguration();
    dispatcher = new btCollisionDispatcher(collisionConfiguration);
    solver = new btSequentialImpulseConstraintSolver();

    dynamicsWorld = new btDiscreteDynamicsWorld(dispatcher, broadphase, solver, collisionConfiguration);
    dynamicsWorld->setGravity(btVector3(0, -9.81, 0));

    // Load the circuit mesh
    btTriangleMesh* circuitMesh = loadMesh("models/Track.obj");
    btBvhTriangleMeshShape* circuitShape = new btBvhTriangleMeshShape(circuitMesh, true);
    collisionShapes.push_back(circuitShape);

    // Create circuit motion state
    btDefaultMotionState* circuitMotionState = new btDefaultMotionState(btTransform(btQuaternion(0, 0, 0, 1), btVector3(0, 0, 0)));
    btRigidBody::btRigidBodyConstructionInfo circuitRigidBodyCI(0, circuitMotionState, circuitShape, btVector3(0, 0, 0));
    btRigidBody* circuitRigidBody = new btRigidBody(circuitRigidBodyCI);

    //circuitRigidBody->setFriction(0.8f); // Imposta un attrito moderato per il circuito

    dynamicsWorld->addRigidBody(circuitRigidBody);

    // Car initialization
   // Aggiunta della macchina come corpo rigido dinamico con btCompoundShape
    btCollisionShape* carBoxShape = new btBoxShape(btVector3(1.0, 0.5, 2.0));
    btCompoundShape* carCompoundShape = new btCompoundShape();
    btTransform localTransform;
    localTransform.setIdentity();
    carCompoundShape->addChildShape(localTransform, carBoxShape);

    btDefaultMotionState* carMotionState = new btDefaultMotionState(btTransform(btQuaternion(0, 0, 0, 1), btVector3(0, 1, 0)));
    btScalar mass = 800;
    btVector3 carInertia(0, 0, 0);
    carCompoundShape->calculateLocalInertia(mass, carInertia);
    btRigidBody::btRigidBodyConstructionInfo carRigidBodyCI(mass, carMotionState, carCompoundShape, carInertia);
    carRigidBody = new btRigidBody(carRigidBodyCI);
    carRigidBody->setFriction(1.0f);
    carRigidBody->setRollingFriction(0.1f);
    carRigidBody->setDamping(0.2, 0.2);
    dynamicsWorld->addRigidBody(carRigidBody);

}

void cleanupPhysics(){
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

void updatePhysics(float deltaT) {
    dynamicsWorld->stepSimulation(deltaT, 15);

    btTransform trans;
    carRigidBody->getMotionState()->getWorldTransform(trans);
    btVector3 pos = trans.getOrigin();
    btVector3 linVel = carRigidBody->getLinearVelocity();
    btVector3 angVel = carRigidBody->getAngularVelocity();

    std::cout << "Car Position: " << pos.getX() << ", " << pos.getY() << ", " << pos.getZ() << std::endl;
    std::cout << "Car Linear Velocity: " << linVel.getX() << ", " << linVel.getY() << ", " << linVel.getZ() << std::endl;
    std::cout << "Car Angular Velocity: " << angVel.getX() << ", " << angVel.getY() << ", " << angVel.getZ() << std::endl;
}



#endif


