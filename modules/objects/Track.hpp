#ifndef TRACK_HPP
#define TRACK_HPP

#include "../modules/engine/pattern/Receiver.hpp"
#include "../modules/objects/DirectionBarrier.hpp"
#include "../modules/other/Checkpoint.hpp"
#include "../modules/data/WorldData.hpp"
#include "../modules/data/Signals.hpp"

std::vector<Checkpoint*> checkpointsLap; // variabile che racchiude solo i checpoint relativi al giro che si sta percorrendo
btTransform lastCheckpointTransform; // variabile per memorizzare l'ultimo checkpoint visitato e gestire i casi in cui la macchina esce dal circuito

class Track: public GameObject, public StaticRigidBody, public Receiver {
    
protected:

    std::vector<Checkpoint*> checkpoints;
    
    void setCheckpointsBasedOnLap(int lapNumber){
        checkpointsLap.clear();
        std::cout << "Setting checkpoints based on lap: " << lapNumber << std::endl;
        switch(lapNumber){
            case 1:
                checkpointsLap.push_back(checkpoints[0]); // checkpoint 1
                checkpointsLap.push_back(checkpoints[1]); // checkpoint 2
                checkpointsLap.push_back(checkpoints[2]); // checkpoint 3
                checkpointsLap.push_back(checkpoints[3]); // checkpoint 4
                checkpointsLap.push_back(checkpoints[5]); // checkpoint final
                break;
            case 2:
                checkpointsLap.push_back(checkpoints[4]); // checkpoint 5
                checkpointsLap.push_back(checkpoints[2]); // checkpoint 3
                checkpointsLap.push_back(checkpoints[3]); // checkpoint 4
                checkpointsLap.push_back(checkpoints[5]); // checkpoint final
                break;
            default:
                for(Checkpoint* checkpoint : checkpoints){
                    checkpoint->disable();
                }
                return;
        }
        nextCheckpointId = checkpointsLap[0]->getId();
    }
    
    void setBarrierStatus(std::string barrier_id, bool status){
        for (GameObject* obj : gameObjects) {
            if (obj->getId().starts_with(barrier_id)) {
                // Verifica se l'oggetto è un'istanza di DirectionBarrier
                DirectionBarrier* barrier = dynamic_cast<DirectionBarrier*>(obj);
                if (barrier) {
                    if(status){
                        obj->enable();
                        addRigidBodyQueue.push_back(barrier->getRigidBody());
                    }
                    else{
                        obj->disable();
                        removeRigidBodyQueue.push_back(barrier->getRigidBody());
                    }
                }
                else{
                    std::cerr << "An object whose id starts with " << barrier_id << " was not properly casted into DirectionBarrier type.\n";
                }
            }
        }
    }
    
    void onUpdateNextCheckpoint(){
        int hitCount = 0;
        for (Checkpoint* checkpointLap : checkpointsLap){
            if (!checkpointLap->wasHit()){
                nextCheckpointId = checkpointLap->getId();
                lastCheckpointTransform.setOrigin(checkpointLap->getPosition());
                lastCheckpointTransform.setRotation(vehicle->getChassisWorldTransform().getRotation());
                break;
            }
            else {
                hitCount++;
            }
        }
        if(hitCount == checkpointsLap.size()){
            lapsSignal.emit({});
        }
        std::cout << "Next checkpoint is: " << nextCheckpointId << std::endl;
    }
    
    void onLapChanged(){
        currentLap = (currentLap + 1) % 3;
        for (Checkpoint* checkpoint : checkpointsLap){
            checkpoint->reset();
        }
        if (currentLap == 2) {
            setBarrierStatus("dir_barrier_inner", true);
            setBarrierStatus("dir_barrier_oval", false);
            
        }
        else {
            setBarrierStatus("dir_barrier_inner", false);
        }
        setCheckpointsBasedOnLap(currentLap);
    }

public:
    
    Track(std::string id, Model* m, Texture* t, glm::mat4 wm, DescriptorSet* ds, PipelineType pt, std::unordered_map<std::string, float> props) :
    GameObject(id, m, t, wm, ds, pt, props),
    StaticRigidBody(m, wm, 0.0f, 0.0f) {
        currentLap = 1;
        this->checkpoints = {
            new Checkpoint("checkpoint_1", btVector3(-0.25 + 2.5, -0.95 + 5.0, 0.0), btVector3(10, 5, 1)),
            new Checkpoint("checkpoint_2", btVector3(-340.0, -40.0, 785.0 - 7.5), btVector3(1, 5, 10)),
            new Checkpoint("checkpoint_3", btVector3(-693.5 - 7.5, -1 + 5.0, -100.0), btVector3(10, 5, 1)),
            new Checkpoint("checkpoint_4", btVector3(-350.0, -1.0 + 5.0, -315.5 - 7.5), btVector3(1, 5, 10)),
            new Checkpoint("checkpoint_5", btVector3(-221.0 - 7.5, 88.0 + 5.0, 585.0), btVector3(7.5, 5, 1)),
            new Checkpoint("checkpoint_final", btVector3(-0.25 + 2.5, -0.95 + 5.0, 0.0), btVector3(10, 5, 1)),
        };
        btVector3 initialPosition(0, -0.5, -10); // Coordinate iniziali FIXME: prendere i dati dalla macchina
        lastCheckpointTransform.setIdentity(); // Inizializza la trasformazione a identità
        lastCheckpointTransform.setOrigin(initialPosition); // Imposta la posizione iniziale
        setCheckpointsBasedOnLap(currentLap);
    }
    
    void init() override {
        setBarrierStatus("dir_barrier_inner", false);
        std::vector<Signal*> trackSignals = { &updateNextCheckpointSignal, &lapsSignal };
        for (Signal* signal : trackSignals) {
            signal->addListener([this, signal](std::string id, std::any data) {
                this->onSignal(signal->getId(), {});
            });
        }
    }
    
    void handleData(std::string id, std::any data) override {
        if (id == UPDATE_NEXT_CHECKPOINT_SIGNAL) {
            onUpdateNextCheckpoint();
        } else if (id == LAPS_SIGNAL) {
            onLapChanged();
        }
        else {
            std::cerr << "Unknown signal type: " << id << std::endl;
        }
    }
    
};

#endif
