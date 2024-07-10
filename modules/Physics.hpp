#ifndef PHYSICS_HPP
#define PHYSICS_HPP

#include <btBulletDynamicsCommon.h>
#include <vector>
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>
#include <glm/gtx/quaternion.hpp>
#include <glm/gtx/string_cast.hpp>
#include <iostream>

//#define TINYOBJLOADER_IMPLEMENTATION
//#include <tiny_obj_loader.h>

// physics global properties
btBroadphaseInterface* broadphase;
btDefaultCollisionConfiguration* collisionConfiguration;
btCollisionDispatcher* dispatcher;
btSequentialImpulseConstraintSolver* solver;
btDiscreteDynamicsWorld* dynamicsWorld;
btRigidBody* carRigidBody;
btRigidBody* circuitRigidBody;
btRaycastVehicle* vehicle;

// This will hold the static collision shapes
std::vector<btCollisionShape*> collisionShapes;

const float CUBE_HALF_EXTENTS = 1.0f; // Half extents of the vehicle chassis
const float wheelWidth = 0.4f; // Width of the wheels

void setupRaycastVehicle(btDiscreteDynamicsWorld* dynamicsWorld, btRigidBody* carRigidBody, btRaycastVehicle*& vehicle);
void printWheelPositions(btRaycastVehicle* vehicle);

// This function uses the library assimp to take the xxx.obj file and convert it to a mesh
btTriangleMesh* loadMesh(const std::string& filePath) {
    tinyobj::attrib_t attrib;
    std::vector<tinyobj::shape_t> shapes;
    std::vector<tinyobj::material_t> materials;
    std::string warn, err;

    if (!tinyobj::LoadObj(&attrib, &shapes, &materials, &warn, &err, filePath.c_str())) {
        throw std::runtime_error("Failed to load .obj file: " + warn + err);
    }

    btTriangleMesh* mesh = new btTriangleMesh();

    for (const auto& shape : shapes) {
        size_t indexOffset = 0;
        for (size_t f = 0; f < shape.mesh.num_face_vertices.size(); f++) {
            int fv = shape.mesh.num_face_vertices[f];

            if (fv != 3) {
                continue; // Ignora le facce non triangolari
            }
            btVector3 vertices[3];
            for (int v = 0; v < fv; v++) {
                tinyobj::index_t idx = shape.mesh.indices[indexOffset + v];
                float vx = attrib.vertices[3 * idx.vertex_index + 0];
                float vy = attrib.vertices[3 * idx.vertex_index + 1];
                float vz = attrib.vertices[3 * idx.vertex_index + 2];
                vertices[v] = btVector3(vx, vy, vz);
            }

            mesh->addTriangle(vertices[0], vertices[1], vertices[2]);
            indexOffset += fv;
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

void initPhysics(const Scene* s) {
    broadphase = new btDbvtBroadphase();
    collisionConfiguration = new btDefaultCollisionConfiguration();
    dispatcher = new btCollisionDispatcher(collisionConfiguration);
    solver = new btSequentialImpulseConstraintSolver();

    dynamicsWorld = new btDiscreteDynamicsWorld(dispatcher, broadphase, solver, collisionConfiguration);
    dynamicsWorld->setGravity(btVector3(0, -9.81, 0));
    dynamicsWorld->getSolverInfo().m_solverMode |= SOLVER_USE_2_FRICTION_DIRECTIONS;

    // Load the circuit mesh
    //btTriangleMesh* circuitMesh = loadMesh(s->M[s->MeshIds.at("track")]);
    btTriangleMesh* circuitMesh = loadMesh("models/Track.obj");
    std::cout << circuitMesh->getNumTriangles() << std::endl;
    btBvhTriangleMeshShape* circuitShape = new btBvhTriangleMeshShape(circuitMesh, true);
    collisionShapes.push_back(circuitShape);

    // Create circuit motion state
    btDefaultMotionState* circuitMotionState = new btDefaultMotionState(btTransform(btQuaternion(0, 0, 0, 1), btVector3(0, 0, 0)));
    btRigidBody::btRigidBodyConstructionInfo circuitRigidBodyCI(0, circuitMotionState, circuitShape, btVector3(0, 0, 0));
    circuitRigidBody = new btRigidBody(circuitRigidBodyCI);
    circuitRigidBody->setFriction(0.9); // Valore di attrito per asfalto (0.8 - 1.0)
    circuitRigidBody->setCollisionFlags(circuitRigidBody->getCollisionFlags() | btCollisionObject::CF_STATIC_OBJECT);
    //circuitRigidBody->setRestitution(0.0f);
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
    //btTriangleMesh* rampMesh = loadMesh(s->M[s->MeshIds.at("small_ramps")]);
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
    btDefaultMotionState* carMotionState = new btDefaultMotionState(btTransform(btQuaternion(0, 0, 0, 1), btVector3(0, 1, -10)));
    btScalar mass = 800.0f;
    btVector3 carInertia(0, 0, 0);
    carBoxShape->calculateLocalInertia(mass, carInertia);
    btRigidBody::btRigidBodyConstructionInfo carRigidBodyCI(mass, carMotionState, carBoxShape, carInertia);
    btRigidBody* carRigidBody = new btRigidBody(carRigidBodyCI);
    carRigidBody->setActivationState(DISABLE_DEACTIVATION); // Ensure car stays active
    carRigidBody->activate(true);
    //carRigidBody->setFriction(1.0f);
    carRigidBody->setDamping(0.2f, 0.2f); // Aumentato il damping lineare e angolare

    dynamicsWorld->addRigidBody(carRigidBody);

    // Set up the Raycast Vehicle
    
    setupRaycastVehicle(dynamicsWorld, carRigidBody, vehicle);
}

void setupRaycastVehicle(btDiscreteDynamicsWorld* dynamicsWorld, btRigidBody* carRigidBody, btRaycastVehicle*& vehicle) {
    // Raycast vehicle setup
    btRaycastVehicle::btVehicleTuning tuning;
    btVehicleRaycaster* vehicleRaycaster = new btDefaultVehicleRaycaster(dynamicsWorld);
    vehicle = new btRaycastVehicle(tuning, carRigidBody, vehicleRaycaster);
    dynamicsWorld->addAction(vehicle); // add action o vehicle

    // Set coordinate system (X right, Y up, Z forward)
    vehicle->setCoordinateSystem(0, 1, 2);

    // Verifica la direzione delle ruote e delle sospensioni
    btVector3 wheelDirectionCS0(0, -1, 0); // Direzione della sospensione verso il basso lungo l'asse Z negativo
    btVector3 wheelAxleCS(-1, 0, 0); // Assale della ruota lungo l'asse X negativo

    // Aggiungi le ruote
    btVector3 connectionPointCS0;
    bool isFrontWheel;

    // Lunghezza a riposo della sospensione
    btScalar suspensionRestLength = 0.7;

    // Raggio della ruota
    btScalar wheelRadius = 0.5;

    // Anteriore sinistra
    connectionPointCS0 = btVector3(-1, 0.5, 2);
    isFrontWheel = true;
    vehicle->addWheel(connectionPointCS0, wheelDirectionCS0, wheelAxleCS, suspensionRestLength, wheelRadius, tuning, isFrontWheel);

    // Anteriore destra
    connectionPointCS0 = btVector3(1, 0.5, 2);
    isFrontWheel = true;
    vehicle->addWheel(connectionPointCS0, wheelDirectionCS0, wheelAxleCS, suspensionRestLength, wheelRadius, tuning, isFrontWheel);

    // Posteriore sinistra
    connectionPointCS0 = btVector3(-1, 0.5, -2);
    isFrontWheel = false;
    vehicle->addWheel(connectionPointCS0, wheelDirectionCS0, wheelAxleCS, suspensionRestLength, wheelRadius, tuning, isFrontWheel);

    // Posteriore destra
    connectionPointCS0 = btVector3(1, 0.5, -2);
    isFrontWheel = false;
    vehicle->addWheel(connectionPointCS0, wheelDirectionCS0, wheelAxleCS, suspensionRestLength, wheelRadius, tuning, isFrontWheel);

    // Set wheel parameters
    for (int i = 0; i < vehicle->getNumWheels(); i++) {
        btWheelInfo& wheel = vehicle->getWheelInfo(i);
        wheel.m_suspensionStiffness = 100.0f;
        wheel.m_wheelsDampingRelaxation = 20.0f;
        wheel.m_wheelsDampingCompression = 20.0f;
        wheel.m_frictionSlip = 1000.0f;
        wheel.m_rollInfluence = 0.0f;
        wheel.m_maxSuspensionTravelCm = 500.0f;
    }
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

}
#endif


