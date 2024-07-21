#ifndef PHYSICS_HPP
#define PHYSICS_HPP

#include <btBulletDynamicsCommon.h>
#include "Utils.hpp"
#include "Subject.hpp"

// physics global properties
btBroadphaseInterface* broadphase;
btDefaultCollisionConfiguration* collisionConfiguration;
btCollisionDispatcher* dispatcher;
btSequentialImpulseConstraintSolver* solver;
btDiscreteDynamicsWorld* dynamicsWorld;

std::vector<btCollisionShape*> collisionShapes;

std::vector<btRigidBody*> coinColliders;
std::unordered_map<std::string, btRigidBody*> coinMap;

std::unordered_map<std::string, std::vector<float>> physicsObjectsMap = {
    // id                       friction    restitution
    {"track",                   {0.0f,      0.0f}},
    {"barrier",                 {0.8f,      0.5f}},
    {"ramps",                   {0.0f,      0.0f}},
    {"tires_pile_1",            {0.8f,      0.5f}},
    {"tires_pile_2",            {0.8f,      0.5f}},
    {"tires_pile_3",            {0.8f,      0.5f}},
    {"tires_pile_4",            {0.8f,      0.5f}},
    {"tires_pile_5",            {0.8f,      0.5f}},
    {"tires_pile_6",            {0.8f,      0.5f}},
    {"tires_pile_7",            {0.8f,      0.5f}},
    {"tires_pile_8",            {0.8f,      0.5f}},
    {"tires_pile_9",            {0.8f,      0.5f}},
    {"tires_pile_10",           {0.8f,      0.5f}},
    {"dir_barrier_oval",        {0.8f,      0.5f}}
};
std::unordered_map<std::string, btRigidBody*> rigidBodyMap;

// Struttura per le coordinate del checkpoint
struct Checkpoint {
    std::string id;
    btVector3 position;
    btVector3 halfExtents; // Dimensioni del checkpoint (mezze estensioni)
};
// Definizione dei checkpoint con posizione e dimensione
std::vector<Checkpoint> checkpoints = {
    { "checkpoint_1", btVector3(-0.25 + 2.5, -0.95 + 5.0, 0.0), btVector3(10, 5, 1)},
    { "checkpoint_2", btVector3(-340.0, -40.0, 785.0 - 7.5), btVector3(1, 5, 10)},
    { "checkpoint_3", btVector3(-693.5 - 7.5, -1 + 5.0, -100.0), btVector3(10, 5, 1)},
    { "checkpoint_4", btVector3(-350.0, -1.0 + 5.0, -315.5 - 7.5), btVector3(1, 5, 10)},
    { "checkpoint_5", btVector3(-221.0 - 7.5, 88.0 + 5.0, 585.0), btVector3(7.5, 5, 1)},
};
std::vector<btRigidBody*> allCheckpointBodies; //variabile che racchiude tutti i checkpoint
std::vector<btRigidBody*> checkpointsLap; // variabile che racchiude solo i checpoint relativi al giro che si sta percorrendo
btTransform lastCheckpointTransform; // variabile per memorizzare l'ultimo checkpoint visitato e gestire i casi in cui la macchina esce dal circuito

bool firstLap, secondLap; // Variabile indicare il giro corrente
bool updateCheckpoints; // Variabile di stato per indicare se i checkpoint devono essere aggiornati

// subject to be observed by UI
Subject collectedCoinsSubject;
Subject checkLapsSubject;

// prototypes declaration
btBvhTriangleMeshShape* getCollisionShape(std::string filepath, std::string format, glm::mat4 TransformMatrix);
void addRigidBodyToDynamicsWorld(btBvhTriangleMeshShape* collisionShape, const std::string& id, float friction, float restitution);
void removeRigidBodyFromDynamicsWorld(const std::string& id);
void printWheelPositions(btRaycastVehicle* vehicle);
void checkCollisions(btRaycastVehicle* vehicle, nlohmann::json& sceneJson);
void setupCollisionProperties(btCollisionObject* obj);
void createCheckpoints();
void checkCheckPoint(btRaycastVehicle* vehicle);
std::vector<btRigidBody*> getCheckpointsForLap(bool firstLap, bool secondLap);
void changeCircuit(nlohmann::json& sceneJson);

class GameObjectCollisionCallback : public btCollisionWorld::ContactResultCallback {
public:
    std::unordered_map<std::string, btRigidBody*>& coinMap;
    std::vector<btRigidBody*>& coinColliders;
    std::vector<btRigidBody*> checkpointsLap;
    nlohmann::json& sceneJson;
    btRaycastVehicle* vehicle;
    bool isCoinCollected = false;
    bool isCheckpointHit = false;
    std::string collectedCoinID;
    int nextCheckpointIndex = 0;
    bool allCheckpointsCollected = false;

    GameObjectCollisionCallback(std::unordered_map<std::string, btRigidBody*>& coinMap,
        std::vector<btRigidBody*>& coinColliders,
        std::vector<btRigidBody*>& checkpointsLap,
        nlohmann::json& sceneJson,
        btRaycastVehicle* vehicle)
        : coinMap(coinMap), coinColliders(coinColliders), checkpointsLap(checkpointsLap), sceneJson(sceneJson), vehicle(vehicle) {
        btVector3 initialPosition(0, -0.5, -10); // Coordinate iniziali
        lastCheckpointTransform.setIdentity(); // Inizializza la trasformazione a identità
        lastCheckpointTransform.setOrigin(initialPosition); // Imposta la posizione iniziale
    }

    btScalar addSingleResult(btManifoldPoint& cp,
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
        else if ((obj0 == vehicle->getRigidBody() && isCheckpoint(obj1)) || (obj1 == vehicle->getRigidBody() && isCheckpoint(obj0))) {
            //std::cout << "collision" << std::endl;
            if (isNextCheckpoint(obj0 == vehicle->getRigidBody() ? obj1 : obj0)) {
                isCheckpointHit = true;
                lastCheckpointTransform.setOrigin(checkpoints[nextCheckpointIndex].position);
                lastCheckpointTransform.setRotation(vehicle->getChassisWorldTransform().getRotation());
                nextCheckpointIndex++;
                if(allCheckpointsCollected){
                    // check if a lap is compleated
                    allCheckpointsCollected = false;
                    checkLapsSubject.notifyCheckLaps(1);
                }
                if (nextCheckpointIndex >= checkpointsLap.size()) {
                    std::cout << "Tutti i checkpoint attraversati" << std::endl;
                    allCheckpointsCollected = true;
                    changeCircuit(sceneJson);
                    nextCheckpointIndex = 0; // Reset the index for the next lap
                }
                return 0;
            }
        }
        return 0;
    }

    bool isCoin(const btCollisionObject* obj) {
        return obj->getUserPointer() != nullptr && coinMap.find(*static_cast<std::string*>(obj->getUserPointer())) != coinMap.end();
    }

    bool isCheckpoint(const btCollisionObject* obj) {
        return obj->getUserPointer() != nullptr;
    }

    bool isNextCheckpoint(const btCollisionObject* obj) {
        if (nextCheckpointIndex < checkpointsLap.size()) {
            return obj == checkpointsLap[nextCheckpointIndex];
        }
        else {
            //std::cerr << "nextCheckpointIndex is out of bounds!" << std::endl;
            return false;
        }
    }

    std::string getCoinID(const btCollisionObject* obj) {
        return *static_cast<std::string*>(obj->getUserPointer());
    }

    std::string getCheckpointID(const btCollisionObject* obj) {
        return *static_cast<std::string*>(obj->getUserPointer());
    }

    void removeCoin(const std::string& coinID, btDynamicsWorld* dynamicsWorld) {
        if (coinMap.find(coinID) != coinMap.end()) {
            btCollisionObject* collectedCoin = coinMap[coinID];
            dynamicsWorld->removeCollisionObject(collectedCoin);
            coinColliders.erase(std::remove(coinColliders.begin(), coinColliders.end(), collectedCoin), coinColliders.end());
            coinMap.erase(coinID);

            delete collectedCoin->getCollisionShape();
            delete static_cast<std::string*>(collectedCoin->getUserPointer());
            delete collectedCoin;
        }
        else {
            std::cout << "Coin '" << coinID << "' not found in coinMap." << std::endl;
        }
    }
};

GameObjectCollisionCallback* gameObjectCallback = nullptr;

void initPhysics(json sceneJson) {

    broadphase = new btDbvtBroadphase();
    collisionConfiguration = new btDefaultCollisionConfiguration();
    dispatcher = new btCollisionDispatcher(collisionConfiguration);
    solver = new btSequentialImpulseConstraintSolver();

    dynamicsWorld = new btDiscreteDynamicsWorld(dispatcher, broadphase, solver, collisionConfiguration);
    dynamicsWorld->setGravity(btVector3(0, -12.75, 0));
    dynamicsWorld->getSolverInfo().m_solverMode |= SOLVER_USE_2_FRICTION_DIRECTIONS;
    dynamicsWorld->getSolverInfo().m_splitImpulse = true;
    dynamicsWorld->getSolverInfo().m_numIterations = 10;

    collectedCoins = 0;
    firstLap = true;
    secondLap = false;
    updateCheckpoints = true;
    
    for (auto it = physicsObjectsMap.begin(); it != physicsObjectsMap.end(); ++it) {
        std::string id = it->first;

        // Cercare l'istanza corrispondente nell'array "instances"
        auto instanceIt = std::find_if(sceneJson["instances"].begin(), sceneJson["instances"].end(), [&id](const json& instance) {
            return instance["id"] == id;
            });

        if (instanceIt != sceneJson["instances"].end()) {
            std::string modelId = (*instanceIt)["model"];

            // Cercare il modello corrispondente nell'array "models" usando il modelId trovato nell'istanza
            auto modelIt = std::find_if(sceneJson["models"].begin(), sceneJson["models"].end(), [&modelId](const json& model) {
                return model["id"] == modelId;
                });

            if (modelIt != sceneJson["models"].end()) {

                std::string modelFilename = (*modelIt)["model"];
                std::string format = (*modelIt)["format"];
                json TMjson = (*instanceIt)["transform"];

                float TMj[16];
                for (int l = 0; l < 16; l++) { TMj[l] = TMjson[l]; }
                glm::mat4 TransformMatrix = glm::mat4(TMj[0], TMj[4], TMj[8], TMj[12], TMj[1], TMj[5], TMj[9], TMj[13], TMj[2], TMj[6], TMj[10], TMj[14], TMj[3], TMj[7], TMj[11], TMj[15]);

                btBvhTriangleMeshShape* collisionShape = getCollisionShape(modelFilename, format, TransformMatrix);
                collisionShapes.push_back(collisionShape);
                addRigidBodyToDynamicsWorld(collisionShape, modelId, it->second[0], it->second[1]);

            }
            else {
                std::cout << "Model for " << modelId << " not found!" << std::endl;
                exit(-1);
            }
        }
        else {
            std::cout << "Instance for " << id << " not found!" << std::endl;
            exit(-1);
        }
    }

        //initiate coins
        for (const auto& instance : sceneJson["instances"]) {
            if (instance["model"] == "coin") {
                // Converti i dati della trasformazione in btTransform
                auto& t = instance["transform"];
                float matrix[16];
                for (int i = 0; i < 16; ++i) {
                    matrix[i] = t[i];
                }

                // Conversione della matrice row-major in column-major
                btMatrix3x3 basis(
                    matrix[0], matrix[1], matrix[2],
                    matrix[4], matrix[5], matrix[6],
                    matrix[8], matrix[9], matrix[10]
                );
                btVector3 origin(matrix[3], matrix[7], matrix[11]);

                btTransform transform(basis, origin);

                std::string coinID = instance["id"];
                btCollisionShape* shape = new btSphereShape(1.0f);
                btDefaultMotionState* motionState = new btDefaultMotionState(btTransform(btQuaternion(0, 0, 0, 1), btVector3(0, 0, 0)));
                btRigidBody::btRigidBodyConstructionInfo rigidBodyCI(0, motionState, shape, btVector3(0, 0, 0));
                btRigidBody* coin = new btRigidBody(rigidBodyCI);
                coin->setCollisionFlags(coin->getCollisionFlags() | btCollisionObject::CF_KINEMATIC_OBJECT);

                coin->setWorldTransform(transform);
                coin->setUserPointer(new std::string(coinID)); // Store the ID in the user pointer

                dynamicsWorld->addRigidBody(coin);
                coinColliders.push_back(coin);

                // Associare la moneta con il suo ID
                coinMap[coinID] = coin;
            }
        }

        //initiate checkpoint
        createCheckpoints();
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

btTriangleMesh* loadMeshFromOBJ(const std::string& filePath) {
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

btTriangleMesh* loadMeshFromMGCG(const std::string& filePath, glm::mat4 TransformMatrix) {
    // Carica e decrittografa il file MGCG (GLTF crittografato)
    tinygltf::Model model;
    tinygltf::TinyGLTF loader;
    std::string warn, err;

    auto modelString = readFile(filePath);

    const std::vector<unsigned char> key = plusaes::key_from_string(&"CG2023SkelKey128"); // 16-char = 128-bit
    const unsigned char iv[16] = {
        0x00, 0x01, 0x02, 0x03, 0x04, 0x05, 0x06, 0x07,
        0x08, 0x09, 0x0A, 0x0B, 0x0C, 0x0D, 0x0E, 0x0F,
    };

    // Decrypt
    unsigned long padded_size = 0;
    std::vector<unsigned char> decrypted(modelString.size());

    plusaes::decrypt_cbc((unsigned char*)modelString.data(), modelString.size(), &key[0], key.size(), &iv, &decrypted[0], decrypted.size(), &padded_size);

    int size = 0;
    void* decomp;

    sscanf(reinterpret_cast<char* const>(&decrypted[0]), "%d", &size);
    decomp = calloc(size, 1);
    sinflate(decomp, (int)size, &decrypted[16], (int)decrypted.size() - 16);

    if (!loader.LoadASCIIFromString(&model, &warn, &err, reinterpret_cast<const char*>(decomp), size, "/")) {
        throw std::runtime_error(warn + err);
    }

    // Crea la mesh Bullet
    btTriangleMesh* mesh = new btTriangleMesh();

    for (const auto& gltfMesh : model.meshes) {
        for (const auto& primitive : gltfMesh.primitives) {
            if (primitive.indices < 0) {
                continue;
            }

            const float* bufferPos = nullptr;

            auto pIt = primitive.attributes.find("POSITION");
            if (pIt != primitive.attributes.end()) {
                const tinygltf::Accessor& posAccessor = model.accessors[pIt->second];
                const tinygltf::BufferView& posView = model.bufferViews[posAccessor.bufferView];
                bufferPos = reinterpret_cast<const float*>(&(model.buffers[posView.buffer].data[posAccessor.byteOffset + posView.byteOffset]));
            } else {
                std::cerr << "Error: No position attribute found in GLTF primitive.\n";
                continue;
            }

            const tinygltf::Accessor& accessor = model.accessors[primitive.indices];
            const tinygltf::BufferView& bufferView = model.bufferViews[accessor.bufferView];
            const tinygltf::Buffer& buffer = model.buffers[bufferView.buffer];

            std::vector<uint32_t> indices;
            switch (accessor.componentType) {
                case TINYGLTF_PARAMETER_TYPE_UNSIGNED_SHORT: {
                    const uint16_t* bufferIndex = reinterpret_cast<const uint16_t*>(&(buffer.data[accessor.byteOffset + bufferView.byteOffset]));
                    for (int i = 0; i < accessor.count; i++) {
                        indices.push_back(bufferIndex[i]);
                    }
                    break;
                }
                case TINYGLTF_PARAMETER_TYPE_UNSIGNED_INT: {
                    const uint32_t* bufferIndex = reinterpret_cast<const uint32_t*>(&(buffer.data[accessor.byteOffset + bufferView.byteOffset]));
                    for (int i = 0; i < accessor.count; i++) {
                        indices.push_back(bufferIndex[i]);
                    }
                    break;
                }
                default:
                    std::cerr << "Index component type " << accessor.componentType << " not supported!" << std::endl;
                    throw std::runtime_error("Error loading GLTF component");
            }

            for (size_t i = 0; i < indices.size(); i += 3) {
                btVector3 vertices[3];
                for (int j = 0; j < 3; j++) {
                    int idx = indices[i + j];
                    glm::vec4 pos = glm::vec4(
                        bufferPos[3 * idx + 0],
                        bufferPos[3 * idx + 1],
                        bufferPos[3 * idx + 2],
                        1.0f
                    );

                    // Applica la trasformazione
                    glm::vec4 transformedPos = TransformMatrix * pos;
                    vertices[j] = btVector3(
                        transformedPos.x,
                        transformedPos.y,
                        transformedPos.z
                    );
                }
                mesh->addTriangle(vertices[0], vertices[1], vertices[2]);
                
            }
        }
    }

    return mesh;
}

btBvhTriangleMeshShape* getCollisionShape(std::string filepath, std::string format, glm::mat4 TransformMatrix){
    btTriangleMesh* mesh;
    if(format == "OBJ"){
        mesh = loadMeshFromOBJ(filepath);
    } else {
        mesh = loadMeshFromMGCG(filepath, TransformMatrix);
    }
    return new btBvhTriangleMeshShape(mesh, true);
}

void addRigidBodyToDynamicsWorld(btBvhTriangleMeshShape* collisionShape, const std::string& id, float friction, float restitution){
    // Create circuit motion state
    btDefaultMotionState* motionState = new btDefaultMotionState(btTransform(btQuaternion(0, 0, 0, 1), btVector3(0, 0, 0)));
    btRigidBody::btRigidBodyConstructionInfo rigidBodyCI(0, motionState, collisionShape, btVector3(0, 0, 0));
    btRigidBody* rigidBody = new btRigidBody(rigidBodyCI);
    rigidBody->setFriction(friction);
    rigidBody->setRestitution(restitution);
    rigidBody->setCollisionFlags(rigidBody->getCollisionFlags() | btCollisionObject::CF_STATIC_OBJECT);
    dynamicsWorld->addRigidBody(rigidBody);
    rigidBodyMap[id] = rigidBody;
}

void removeRigidBodyFromDynamicsWorld(const std::string& id) {
    auto it = rigidBodyMap.find(id);
    if (it != rigidBodyMap.end()) {
        btRigidBody* body = it->second;
        dynamicsWorld->removeRigidBody(body);

        delete body->getMotionState();
        delete body;
        rigidBodyMap.erase(it);
    }
}

void checkCollisions(btRaycastVehicle* vehicle, nlohmann::json& sceneJson) {
    static GameObjectCollisionCallback* gameObjectCallback = nullptr;

    if (updateCheckpoints) {
        std::vector<btRigidBody*> checkpointsLap = getCheckpointsForLap(firstLap, secondLap);
        if (gameObjectCallback == nullptr) {
            // Inizializza il callback delle collisioni la prima volta che viene chiamata checkCollisions
            gameObjectCallback = new GameObjectCollisionCallback(coinMap, coinColliders, checkpointsLap, sceneJson, vehicle);
        }
        else {
            gameObjectCallback->checkpointsLap = checkpointsLap;
            gameObjectCallback->nextCheckpointIndex = 0; // Reset nextCheckpointIndex at the beginning of each lap
        }
        updateCheckpoints = false; // Reset the flag after updating
    }

    // Lista per accumulare le monete da rimuovere
    std::vector<std::string> coinsToRemove;

    // Controlla le collisioni
    dynamicsWorld->contactTest(vehicle->getRigidBody(), *gameObjectCallback);
    if (gameObjectCallback->isCoinCollected) {
        collectedCoins += 1;
        std::string collectedCoinID = gameObjectCallback->collectedCoinID;
        collectedCoinsSubject.notifyCoinCollected(collectedCoins);
        coinsToRemove.push_back(collectedCoinID);
        gameObjectCallback->isCoinCollected = false; // Resetta il flag
    }

    if (gameObjectCallback->isCheckpointHit) {
        //std::cout << "Checkpoint attraversato: " << gameObjectCallback->nextCheckpointIndex << std::endl;
        gameObjectCallback->isCheckpointHit = false; // Resetta il flag
    }

    // Rimuove le monete raccolte
    for (std::string& coinID : coinsToRemove) {
        gameObjectCallback->removeCoin(coinID, dynamicsWorld);
    }
}

void createCheckpoints() {
    for (const auto& checkpoint : checkpoints) {
        // Creazione del collision shape (usiamo un box shape per semplicit‡)
        btCollisionShape* checkpointShape = new btBoxShape(checkpoint.halfExtents);

        // Creazione della trasformazione per il checkpoint
        btTransform checkpointTransform;
        checkpointTransform.setIdentity();
        checkpointTransform.setOrigin(checkpoint.position);

        // Creazione del motion state
        btDefaultMotionState* motionState = new btDefaultMotionState(checkpointTransform);

        // Creazione del rigid body senza massa (per oggetto cinematico)
        btRigidBody::btRigidBodyConstructionInfo rigidBodyCI(0, motionState, checkpointShape, btVector3(0, 0, 0));
        btRigidBody* rigidBody = new btRigidBody(rigidBodyCI);

        // Imposta il corpo rigido come cinematico
        rigidBody->setCollisionFlags(rigidBody->getCollisionFlags() | btCollisionObject::CF_KINEMATIC_OBJECT | btCollisionObject::CF_NO_CONTACT_RESPONSE);

        // Imposta l'ID del checkpoint come user pointer per identificazione
        rigidBody->setUserPointer(new std::string(checkpoint.id));

        // Aggiungi il rigid body al mondo fisico
        dynamicsWorld->addRigidBody(rigidBody);

        // Aggiungi il rigid body alla lista dei checkpoint
        allCheckpointBodies.push_back(rigidBody);
    }
}

std::vector<btRigidBody*> getCheckpointsForLap(bool firstLap, bool secondLap) {
    if (firstLap && !secondLap) {
        // Aggiungi i checkpoint per il primo giro
        checkpointsLap.push_back(allCheckpointBodies[0]); // checkpoint 1
        checkpointsLap.push_back(allCheckpointBodies[1]); // checkpoint 2
        checkpointsLap.push_back(allCheckpointBodies[2]); // checkpoint 3
        checkpointsLap.push_back(allCheckpointBodies[3]); // checkpoint 4
    }
    else if (secondLap && !firstLap) {
        // Aggiungi i checkpoint per il secondo giro
        checkpointsLap.push_back(allCheckpointBodies[0]); // checkpoint 1
        checkpointsLap.push_back(allCheckpointBodies[4]); // checkpoint 5
        checkpointsLap.push_back(allCheckpointBodies[2]); // checkpoint 3
        checkpointsLap.push_back(allCheckpointBodies[3]); // checkpoint 4
    }
    else {
        checkpointsLap.clear();
        checkpointsLap.push_back(allCheckpointBodies[0]); // checkpoint 1
    }

    return checkpointsLap;
}

void changeCircuit(nlohmann::json& sceneJson) {
    checkpointsLap.clear();
    updateCheckpoints = true;
    if (firstLap && !secondLap) {
        firstLap = false;
        secondLap = true;
        removeRigidBodyFromDynamicsWorld("dir_barrier_oval");
        // Aggiungere il corpo rigido per dir_barrier_inner
        // Cercare l'istanza corrispondente nell'array "instances"
        std::string targetId = "dir_barrier_inner";
        auto instanceIt = std::find_if(sceneJson["instances"].begin(), sceneJson["instances"].end(), [&targetId](const json& instance) {
            return instance["id"] == "dir_barrier_inner";
            });
        auto modelIt = std::find_if(sceneJson["models"].begin(), sceneJson["models"].end(), [](const json& model) {
            return model["id"] == "dir_barrier_inner";
            });

        if (modelIt != sceneJson["models"].end()) {

            std::string modelFilename = (*modelIt)["model"];
            std::string format = (*modelIt)["format"];
            json TMjson = (*instanceIt)["transform"];

            float TMj[16];
            for (int l = 0; l < 16; l++) { TMj[l] = TMjson[l]; }
            glm::mat4 TransformMatrix = glm::mat4(TMj[0], TMj[4], TMj[8], TMj[12], TMj[1], TMj[5], TMj[9], TMj[13], TMj[2], TMj[6], TMj[10], TMj[14], TMj[3], TMj[7], TMj[11], TMj[15]);

            btBvhTriangleMeshShape* collisionShape = getCollisionShape(modelFilename, format, TransformMatrix);
            collisionShapes.push_back(collisionShape);
            addRigidBodyToDynamicsWorld(collisionShape, "dir_barrier_inner", 0.8, 0.5);

        }
        else {
            std::cout << "Model for dir_barrier_inner not found!" << std::endl;
            exit(-1);
        }
    }
    else if (!firstLap && secondLap) {
        firstLap = false;
        secondLap = false;
        removeRigidBodyFromDynamicsWorld("dir_barrier_inner");
    }
}
#endif
