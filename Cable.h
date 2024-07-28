#pragma once 
#include "GDPHYSX.h"


//renamed Chain class from the most recent quiz
namespace physics
{
    class Cable : public ParticleLink
    {
    public:
        float length = 1;
        float restitution = 0.9;
        Particle decoy;

        Cable(Particle* _p, MyVector anchorPoint);
        ParticleContact* GetContact() override;

    };
}
