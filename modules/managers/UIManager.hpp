#ifndef UI_MANAGER_HPP
#define UI_MANAGER_HPP

#include "engine/vulkan/TextMaker.hpp"  // text header
#include "engine/main/Manager.hpp"

class UIManager: public Manager, public Receiver {
    
protected:
    
    // Text Positions
    glm::vec2 outLapsPosition = glm::vec2(-0.94f, 0.815f);
    glm::vec2 outTimerPosition = glm::vec2(-0.52f, 0.815f);
    glm::vec2 outSpeedPosition = glm::vec2(-0.06f, 0.815f);
    glm::vec2 outCoinsPosition = glm::vec2(0.56f, 0.815f);
    
    // Text Vectors
    std::vector<SingleText> outTimer = {
        {1, {"Time: 00:00"}, 0, 0, outTimerPosition}
    };
    
    std::vector<SingleText> outLaps = {
        {1, {"Lap: 1/2"}, 0, 0, outLapsPosition}
    };
    
    std::vector<SingleText> outSpeed = {
        {1, {"Speed: 0 km/h"}, 0, 0, outSpeedPosition}
    };
    
    std::vector<SingleText> outCoins = {
        {1, {"Coins: 0"}, 0, 0, outCoinsPosition}
    };
    
    // Text Makers
    TextMaker laps;
    TextMaker timer;
    TextMaker speed;
    TextMaker coins;
    
    // timer handle function
    void onTimeChanged(std::string timeString){
        outTimer[0] = {1, {"Time: " + timeString}, 0, 0, outTimerPosition};
        timer.changeText(&outTimer);
    }
    
    void onSpeedChanged(int currentSpeedKmh) {
        outSpeed[0] = {1, {"Speed: " + std::to_string(currentSpeedKmh) + " km/h"}, 0, 0, outSpeedPosition};
        speed.changeText(&outSpeed);
    }
    
    void onCoinsChanged(int collectedCoins) {
        outCoins[0] = {1, {"Coins: " + std::to_string(collectedCoins)}, 0, 0, outCoinsPosition};
        coins.changeText(&outCoins);
    }
    
    void onLapChanged(int currentLap) {
        if (currentLap != 0){
            outLaps[0] = {1, {"Lap: " + std::to_string(currentLap) + "/2"}, 0, 0, outLapsPosition};
            laps.changeText(&outLaps);
        }
    }
    
    void onScoreGenerated(int score){
        outLaps[0] = {1, {"Score: " + std::to_string(score)}, 0, 0, outLapsPosition};
        laps.changeText(&outLaps);
    }


public:

    void init() override {
        // init TextMakers
        laps.init(EngineBaseProject, &outLaps);
        timer.init(EngineBaseProject, &outTimer);
        speed.init(EngineBaseProject, &outSpeed);
        coins.init(EngineBaseProject, &outCoins);
    }
    
    // lifecycle methods
    void pipelinesAndDescriptorSetsInit() {
        laps.pipelinesAndDescriptorSetsInit();
        timer.pipelinesAndDescriptorSetsInit();
        speed.pipelinesAndDescriptorSetsInit();
        coins.pipelinesAndDescriptorSetsInit();

    }
    
    void pipelinesAndDescriptorSetsCleanup() {
        laps.pipelinesAndDescriptorSetsCleanup();
        timer.pipelinesAndDescriptorSetsCleanup();
        speed.pipelinesAndDescriptorSetsCleanup();
        coins.pipelinesAndDescriptorSetsCleanup();

    }
    
    void populateCommandBuffer(VkCommandBuffer commandBuffer, int currentImage) {
        laps.populateCommandBuffer(commandBuffer, currentImage);
        timer.populateCommandBuffer(commandBuffer, currentImage);
        speed.populateCommandBuffer(commandBuffer, currentImage);
        coins.populateCommandBuffer(commandBuffer, currentImage);
    }
    
    void update() override {}
    
    void cleanup() override {
        laps.localCleanup();
        timer.localCleanup();
        speed.localCleanup();
        coins.localCleanup();
    }
    
    void onSignal(std::string id, std::any data) override {
        if (id == SPEED_SIGNAL) {
            onSpeedChanged(std::any_cast<int>(data));
        } else if (id == TIME_SIGNAL) {
            onTimeChanged(std::any_cast<std::string>(data));
        } else if (id == COINS_SIGNAL) {
            onCoinsChanged(std::any_cast<int>(data));
        } else if (id == LAPS_SIGNAL) {
            onLapChanged(std::any_cast<int>(data));
        } else if (id == SCORE_SIGNAL) {
            onScoreGenerated(std::any_cast<int>(data));
        }
        else {
            std::cerr << "Unknown signal type: " << id << std::endl;
        }
    }

};

#endif

