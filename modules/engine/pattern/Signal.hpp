#ifndef SIGNAL_HPP
#define SIGNAL_HPP

#include <vector>
#include <functional>
#include <any>

class Signal {
public:
    Signal(std::string signalId) : id(signalId) {}
    
    using Callback = std::function<void(std::string, std::any)>;

    void addListener(Callback callback) {
        listeners.push_back(callback);
    }

    void emit(std::any data) {
        for (auto& listener : listeners) {
            listener(id, data);
        }
    }
    
    std::string getId() const { return id; }

private:
    std::string id;
    std::vector<Callback> listeners;
};

#endif
