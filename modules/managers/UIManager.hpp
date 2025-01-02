#ifndef UI_MANAGER_HPP
#define UI_MANAGER_HPP

#include "engine/vulkan/TextMaker.hpp"  // text header
#include <chrono>                       // for time tracking
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
        {1, {"Time: 00:00"}, 0, 0, outTimerPosition},
        {1, {"Time: 00:00"}, 0, 0, outTimerPosition}
    };
    
    std::vector<SingleText> outLaps = {
        {1, {"Lap: 1/2"}, 0, 0, outLapsPosition},
        {1, {"Lap: 1/2"}, 0, 0, outLapsPosition}
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
    TextMaker laps;
    TextMaker timer;
    TextMaker speed;
    TextMaker coins;
    
    // Logic variables
    std::chrono::time_point<std::chrono::high_resolution_clock> startTime;
    std::chrono::time_point<std::chrono::high_resolution_clock> lastUpdateTime;
    
    std::chrono::time_point<std::chrono::high_resolution_clock> startTimeAfterBegin;
    std::chrono::time_point<std::chrono::high_resolution_clock> lastUpdateTimeAfterBegin;
    
    // start-timer handle function
    void handleStartTimer(){
        // Start timer will be deleted for semaphores
        if (countdownValue <= 0) {
            return; // Stop updating when countdown reaches zero
        }
        auto now = std::chrono::high_resolution_clock::now();
        auto durationSinceLastUpdate = std::chrono::duration_cast<std::chrono::seconds>(now - lastUpdateTime);

        if (durationSinceLastUpdate.count() >= 1) {
            startTimerSignal.emit({});
            countdownValue--;
            lastUpdateTime = now; // Update the last update time
            
            // Start real game timer
            if(countdownValue <= 0){
                isGameStarted = true;
                startTimeAfterBegin = std::chrono::high_resolution_clock::now();
                lastUpdateTimeAfterBegin = startTimeAfterBegin;
            }
        }
    }
    
    // timer handle function
    void handleTimer(){
        auto now = std::chrono::high_resolution_clock::now();
        auto durationSinceLastUpdate = std::chrono::duration_cast<std::chrono::seconds>(now - lastUpdateTimeAfterBegin);
        
        if (durationSinceLastUpdate.count() >= 1) {
            auto durationSinceStart = std::chrono::duration_cast<std::chrono::seconds>(now - startTimeAfterBegin);
            int totalSeconds = static_cast<int>(durationSinceStart.count());
            int minutes = totalSeconds / 60;
            int seconds = totalSeconds % 60;
            std::string timeString = (minutes < 10 ? "0" : "") + std::to_string(minutes) + ":" + (seconds < 10 ? "0" : "") + std::to_string(seconds);
            outTimer[0] = {1, {"Time: " + timeString}, 0, 0, outTimerPosition};
            outTimer[1] = {1, {"Time: " + timeString}, 0, 0, outTimerPosition};
            timer.changeText(&outTimer);
            lastUpdateTimeAfterBegin = now; // Update the last update time
        }
    }
    
    void onSpeedChanged() {
        outSpeed[0] = {1, {"Speed: " + std::to_string(currentSpeedKmh) + " km/h"}, 0, 0, outSpeedPosition};
        outSpeed[1] = {1, {"Speed: " + std::to_string(currentSpeedKmh) + " km/h"}, 0, 0, outSpeedPosition};
        speed.changeText(&outSpeed);
    }
    
    void onCoinCollected() {
        outCoins[0] = {1, {"Coins: " + std::to_string(collectedCoins)}, 0, 0, outCoinsPosition};
        outCoins[1] = {1, {"Coins: " + std::to_string(collectedCoins)}, 0, 0, outCoinsPosition};
        coins.changeText(&outCoins);
    }
    
    void onLapChanged() {
        // std:: cout << "this lap is the: " << lapsLabel << "\n";

        // after the second lap stop the timer
        if(currentLap == 0 && !isGameFinished) {
            auto endTime = std::chrono::duration_cast<std::chrono::seconds>(lastUpdateTimeAfterBegin - startTimeAfterBegin);
            
            // std:: cout << "the game is finished lap: " << lapsLabel << "\n";
            isGameFinished = true;
            int finalScore = computeFinalScore(endTime);
            
            // std::cout << "coins" <<  collectedCoins << "\n";

            outLaps[0] = {1, {"Score: " + std::to_string(finalScore)}, 0, 0, outLapsPosition};
            outLaps[1] = {1, {"Score: " + std::to_string(finalScore)}, 0, 0, outLapsPosition};
            laps.changeText(&outLaps);
            return;
        }
        // after the second lap do not update the UI anymore
        else if(currentLap == 0) return;
        
        outLaps[0] = {1, {"Lap: " + std::to_string(currentLap) + "/2"}, 0, 0, outLapsPosition};
        outLaps[1] = {1, {"Lap: " + std::to_string(currentLap) + "/2"}, 0, 0, outLapsPosition};
        laps.changeText(&outLaps);
    }
    
    int computeFinalScore(std::chrono::seconds endTime){
        int remainingTime = 500 - static_cast<int>(endTime.count());
        return std::max(remainingTime, 0) + collectedCoins;
    }


public:

    void init() override {
        
        // init TextMakers
        laps.init(EngineBaseProject, &outLaps);
        timer.init(EngineBaseProject, &outTimer);
        speed.init(EngineBaseProject, &outSpeed);
        coins.init(EngineBaseProject, &outCoins);
        
        startTime = std::chrono::high_resolution_clock::now();
        lastUpdateTime = startTime;
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
    
    void populateCommandBuffer(VkCommandBuffer commandBuffer, int currentImage, int currScene) {
        //startTimer.populateCommandBuffer(commandBuffer, currentImage, currScene);
        laps.populateCommandBuffer(commandBuffer, currentImage, currScene);
        timer.populateCommandBuffer(commandBuffer, currentImage, currScene);
        speed.populateCommandBuffer(commandBuffer, currentImage, currScene);
        coins.populateCommandBuffer(commandBuffer, currentImage, currScene);
    }
    
    // update function
    void update() override {
        if(!isGameStarted) handleStartTimer();  // if start-timer changes update the UI
        else if(!isGameFinished) handleTimer(); // if real timer changes update UI
    }
    
    void cleanup() override {
        laps.localCleanup();
        timer.localCleanup();
        speed.localCleanup();
        coins.localCleanup();
    }
    
    void handleData(std::string id, std::any data) override {
        if (id == SPEED_SIGNAL) {
            onSpeedChanged();
        } else if (id == COINS_SIGNAL) {
            onCoinCollected();
        } else if (id == LAPS_SIGNAL) {
            onLapChanged();
        }
        else {
            std::cerr << "Unknown signal type: " << id << std::endl;
        }
    }

};

#endif

