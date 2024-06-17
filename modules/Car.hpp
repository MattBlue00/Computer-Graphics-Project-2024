#ifndef CAR_HPP
#define CAR_HPP

// car parameters
const float STEERING_SPEED = DEG_30;
const float MAX_STEERING_ANGLE = DEG_35;
const float MOVE_SPEED = 7.0f;

// car variables
static float SteeringAng = 0.0f;
static float dampedVel = 0.0f;

void updateSteeringAngle(glm::vec3 carMovementInput, float deltaT){
    SteeringAng += -carMovementInput.x * STEERING_SPEED * deltaT;
    SteeringAng = (SteeringAng < -MAX_STEERING_ANGLE ? -MAX_STEERING_ANGLE :
                  (SteeringAng > MAX_STEERING_ANGLE  ? MAX_STEERING_ANGLE  : SteeringAng));
}

void updateSpeed(glm::vec3 carMovementInput, float deltaT){
    
    double lambdaVel = 8.0f;
    double dampedVelEpsilon = 0.001f;
    
    dampedVel =  MOVE_SPEED * deltaT * carMovementInput.z * (1 - exp(-lambdaVel * deltaT)) +
                 dampedVel * exp(-lambdaVel * deltaT);
    dampedVel = ((fabs(dampedVel) < dampedVelEpsilon) ? 0.0f : dampedVel);
    
}

void moveCar(glm::vec3 carMovementInput, float deltaT, glm::vec3* Pos, float* Yaw){
    if(dampedVel != 0.0f) {
        if(SteeringAng != 0.0f) {
            const float l = 2.78f;
            float r = l / tan(SteeringAng);
            float cx = Pos->x + r * cos(*Yaw);
            float cz = Pos->z - r * sin(*Yaw);
            float Dbeta = dampedVel / r;
            *Yaw = *Yaw - Dbeta;
            Pos->x = cx - r * cos(*Yaw);
            Pos->z = cz + r * sin(*Yaw);
        } else {
            Pos->x = Pos->x - sin(*Yaw) * dampedVel;
            Pos->z = Pos->z - cos(*Yaw) * dampedVel;
        }
        if(carMovementInput.x == 0) {
            if(SteeringAng > STEERING_SPEED * deltaT) {
                SteeringAng -= STEERING_SPEED * deltaT;
            } else if(SteeringAng < -STEERING_SPEED * deltaT) {
                SteeringAng += STEERING_SPEED * deltaT;
            } else {
                SteeringAng = 0.0f;
            }
        }
    }
}

#endif
