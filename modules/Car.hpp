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

/*void updateSpeed(btRigidBody* carRigidBody, glm::vec3 carMovementInput, float deltaT) {
    btTransform transform;
    carRigidBody->getMotionState()->getWorldTransform(transform);

    if (carMovementInput.z < 0) { // Controlliamo se il tasto W è premuto
        btVector3 forwardDir = transform.getBasis() * btVector3(0, 0, 1);
        btScalar forceMagnitude = MOVE_SPEED * carRigidBody->getMass(); // Utilizza MOVE_SPEED e la massa della macchina per calcolare la forza
        btVector3 forwardForce = forwardDir * forceMagnitude * deltaT; // Usa deltaT per scalare la forza in base al tempo

        carRigidBody->applyCentralForce(forwardForce);
        std::cout << "Applying forward force: " << forwardForce.getX() << ", " << forwardForce.getY() << ", " << forwardForce.getZ() << std::endl;
    }
}*/
/*
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
    //std::cout << "Applying force: " << totalImpulse.getX() << ", " << totalImpulse.getY() << ", " << totalImpulse.getZ() << std::endl;
    //std::cout << "Car Linear Velocity: " << linVel.getX() << ", " << linVel.getY() << ", " << linVel.getZ() << std::endl;
}
*/
/*
// Funzione per aggiornare il movimento della macchina
void updateCarMovement(btRigidBody* carBody, float deltaTime, const glm::vec3& carMovementInput, float& Yaw) {
    // Ottenere la trasformazione attuale della macchina
    btTransform transform;
    carBody->getMotionState()->getWorldTransform(transform);

    // Direzione in avanti della macchina
    btVector3 forwardDir = transform.getBasis()[2];
    forwardDir.normalize();

    float inputZ = carMovementInput.z; // W e S
    float inputX = carMovementInput.x; // A e D

    // Movimento in avanti/indietro
    if (inputZ != 0) {
        dampedVel = MOVE_SPEED * deltaTime * inputZ * (1 - exp(-8.0f * deltaTime)) + dampedVel * exp(-8.0f * deltaTime);
        dampedVel = ((fabs(dampedVel) < 0.001f) ? 0.0f : dampedVel);
    }
    else {
        dampedVel *= FRICTION;
    }

    // Applicare la forza di movimento
    btVector3 force = forwardDir * dampedVel * ACCELERATION;

    // Limitare la forza applicata alla forza massima
    if (force.length() > MAX_FORCE) {
        force = force.normalized() * MAX_FORCE;
    }

    carBody->applyCentralForce(force);

    // Debug della forza applicata
    std::cout << "Applied force: " << force.getX() << ", " << force.getY() << ", " << force.getZ() << std::endl;

    // Rotazione della macchina
    if (inputX != 0) {
        SteeringAng += -inputX * STEERING_SPEED * deltaTime;
        SteeringAng = (SteeringAng < -MAX_STEERING_ANGLE ? -MAX_STEERING_ANGLE : (SteeringAng > MAX_STEERING_ANGLE ? MAX_STEERING_ANGLE : SteeringAng));
    }
    else {
        if (SteeringAng > STEERING_SPEED * deltaTime) {
            SteeringAng -= STEERING_SPEED * deltaTime;
        }
        else if (SteeringAng < -STEERING_SPEED * deltaTime) {
            SteeringAng += STEERING_SPEED * deltaTime;
        }
        else {
            SteeringAng = 0.0f;
        }
    }

    if (SteeringAng != 0) {
        float l = 2.78f; // Distanza tra gli assi delle ruote
        float r = l / tan(SteeringAng);
        float Dbeta = dampedVel / r * deltaTime;
        Yaw += Dbeta;
    }
    // Aggiornare la direzione della macchina in base alla Yaw
    btQuaternion rotation(btVector3(0, 1, 0), Yaw);
    transform.setRotation(rotation);
    carBody->getMotionState()->setWorldTransform(transform);
    carBody->setCenterOfMassTransform(transform);

    // Limitare la velocità massima
    btVector3 velocity = carBody->getLinearVelocity();
    if (velocity.length() > MAX_SPEED) {
        velocity = velocity.normalized() * MAX_SPEED;
        carBody->setLinearVelocity(velocity);
    }

    // Rimuovere eventuali rotazioni non desiderate
    btVector3 angularVelocity = carBody->getAngularVelocity();
    angularVelocity.setY(0);
    carBody->setAngularVelocity(angularVelocity);

    // Debug della velocità aggiornata
    btVector3 updatedVelocity = carBody->getLinearVelocity();
    std::cout << "Updated velocity: " << updatedVelocity.getX() << ", " << updatedVelocity.getY() << ", " << updatedVelocity.getZ() << std::endl;
}
*/
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

    // Applicare la forza per avanzare o indietreggiare
    if (carMovementInput.z != 0.0f) {
        btVector3 force = forwardDir * (-carMovementInput.z * forwardForce);
        carBody->applyCentralForce(force);
        //std::cout << "Applied force: " << force.getX() << ", " << force.getY() << ", " << force.getZ() << std::endl;
    }
    else {
        // Applicare una forza frenante se non si preme W o S
        if (velocity.length2() > 0.0) {
            btVector3 brakeForce = -velocity.normalized() * brakingForce;
            carBody->applyCentralForce(brakeForce);
            //std::cout << "Applied brakeforce: " << brakeForce.getX() << ", " << brakeForce.getY() << ", " << brakeForce.getZ() << std::endl;
        }
    }

    // Variabile per coppia di torsione
    btVector3 torque(0.0, 0.0, 0.0);

    btVector3 angularVelocity = carBody->getAngularVelocity();

    // Applicare il momento torcente per girare
    std::cout << carMovementInput.x << "\n";
    if (carMovementInput.x != 0.0f) {
        // Calcola il torque da applicare
        btVector3 torque(0.0, -carMovementInput.x * turningTorque, 0.0); // Aumentato il valore del torque per test

        // Applica il torque
        carBody->applyTorque(torque);

        // Stampa il torque applicato e la velocità angolare corrente
        //std::cout << "Applied Torque: " << torque.getX() << ", " << torque.getY() << ", " << torque.getZ() << std::endl;
        //std::cout << "Current Angular Velocity: " << angularVelocity.getX() << ", " << angularVelocity.getY() << ", " << angularVelocity.getZ() << std::endl;
    }
}
#endif
