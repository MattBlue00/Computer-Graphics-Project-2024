#ifndef RECEIVER_HPP
#define RECEIVER_HPP

#include <any>

class Receiver {
public:
    virtual ~Receiver() = default;

protected:
    virtual void onSignal(std::string id, std::any data) = 0;
};

#endif
