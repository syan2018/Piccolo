#pragma once

namespace Piccolo
{

    static class MotorClimb
    {
       public:
        static float climb_z;

        MotorClimb();
        ~MotorClimb();

        static float GetClimbZ();
        static void SetClimbZ(float);

    };

} 