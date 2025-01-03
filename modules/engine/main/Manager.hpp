#ifndef MANAGER_HPP
#define MANAGER_HPP

class Manager {
public:
    
    virtual void init() = 0;
    virtual void update() = 0;
    virtual void cleanup() = 0;

    virtual ~Manager() = default;
};

#endif
