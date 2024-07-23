#ifndef OBSERVER_HPP
#define OBSERVER_HPP

class Observer {
public:
    // add all onXXhanged methods here
    virtual void onSpeedChanged(int newSpeed){};
    virtual void onCoinCollected(int collectedCoins){};
    virtual void onCheckLaps(int lapsDone){};
    virtual void onStartSemaphore(int countDownValue){};
    virtual void onBrakeActive(bool isBrakeActive){};
    virtual void onHeadlightsChange(){};
    
    virtual void onQuit(){};
    virtual void onChangeScene(){};
    virtual void onUpdateDebounce(){};
    
    virtual void onChangeView(){};
    virtual void onChangeHeadlightsStatus(){};
    
    virtual void onChangeCircuit(){};

    virtual ~Observer() = default;
};

#endif
