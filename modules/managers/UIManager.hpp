#ifndef UI_MANAGER_HPP
#define UI_MANAGER_HPP

#include "TextMaker.hpp"  // text header
#include <chrono>         // for time tracking
#include "engine/Subject.hpp"
#include "engine/Manager.hpp"

struct UIManager: public Observer, public Manager {
    
protected:
    // Text Positions
    glm::vec2 outTimerPosition = glm::vec2(0.36f, -0.8f);
    glm::vec2 outLapsPosition = glm::vec2(0.36f, -0.9f);
    glm::vec2 outSpeedPosition = glm::vec2(0.36f, -0.7f);
    glm::vec2 outCoinsPosition = glm::vec2(0.36f, -0.6f);
    
    BaseProject *BP;
    
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

    int countdownValue = 6; // two seconds of nothing and then 4,3,2 at 1 start
    bool isGameStarted = false;
    bool isGameFinished = false;
    int lapsLabel = 1;
    int collectedCoins = 0;

public:
    
    Subject startTimerSubject;

    void init(std::vector<void*> params) override {
        
        BaseProject* _BP = nullptr;
        if (params.size() == 1) {
            _BP = static_cast<BaseProject*>(params[0]);
        } else {
            std::cout << "UIManager.init(): Wrong Parameters" << std::endl;
            exit(-1);
        }
        BP = _BP;
        
        // init TextMakers
        laps.init(BP, &outLaps);
        timer.init(BP, &outTimer);
        speed.init(BP, &outSpeed);
        coins.init(BP, &outCoins);
        
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
    void update(std::vector<void*>) override {
        if(!isGameStarted) handleStartTimer();  // if start-timer changes update the UI
        else if(!isGameFinished) handleTimer(); // if real timer changes update UI
    }
    
    void cleanup() override {
        laps.localCleanup();
        timer.localCleanup();
        speed.localCleanup();
        coins.localCleanup();
    }
    
    // start-timer handle function
    void handleStartTimer(){
        // Start timer will be deleted for semaphores
        if (countdownValue <= 0) {
            return; // Stop updating when countdown reaches zero
        }
        auto now = std::chrono::high_resolution_clock::now();
        auto durationSinceLastUpdate = std::chrono::duration_cast<std::chrono::seconds>(now - lastUpdateTime);

        if (durationSinceLastUpdate.count() >= 1) {
            startTimerSubject.notifyStartSemaphore(countdownValue);
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
    
    void onSpeedChanged(int newSpeed) override {
        outSpeed[0] = {1, {"Speed: " + std::to_string(newSpeed) + " km/h"}, 0, 0, outSpeedPosition};
        outSpeed[1] = {1, {"Speed: " + std::to_string(newSpeed) + " km/h"}, 0, 0, outSpeedPosition};
        speed.changeText(&outSpeed);
    }
    
    void onCoinCollected(int collectedCoins) override {
        this->collectedCoins = collectedCoins;
        outCoins[0] = {1, {"Coins: " + std::to_string(collectedCoins)}, 0, 0, outCoinsPosition};
        outCoins[1] = {1, {"Coins: " + std::to_string(collectedCoins)}, 0, 0, outCoinsPosition};
        coins.changeText(&outCoins);
    }
    
    void onCheckLaps(int lapsDone) override {
        // update lap counter (starts from 1)
        lapsLabel += lapsDone;
        
        // std:: cout << "this lap is the: " << lapsLabel << "\n";

        // after the second lap stop the timer
        if(lapsLabel == 3) {
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
        else if(lapsLabel >= 3) return;
        
        outLaps[0] = {1, {"Lap: " + std::to_string(lapsLabel) + "/2"}, 0, 0, outLapsPosition};
        outLaps[1] = {1, {"Lap: " + std::to_string(lapsLabel) + "/2"}, 0, 0, outLapsPosition};
        laps.changeText(&outLaps);
    }
    
    int computeFinalScore(std::chrono::seconds endTime){
        int remainingTime = 500 - static_cast<int>(endTime.count());
        return std::max(remainingTime, 0) + collectedCoins;
    }
};

#endif

