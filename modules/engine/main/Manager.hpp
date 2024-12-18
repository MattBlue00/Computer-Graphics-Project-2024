#ifndef MANAGER_HPP
#define MANAGER_HPP

#include "utils/ManagerInitData.hpp"
#include "utils/ManagerUpdateData.hpp"

class Manager {
public:
    
    virtual void init(ManagerInitData* param) = 0;
    virtual void update(ManagerUpdateData* param) = 0;
    virtual void cleanup() = 0;

    virtual ~Manager() = default;
};

#endif
