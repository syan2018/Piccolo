#include "runtime/function/framework/component/motor/motor_climb.h"


namespace Piccolo
{

    float MotorClimb::GetClimbZ(){
        return climb_z;
    }

    void MotorClimb::SetClimbZ(float value){
        climb_z = value;
    }
    
    MotorClimb::MotorClimb(){ }
    
    MotorClimb::~MotorClimb(){ }

    float MotorClimb::climb_z = 0.f;

}