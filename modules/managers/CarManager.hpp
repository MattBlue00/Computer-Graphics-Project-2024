#ifndef CAR_MANAGER_HPP
#define CAR_MANAGER_HPP

#include <btBulletDynamicsCommon.h>

#include "../modules/data/Signals.hpp"
#include "../modules/objects/Track.hpp"

class CarManager: public Manager, public Receiver {
    
protected:
    
    const float ENGINE_FORCE = 6000.0f; // Maximum force applied to wheels
    const float BRAKE_FORCE = 250.0f; // Maximum brake force
    const float STEERING_INCREMENT_PER_FRAME = 0.08f;
    const float STEERING_DEGRADATION_EXP = 0.15f;
    const float SPEED_START_STEERING_DEGRADATION = 10.0f;
    const float MAX_STEERING = 0.3f;
    const float MIN_STEERING = 0.001f;
    const float MAX_SPEED = 50.52f; // 181kmh
    
    const float RAYCAST_DISTANCE = 2.0f;
    
    bool canStart = false;
    
    bool goingOnwards = true;
    int mayBeBlocked = 0;
    int lastSpeedKmh = 0;
    
    int currentSpeedKmh = 0;
    
    void setSuspensions() {
        for (int i = 0; i < vehicle->getNumWheels(); i++) {
            btWheelInfo& wheel = vehicle->getWheelInfo(i);
            wheel.m_suspensionStiffness = 80.0f;
            wheel.m_wheelsDampingRelaxation = 10.0f;
            wheel.m_wheelsDampingCompression = 8.0f;
            wheel.m_frictionSlip = 1000.0f;
            wheel.m_rollInfluence = 0.1f;
            wheel.m_maxSuspensionTravelCm = 10.0f;
            wheel.m_maxSuspensionForce = 1000000.0f;
        }
    }
    
    bool isVehicleStopped(float threshold) {
        btVector3 linearVelocity = vehicle->getRigidBody()->getLinearVelocity();
        return linearVelocity.length() < threshold;
    }
    
    bool isVehicleBlocked() {
        float linearVelocity = vehicle->getRigidBody()->getLinearVelocity().length();
        if(linearVelocity > 0.1f && linearVelocity < 5.0f){
            mayBeBlocked++;
            return mayBeBlocked > 60;
        }
        else{
            mayBeBlocked = 0;
            return false;
        }
    }
    
    bool isVehicleInAir1() {
        for (int i = 0; i < vehicle->getNumWheels(); i++) {
            if (vehicle->getWheelInfo(i).m_raycastInfo.m_isInContact) {
                return false;
            }
        } return true;
    }
    
    bool isVehicleInAir2() {
        btTransform chassisTransform;
        vehicle->getRigidBody()->getMotionState()->getWorldTransform(chassisTransform);
        
        btVector3 start = chassisTransform.getOrigin();
        btVector3 end = start - btVector3(0, 2.0f, 0); // Regola questa distanza secondo necessità
        
        btCollisionWorld::ClosestRayResultCallback rayCallback(start, end);
        dynamicsWorld->rayTest(start, end, rayCallback);
        
        // Se il ray test non ha colpito nulla, il veicolo è in aria
        return !rayCallback.hasHit();
    }
    
    bool isVehicleInAir(){
        return isVehicleInAir1() && isVehicleInAir2();
    }
    
    // Funzione per limitare la rotazione del veicolo in aria
    void limitVehicleRotationInAir() {
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
        
        btVector3 correctionTorque = up.cross(desiredUp) * 0.4; // 0.1 è un fattore di correzione
        vehicle->getRigidBody()->applyTorque(correctionTorque);
    }
    
    void checkVehiclePosition() {
        btVector3 vehiclePosition = vehicle->getChassisWorldTransform().getOrigin();
        if (vehiclePosition.getY() < -50.0f) {
            respawnVehicle();
        }
    }
    
    void respawnVehicle() {
        btTransform transform = lastCheckpointTransform;
        transform.setOrigin(transform.getOrigin());
        vehicle->getRigidBody()->setWorldTransform(transform);
        vehicle->getRigidBody()->setLinearVelocity(btVector3(0, 0, 0));
        vehicle->getRigidBody()->setAngularVelocity(btVector3(0, 0, 0));
        vehicle->getRigidBody()->clearForces();
    }
    
    btTransform getVehicleTransform(){
        btTransform transform;
        vehicle->getRigidBody()->getMotionState()->getWorldTransform(transform);
        return transform;
    }
    
    glm::vec3 getVehiclePosition(){
        btTransform transform = getVehicleTransform();
        return glm::vec3(transform.getOrigin().getX(), transform.getOrigin().getY(), transform.getOrigin().getZ());
    }
    
    glm::quat getVehicleRotation(){
        btTransform transform = getVehicleTransform();
        glm::vec3 bodyPosition = glm::vec3(transform.getOrigin().getX(), transform.getOrigin().getY(), transform.getOrigin().getZ());
        btQuaternion rotationBt = transform.getRotation();
        return glm::quat(rotationBt.getW(), rotationBt.getX(), rotationBt.getY(), rotationBt.getZ());
    }
    
    float getVehicleYaw() {
        glm::quat rotation = getVehicleRotation();
        return atan2(2.0f * (rotation.y * rotation.w + rotation.x * rotation.z),
                     1.0f - 2.0f * (rotation.y * rotation.y + rotation.x * rotation.x));
    }
    
    float getVehiclePitch() {
        glm::quat rotation = getVehicleRotation();
        float sinPitch = 2.0f * (rotation.w * rotation.x - rotation.z * rotation.y);
        if (std::abs(sinPitch) >= 1.0f) {
            return std::copysign(glm::half_pi<float>(), sinPitch); // Usa 90 gradi se fuori range
        } else {
            return std::asin(sinPitch);
        }
    }
    
    float getVehicleRoll() {
        glm::quat rotation = getVehicleRotation();
        return atan2(2.0f * (rotation.w * rotation.z + rotation.x * rotation.y),
                     1.0f - 2.0f * (rotation.y * rotation.y + rotation.z * rotation.z));
    }
    
    void onCountdown(int countdownValue) {
        if (countdownValue <= 1){
            canStart = true;
        }
    }
    
public:
    
    void init() override {
        // Car initialization with btBoxShape and btCompoundShape
        btBoxShape* chassisShape = new btBoxShape(btVector3(1.6, 0.5, 1.95));
        
        // Create a compound shape and add the chassis shape at the origin
        btCompoundShape* vehicleShape = new btCompoundShape();
        btTransform localTrans;
        localTrans.setIdentity();
        localTrans.setOrigin(btVector3(0, 0, 0)); // Chassis remains at the origin
        vehicleShape->addChildShape(localTrans, chassisShape);
        
        btDefaultMotionState* carMotionState = new btDefaultMotionState(btTransform(btQuaternion(0, 0, 0, 1), btVector3(0, -0.5, -30)));
        btScalar mass = 1000.0f;
        btVector3 carInertia(0, 0, 0);
        vehicleShape->calculateLocalInertia(mass, carInertia);
        btRigidBody::btRigidBodyConstructionInfo carRigidBodyCI(mass, carMotionState, vehicleShape, carInertia);
        btRigidBody* carRigidBody = new btRigidBody(carRigidBodyCI);
        
        carRigidBody->setActivationState(DISABLE_DEACTIVATION); // Ensure car stays active
        carRigidBody->activate(true);
        carRigidBody->setDamping(0.2f, 0.2f); // Increased linear and angular damping
        
        dynamicsWorld->addRigidBody(carRigidBody);
        
        // Raycast vehicle setup
        btRaycastVehicle::btVehicleTuning tuning;
        btVehicleRaycaster* vehicleRaycaster = new btDefaultVehicleRaycaster(dynamicsWorld);
        vehicle = new btRaycastVehicle(tuning, carRigidBody, vehicleRaycaster);
        dynamicsWorld->addVehicle(vehicle);
        
        // Verify the direction of the wheels and suspensions
        btVector3 wheelDirectionCS0(0, -1, 0); // Suspension direction down along the negative Y axis
        btVector3 wheelAxleCS(-1, 0, 0); // Wheel axle along the negative X axis
        
        // Add wheels
        btVector3 connectionPointCS0;
        bool isFrontWheel;
        
        // Rest length of the suspension
        btScalar suspensionRestLength = 0.7;
        
        // Wheel radius
        btScalar wheelRadius = 0.5;
        
        // Front left wheel
        connectionPointCS0 = btVector3(-1, 0.5, 2);
        isFrontWheel = true;
        vehicle->addWheel(connectionPointCS0, wheelDirectionCS0, wheelAxleCS, suspensionRestLength, wheelRadius, tuning, isFrontWheel);
        
        // Front right wheel
        connectionPointCS0 = btVector3(1, 0.5, 2);
        isFrontWheel = true;
        vehicle->addWheel(connectionPointCS0, wheelDirectionCS0, wheelAxleCS, suspensionRestLength, wheelRadius, tuning, isFrontWheel);
        
        // Rear left wheel
        connectionPointCS0 = btVector3(-1, 0.5, -1.88);
        isFrontWheel = false;
        vehicle->addWheel(connectionPointCS0, wheelDirectionCS0, wheelAxleCS, suspensionRestLength, wheelRadius, tuning, isFrontWheel);
        
        // Rear right wheel
        connectionPointCS0 = btVector3(1, 0.5, -1.88);
        isFrontWheel = false;
        vehicle->addWheel(connectionPointCS0, wheelDirectionCS0, wheelAxleCS, suspensionRestLength, wheelRadius, tuning, isFrontWheel);
        
        // Set wheel parameters
        setSuspensions();
        
        // Set coordinate system (X right, Y up, Z forward)
        vehicle->setCoordinateSystem(0, 1, 2);
    }
    
    void update() override {
        
        glm::vec3 carPosition = getVehiclePosition();
        float yaw = getVehicleYaw();
        float pitch = getVehiclePitch();
        float roll = getVehicleRoll();
        carWorldData = CarWorldData(pitch, yaw, roll, carPosition);
        
        if(!canStart) return;
        
        // Controlli del veicolo
        float engineForce = 0.0f;
        float brakeForce = 0.0f;
        float steering = vehicle->getSteeringValue(0); // Assumi che le ruote anteriori siano a indice 0 e 1
        
        // Calcolo della velocità attuale
        float currentSpeed = vehicle->getRigidBody()->getLinearVelocity().length();
        
        // Movimento avanti/indietro
        if (carMovementInput.z < 0) { // W premuto
            engineForce = ENGINE_FORCE;
            brakeForce = 0.0f;
            if(isVehicleStopped(0.5f) && !goingOnwards){
                goingOnwards = true;
            }
        }
        else if (carMovementInput.z > 0 && goingOnwards) { // S premuto
            engineForce = 0.0f; // Forza negativa per andare in retro
            brakeForce = BRAKE_FORCE;
            if(isVehicleStopped(0.5f)){
                goingOnwards = false;
            }
            
        }
        else if (carMovementInput.z > 0 && !goingOnwards) { // S premuto
            engineForce = -ENGINE_FORCE; // Forza negativa per andare in retro
            brakeForce = 0.0f;
            goingOnwards = false;
        }
        else { // Nessun input
            if (isVehicleStopped(0.5f)){
                brakeForce = BRAKE_FORCE;
                engineForce = 0.0f;
            }
            else {
                if(isVehicleStopped(3.0f)){
                    engineForce = 0.0f;
                }
                else{
                    engineForce = ENGINE_FORCE * 0.75f * glm::clamp(currentSpeed / MAX_SPEED, 0.0f, 1.0f);
                }
                brakeForce = 0.0f;
            }
        }
        
        // Fattore di riduzione della sterzata basato sulla velocità
        float speedFactor = currentSpeed < SPEED_START_STEERING_DEGRADATION ?
        1.0f :
        glm::clamp(1.0f - glm::pow((currentSpeed - SPEED_START_STEERING_DEGRADATION) / (MAX_SPEED - SPEED_START_STEERING_DEGRADATION), STEERING_DEGRADATION_EXP),
                   0.0f,
                   1.0f);
        
        float dynamicSteeringIncrement = glm::clamp(STEERING_INCREMENT_PER_FRAME * speedFactor, MIN_STEERING, MAX_STEERING);
        
        // Sterzata destra/sinistra
        if (carMovementInput.x > 0) {
            steering -= dynamicSteeringIncrement;
            if (steering < -MAX_STEERING) {
                steering = -MAX_STEERING;
            }
        }
        else if (carMovementInput.x < 0) {
            steering += dynamicSteeringIncrement;
            if (steering > MAX_STEERING) {
                steering = MAX_STEERING;
            }
        }
        else {
            // Se non ci sono input, ritorna gradualmente la sterzata a zero
            if (steering > 0) {
                steering -= (STEERING_INCREMENT_PER_FRAME / 2.0f);
                if (steering < 0) steering = 0;
            }
            else if (steering < 0) {
                steering += (STEERING_INCREMENT_PER_FRAME / 2.0f);
                if (steering > 0) steering = 0;
            }
        }
        
        // Applicazione dei controlli
        if(!isVehicleInAir()){
            if(vehicle->getRigidBody()->getLinearVelocity().length() <= MAX_SPEED){
                if(engineForce != 0.0f && isVehicleBlocked()){
                    float bonusCoefficient = mayBeBlocked * 0.15f;
                    vehicle->applyEngineForce(bonusCoefficient * engineForce, 0);
                    vehicle->applyEngineForce(bonusCoefficient * engineForce, 1);
                    vehicle->applyEngineForce(bonusCoefficient * engineForce, 2);
                    vehicle->applyEngineForce(bonusCoefficient * engineForce, 3);
                }
                else{
                    vehicle->applyEngineForce(0.0f, 0);
                    vehicle->applyEngineForce(0.0f, 1);
                    vehicle->applyEngineForce(engineForce, 2); // Ruote posteriori
                    vehicle->applyEngineForce(engineForce, 3);
                }
            }
            else{
                // std::cout << "Max Speed Reached!" << std::endl;
                vehicle->applyEngineForce(0.0f, 2); // Ruote posteriori
                vehicle->applyEngineForce(0.0f, 3);
            }
            vehicle->setBrake(brakeForce, 2);
            vehicle->setBrake(brakeForce, 3);
            vehicle->setSteeringValue(steering, 0); // Ruote anteriori
            vehicle->setSteeringValue(steering, 1);
        }
        else{
            limitVehicleRotationInAir();
        }
        
        if(engineForce == 0.0f && isVehicleStopped(0.5f)){
            vehicle->getRigidBody()->setLinearVelocity(btVector3(0.0f, 0.0f, 0.0f));
            vehicle->getRigidBody()->setAngularVelocity(btVector3(0.0f, 0.0f, 0.0f));
            btVector3 upwardForce = -vehicle->getRigidBody()->getMass() * dynamicsWorld->getGravity();
            vehicle->getRigidBody()->applyCentralForce(upwardForce);
        }
        
        // make micro-movements have no impact on displayed speed
        if(currentSpeed < 0.4f){
            currentSpeed = 0;
        }
        
        checkVehiclePosition();
        
        if (!goingOnwards || (carMovementInput.z > 0 && currentSpeed == 0)){
            reverseSignal.emit({});
        }
        
        if ((carMovementInput.z > 0 && goingOnwards) || (carMovementInput.z < 0 && !goingOnwards)){
            brakeSignal.emit({});
        }
        
        // update Kmh Speed and notify UI only when necessary
        currentSpeedKmh = static_cast<int>(std::abs(std::floor(currentSpeed * 3.6)));
        if(lastSpeedKmh != currentSpeedKmh){
            // fix the flickering speed number at maxspeed
            if(currentSpeedKmh == std::abs(std::floor(MAX_SPEED * 3.6))) return;
            speedSignal.emit(currentSpeedKmh);
            lastSpeedKmh = currentSpeedKmh;
        }
        
    }
    
    void cleanup() override {}
    
    void onSignal(std::string id, std::any data) override {
        if (id == COUNTDOWN_SIGNAL) {
            onCountdown(std::any_cast<int>(data));
        }
        else {
            std::cerr << "Unknown signal type: " << id << std::endl;
        }
    }
    
};

#endif
