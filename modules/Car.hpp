#ifndef CAR_HPP
#define CAR_HPP

static float SteeringAng = 0.0f;
static float dampedVel = 0.0f;

void rotateCar(glm::vec3 carMovementInput, float deltaT){
    SteeringAng += -carMovementInput.x * STEERING_SPEED * deltaT;
    SteeringAng = (SteeringAng < -MAX_STEERING_ANGLE ? -MAX_STEERING_ANGLE :
                  (SteeringAng > MAX_STEERING_ANGLE  ? MAX_STEERING_ANGLE  : SteeringAng));
}

void updateCarSpeed(glm::vec3 carMovementInput, float deltaT){
    
    double lambdaVel = 8.0f;
    double dampedVelEpsilon = 0.001f;
    
    dampedVel =  MOVE_SPEED * deltaT * carMovementInput.z * (1 - exp(-lambdaVel * deltaT)) +
                 dampedVel * exp(-lambdaVel * deltaT);
    dampedVel = ((fabs(dampedVel) < dampedVelEpsilon) ? 0.0f : dampedVel);
    
}

#endif
