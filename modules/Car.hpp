#ifndef CAR_HPP
#define CAR_HPP

#include <btBulletDynamicsCommon.h>

float forwardForce = 20000.0f;
float brakingForce = 4000.0f;
float turningTorque = 5000.0f;
float maxSpeed = 25.0f; //metri al secondo (25 -> 90 km/h)

// Variabile statica per mantenere la velocità desiderata tra i frame
static float desiredSpeed = 0.0f;
const float acceleration = 3.5f; // Incrementa la velocità desiderata ogni secondo
const float deceleration = 3.5f; // Riduci la velocità desiderata ogni secondo

const float gravityMultiplier = 10.0f; // Forza extra verso il basso
const float raycastDistance = 5.0f; // Distanza del raycast verso il basso

float dampedVel = 0.0f;
float SteeringAng = 0.0f;

const float maxEngineForce = 20000.0f; // Maximum force applied to wheels
const float maxBrakeForce = 100.0f; // Maximum brake force
const float steeringIncrement = 0.04f;
const float steeringClamp = 0.3f;

// Funzione per rilevare se la macchina è in contatto con il terreno
bool isCarOnGround(btRigidBody* carBody, btDiscreteDynamicsWorld* dynamicsWorld, float& distance) {
    btTransform transform;
    carBody->getMotionState()->getWorldTransform(transform);
    btVector3 start = transform.getOrigin();
    btVector3 end = start - btVector3(0, raycastDistance, 0);

    btCollisionWorld::ClosestRayResultCallback rayCallback(start, end);
    dynamicsWorld->rayTest(start, end, rayCallback);

    if (rayCallback.hasHit()) {
        distance = start.getY() - rayCallback.m_hitPointWorld.getY();
        std::cout << "Distance to ground: " << distance << std::endl;
        return true;
    }

    distance = raycastDistance; // Se non c'è collisione, impostiamo la distanza alla distanza massima del raycast
    return false;
}

// Funzione per aggiornare il movimento della macchina
void updateCarMovement(btRigidBody* carBody, const glm::vec3& carMovementInput, float deltaT, btDiscreteDynamicsWorld* dynamicsWorld) {

    float distanceToGround;
    bool onGround = isCarOnGround(carBody, dynamicsWorld, distanceToGround);

    // Forza extra proporzionale alla distanza dal terreno
    float additionalDownForceMagnitude = gravityMultiplier * distanceToGround * carBody->getMass();
    if (distanceToGround > 1.0f) {
        btVector3 additionalDownForce(0, -additionalDownForceMagnitude, 0);
        carBody->applyCentralForce(additionalDownForce);
    }
    
    // Ottieni la direzione frontale della macchina
    btTransform transform;
    carBody->getMotionState()->getWorldTransform(transform);
    btVector3 forwardDir = transform.getBasis().getColumn(2);
    forwardDir.setY(0); // Ignora la componente Y per mantenere il movimento sul piano XZ
    forwardDir.normalize();

    // Gestione dell'accelerazione e della decelerazione
    if (carMovementInput.z != 0.0f) {
        // Incrementa o decrementa la velocità desiderata in base all'input
        desiredSpeed += -carMovementInput.z * acceleration * deltaT;
        // Limita la velocità desiderata alla velocità massima
        desiredSpeed = std::clamp(desiredSpeed, -maxSpeed, maxSpeed);
    }
    else {
        // Riduci gradualmente la velocità desiderata se non si preme W o S
        if (desiredSpeed > 0.0f) {
            desiredSpeed -= deceleration * deltaT;
            if (desiredSpeed < 0.0f) {
                desiredSpeed = 0.0f;
            }
        }
        else if (desiredSpeed < 0.0f) {
            desiredSpeed += deceleration * deltaT;
            if (desiredSpeed > 0.0f) {
                desiredSpeed = 0.0f;
            }
        }
    }

    // Limita la velocità massima
    btVector3 velocity = carBody->getLinearVelocity();
    if (velocity.length2() > maxSpeed * maxSpeed) {
        velocity = velocity.normalized() * maxSpeed;
        carBody->setLinearVelocity(velocity);
    }

    // Debug: Stampa la velocità attuale e desiderata
    //std::cout << "Current Speed: " << velocity.length() << ", Desired Speed: " << desiredSpeed << std::endl;

    // Applicare la forza per avanzare o indietreggiare
    if (desiredSpeed != 0.0f) {
        btVector3 force = forwardDir * desiredSpeed * forwardForce * deltaT;
        carBody->applyCentralForce(force);
        /*
        std::cout << "Total Force: ("
            << force.getX() << ", "
            << force.getY() << ", "
            << force.getZ() << ")" << std::endl;*/
    }
    else {
        // Applicare una forza frenante se non c'è velocità desiderata
        if (velocity.length2() > 0.0) {
            btVector3 brakeForce = -velocity.normalized() * brakingForce * deltaT;
            carBody->applyCentralForce(brakeForce);
        }
    }

    // Applicare il momento torcente per girare
    if (carMovementInput.x != 0.0f) {
        // Calcola il torque da applicare
        btVector3 torque(0.0, -carMovementInput.x * turningTorque, 0.0);

        // Applica il torque
        carBody->applyTorque(torque);
    }
    else {
        // Se non c'è movimento su x, azzera la velocità angolare
        btVector3 angularVelocity = carBody->getAngularVelocity();
        angularVelocity.setY(0); // Mantieni solo la componente Y della velocità angolare
        carBody->setAngularVelocity(angularVelocity);
    }
}

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
    std::cout << "Engine Force: " << engineForce << ", Brake Force: " << brakeForce << ", Steering: " << steering << std::endl;
}

#endif
