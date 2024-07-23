#ifndef SUBJECT_HPP
#define SUBJECT_HPP

#include "Observer.hpp"
#include <vector>
#include <algorithm>

struct Subject {
    std::vector<Observer*> observers;

    void addObserver(Observer* observer) {
        observers.push_back(observer);
    }

    void removeObserver(Observer* observer) {
        observers.erase(std::remove(observers.begin(), observers.end(), observer), observers.end());
    }

    // Add all notifyXChanged here
    void notifySpeedChanged(int newSpeed) {
        for (Observer* observer : observers) {
            observer->onSpeedChanged(newSpeed);
        }
    }
    
    void notifyCoinCollected(int collectedCoins){
        for (Observer* observer : observers) {
            observer->onCoinCollected(collectedCoins);
        }
    }
    
    void notifyCheckLaps(int lapsDone){
        for (Observer* observer : observers) {
            observer->onCheckLaps(lapsDone);
        }
    }
    
    void notifyStartSemaphore(int countDownValue){
        for (Observer* observer : observers) {
            observer->onStartSemaphore(countDownValue);
        }
    }
    
    void notifyBrake(bool isBrakeActive){
        for (Observer* observer : observers) {
            observer->onBrakeActive(isBrakeActive);
        }
    }
    
    /*--------------------------------*/
    
    void notifyQuit(){
        for (Observer* observer : observers) {
            observer->onQuit();
        }
    }
    
    void notifyChangeScene(){
        for (Observer* observer : observers) {
            observer->onChangeScene();
        }
    }
    
    void notifyUpdateDebounce(){
        for (Observer* observer : observers) {
            observer->onUpdateDebounce();
        }
    }
    
    void notifyChangeView(){
        for (Observer* observer : observers) {
            observer->onChangeView();
        }
    }
    
    void notifyChangeHeadlightsStatus(){
        for (Observer* observer : observers) {
            observer->onChangeHeadlightsStatus();
        }
    }
    
    void notifyChangeCircuit(){
        for (Observer* observer : observers) {
            observer->onChangeCircuit();
        }
    }
    
};

#endif

