/*
What this does, is basically tether a particle to a point by preventing it from moving further from said point in any given direction,
stopping in its tracks using the Rod's logic. Otherwise though, the particle's movement is unhampered while it is inside the given 
radius determined by the chain's length.

In other words, this acts as a chain by doing nothing to the particle's movement as long as it doesn't wonder too far from the 
given point, but when it does, it's movement is stopped in its tracks until and unless the particle starts to move closer
to the point instead.
*/


//renamed Chain class from the most recent quiz
#pragma once
#include "Cable.h"

using namespace physics;


Cable::Cable(Particle* _p, MyVector anchorPoint)
{
    this->particles[0] = _p;
    this->particles[1] =
        new Particle(anchorPoint,
            MyVector(0.f),
            MyVector(0.f),
            MyVector(0.f),
            nullptr);

    decoy = *this->particles[1];

}


ParticleContact* Cable::GetContact()
{
    float currLen = this->CurrentLength();

    if (abs(currLen) <= abs(this->length))//only does something if the particle is too far from the anchored point
        return nullptr;

    ParticleContact* ret = new ParticleContact();
    ret->particles[0] = particles[0];

    this->decoy = *this->particles[1];//"resets" the "second particle's" position to the anchored point
    ret->particles[1] = &this->decoy;//this way, it's treated as an immovable that the main particle can't move too far from

    auto dir = particles[1]->getPos() - particles[0]->getPos();
    dir = dir.Direction();

    if (currLen > this->length)
    {
        ret->contactNormal = dir;
        ret->depth = currLen - length;
    }
    else
    {
        ret->contactNormal = dir * -1;
        ret->depth = length - currLen;
    }

    ret->restitution = 0;

    return ret;
}
