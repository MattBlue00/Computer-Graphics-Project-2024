#ifndef SCENE_HPP
#define SCENE_HPP

#include "tools/Types.hpp"
#include "engine/main/GameObject.hpp"
#include "../modules/data/WorldData.hpp"

class Scene {
protected:

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
    
    virtual void buildMultipleInstances(json* instances, json* sceneJson) = 0;

public:
    
	void load(std::string file, VertexDescriptor* vertexDescriptor) {
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
            
            sceneJson = js;
			
			// MODELS
			json ms = js["models"];
			ModelCount = (int)ms.size();
			std::cout << "Models count: " << ModelCount << "\n";

			Models = (Model **)calloc(ModelCount, sizeof(Model *));
			for(int k = 0; k < ModelCount; k++) {
                std::string modelName = ms[k]["id"];
				ModelIds[modelName] = k;
				std::string MT = ms[k]["format"].template get<std::string>();
                Models[k] = new Model();
                
                ModelType type = (MT[0] == 'O') ? OBJ : ((MT[0] == 'G') ? GLTF : MGCG);
                std::string fileName = ms[k]["model"];

                Models[k]->init(EngineBaseProject, vertexDescriptor, modelName, fileName, type);
			}
			
			// TEXTURES
			json ts = js["textures"];
			TextureCount = (int)ts.size();
			std::cout << "Textures count: " << TextureCount << "\n";

			Textures = (Texture **)calloc(ModelCount, sizeof(Texture *));
			for(int k = 0; k < TextureCount; k++) {
				TextureIds[ts[k]["id"]] = k;
                Textures[k] = new Texture();

                Textures[k]->init(EngineBaseProject, ts[k]["texture"]);
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
    
    virtual void init(){
        for(auto obj : gameObjects) {
            obj->init();
        }
    }
    
    void descriptorSetsInit(DescriptorSetLayout &dsl){
        for(auto obj : gameObjects) {
            obj->descriptorSetInit(dsl);
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
	
    void populateCommandBuffer(VkCommandBuffer commandBuffer, int currentImage, std::unordered_map<PipelineType, Pipeline*> pipelines) {
        for(auto obj : gameObjects) {
            obj->populateCommandBuffer(commandBuffer, currentImage, pipelines[obj->getPipelineType()]);
        }
	}
};
    
#endif
