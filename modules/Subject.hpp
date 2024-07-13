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
};

#endif

