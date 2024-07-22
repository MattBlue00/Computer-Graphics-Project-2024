#ifndef GAME_MANAGER_HPP
#define GAME_MANAGER_HPP

#include "Observer.hpp"

class Manager {
public:
    
    virtual void init(std::vector<void*> params){};
    virtual void update(std::vector<void*> params){};
    virtual void cleanup(){};

    virtual ~Manager() = default;
};

#endif
