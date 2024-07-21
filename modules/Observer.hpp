#ifndef OBSERVER_HPP
#define OBSERVER_HPP

class Observer {
public:
    // add all onXXhanged methods here
    virtual void onSpeedChanged(int newSpeed){};
    virtual void onCoinCollected(int collectedCoins){};
    virtual void onCheckLaps(int lapsDone){};
    virtual void onStartSemaphore(int countDownValue){};

    virtual ~Observer() = default;
};

#endif
