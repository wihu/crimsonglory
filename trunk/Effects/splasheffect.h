#ifndef SPLASHEFFECT_H_INCLUDED
#define SPLASHEFFECT_H_INCLUDED

#include "../Engine/engine.h"
#include "effect.h"

////////////////////////////////////////////
// CSplashEffect 
////////////////////////////////////////////

class CSplashEffect : public CEffect
{
  public:
    CSplashEffect( vector3df OldPos, vector3df NewPos, f32 oneOverMassConst = 1.0f, f32 radiusConst = 1.0f, int aliveTime = 60 );
    virtual ~CSplashEffect();

    virtual void Think();           
    virtual void Reset();

  protected:
};

#endif