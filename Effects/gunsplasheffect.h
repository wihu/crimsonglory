#ifndef GUNSPLASHEFFECT_H_INCLUDED
#define GUNSPLASHEFFECT_H_INCLUDED

#include "../Engine/engine.h"
#include "effect.h"

////////////////////////////////////////////
// CGunSplashEffect 
////////////////////////////////////////////

class CGunSplashEffect : public CEffect
{
  public:
    CGunSplashEffect( vector3df OldPos, vector3df NewPos, f32 oneOverMassConst = 1.0f, f32 radiusConst = 1.0f, int aliveTime = 60 );
    virtual ~CGunSplashEffect();

    virtual void Think();           
    virtual void Reset();

  protected:

    f32 fStartScale;
};

#endif