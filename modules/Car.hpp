#ifndef CAR_HPP
#define CAR_HPP

#include <btBulletDynamicsCommon.h>

float forwardForce = 25000.0f;
float brakingForce = 4000.0f;
float turningTorque = 5000.0f;
float maxSpeed = 30.0f; //metri al secondo (25 -> 90 km/h)

// Variabile statica per mantenere la velocit� desiderata tra i frame
static float desiredSpeed = 0.0f;
const float acceleration = 3.5f; // Incrementa la velocit� desiderata ogni secondo
const float deceleration = 3.5f; // Riduci la velocit� desiderata ogni secondo

float dampedVel = 0.0f;
float SteeringAng = 0.0f;

// Funzione per rilevare se la macchina � in contatto con il terreno
bool isCarOnGround(btDiscreteDynamicsWorld* dynamicsWorld, btRigidBody* carBody) {
    const btScalar threshold = 1.0f; // Distanza sotto la quale consideriamo la macchina sul terreno
    btVector3 carPosition = carBody->getCenterOfMassPosition();
    btVector3 rayStart = carPosition;
    btVector3 rayEnd = carPosition - btVector3(0, threshold, 0); // Raggio verso il basso

    btCollisionWorld::ClosestRayResultCallback rayCallback(rayStart, rayEnd);
    dynamicsWorld->rayTest(rayStart, rayEnd, rayCallback);

    return rayCallback.hasHit();
}

// Funzione per aggiornare il movimento della macchina
void updateCarMovement(btRigidBody* carBody, const glm::vec3& carMovementInput, float deltaT, btDiscreteDynamicsWorld* dynamicsWorld) {
    // Controlla se la macchina � in contatto con il terreno
    bool onGround = isCarOnGround(dynamicsWorld, carBody);
    std::cout << onGround << std::endl;
    glm::vec3 adjustedMovementInput = carMovementInput;
    if (!onGround) {
        adjustedMovementInput = glm::vec3(0.0f); // Disabilita i controlli
    }
    // Ottieni la direzione frontale della macchina
    btTransform transform;
    carBody->getMotionState()->getWorldTransform(transform);
    btVector3 forwardDir = transform.getBasis().getColumn(2);
    forwardDir.setY(0); // Ignora la componente Y per mantenere il movimento sul piano XZ
    forwardDir.normalize();

    // Gestione dell'accelerazione e della decelerazione
    if (adjustedMovementInput.z != 0.0f) {
        // Incrementa o decrementa la velocit� desiderata in base all'input
        desiredSpeed += -adjustedMovementInput.z * acceleration * deltaT;
        // Limita la velocit� desiderata alla velocit� massima
        desiredSpeed = std::clamp(desiredSpeed, -maxSpeed, maxSpeed);
    }
    else {
        // Riduci gradualmente la velocit� desiderata se non si preme W o S
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

    // Limita la velocit� massima
    btVector3 velocity = carBody->getLinearVelocity();
    if (velocity.length2() > maxSpeed * maxSpeed) {
        velocity = velocity.normalized() * maxSpeed;
        carBody->setLinearVelocity(velocity);
    }

    // Debug: Stampa la velocit� attuale e desiderata
    std::cout << "Current Speed: " << velocity.length() << ", Desired Speed: " << desiredSpeed << std::endl;

    // Applicare la forza per avanzare o indietreggiare
    if (desiredSpeed != 0.0f) {
        btVector3 force = forwardDir * desiredSpeed * forwardForce * deltaT;
        carBody->applyCentralForce(force);
    }
    else {
        // Applicare una forza frenante se non c'� velocit� desiderata
        if (velocity.length2() > 0.0) {
            btVector3 brakeForce = -velocity.normalized() * brakingForce * deltaT;
            carBody->applyCentralForce(brakeForce);
        }
    }

    // Applicare il momento torcente per girare
    if (adjustedMovementInput.x != 0.0f) {
        // Calcola il torque da applicare
        btVector3 torque(0.0, -adjustedMovementInput.x * turningTorque, 0.0);

        // Applica il torque
        carBody->applyTorque(torque);
    }
    else {
        // Se non c'� movimento su x, azzera la velocit� angolare
        btVector3 angularVelocity = carBody->getAngularVelocity();
        angularVelocity.setY(0); // Mantieni solo la componente Y della velocit� angolare
        carBody->setAngularVelocity(angularVelocity);
    }
}

#endif
