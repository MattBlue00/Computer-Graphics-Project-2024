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

std::vector<btCollisionObject*> coinColliders;
std::unordered_map<std::string, btCollisionObject*> coinMap;

std::unordered_map<std::string, std::vector<float>> physicsObjectsMap = {
    // id           friction    restitution
    {"track",       {0.9f,      0.0f}},
    {"barrier",     {0.8f,      0.5f}},
    {"ramps",       {0.0f,      0.0f}}
};

// prototypes declaration
btBvhTriangleMeshShape* getCollisionShape(std::string filepath);
void addRigidBodyToDynamicsWorld(btBvhTriangleMeshShape* collisionShape, float friction, float restitution);
void printWheelPositions(btRaycastVehicle* vehicle);
btCollisionObject* createCoinCollider(const btTransform& transform, const std::string& id); //crea il collider per una coin
void checkCollisions(btRaycastVehicle* vehicle, nlohmann::json sceneJson);

class CoinCollectorCallback : public btCollisionWorld::ContactResultCallback {
public:
    std::unordered_map<std::string, btCollisionObject*>& coinMap;
    std::vector<btCollisionObject*>& coinColliders;
    nlohmann::json& sceneJson;
    btRaycastVehicle* vehicle;
    bool isCoinCollected = false;
    std::string collectedCoinID;

    CoinCollectorCallback(std::unordered_map<std::string, btCollisionObject*>& coinMap,
        std::vector<btCollisionObject*>& coins,
        nlohmann::json& sceneJson,
        btRaycastVehicle* vehicle)
        : coinMap(coinMap), coinColliders(coins), sceneJson(sceneJson), vehicle(vehicle) {}

    virtual btScalar addSingleResult(btManifoldPoint& cp,
        const btCollisionObjectWrapper* colObj0Wrap,
        int partId0, int index0,
        const btCollisionObjectWrapper* colObj1Wrap,
        int partId1, int index1) override {
        const btCollisionObject* obj0 = colObj0Wrap->getCollisionObject();
        const btCollisionObject* obj1 = colObj1Wrap->getCollisionObject();
        if ((obj0 == vehicle->getRigidBody() && isCoin(obj1)) || (obj1 == vehicle->getRigidBody() && isCoin(obj0))) {
            isCoinCollected = true;
            collectedCoinID = getCoinID(obj0 == vehicle->getRigidBody() ? obj1 : obj0);
            return 0;
        }
        return 1;
    }

    bool isCoin(const btCollisionObject* obj) {
        return obj->getUserPointer() != nullptr;
    }

    std::string getCoinID(const btCollisionObject* obj) {
        return *static_cast<std::string*>(obj->getUserPointer());
    }
};

CoinCollectorCallback* coinCallback;

void initPhysics(nlohmann::json sceneJson) {

    std::cout << sceneJson.dump(4) << std::endl;

    broadphase = new btDbvtBroadphase();
    collisionConfiguration = new btDefaultCollisionConfiguration();
    dispatcher = new btCollisionDispatcher(collisionConfiguration);
    solver = new btSequentialImpulseConstraintSolver();

    dynamicsWorld = new btDiscreteDynamicsWorld(dispatcher, broadphase, solver, collisionConfiguration);
    dynamicsWorld->setGravity(btVector3(0, -9.81, 0));
    dynamicsWorld->getSolverInfo().m_solverMode |= SOLVER_USE_2_FRICTION_DIRECTIONS;
    dynamicsWorld->getSolverInfo().m_splitImpulse = true;
    dynamicsWorld->getSolverInfo().m_numIterations = 10;
    
    for (auto it = physicsObjectsMap.begin(); it != physicsObjectsMap.end(); ++it) {
        std::string id = it->first;
        
        // Cercare il modello corrispondente nell'array "models"
        auto modelIt = std::find_if(sceneJson["models"].begin(), sceneJson["models"].end(), [&id](const nlohmann::json& model) {
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

    for (const auto& instance : sceneJson["instances"]) {
        std::cout << "sono nel for" << std::endl;
        std::cout << "instance[\"model\"]: " << instance["model"] << std::endl;  // Stampa il valore di instance["model"]
        if (instance["model"] == "coin") {
            std::cout << "sono nell'if" << std::endl;
            btTransform transform;
            // Converti i dati della trasformazione in btTransform
            const auto& t = instance["transform"];
            float matrix[16];
            for (int i = 0; i < 16; ++i) {
                matrix[i] = t[i];
            }
            transform.setFromOpenGLMatrix(matrix);

            std::string coinID = instance["id"];
            btCollisionObject* coin = createCoinCollider(transform, coinID);
            dynamicsWorld->addCollisionObject(coin);
            coinColliders.push_back(coin);

            // Associare la moneta con il suo ID
            coinMap[coinID] = coin;

            // Stampa di conferma per la creazione del collider della moneta
            std::cout << "Created coin collider with ID: " << coinID << std::endl;
        }
    }
    // Stampa della mappa finale dei collider delle monete
    std::cout << "Final coinMap contents:" << std::endl;
    for (const auto& pair : coinMap) {
        std::cout << "Coin ID: " << pair.first << std::endl;
    }
}

void updatePhysics(float deltaT) {
    dynamicsWorld->stepSimulation(deltaT, 60);
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
    rigidBody->setRestitution(restitution);
    rigidBody->setCollisionFlags(rigidBody->getCollisionFlags() | btCollisionObject::CF_STATIC_OBJECT);
    dynamicsWorld->addRigidBody(rigidBody);
}


btCollisionObject* createCoinCollider(const btTransform& transform, const std::string& id) {
    btCollisionShape* coinShape = new btSphereShape(0.03f); // Assuming a small sphere collider for the coin
    btCollisionObject* coin = new btCollisionObject();
    coin->setCollisionShape(coinShape);
    coin->setWorldTransform(transform);
    coin->setUserPointer(new std::string(id)); // Store the ID in the user pointer
    return coin;
}

void checkCollisions(btRaycastVehicle* vehicle, nlohmann::json sceneJson) {

    if (coinCallback == nullptr) {
        // Inizializza il callback delle collisioni la prima volta che viene chiamata checkCollisions
        coinCallback = new CoinCollectorCallback(coinMap, coinColliders, sceneJson, vehicle);
    }

    dynamicsWorld->contactTest(vehicle->getRigidBody(), *coinCallback);
    if (coinCallback->isCoinCollected) {
        std::string collectedCoinID = coinCallback->collectedCoinID;

        // Stampa in console che c'è stata una collisione
        std::cout << "Collision detected with coin ID: " << collectedCoinID << std::endl;

        // Aggiorna il punteggio
        //updateScore(collectedCoinID);

        btCollisionObject* collectedCoin = coinMap[collectedCoinID];
        dynamicsWorld->removeCollisionObject(collectedCoin);
        coinColliders.erase(std::remove(coinColliders.begin(), coinColliders.end(), collectedCoin), coinColliders.end());
        coinMap.erase(collectedCoinID);

        delete collectedCoin->getCollisionShape();
        delete static_cast<std::string*>(collectedCoin->getUserPointer());
        delete collectedCoin;


    }
}

#endif


