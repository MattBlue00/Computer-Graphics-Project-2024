#ifndef CAR_HPP
#define CAR_HPP

#include <btBulletDynamicsCommon.h>

float forwardForce = 25000.0f;
float brakingForce = 5000.0f;
float turningTorque = 6000.0f;
float maxSpeed = 100.0f;
float currentAngularDirection = 0.0f;

float dampedVel = 0.0f;
float SteeringAng = 0.0f;

// Funzione per aggiornare il movimento della macchina
void updateCarMovement(btRigidBody* carBody, const glm::vec3& carMovementInput) {

    // Limita la velocità massima
    btVector3 velocity = carBody->getLinearVelocity();
    if (velocity.length2() > maxSpeed * maxSpeed) {
        velocity = velocity.normalized() * maxSpeed;
        carBody->setLinearVelocity(velocity);
    }

    // Ottieni la direzione frontale della macchina
    btTransform transform;
    carBody->getMotionState()->getWorldTransform(transform);
    btVector3 forwardDir = transform.getBasis().getColumn(2);
    forwardDir.setY(0); // Ignora la componente Y per mantenere il movimento sul piano XZ
    forwardDir.normalize();

    // Stampa la direzione frontale della macchina
    /*
    std::cout << "Forward Direction: ("
        << forwardDir.getX() << ", "
        << forwardDir.getY() << ", "
        << forwardDir.getZ() << ")"
        << std::endl;
    */
    // Applicare la forza per avanzare o indietreggiare
    if (carMovementInput.z != 0.0f) {
        btVector3 force = forwardDir * (-carMovementInput.z * forwardForce);
        carBody->applyCentralForce(force);
    }
    else {
        // Applicare una forza frenante se non si preme W o S
        if (velocity.length2() > 0.0) {
            btVector3 brakeForce = -velocity.normalized() * brakingForce;
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

#endif
