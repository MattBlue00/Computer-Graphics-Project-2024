#ifndef PHYSICS_HPP
#define PHYSICS_HPP

#include <btBulletDynamicsCommon.h>
#include <assimp/Importer.hpp>
#include <assimp/scene.h>
#include <assimp/postprocess.h>
#include <vector>
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>
#include <glm/gtx/quaternion.hpp>
#include <glm/gtx/string_cast.hpp>
#include <iostream>

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
    const aiScene* scene = importer.ReadFile(filePath,  aiProcess_Triangulate | 
                                                        aiProcess_JoinIdenticalVertices | 
                                                        aiProcess_GenSmoothNormals | 
                                                        aiProcess_PreTransformVertices | 
                                                        aiProcess_ImproveCacheLocality | 
                                                        aiProcess_SplitLargeMeshes |
                                                        aiProcess_LimitBoneWeights |
                                                        aiProcess_OptimizeMeshes |
                                                        aiProcess_OptimizeGraph);
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

// Funzione per estrarre la matrice di trasformazione e verificarne l'orientamento
void checkCarOrientation(btRigidBody* carBody) {
    // Estrai la matrice di trasformazione dal corpo rigido
    btTransform trans;
    carBody->getMotionState()->getWorldTransform(trans);
    btScalar m[16];
    trans.getOpenGLMatrix(m);

    // Converti l'array btScalar in glm::mat4
    glm::mat4 modelMatrix = glm::make_mat4(m);

    // Estrai la posizione
    glm::vec3 position = glm::vec3(modelMatrix[3]);

    // Estrai l'orientamento come quaternion
    glm::quat orientation = glm::quat_cast(modelMatrix);

    // Ottieni gli angoli di Eulero dall'orientamento
    glm::vec3 eulerAngles = glm::eulerAngles(orientation);

    // Stampa la posizione e gli angoli di Eulero
    std::cout << "Position: " << glm::to_string(position) << std::endl;
    std::cout << "Orientation (Euler angles): " << glm::to_string(eulerAngles) << std::endl;
}

void initPhysics() {
    broadphase = new btDbvtBroadphase();
    collisionConfiguration = new btDefaultCollisionConfiguration();
    dispatcher = new btCollisionDispatcher(collisionConfiguration);
    solver = new btSequentialImpulseConstraintSolver();

    dynamicsWorld = new btDiscreteDynamicsWorld(dispatcher, broadphase, solver, collisionConfiguration);
    dynamicsWorld->setGravity(btVector3(0, -9.81, 0));
    dynamicsWorld->getSolverInfo().m_solverMode |= SOLVER_USE_2_FRICTION_DIRECTIONS;

    // Load the circuit mesh
    btTriangleMesh* circuitMesh = loadMesh("models/Track.obj");
    btBvhTriangleMeshShape* circuitShape = new btBvhTriangleMeshShape(circuitMesh, true);
    //circuitShape->setMargin(0.1f); // Adding collision margin
    collisionShapes.push_back(circuitShape);

    // Create circuit motion state
    btDefaultMotionState* circuitMotionState = new btDefaultMotionState(btTransform(btQuaternion(0, 0, 0, 1), btVector3(0, 0, 0)));
    btRigidBody::btRigidBodyConstructionInfo circuitRigidBodyCI(0, circuitMotionState, circuitShape, btVector3(0, 0, 0));
    circuitRigidBody = new btRigidBody(circuitRigidBodyCI);
    circuitRigidBody->setFriction(0.9); // Valore di attrito per asfalto (0.8 - 1.0)
    //circuitRigidBody->setRestitution(0.0f); // Reduce bounciness
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
    carBoxShape->setMargin(0.4f); // Adding collision margin
    collisionShapes.push_back(carBoxShape);

    btDefaultMotionState* carMotionState = new btDefaultMotionState(btTransform(btQuaternion(0, 0, 0, 1), btVector3(0, 2, -10)));
    btScalar mass = 800.0f;
    btVector3 carInertia(0, 0, 0);
    carBoxShape->calculateLocalInertia(mass, carInertia);
    btRigidBody::btRigidBodyConstructionInfo carRigidBodyCI(mass, carMotionState, carBoxShape, carInertia);
    carRigidBody = new btRigidBody(carRigidBodyCI);
    carRigidBody->setFriction(0.5f);
    //carRigidBody->setRollingFriction(0.0f);
    carRigidBody->setDamping(0.9f, 0.9f);
    carRigidBody->setActivationState(DISABLE_DEACTIVATION); // Ensure car stays active
    carRigidBody->setAngularFactor(btVector3(1, 1, 1)); // LPermette al corpo rigido di ruotare su tutti gli assi
    //carRigidBody->setRestitution(0.0f); // Reduce bounciness
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

void updatePhysics(float deltaT) {
    dynamicsWorld->stepSimulation(deltaT, 60);

    //checkCarOrientation(carRigidBody);

}

#endif


