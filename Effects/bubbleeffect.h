#ifndef BUBBLEEFFECT_H_INCLUDED
#define BUBBLEEFFECT_H_INCLUDED

#include "../Engine/engine.h"
#include "effect.h"

////////////////////////////////////////////
// CBubbleEffect 
////////////////////////////////////////////

class CBubbleEffect : public CEffect
{
  public:
    CBubbleEffect( vector3df OldPos, vector3df NewPos, f32 oneOverMassConst = 1.0f, f32 radiusConst = 1.0f, int aliveTime = 60 );
    virtual ~CBubbleEffect();

    virtual void Think();           
    virtual void Reset();

  protected:
};

#endif