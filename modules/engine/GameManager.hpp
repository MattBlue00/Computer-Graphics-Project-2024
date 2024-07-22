#ifndef GAME_MANAGER_HPP
#define GAME_MANAGER_HPP

class GameManager {
    public:
    virtual void init();
    virtual void update();
    virtual void cleanup();
    
    virtual ~GameManager() = default;
};

#endif
