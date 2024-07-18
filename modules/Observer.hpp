#ifndef OBSERVER_HPP
#define OBSERVER_HPP

class Observer {
public:
    // add all onXXhanged methods here
    virtual void onSpeedChanged(int newSpeed) = 0;
    virtual void onCoinCollected(int collectedCoins) = 0;
    virtual void onCheckLaps(int lapsDone) = 0;
    virtual ~Observer() = default;
};

#endif
