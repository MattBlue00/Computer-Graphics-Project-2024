#ifndef SCENE_HPP
#define SCENE_HPP

#include "tools/Types.hpp"
#include "engine/main/GameObject.hpp"

class Scene {
protected:
	VertexDescriptor *VD;
	
	BaseProject *BP;

	// Models, textures and Descriptors (values assigned to the uniforms)
	// Please note that Model objects depends on the corresponding vertex structure
	// Models
	int ModelCount = 0;
	Model **Models;
	std::unordered_map<std::string, int> ModelIds;

	// Textures
	int TextureCount = 0;
	Texture **Textures;
	std::unordered_map<std::string, int> TextureIds;
    
    int InstanceCount = 0;
    json Instances;
    
    std::unordered_map<std::string, glm::mat4> WorldMatrices;
    
    json sceneJson;
    
    // game objects
    std::vector<GameObject*> gameObjects = {};
    
    virtual void buildMultipleInstances(json* instances, json* sceneJson) = 0;

public:
    
    std::vector<GameObject*> getGameObjects() {
        return gameObjects;
    }
    
    json getSceneJson(){
        return sceneJson;
    }
    
	void load(BaseProject *_BP, VertexDescriptor *VD, DescriptorSetLayout &DSL, std::string file) {
		BP = _BP;
		// Models, textures and Descriptors (values assigned to the uniforms)
		json js;
		std::ifstream ifs(file);
		if (!ifs.is_open()) {
		  std::cout << "Error! Scene file not found!";
		  exit(-1);
		}
		try {
			std::cout << "Parsing scene\n";
			ifs >> js;
			ifs.close();
			std::cout << "\n\n\nScene contains " << js.size() << " definitions sections\n\n\n";
            
            sceneJson = js;
			
			// MODELS
			json ms = js["models"];
			ModelCount = (int)ms.size();
			std::cout << "Models count: " << ModelCount << "\n";

			Models = (Model **)calloc(ModelCount, sizeof(Model *));
			for(int k = 0; k < ModelCount; k++) {
				ModelIds[ms[k]["id"]] = k;
				std::string MT = ms[k]["format"].template get<std::string>();
                Models[k] = new Model();

                Models[k]->init(BP, VD, ms[k]["model"], (MT[0] == 'O') ? OBJ : ((MT[0] == 'G') ? GLTF : MGCG));
			}
			
			// TEXTURES
			json ts = js["textures"];
			TextureCount = (int)ts.size();
			std::cout << "Textures count: " << TextureCount << "\n";

			Textures = (Texture **)calloc(ModelCount, sizeof(Texture *));
			for(int k = 0; k < TextureCount; k++) {
				TextureIds[ts[k]["id"]] = k;
                Textures[k] = new Texture();

                Textures[k]->init(BP, ts[k]["texture"]);
			}

			// INSTANCES TextureCount
			Instances = js["instances"];
            
            // WARNING: ADDED BY US
            buildMultipleInstances(&Instances, &sceneJson);
            
			InstanceCount = (int)Instances.size();
			std::cout << "Instances count: " << InstanceCount << "\n";

			for(int k = 0; k < InstanceCount; k++) {
                json TMjson = Instances[k]["transform"];
                
                float TMj[16];
                for(int l = 0; l < 16; l++) {
                    TMj[l] = TMjson[l];
                }
                
                glm::mat4 worldMatrix = glm::mat4(TMj[0],TMj[4],TMj[8],TMj[12],TMj[1],TMj[5],TMj[9],TMj[13],TMj[2],TMj[6],TMj[10],TMj[14],TMj[3],TMj[7],TMj[11],TMj[15]);
                
                WorldMatrices[Instances[k]["id"]] = worldMatrix;
				
			}
            
		} catch (const json::exception& e) {
			std::cout << e.what() << '\n';
		}
	}
    
    virtual void init() = 0;

	void pipelinesAndDescriptorSetsInit(DescriptorSetLayout &DSL) {
        for(auto obj : gameObjects) {
            obj->init(BP, DSL);
        }
	}
	
	void pipelinesAndDescriptorSetsCleanup() {
		// Cleanup datasets
        for (auto obj : gameObjects) {
            obj->descriptorSetCleanup();
        }
	}
    
    void localCleanup() {
        for (auto obj : gameObjects) {
            obj->localCleanup();
        }
        free(Models);
        free(Textures);
    }
	
    void populateCommandBuffer(VkCommandBuffer commandBuffer, int currentImage, Pipeline &P) {
        for(auto obj : gameObjects) {
            obj->populateCommandBuffer(commandBuffer, currentImage, P);
        }
	}
};
    
#endif
