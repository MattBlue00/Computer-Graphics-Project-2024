#ifndef CAR_HPP
#define CAR_HPP

#include <btBulletDynamicsCommon.h>

// car variables
static float SteeringAng = 0.0f;
static float dampedVel = 0.0f;

/*void updateSpeed(btRigidBody* carRigidBody, glm::vec3 carMovementInput, float deltaT) {
    btTransform transform;
    carRigidBody->getMotionState()->getWorldTransform(transform);

    if (carMovementInput.z < 0) { // Controlliamo se il tasto W è premuto
        btVector3 forwardDir = transform.getBasis() * btVector3(0, 0, 1);
        btScalar forceMagnitude = MOVE_SPEED * carRigidBody->getMass() * 2; // Utilizza MOVE_SPEED e la massa della macchina per calcolare la forza
        btVector3 forwardForce = forwardDir * forceMagnitude * deltaT; // Usa deltaT per scalare la forza in base al tempo

        carRigidBody->applyCentralForce(forwardForce);
        std::cout << "Applying forward force: " << forwardForce.getX() << ", " << forwardForce.getY() << ", " << forwardForce.getZ() << std::endl;
    }
}*/

void updateSpeed(btRigidBody* carRigidBody, glm::vec3 carMovementInput, float deltaT) {
    btTransform transform;
    carRigidBody->getMotionState()->getWorldTransform(transform);

    // Direzioni per i movimenti
    btVector3 forwardDir = transform.getBasis() * btVector3(0, 0, 1);
    btVector3 rightDir = transform.getBasis() * btVector3(1, 0, 0);

    // Forza da applicare
    btScalar forceMagnitude = MOVE_SPEED * carRigidBody->getMass() * deltaT * 2;

    // Forza totale da applicare
    btVector3 totalImpulse(0, 0, 0);

    if (carMovementInput.z < 0) { // Movimento in avanti (W)
        totalImpulse += forwardDir * forceMagnitude;
    }
    if (carMovementInput.z > 0) { // Movimento indietro (S)
        totalImpulse -= forwardDir * forceMagnitude;
    }
    if (carMovementInput.x > 0) { // Movimento a sinistra (A)
        totalImpulse -= rightDir * forceMagnitude;
    }
    if (carMovementInput.x < 0) { // Movimento a destra (D)
        totalImpulse += rightDir * forceMagnitude;
    }

    carRigidBody->applyCentralImpulse(totalImpulse);

    // Debug output
    btVector3 linVel = carRigidBody->getLinearVelocity();
    std::cout << "Applying force: " << totalImpulse.getX() << ", " << totalImpulse.getY() << ", " << totalImpulse.getZ() << std::endl;
    std::cout << "Car Linear Velocity: " << linVel.getX() << ", " << linVel.getY() << ", " << linVel.getZ() << std::endl;
}

#endif
