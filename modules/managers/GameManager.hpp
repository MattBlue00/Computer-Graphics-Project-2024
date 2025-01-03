#ifndef GAME_MANAGER_HPP
#define GAME_MANAGER_HPP

#include <chrono>                       // for time tracking

class GameManager : public Manager, public Receiver {
    
protected:
    
    int countdownValue = 8;
    bool isGameStarted = false;
    bool isGameFinished = false;
    
    int collectedCoins = 0;
    
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
            countdownSignal.emit(countdownValue);
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
            timeSignal.emit(timeString);
            lastUpdateTimeAfterBegin = now; // Update the last update time
        }
    }
    
    int computeFinalScore(std::chrono::seconds endTime){
        int remainingTime = 500 - static_cast<int>(endTime.count());
        return std::max(remainingTime, 0) + collectedCoins;
    }
    
    void onLapChanged(int currentLap) {
        // after the second lap stop the timer
        if(currentLap == 0 && !isGameFinished) {
            auto endTime = std::chrono::duration_cast<std::chrono::seconds>(lastUpdateTimeAfterBegin - startTimeAfterBegin);
            
            isGameFinished = true;
            int finalScore = computeFinalScore(endTime);
            
            scoreSignal.emit(finalScore);
            return;
        }
    }
    
    void onCoinCollected(){
        collectedCoins++;
        coinsSignal.emit(collectedCoins);
    }
    
public:
    
    void init() override {
        startTime = std::chrono::high_resolution_clock::now();
        lastUpdateTime = startTime;
    }
    
    void update() override {
        if(!isGameStarted) handleStartTimer();  // if start-timer changes update the UI
        else if(!isGameFinished) handleTimer(); // if real timer changes update UI
    }
    
    void cleanup() override {}
    
    void onSignal(std::string id, std::any data) override {
        if (id == LAPS_SIGNAL) {
            onLapChanged(std::any_cast<int>(data));
        } else if (id == COIN_COLLECTED_SIGNAL){
            onCoinCollected();
        }
        else {
            std::cerr << "Unknown signal type: " << id << std::endl;
        }
    }
    
};

#endif
