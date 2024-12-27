#ifndef RECEIVER_HPP
#define RECEIVER_HPP

#include <any>

class Receiver {
public:
    virtual ~Receiver() = default;

    void onSignal(std::string id, std::any data) {
        try {
            handleData(id, data);
        } catch (const std::bad_any_cast& e) {
            std::cout << "Could not cast data for signal ID " << id << ": " << e.what() << "\n";
        }
    }

protected:
    virtual void handleData(std::string id, std::any data) = 0;
};

#endif
