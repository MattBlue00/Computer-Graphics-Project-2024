#ifndef MAIN_SCENE_HPP
#define MAIN_SCENE_HPP

#include "../modules/objects/Airplane.hpp"
#include "../modules/objects/Airship.hpp"
#include "../modules/objects/Car.hpp"
#include "../modules/objects/Coin.hpp"
#include "../modules/objects/DirectionBarrier.hpp"
#include "../modules/objects/Earth.hpp"
#include "../modules/objects/Firework.hpp"
#include "../modules/objects/Moon.hpp"
#include "../modules/objects/Spaceship.hpp"
#include "../modules/objects/StaticObject.hpp"
#include "../modules/engine/main/GameObject.hpp"
#include "../managers/DrawManager.hpp"
#include "Utils.hpp"
#include <random>

class MainScene: public Scene{

public:
    
    void init() override {
        // Crea un generatore di numeri casuali
        std::random_device rd;  // Entropia hardware (se disponibile)
        std::mt19937 gen(rd()); // Generatore di numeri casuali Mersenne Twister
        std::uniform_int_distribution<> distrib(0, 40); // Intervallo [0, 40]

        for (json instance : Instances) {
            const std::string id = instance["id"];
            GameObject* object = nullptr;

            if (id.starts_with("airplane")) {
                object = new Airplane(
                    std::string(instance["id"]),
                    Models[ModelIds[instance["model"]]],
                    Textures[TextureIds[instance["texture"]]],
                    WorldMatrices[instance["id"]],
                    new DescriptorSet()
                );
            } else if (id.starts_with("airship")) {
                object = new Airship(
                    std::string(instance["id"]),
                    Models[ModelIds[instance["model"]]],
                    Textures[TextureIds[instance["texture"]]],
                    WorldMatrices[instance["id"]],
                    new DescriptorSet()
                );
            } else if (id.starts_with("car")) {
                object = new Car(
                    std::string(instance["id"]),
                    Models[ModelIds[instance["model"]]],
                    Textures[TextureIds[instance["texture"]]],
                    WorldMatrices[instance["id"]],
                    new DescriptorSet()
                );
            } else if (id.starts_with("coin")) {
                object = new Coin(
                    std::string(instance["id"]),
                    Models[ModelIds[instance["model"]]],
                    Textures[TextureIds[instance["texture"]]],
                    WorldMatrices[instance["id"]],
                    new DescriptorSet()
                );
            } else if (id.starts_with("dir_barrier")) {
                object = new DirectionBarrier(
                    std::string(instance["id"]),
                    Models[ModelIds[instance["model"]]],
                    Textures[TextureIds[instance["texture"]]],
                    WorldMatrices[instance["id"]],
                    new DescriptorSet()
                );
            } else if (id.starts_with("earth")) {
                object = new Earth(
                    std::string(instance["id"]),
                    Models[ModelIds[instance["model"]]],
                    Textures[TextureIds[instance["texture"]]],
                    WorldMatrices[instance["id"]],
                    new DescriptorSet()
                );
            } else if (id.starts_with("firework")) {
                // Generate a random number
                int random_number = distrib(gen);
                // Generate a firework
                object = new Firework(
                    std::string(instance["id"]),
                    Models[ModelIds[instance["model"]]],
                    Textures[TextureIds[instance["texture"]]],
                    WorldMatrices[instance["id"]],
                    new DescriptorSet(),
                    random_number
                );
            } else if (id.starts_with("moon")) {
                object = new Moon(
                    std::string(instance["id"]),
                    Models[ModelIds[instance["model"]]],
                    Textures[TextureIds[instance["texture"]]],
                    WorldMatrices[instance["id"]],
                    new DescriptorSet()
                );
            } else if (id.starts_with("spaceship")) {
                object = new Spaceship(
                    std::string(instance["id"]),
                    Models[ModelIds[instance["model"]]],
                    Textures[TextureIds[instance["texture"]]],
                    WorldMatrices[instance["id"]],
                    new DescriptorSet()
                );
            } else {
                object = new StaticObject(
                  std::string(instance["id"]),
                  Models[ModelIds[instance["model"]]],
                  Textures[TextureIds[instance["texture"]]],
                  WorldMatrices[instance["id"]],
                  new DescriptorSet()
                );
            }

            if (object) {
                gameObjects.push_back(object);
            }
        }
    }
    
    void buildMultipleInstances(json* instances, json* sceneJson) override {
        
        json instance;
        // bleachers on the first straight line
        for(int i = 1; i <= FIRST_BLEACHERS_COUNT; i++) {
            
            // left bleachers
            instance = {
                {"id", "bleachers_l" + std::to_string(i)},
                {"model", "bleachers"},
                {"texture", "bleachers"},
                {"transform",  {0, 0, 1, 20,
                                0, 1, 0, 5,
                                -1, 0, 0, FIRST_BLEACHERS_START + BLEACHERS_STEP * (i - 1),
                                0, 0, 0, 1}}
            };
            instances->push_back(instance);
            (*sceneJson)["instances"].push_back(instance);
            
            // right bleachers
            instance = {
                {"id", "bleachers_r" + std::to_string(i)},
                {"model", "bleachers"},
                {"texture", "bleachers"},
                {"transform",  {0, 0, -1, -20,
                                0, 1, 0, 5,
                                1, 0, 0, FIRST_BLEACHERS_START + BLEACHERS_STEP * (i - 1),
                                0, 0, 0, 1}}
            };
            instances->push_back(instance);
            (*sceneJson)["instances"].push_back(instance);
            
        }
        
        // hollow bleachers
        for(int i = 1; i <= 9; i++) {
            
            // left bleachers
            instance = {
                {"id", "bleachers_hl" + std::to_string(i)},
                {"model", "bleachers"},
                {"texture", "bleachers"},
                {"transform",  {1, 0, 0, -300 - (10 * i),
                                0, 1, 0, -30,
                                0, 0, 1, 800,
                                0, 0, 0, 1}}
            };
            instances->push_back(instance);
            (*sceneJson)["instances"].push_back(instance);
            
            // right bleachers
            instance = {
                {"id", "bleachers_hr" + std::to_string(i)},
                {"model", "bleachers"},
                {"texture", "bleachers"},
                {"transform",  {-1, 0, 0, -300 - (10 * i),
                                0, 1, 0, -30,
                                0, 0, -1, 760,
                                0, 0, 0, 1}}
            };
            instances->push_back(instance);
            (*sceneJson)["instances"].push_back(instance);
            
        }
        
        // last bleachers
        for(int i = 1; i <= 20; i++) {
            instance = {
                {"id", "bleachers_" + std::to_string(i)},
                {"model", "bleachers"},
                {"texture", "bleachers"},
                {"transform",  {0, 0, -1, -730,
                                0, 1, 0, 5,
                                1, 0, 0, -60 - BLEACHERS_STEP * (i - 1),
                                0, 0, 0, 1}}
            };
            instances->push_back(instance);
            (*sceneJson)["instances"].push_back(instance);
        }
        
        // first three coins
        for(int i = 0; i < 3; i++){
            instance = {
                {"id", "coin_" + std::to_string(globalCoinCount)},
                {"model", "coin"},
                {"texture", "coin"},
                {"transform",  {0.03, 0, 0, -7.25 + 7.25 * i,
                                0, 0, -0.03, 2.2,
                                0, 0.03, 0, 42.5,
                                0, 0, 0, 1}}
            };
            addCoin(instance, instances, sceneJson);
        }
        
        // three coins on the right
        for(int i = 0; i < 3; i++){
            instance = {
                {"id", "coin_" + std::to_string(globalCoinCount)},
                {"model", "coin"},
                {"texture", "coin"},
                {"transform",  {0.03, 0, 0, -7.25,
                                0, 0, -0.03, 0.5,
                                0, 0.03, 0, 90 + 7 * i,
                                0, 0, 0, 1}}
            };
            addCoin(instance, instances, sceneJson);
        }
        
        // three coins on the left
        for(int i = 0; i < 3; i++){
            instance = {
                {"id", "coin_" + std::to_string(globalCoinCount)},
                {"model", "coin"},
                {"texture", "coin"},
                {"transform",  {0.03, 0, 0, 7.25,
                                0, 0, -0.03, 0.5,
                                0, 0.03, 0, 120 + 7 * i,
                                0, 0, 0, 1}}
            };
            addCoin(instance, instances, sceneJson);
        }
        
        // three coins in the middle
        for(int i = 0; i < 3; i++){
            instance = {
                {"id", "coin_" + std::to_string(globalCoinCount)},
                {"model", "coin"},
                {"texture", "coin"},
                {"transform",  {0.03, 0, 0, 0.25,
                                0, 0, -0.03, 0.5,
                                0, 0.03, 0, 180 + 7 * i,
                                0, 0, 0, 1}}
            };
            addCoin(instance, instances, sceneJson);
        }
        
        // three coins under the rainbow
        for(int i = 0; i < 3; i++){
            instance = {
                {"id", "coin_" + std::to_string(globalCoinCount)},
                {"model", "coin"},
                {"texture", "coin"},
                {"transform",  {0.03, 0, 0, -7 + 7.25 * i,
                                0, 0, -0.03, 0.5,
                                0, 0.03, 0, 250,
                                0, 0, 0, 1}}
            };
            addCoin(instance, instances, sceneJson);
        }
        
        // three coins after the crossroads
        for(int i = 0; i < 3; i++){
            instance = {
                {"id", "coin_" + std::to_string(globalCoinCount)},
                {"model", "coin"},
                {"texture", "coin"},
                {"transform",  {0.03, 0, 0, -6.5 + 7 * i,
                                0, 0, -0.03, 2.2,
                                0, 0.03, 0, i != 1 ? 322.5 : 332.5,
                                0, 0, 0, 1}}
            };
            addCoin(instance, instances, sceneJson);
        }
        
        // ten coins near the star and the rocket
        for(int i = 0; i < 10; i++){
            instance = {
                {"id", "coin_" + std::to_string(globalCoinCount)},
                {"model", "coin"},
                {"texture", "coin"},
                {"transform",  {0.03, 0, 0, i%2 == 0 ? -4.25 : 4.75,
                                0, 0, -0.03, 0.5,
                                0, 0.03, 0, 400 + 15 * i,
                                0, 0, 0, 1}}
            };
            addCoin(instance, instances, sceneJson);
        }
        
        // two coins near the first tires pile
        for(int i = 0; i < 2; i++){
            instance = {
                {"id", "coin_" + std::to_string(globalCoinCount)},
                {"model", "coin"},
                {"texture", "coin"},
                {"transform",  {0.03, 0, 0, i == 0 ? 7.75 : 2.25,
                                0, 0, -0.03, 2.2,
                                0, 0.03, 0, 602.5,
                                0, 0, 0, 1}}
            };
            addCoin(instance, instances, sceneJson);
        }
        
        // coins on the first turn
        for(int i = 0; i < 3; i++){
            instance = {
                {"id", "coin_" + std::to_string(globalCoinCount)},
                {"model", "coin"},
                {"texture", "coin"},
                {"transform",  {0.03, 0, 0, -14 - i * 3.5,
                                0, 0, -0.03, 0.5,
                                0, 0.03, 0, 750 + i * 5,
                                0, 0, 0, 1}}
            };
            addCoin(instance, instances, sceneJson);
        }
        
        // 25 coins in the hollow
        for(int i = 0; i < 5; i++){
            for(int j = 0; j < 5; j++){
                instance = {
                    {"id", "coin_" + std::to_string(globalCoinCount)},
                    {"model", "coin"},
                    {"texture", "coin"},
                    {"transform",  {0.03, 0, 0, -340 - 5 * i,
                                    0, 0, -0.03, i <= 2 ? -40.5 - i * 0.5 : -41.5 + (i-2) * 0.75,
                                    0, 0.03, 0, 785 - 3.5 * j,
                                    0, 0, 0, 1}}
                };
                addCoin(instance, instances, sceneJson);
            }
        }
        
        // coins on the second turn
        for(int i = 0; i < 3; i++){
            instance = {
                {"id", "coin_" + std::to_string(globalCoinCount)},
                {"model", "coin"},
                {"texture", "coin"},
                {"transform",  {0.03, 0, 0, -677 - i * 3.5,
                                0, 0, -0.03, 0.5,
                                0, 0.03, 0, 760 - i * 5,
                                0, 0, 0, 1}}
            };
            addCoin(instance, instances, sceneJson);
        }
        
        // coins at the chicane
        for(int i = 0; i < 2; i++){
            instance = {
                {"id", "coin_" + std::to_string(globalCoinCount)},
                {"model", "coin"},
                {"texture", "coin"},
                {"transform",  {0.03, 0, 0, i == 0 ? -707 : -695,
                                0, 0, -0.03, 0.5,
                                0, 0.03, 0, i == 0 ? 650 : 620,
                                0, 0, 0, 1}}
            };
            addCoin(instance, instances, sceneJson);
        }
        
        // three coins after the chicane
        for(int i = 0; i < 3; i++){
            instance = {
                {"id", "coin_" + std::to_string(globalCoinCount)},
                {"model", "coin"},
                {"texture", "coin"},
                {"transform",  {0.03, 0, 0, -695 - 7.25 * i,
                                0, 0, -0.03, 2.2,
                                0, 0.03, 0, 547.5,
                                0, 0, 0, 1}}
            };
            addCoin(instance, instances, sceneJson);
        }
        
        // rhombus coins (central)
        for(int i = 0; i < 6; i++){
            instance = {
                {"id", "coin_" + std::to_string(globalCoinCount)},
                {"model", "coin"},
                {"texture", "coin"},
                {"transform",  {0.03, 0, 0, -702.25,
                                0, 0, -0.03, 0.5,
                                0, 0.03, 0, 500 - 15 * i,
                                0, 0, 0, 1}}
            };
            addCoin(instance, instances, sceneJson);
        }
        
        // rhombus coins (left)
        for(int i = 0; i < 5; i++){
            instance = {
                {"id", "coin_" + std::to_string(globalCoinCount)},
                {"model", "coin"},
                {"texture", "coin"},
                {"transform",  {0.03, 0, 0, -709.5,
                                0, 0, -0.03, 0.5,
                                0, 0.03, 0, 492.5 - 15 * i,
                                0, 0, 0, 1}}
            };
            addCoin(instance, instances, sceneJson);
        }
        
        // rhombus coins (right)
        for(int i = 0; i < 5; i++){
            instance = {
                {"id", "coin_" + std::to_string(globalCoinCount)},
                {"model", "coin"},
                {"texture", "coin"},
                {"transform",  {0.03, 0, 0, -695,
                                0, 0, -0.03, 0.5,
                                0, 0.03, 0, 492.5 - 15 * i,
                                0, 0, 0, 1}}
            };
            addCoin(instance, instances, sceneJson);
        }
        
        // five coins after the landing track
        for(int i = 0; i < 5; i++){
            instance = {
                {"id", "coin_" + std::to_string(globalCoinCount)},
                {"model", "coin"},
                {"texture", "coin"},
                {"transform",  {0.03, 0, 0, i == 2 ? -702.3 : -695.3 - 13.95 * (i % 3),
                                0, 0, -0.03, 2.2,
                                0, 0.03, 0, i == 2 ? 247.5 : (i < 2 ? 297.5 : 197.5),
                                0, 0, 0, 1}}
            };
            addCoin(instance, instances, sceneJson);
        }
        
        // three coins on the last ramp before the tracks rejoin
        for(int i = 0; i < 3; i++){
            instance = {
                {"id", "coin_" + std::to_string(globalCoinCount)},
                {"model", "coin"},
                {"texture", "coin"},
                {"transform",  {0.03, 0, 0, -695 - i * 7,
                                0, 0, -0.03, 2.2,
                                0, 0.03, 0, 97.5,
                                0, 0, 0, 1}}
            };
            addCoin(instance, instances, sceneJson);
        }
        
        // fifteen coins after the tracks rejoin
        for(int i = 0; i < 5; i++){
            for(int j = 0; j < 3; j++){
                instance = {
                    {"id", "coin_" + std::to_string(globalCoinCount)},
                    {"model", "coin"},
                    {"texture", "coin"},
                    {"transform",  {0.03, 0, 0, -694.5 - j * 7,
                                    0, 0, -0.03, 0.5,
                                    0, 0.03, 0, -100 - 10 * i,
                                    0, 0, 0, 1}}
                };
                addCoin(instance, instances, sceneJson);
            }
        }
        
        // coins on the third turn
        for(int i = 0; i < 3; i++){
            instance = {
                {"id", "coin_" + std::to_string(globalCoinCount)},
                {"model", "coin"},
                {"texture", "coin"},
                {"transform",  {0.03, 0, 0, -685 + i * 5,
                                0, 0, -0.03, 0.5,
                                0, 0.03, 0, -294 - i * 5,
                                0, 0, 0, 1}}
            };
            addCoin(instance, instances, sceneJson);
        }
        
        // three coins after the third turn
        for(int i = 0; i < 3; i++){
            instance = {
                {"id", "coin_" + std::to_string(globalCoinCount)},
                {"model", "coin"},
                {"texture", "coin"},
                {"transform",  {0.03, 0, 0, -582.8,
                                0, 0, -0.03, 2.2,
                                0, 0.03, 0, -314.5 - i * 7,
                                0, 0, 0, 1}}
            };
            addCoin(instance, instances, sceneJson);
        }
        
        // 15 coins near the big rainbow and before the shortcut
        for(int i = 0; i < 5; i++){
            for(int j = 0; j < 3; j++){
                instance = {
                    {"id", "coin_" + std::to_string(globalCoinCount)},
                    {"model", "coin"},
                    {"texture", "coin"},
                    {"transform",  {0.03, 0, 0, -480 + i * 30,
                                    0, 0, -0.03, 0.5,
                                    0, 0.03, 0, -316.5 - 7 * j,
                                    0, 0, 0, 1}}
                };
                addCoin(instance, instances, sceneJson);
            }
        }
        
        // 5 coins before the fourth turn
        for(int i = 0; i < 5; i++){
            instance = {
                {"id", "coin_" + std::to_string(globalCoinCount)},
                {"model", "coin"},
                {"texture", "coin"},
                {"transform",  {0.03, 0, 0, -270 + i * 30,
                                0, 0, -0.03, 0.5,
                                0, 0.03, 0, -327,
                                0, 0, 0, 1}}
            };
            addCoin(instance, instances, sceneJson);
        }
        
        // coins on the fourth turn
        for(int i = 0; i < 3; i++){
            instance = {
                {"id", "coin_" + std::to_string(globalCoinCount)},
                {"model", "coin"},
                {"texture", "coin"},
                {"transform",  {0.03, 0, 0, -32.5 + i * 7,
                                0, 0, -0.03, 0.5,
                                0, 0.03, 0, -304.5 + i * 4.5,
                                0, 0, 0, 1}}
            };
            addCoin(instance, instances, sceneJson);
        }
        
        // coins on the shortcut
        for(int i = 0; i < 15; i++){
            for(int j = 0; j < 2; j++){
                instance = {
                    {"id", "coin_" + std::to_string(globalCoinCount)},
                    {"model", "coin"},
                    {"texture", "coin"},
                    {"transform",  {0.03, 0, 0, -130 + i * 7.5,
                        0, 0, -0.03, 0.5,
                        0, 0.03, 0, (j == 0 ? -270 : -274) + i * 5.3,
                        0, 0, 0, 1}}
                };
                addCoin(instance, instances, sceneJson);
            }
        }
        
        // coins on the first ramp of the inner track
        for(int i = 0; i < 3; i++){
            instance = {
                {"id", "coin_" + std::to_string(globalCoinCount)},
                {"model", "coin"},
                {"texture", "coin"},
                {"transform",  {0.03, 0, 0, -196.5 - i * 1.5,
                                0, 0, -0.03, 2.2,
                                0, 0.03, 0, 375 - i * 7,
                                0, 0, 0, 1}}
            };
            addCoin(instance, instances, sceneJson);
        }
        
        // coins under the rainbow tunnel
        for(int i = 0; i < 15; i++){
            instance = {
                {"id", "coin_" + std::to_string(globalCoinCount)},
                {"model", "coin"},
                {"texture", "coin"},
                {"transform",  {0.03, 0, 0, -280 - i * 7,
                                0, 0, -0.03, i < 10 ? 0.5 : 0.5 + 0.1 * (i-10),
                                0, 0.03, 0, 384.5 + i * 1.25,
                                0, 0, 0, 1}}
            };
            addCoin(instance, instances, sceneJson);
        }
        
        // three coins on the first inner turn
        for(int i = 0; i < 3; i++){
            instance = {
                {"id", "coin_" + std::to_string(globalCoinCount)},
                {"model", "coin"},
                {"texture", "coin"},
                {"transform",  {0.03, 0, 0, -530 - i * 3,
                                0, 0, -0.03, 7.5 + i * 0.75,
                                0, 0.03, 0, 470 + i * 4,
                                0, 0, 0, 1}}
            };
            addCoin(instance, instances, sceneJson);
        }
        
        // three coins on the second inner turn
        for(int i = 0; i < 3; i++){
            instance = {
                {"id", "coin_" + std::to_string(globalCoinCount)},
                {"model", "coin"},
                {"texture", "coin"},
                {"transform",  {0.03, 0, 0, -532.5 + i * 3.5,
                                0, 0, -0.03, 34.5 + i * 1.5,
                                0, 0.03, 0, 578 + i * 4,
                                0, 0, 0, 1}}
            };
            addCoin(instance, instances, sceneJson);
        }
        
        // three coins between the second and the third inner turn
        for(int i = 0; i < 3; i++){
            instance = {
                {"id", "coin_" + std::to_string(globalCoinCount)},
                {"model", "coin"},
                {"texture", "coin"},
                {"transform",  {0.03, 0, 0, -430 + i * 7,
                                0, 0, -0.03, 61.5 + i * 1.25,
                                0, 0.03, 0, 649 + i * 1.5,
                                0, 0, 0, 1}}
            };
            addCoin(instance, instances, sceneJson);
        }
        
        // three coins on the third inner turn
        for(int i = 0; i < 3; i++){
            instance = {
                {"id", "coin_" + std::to_string(globalCoinCount)},
                {"model", "coin"},
                {"texture", "coin"},
                {"transform",  {0.03, 0, 0, -261 + i * 4.5,
                                0, 0, -0.03, 84.5 + i * 0.5,
                                0, 0.03, 0, 636 - i * 4,
                                0, 0, 0, 1}}
            };
            addCoin(instance, instances, sceneJson);
        }
        
        // coins into the rainbow tunnel
        for(int i = 0; i < 7; i++){
            for(int j = 0; j < 3; j++){
                instance = {
                    {"id", "coin_" + std::to_string(globalCoinCount)},
                    {"model", "coin"},
                    {"texture", "coin"},
                    {"transform",  {0.03, 0, 0, -278 - i * 3.5 + j * 7,
                                    0, 0, -0.03, 92.5,
                                    0, 0.03, 0, 422.5 - i * 10,
                                    0, 0, 0, 1}}
                };
                addCoin(instance, instances, sceneJson);
            }
        }
        
        // coins in air
        for(int i = 0; i < 3; i++){
            instance = {
                {"id", "coin_" + std::to_string(globalCoinCount)},
                {"model", "coin"},
                {"texture", "coin"},
                {"transform",  {0.03, 0, 0, -311.5 - i * 2,
                                0, 0, -0.03, 94 + i * 1.2,
                                0, 0.03, 0, 308.8 - i * 6,
                                0, 0, 0, 1}}
            };
            addCoin(instance, instances, sceneJson);
        }
        
        // coins before the s turn
        for(int i = 0; i < 5; i++){
            instance = {
                {"id", "coin_" + std::to_string(globalCoinCount)},
                {"model", "coin"},
                {"texture", "coin"},
                {"transform",  {0.03, 0, 0, -506 - i * 3,
                                0, 0, -0.03, 0.5,
                                0, 0.03, 0, -86 - i * 4,
                                0, 0, 0, 1}}
            };
            addCoin(instance, instances, sceneJson);
        }
        
        // first three coins on the s turn
        for(int i = 0; i < 3; i++){
            instance = {
                {"id", "coin_" + std::to_string(globalCoinCount)},
                {"model", "coin"},
                {"texture", "coin"},
                {"transform",  {0.03, 0, 0, -580 - i * 4,
                                0, 0, -0.03, 0.5,
                                0, 0.03, 0, -122 + i * 3.5,
                                0, 0, 0, 1}}
            };
            addCoin(instance, instances, sceneJson);
        }
        
        // second three coins on the s turn
        for(int i = 0; i < 3; i++){
            instance = {
                {"id", "coin_" + std::to_string(globalCoinCount)},
                {"model", "coin"},
                {"texture", "coin"},
                {"transform",  {0.03, 0, 0, -581 + i * 5,
                                0, 0, -0.03, 0.5,
                                0, 0.03, 0, -67 + i * 3.5,
                                0, 0, 0, 1}}
            };
            addCoin(instance, instances, sceneJson);
        }
        
        // third three coins on the s turn
        for(int i = 0; i < 3; i++){
            instance = {
                {"id", "coin_" + std::to_string(globalCoinCount)},
                {"model", "coin"},
                {"texture", "coin"},
                {"transform",  {0.03, 0, 0, -567.5 - i * 6,
                                0, 0, -0.03, 0.5,
                                0, 0.03, 0, -19.5 + i * 2,
                                0, 0, 0, 1}}
            };
            addCoin(instance, instances, sceneJson);
        }
        
    }

private:

    // bleachers count and parameters
    const int FIRST_BLEACHERS_COUNT = 29; // per side
    const int FIRST_BLEACHERS_START = -155;
    const int BLEACHERS_STEP = 10;

    void addCoin(json coin, json* instances, json* sceneJson){
        instances->push_back(coin);
        (*sceneJson)["instances"].push_back(coin);
        globalCoinCount++;
    }
    
};

#endif
