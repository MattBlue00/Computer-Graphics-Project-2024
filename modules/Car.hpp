#ifndef CAR_HPP
#define CAR_HPP

#include <btBulletDynamicsCommon.h>

const float maxEngineForce = 1000.0f; // Maximum force applied to wheels
const float maxBrakeForce = 200.0f; // Maximum brake force
const float steeringIncrement = 0.04f;
const float steeringClamp = 0.3f;

void updateVehicle(btRaycastVehicle* vehicle, const glm::vec3& carMovementInput, float deltaT) {

    // Controlli del veicolo
    float engineForce = 0.f;
    float brakeForce = 0.f;
    float steering = vehicle->getSteeringValue(0); // Assumi che le ruote anteriori siano a indice 0 e 1

    // Movimento avanti/indietro
    if (carMovementInput.z < 0) {
        engineForce = maxEngineForce;
        brakeForce = 0.f;
    }
    else if (carMovementInput.z > 0) {
        engineForce = 0.f;
        brakeForce = maxBrakeForce;
    }
    else {
        engineForce = 0.f;
        brakeForce = 0.f;
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
}

#endif
