#ifndef PHYSICS_HPP
#define PHYSICS_HPP

#include <btBulletDynamicsCommon.h>

// physics global properties
btBroadphaseInterface* broadphase;
btDefaultCollisionConfiguration* collisionConfiguration;
btCollisionDispatcher* dispatcher;
btSequentialImpulseConstraintSolver* solver;
btDiscreteDynamicsWorld* dynamicsWorld;

std::vector<btCollisionShape*> collisionShapes;

std::unordered_map<std::string, std::vector<float>> physicsObjectsMap = {
    // id           friction    restitution
    {"track",       {0.0f,      0.0f}},
    {"barrier",     {0.8f,      0.5f}},
    {"ramps",       {0.0f,      0.0f}}
};

// prototypes declaration
btBvhTriangleMeshShape* getCollisionShape(std::string filepath);
void addRigidBodyToDynamicsWorld(btBvhTriangleMeshShape* collisionShape, float friction, float restitution);

void initPhysics(json sceneJson) {
    broadphase = new btDbvtBroadphase();
    collisionConfiguration = new btDefaultCollisionConfiguration();
    dispatcher = new btCollisionDispatcher(collisionConfiguration);
    solver = new btSequentialImpulseConstraintSolver();

    dynamicsWorld = new btDiscreteDynamicsWorld(dispatcher, broadphase, solver, collisionConfiguration);
    dynamicsWorld->setGravity(btVector3(0, -12.5, 0));
    dynamicsWorld->getSolverInfo().m_solverMode |= SOLVER_USE_2_FRICTION_DIRECTIONS;
    dynamicsWorld->getSolverInfo().m_splitImpulse = true;
    dynamicsWorld->getSolverInfo().m_numIterations = 10;
    
    for (auto it = physicsObjectsMap.begin(); it != physicsObjectsMap.end(); ++it) {
        std::string id = it->first;
        
        // Cercare il modello corrispondente nell'array "models"
        auto modelIt = std::find_if(sceneJson["models"].begin(), sceneJson["models"].end(), [&id](const json& model) {
               return model["id"] == id;
        });

        if (modelIt != sceneJson["models"].end()) {
            std::string modelFilename = (*modelIt)["model"];
            btBvhTriangleMeshShape* collisionShape = getCollisionShape(modelFilename);
            collisionShapes.push_back(collisionShape);
            addRigidBodyToDynamicsWorld(collisionShape, it->second[0], it->second[1]);
        } else {
            std::cout << "Model for " << id << " not found!" << std::endl;
            exit(-1);
        }
    }
}

void updatePhysics(float deltaT) {
    dynamicsWorld->stepSimulation(deltaT);
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

    return mesh;
}

btBvhTriangleMeshShape* getCollisionShape(std::string filepath){
    btTriangleMesh* mesh = loadMesh(filepath);
    return new btBvhTriangleMeshShape(mesh, true);
}

void addRigidBodyToDynamicsWorld(btBvhTriangleMeshShape* collisionShape, float friction, float restitution){
    // Create circuit motion state
    btDefaultMotionState* motionState = new btDefaultMotionState(btTransform(btQuaternion(0, 0, 0, 1), btVector3(0, 0, 0)));
    btRigidBody::btRigidBodyConstructionInfo rigidBodyCI(0, motionState, collisionShape, btVector3(0, 0, 0));
    btRigidBody* rigidBody = new btRigidBody(rigidBodyCI);
    rigidBody->setFriction(friction);
    rigidBody->setRestitution(friction);
    rigidBody->setCollisionFlags(rigidBody->getCollisionFlags() | btCollisionObject::CF_STATIC_OBJECT);
    dynamicsWorld->addRigidBody(rigidBody);
}

#endif
