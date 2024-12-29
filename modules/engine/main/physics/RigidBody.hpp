#ifndef RIGID_BODY_HPP
#define RIGID_BODY_HPP

class RigidBody {
    
protected:
    
    btRigidBody* rigidBody = nullptr;
    btCollisionShape* collisionShape = nullptr;
    
    virtual void init(glm::mat4 glmTransform){};
    
    btBvhTriangleMeshShape* getCollisionShape(std::string filepath, ModelType type, glm::mat4 TransformMatrix){
        btTriangleMesh* mesh;
        if(type == OBJ){
            mesh = loadMeshFromOBJ(filepath);
        } else {
            mesh = loadMeshFromMGCG(filepath, TransformMatrix);
        }
        return new btBvhTriangleMeshShape(mesh, true);
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
    
    btTransform glmToBtTransform(const glm::mat4& glmMat) {
        // Estrai la base (rotazione/scalatura) dalla matrice glm
        btMatrix3x3 basis(
            glmMat[0][0], glmMat[1][0], glmMat[2][0], // Colonna 1
            glmMat[0][1], glmMat[1][1], glmMat[2][1], // Colonna 2
            glmMat[0][2], glmMat[1][2], glmMat[2][2]  // Colonna 3
        );

        // Estrai la traslazione dalla matrice glm
        btVector3 origin(glmMat[3][0], glmMat[3][1], glmMat[3][2]);

        // Costruisci il btTransform usando basis e origin
        return btTransform(basis, origin);
    }
    
    void printBtTransform(const btTransform& t) {
        const btMatrix3x3& basis = t.getBasis(); // Ottieni la matrice base
        const btVector3& origin = t.getOrigin(); // Ottieni la traslazione

        std::cout << "btTransform (\n";

        // Stampiamo la matrice 4x4 derivata
        for (int row = 0; row < 3; ++row) {
            std::cout << "    ";
            for (int col = 0; col < 3; ++col) {
                std::cout << basis[row][col]; // Elemento della matrice base
                std::cout << ", ";
            }
            std::cout << origin[row]; // Componente della traslazione
            std::cout << ",\n";
        }

        // Aggiungi l'ultima riga fissa [0, 0, 0, 1]
        std::cout << "    0, 0, 0, 1\n";
        std::cout << ");\n";
    }
    
public:
    
    RigidBody(Model* model, glm::mat4 glmTransform) {
        collisionShape = getCollisionShape(model->fileName, model->type, glmTransform);
    }
    
    RigidBody(btCollisionShape* shape) {
        collisionShape = shape;
    }
    
    btRigidBody* getRigidBody() const { return rigidBody; }
    btCollisionShape* getCollisionShape() const { return collisionShape; }
    
    void cleanup() {
        delete collisionShape;
    }
    
};

class KinematicRigidBody : public RigidBody {
    
protected:
    
    void init(glm::mat4 glmTransform) override {
        // Usa la matrice di trasformazione per creare il btTransform iniziale
        btTransform initialTransform = glmToBtTransform(glmTransform);

        // Crea il motionState con la trasformazione iniziale
        btDefaultMotionState* motionState = new btDefaultMotionState(initialTransform);

        // Crea la rigid body con la massa 0 per un oggetto cinematico
        btRigidBody::btRigidBodyConstructionInfo rigidBodyCI(0, motionState, collisionShape, btVector3(0, 0, 0));
        rigidBody = new btRigidBody(rigidBodyCI);

        // Imposta il flag per indicare che è un oggetto cinematico
        rigidBody->setCollisionFlags(rigidBody->getCollisionFlags() | btCollisionObject::CF_KINEMATIC_OBJECT);

        // Applica la trasformazione iniziale
        rigidBody->setWorldTransform(initialTransform);
    }

public:
    
    KinematicRigidBody(Model* model, glm::mat4 glmTransform) : RigidBody(model, glmTransform) {
        init(glmTransform);
    }
    
    KinematicRigidBody(btCollisionShape* shape, glm::mat4 glmTransform) : RigidBody(shape) {
        init(glmTransform);
    }
    
};

class StaticRigidBody : public RigidBody {

private:
    
    float friction = 0.0f;
    float restitution = 0.0f;
    
protected:
    
    void init(glm::mat4 glmTransform) override {
        btDefaultMotionState* motionState = new btDefaultMotionState(btTransform(btQuaternion(0, 0, 0, 1), btVector3(0, 0, 0)));
        btRigidBody::btRigidBodyConstructionInfo rigidBodyCI(0, motionState, collisionShape, btVector3(0, 0, 0));
        rigidBody = new btRigidBody(rigidBodyCI);
        rigidBody->setFriction(friction);
        rigidBody->setRestitution(restitution);
        rigidBody->setCollisionFlags(rigidBody->getCollisionFlags() | btCollisionObject::CF_STATIC_OBJECT);
    }

public:
    
    StaticRigidBody(Model* model, glm::mat4 glmTransform, float friction, float restitution) : RigidBody(model, glmTransform) {
        this->friction = friction;
        this->restitution = restitution;
        init(glmTransform);
    }
    
};

#endif
