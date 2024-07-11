#ifndef CAR_HPP
#define CAR_HPP

#include <btBulletDynamicsCommon.h>
#include "Physics.hpp"

const float maxEngineForce = 4000.0f; // Maximum force applied to wheels
const float maxBrakeForce = 200.0f; // Maximum brake force
const float steeringIncrement = 0.04f;
const float steeringClamp = 0.3f;
const float maxSpeed = 27.0f;

const float raycastDistance = 2.0f;

btRaycastVehicle* vehicle;

void printVehicleStatus(btRaycastVehicle* vehicle);
bool isVehicleStopped(btRaycastVehicle* vehicle, float threshold);
bool isVehicleBlocked(btRaycastVehicle* vehicle);
bool isVehicleInAir(btDiscreteDynamicsWorld* dynamicsWorld, btRaycastVehicle* vehicle);
void limitVehicleRotationInAir(btRaycastVehicle* vehicle);
void setSuspensions(btRaycastVehicle* vehicle);

void initCar(){
    // Car initialization with btBoxShape
    btBoxShape* carBoxShape = new btBoxShape(btVector3(1.8, 0.5, 1.8));    // semi-working: z=1.81
    btDefaultMotionState* carMotionState = new btDefaultMotionState(btTransform(btQuaternion(0, 0, 0, 1), btVector3(0, 0, -10)));
    btScalar mass = 800.0f;
    btVector3 carInertia(0, 0, 0);
    carBoxShape->calculateLocalInertia(mass, carInertia);
    btRigidBody::btRigidBodyConstructionInfo carRigidBodyCI(mass, carMotionState, carBoxShape, carInertia);
    btRigidBody* carRigidBody = new btRigidBody(carRigidBodyCI);
    carRigidBody->setActivationState(DISABLE_DEACTIVATION); // Ensure car stays active
    carRigidBody->activate(true);
    //carRigidBody->setFriction(1.0f);
    carRigidBody->setDamping(0.2f, 0.2f); // Aumentato il damping lineare e angolare

    dynamicsWorld->addRigidBody(carRigidBody);
    
    // Raycast vehicle setup
    btRaycastVehicle::btVehicleTuning tuning;
    btVehicleRaycaster* vehicleRaycaster = new btDefaultVehicleRaycaster(dynamicsWorld);
    vehicle = new btRaycastVehicle(tuning, carRigidBody, vehicleRaycaster);
    dynamicsWorld->addAction(vehicle); // add action o vehicle

    // Set coordinate system (X right, Y up, Z forward)
    vehicle->setCoordinateSystem(0, 1, 2);

    // Verifica la direzione delle ruote e delle sospensioni
    btVector3 wheelDirectionCS0(0, -1, 0); // Direzione della sospensione verso il basso lungo l'asse Z negativo
    btVector3 wheelAxleCS(-1, 0, 0); // Assale della ruota lungo l'asse X negativo

    // Aggiungi le ruote
    btVector3 connectionPointCS0;
    bool isFrontWheel;

    // Lunghezza a riposo della sospensione
    btScalar suspensionRestLength = 0.7;

    // Raggio della ruota
    btScalar wheelRadius = 0.5;

    // Anteriore sinistra
    connectionPointCS0 = btVector3(-1, 0.5, 2);
    isFrontWheel = true;
    vehicle->addWheel(connectionPointCS0, wheelDirectionCS0, wheelAxleCS, suspensionRestLength, wheelRadius, tuning, isFrontWheel);

    // Anteriore destra
    connectionPointCS0 = btVector3(1, 0.5, 2);
    isFrontWheel = true;
    vehicle->addWheel(connectionPointCS0, wheelDirectionCS0, wheelAxleCS, suspensionRestLength, wheelRadius, tuning, isFrontWheel);

    // Posteriore sinistra
    connectionPointCS0 = btVector3(-1, 0.5, -2);
    isFrontWheel = false;
    vehicle->addWheel(connectionPointCS0, wheelDirectionCS0, wheelAxleCS, suspensionRestLength, wheelRadius, tuning, isFrontWheel);

    // Posteriore destra
    connectionPointCS0 = btVector3(1, 0.5, -2);
    isFrontWheel = false;
    vehicle->addWheel(connectionPointCS0, wheelDirectionCS0, wheelAxleCS, suspensionRestLength, wheelRadius, tuning, isFrontWheel);

    // Set wheel parameters
    setSuspensions(vehicle);
}

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
    
    // Applicazione dei controlli
    if(!isVehicleInAir(dynamicsWorld, vehicle)){
        if(vehicle->getRigidBody()->getLinearVelocity().length() <= maxSpeed){
            if(engineForce != 0.0f && isVehicleBlocked(vehicle)){
                std::cout << "Vehicle may be blocked!\n";
                vehicle->applyEngineForce(3*engineForce, 0);
                vehicle->applyEngineForce(3*engineForce, 1);
                vehicle->applyEngineForce(3*engineForce, 2);
                vehicle->applyEngineForce(3*engineForce, 3);
            }
            else{
                vehicle->applyEngineForce(0.0f, 0);
                vehicle->applyEngineForce(0.0f, 1);
                vehicle->applyEngineForce(engineForce, 2); // Ruote posteriori
                vehicle->applyEngineForce(engineForce, 3);
            }
        }
        else{
            std::cout << "Max Speed Reached!" << std::endl;
            vehicle->applyEngineForce(0.0f, 2); // Ruote posteriori
            vehicle->applyEngineForce(0.0f, 3);
        }
        vehicle->setBrake(brakeForce, 2);
        vehicle->setBrake(brakeForce, 3);
        vehicle->setSteeringValue(steering, 0); // Ruote anteriori
        vehicle->setSteeringValue(steering, 1);
    }
    
    if(isVehicleInAir(dynamicsWorld, vehicle)){
        limitVehicleRotationInAir(vehicle);
    }
    
    if(engineForce == 0.0f && isVehicleStopped(vehicle, 0.5f)){
        vehicle->getRigidBody()->setLinearVelocity(btVector3(0.0f, 0.0f, 0.0f));
        vehicle->getRigidBody()->setAngularVelocity(btVector3(0.0f, 0.0f, 0.0f));
    }
    
    printVehicleStatus(vehicle);
    
}

bool isVehicleStopped(btRaycastVehicle* vehicle, float threshold) {
    btVector3 linearVelocity = vehicle->getRigidBody()->getLinearVelocity();
    return linearVelocity.length() < threshold;
}

bool isVehicleBlocked(btRaycastVehicle* vehicle) {
    btVector3 linearVelocity = vehicle->getRigidBody()->getLinearVelocity();
    return linearVelocity.length() > 0.1f && linearVelocity.length() < 1.0f;
}

bool isVehicleInAir(btRaycastVehicle* vehicle) {
    for (int i = 0; i < vehicle->getNumWheels(); i++) {
        if (vehicle->getWheelInfo(i).m_raycastInfo.m_isInContact) {
            return false;
        }
    } return true;
}

bool isVehicleInAir(btDiscreteDynamicsWorld* dynamicsWorld, btRigidBody* carRigidBody) {
    btTransform chassisTransform;
    carRigidBody->getMotionState()->getWorldTransform(chassisTransform);

    btVector3 start = chassisTransform.getOrigin();
    btVector3 end = start - btVector3(0, 2.0f, 0); // Regola questa distanza secondo necessità

    btCollisionWorld::ClosestRayResultCallback rayCallback(start, end);
    dynamicsWorld->rayTest(start, end, rayCallback);

    // Se il ray test non ha colpito nulla, il veicolo è in aria
    return !rayCallback.hasHit();
}

bool isVehicleInAir(btDiscreteDynamicsWorld* dynamicsWorld, btRaycastVehicle* vehicle){
    return isVehicleInAir(vehicle) && isVehicleInAir(dynamicsWorld, vehicle->getRigidBody());
}

void printVehicleStatus(btRaycastVehicle* vehicle) {
    // Stampa la posizione del telaio
    btTransform chassisTransform = vehicle->getChassisWorldTransform();
    btVector3 chassisPosition = chassisTransform.getOrigin();
    std::cout << "Chassis Position: ("
        << chassisPosition.getX() << ", "
        << chassisPosition.getY() << ", "
        << chassisPosition.getZ() << ")" << std::endl;
    
    /*std::cout << "Linear velocity: " << vehicle->getRigidBody()->getLinearVelocity().getX() <<
        ", " << vehicle->getRigidBody()->getLinearVelocity().getY() <<
        ", " << vehicle->getRigidBody()->getLinearVelocity().getZ() <<
        "\nAngular velocity: " << vehicle->getRigidBody()->getAngularVelocity().getX() <<
        ", " << vehicle->getRigidBody()->getAngularVelocity().getY() <<
        ", " << vehicle->getRigidBody()->getAngularVelocity().getZ() << std::endl;*/
    
    std::cout << "Speed: " << vehicle->getRigidBody()->getLinearVelocity().length() << std::endl;

    // Stampa la posizione di una ruota (ad esempio, la ruota anteriore sinistra)
    /*int wheelIndex = 0; // Indice della ruota da stampare
    btWheelInfo& wheel = vehicle->getWheelInfo(wheelIndex);
    btTransform wheelTransform = wheel.m_worldTransform;
    btVector3 wheelPosition = wheelTransform.getOrigin();
    std::cout << "Wheel " << wheelIndex << " Position: ("
        << wheelPosition.getX() << ", "
        << wheelPosition.getY() << ", "
        << wheelPosition.getZ() << ")" << std::endl;*/
}

void setSuspensions(btRaycastVehicle* vehicle){
    for (int i = 0; i < vehicle->getNumWheels(); i++) {
        btWheelInfo& wheel = vehicle->getWheelInfo(i);
        wheel.m_suspensionStiffness = 20.0f;        // old: 20.0f
        wheel.m_wheelsDampingRelaxation = 2.5f;     // old: 2.3f    // semi-working: 2.5f
        wheel.m_wheelsDampingCompression = 4.0f;    // old: 4.4f    // semi-working: 4.5f
        wheel.m_frictionSlip = 1000.0f;             // old: 1000.0f
        wheel.m_rollInfluence = 0.1f;               // old: 0.1f
        wheel.m_maxSuspensionTravelCm = 20.0f;      // old: 20.0f
        wheel.m_maxSuspensionForce = 1000000.0f;       // old: 6000.0f
    }
}

// Funzione per limitare la rotazione del veicolo in aria
void limitVehicleRotationInAir(btRaycastVehicle* vehicle) {
    btVector3 angularVelocity = vehicle->getRigidBody()->getAngularVelocity();

    // Limita la rotazione attorno agli assi X e Z (rollio e beccheggio)
    btScalar maxAngularVelocity = 0.1; // Limite di velocità angolare
    btVector3 limitedAngularVelocity(
        std::max(std::min(angularVelocity.getX(), maxAngularVelocity), -maxAngularVelocity),
        angularVelocity.getY(), // Lascia l'asse Y (yaw) invariato
        std::max(std::min(angularVelocity.getZ(), maxAngularVelocity), -maxAngularVelocity)
    );

    vehicle->getRigidBody()->setAngularVelocity(limitedAngularVelocity);

    // Applica una coppia correttiva per riportare il veicolo parallelo al terreno
    btTransform trans;
    vehicle->getRigidBody()->getMotionState()->getWorldTransform(trans);
    btMatrix3x3 rotation = trans.getBasis();

    btVector3 up = rotation.getColumn(1); // Y axis
    btVector3 desiredUp(0, 1, 0);

    btVector3 correctionTorque = up.cross(desiredUp) * 0.15; // 0.1 è un fattore di correzione
    vehicle->getRigidBody()->applyTorque(correctionTorque);
}

#endif
