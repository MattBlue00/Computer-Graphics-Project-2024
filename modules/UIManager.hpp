#ifndef UIMANAGER_HPP
#define UIMANAGER_HPP

#include "TextMaker.hpp"  // text header
#include <chrono>         // for time tracking
#include <glm/vec2.hpp>   // for glm::vec2

struct UIManager: public Observer {
    
    // Text Positions
    glm::vec2 outStartTimerPosition = glm::vec2(0.0f, -0.9f);
    glm::vec2 outTimerPosition = glm::vec2(0.4f, -0.8f);
    glm::vec2 outLapsPosition = glm::vec2(0.4f, -0.9f);
    glm::vec2 outSpeedPosition = glm::vec2(0.4f, -0.7f);
    glm::vec2 outCoinsPosition = glm::vec2(0.4, -0.6f);

    
    // Text Vectors
    BaseProject *BP;
    std::vector<SingleText> outStartTimer = {
        {1, {"3"}, 0, 0, outStartTimerPosition},
        {1, {"3"}, 0, 0, outStartTimerPosition}
    };
    
    std::vector<SingleText> outTimer = {
        {1, {"Time: 0s"}, 0, 0, outTimerPosition},
        {1, {"Time: 0s"}, 0, 0, outTimerPosition}
    };
    
    std::vector<SingleText> outLapse = {
        {1, {"Laps Done: 0"}, 0, 0, outLapsPosition},
        {1, {"Laps Done: 0"}, 0, 0, outLapsPosition}
    };
    
    std::vector<SingleText> outSpeed = {
        {1, {"Speed: 0 km/h"}, 0, 0, outSpeedPosition},
        {1, {"Speed: 0 km/h"}, 0, 0, outSpeedPosition}
    };
    
    std::vector<SingleText> outCoins = {
        {1, {"Coins: 0"}, 0, 0, outCoinsPosition},
        {1, {"Coins: 0"}, 0, 0, outCoinsPosition}
    };
    
    // Text Makers
    TextMaker startTimer;
    TextMaker laps;
    TextMaker timer;
    TextMaker speed;
    TextMaker coins;
    
    
    // Logic variables
    std::chrono::time_point<std::chrono::high_resolution_clock> startTime;
    std::chrono::time_point<std::chrono::high_resolution_clock> lastUpdateTime;
    
    std::chrono::time_point<std::chrono::high_resolution_clock> startTimeAfterBegin;
    std::chrono::time_point<std::chrono::high_resolution_clock> lastUpdateTimeAfterBegin;

    int countdownValue = 3;
    bool isGameStarted = false;

    // init UI
    void init(BaseProject *_BP) {
        BP = _BP;
        
        // init TextMakers
        startTimer.init(BP, &outStartTimer);
        laps.init(BP, &outLapse);
        timer.init(BP, &outTimer);
        speed.init(BP, &outSpeed);
        coins.init(BP, &outCoins);
        
        startTime = std::chrono::high_resolution_clock::now();
        lastUpdateTime = startTime;
    }
    
    // lifecycle methods
    void pipelinesAndDescriptorSetsInit() {
        startTimer.pipelinesAndDescriptorSetsInit();
        laps.pipelinesAndDescriptorSetsInit();
        timer.pipelinesAndDescriptorSetsInit();
        speed.pipelinesAndDescriptorSetsInit();
        coins.pipelinesAndDescriptorSetsInit();

    }
    
    void pipelinesAndDescriptorSetsCleanup() {
        startTimer.pipelinesAndDescriptorSetsCleanup();
        laps.pipelinesAndDescriptorSetsCleanup();
        timer.pipelinesAndDescriptorSetsCleanup();
        speed.pipelinesAndDescriptorSetsCleanup();
        coins.pipelinesAndDescriptorSetsCleanup();

    }
    
    void localCleanup() {
        startTimer.localCleanup();
        laps.localCleanup();
        timer.localCleanup();
        speed.localCleanup();
        coins.localCleanup();
    }
    
    void populateCommandBuffer(VkCommandBuffer commandBuffer, int currentImage, int currScene) {
        startTimer.populateCommandBuffer(commandBuffer, currentImage, currScene);
        laps.populateCommandBuffer(commandBuffer, currentImage, currScene);
        timer.populateCommandBuffer(commandBuffer, currentImage, currScene);
        speed.populateCommandBuffer(commandBuffer, currentImage, currScene);
        coins.populateCommandBuffer(commandBuffer, currentImage, currScene);
    }
    
    // update function
    bool shouldUpdateUI() {
        bool shouldUpdate = false;
        
        if(!isGameStarted) shouldUpdate = handleStartTimer();  // if start-timer changes update the UI
        else shouldUpdate = handleTimer();                    // if real timer changes update UI
        
        return shouldUpdate; // UI has not been updated
    }
    
    // start-timer handle function
    bool handleStartTimer(){
        if (countdownValue <= 0) {
            return false; // Stop updating when countdown reaches zero
        }
        auto now = std::chrono::high_resolution_clock::now();
        auto durationSinceLastUpdate = std::chrono::duration_cast<std::chrono::seconds>(now - lastUpdateTime);

        if (durationSinceLastUpdate.count() >= 1) {
            countdownValue--;
            outStartTimer[0] = {1, {std::to_string(countdownValue)}, 0, 0, outStartTimerPosition};
            outStartTimer[1] = {1, {std::to_string(countdownValue)}, 0, 0, outStartTimerPosition};
            startTimer.changeText(&outStartTimer);
            lastUpdateTime = now; // Update the last update time
            
            // Start real game timer
            if(countdownValue <= 0){
                // TODO: investigate
                // this makes the text disappear after 0 seconds ?????????
                // don't really know why but it is a nice effect ?????????
                outStartTimer[0] = {1, {"0"}, 0, 0, outStartTimerPosition};
                outStartTimer[1] = {1, {"0"}, 0, 0, outStartTimerPosition};

                isGameStarted = true;
                startTimeAfterBegin = std::chrono::high_resolution_clock::now();
                lastUpdateTimeAfterBegin = startTimeAfterBegin;
            }
            return true; // UI has been updated
        }
        return false;
    }
    
    // timer handle function
    bool handleTimer(){
        auto now = std::chrono::high_resolution_clock::now();
        auto durationSinceLastUpdate = std::chrono::duration_cast<std::chrono::seconds>(now - lastUpdateTimeAfterBegin);
        
        if (durationSinceLastUpdate.count() >= 1) {
            auto durationSinceStart = std::chrono::duration_cast<std::chrono::seconds>(now - startTimeAfterBegin);
            int seconds = static_cast<int>(durationSinceStart.count());
            outTimer[0] = {1, {"Time: " + std::to_string(seconds) + "s"}, 0, 0, outTimerPosition};
            outTimer[1] = {1, {"Time: " + std::to_string(seconds) + "s"}, 0, 0, outTimerPosition};
            timer.changeText(&outTimer);
            lastUpdateTimeAfterBegin = now; // Update the last update time
            return true; // UI has been updated
        }
        return false; // UI has not been updated
    }
    
    void onSpeedChanged(int newSpeed) override {
        outSpeed[0] = {1, {"Speed: " + std::to_string(newSpeed) + " km/h"}, 0, 0, outSpeedPosition};
        outSpeed[1] = {1, {"Speed: " + std::to_string(newSpeed) + " km/h"}, 0, 0, outSpeedPosition};
        speed.changeText(&outSpeed);
    }
    
    void onCoinCollected(int collectedCoins) override {
        outCoins[0] = {1, {"Coins: " + std::to_string(collectedCoins)}, 0, 0, outCoinsPosition};
        outCoins[1] = {1, {"Coins: " + std::to_string(collectedCoins)}, 0, 0, outCoinsPosition};
        coins.changeText(&outCoins);
    }
};

#endif

