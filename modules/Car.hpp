#ifndef CAR_HPP
#define CAR_HPP

#include <btBulletDynamicsCommon.h>

const float maxEngineForce = 2000.0f; // Maximum force applied to wheels
const float maxBrakeForce = 200.0f; // Maximum brake force
const float steeringIncrement = 0.04f;
const float steeringClamp = 0.3f;

void printVehicleStatus(btRaycastVehicle* vehicle);
bool isVehicleStopped(btRaycastVehicle* vehicle, float threshold);

void updateVehicle(btRaycastVehicle* vehicle, const glm::vec3& carMovementInput, float deltaT) {
    
    // Controlli del veicolo
    float engineForce = 0.0f;
    float brakeForce = 0.0f;
    float steering = vehicle->getSteeringValue(0); // Assumi che le ruote anteriori siano a indice 0 e 1

    // Movimento avanti/indietro
    if (carMovementInput.z < 0) { // W premuto
        engineForce = maxEngineForce;
        brakeForce = 0.0f;
    }
    else if (carMovementInput.z > 0) { // S premuto
        engineForce = -maxEngineForce; // Forza negativa per andare in retro
        brakeForce = 0.0f;
    }
    else { // Nessun input
        engineForce = 0.0f;
        if (isVehicleStopped(vehicle, 0.5f)){
            brakeForce = maxBrakeForce;
            //std::cout << "Eccomi" << std::endl;
            vehicle->getRigidBody()->setLinearVelocity(btVector3(0, 0, 0));
            vehicle->getRigidBody()->setAngularVelocity(btVector3(0, 0, 0));
        }
        else
        {
            brakeForce = 0.0f;
        }
        
    }

    // Sterzata destra/sinistra
    if (carMovementInput.x > 0) {
        steering -= steeringIncrement;
        if (steering < -steeringClamp) {
            steering = -steeringClamp;
        }
    }
    else if (carMovementInput.x < 0) {
        steering += steeringIncrement;
        if (steering > steeringClamp) {
            steering = steeringClamp;
        }
    }
    else {
        // Se non ci sono input, ritorna gradualmente la sterzata a zero
        if (steering > 0) {
            steering -= steeringIncrement;
            if (steering < 0) steering = 0;
        }
        else if (steering < 0) {
            steering += steeringIncrement;
            if (steering > 0) steering = 0;
        }
    }
    //std::cout << steering << std::endl;
    std::cout << vehicle->getRigidBody()->getLinearVelocity().getX() << std::endl;
    // Applicazione dei controlli
    vehicle->applyEngineForce(engineForce, 2); // Ruote posteriori
    vehicle->applyEngineForce(engineForce, 3);
    vehicle->setBrake(brakeForce, 2);
    vehicle->setBrake(brakeForce, 3);
    vehicle->setSteeringValue(steering, 0); // Ruote anteriori
    vehicle->setSteeringValue(steering, 1);

    // Log per il debugging
    //std::cout << "carMovementInput: (" << carMovementInput.x << ", " << carMovementInput.y << ", " << carMovementInput.z << ")" << std::endl;
    //std::cout << "Engine Force: " << engineForce << ", Brake Force: " << brakeForce << ", Steering: " << steering << std::endl;
    //printVehicleStatus(vehicle);
}

bool isVehicleStopped(btRaycastVehicle* vehicle, float threshold) {
    btVector3 linearVelocity = vehicle->getRigidBody()->getLinearVelocity();
    return linearVelocity.length() < threshold;
}

void printVehicleStatus(btRaycastVehicle* vehicle) {
    // Stampa la posizione del telaio
    btTransform chassisTransform = vehicle->getChassisWorldTransform();
    btVector3 chassisPosition = chassisTransform.getOrigin();
    std::cout << "Chassis Position: ("
        << chassisPosition.getX() << ", "
        << chassisPosition.getY() << ", "
        << chassisPosition.getZ() << ")" << std::endl;

    // Stampa la posizione di una ruota (ad esempio, la ruota anteriore sinistra)
    int wheelIndex = 0; // Indice della ruota da stampare
    btWheelInfo& wheel = vehicle->getWheelInfo(wheelIndex);
    btTransform wheelTransform = wheel.m_worldTransform;
    btVector3 wheelPosition = wheelTransform.getOrigin();
    std::cout << "Wheel " << wheelIndex << " Position: ("
        << wheelPosition.getX() << ", "
        << wheelPosition.getY() << ", "
        << wheelPosition.getZ() << ")" << std::endl;
}

#endif
