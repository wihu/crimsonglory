#ifndef EFFECT_H_INCLUDED
#define EFFECT_H_INCLUDED

#include "../Engine/engine.h"
#include "../World/worldpart.h"

////////////////////////////////////////////
// CEffect 
////////////////////////////////////////////

class CEffect : public CWorldPart
{
  public:
    CEffect();
    CEffect( vector3df OldPos, vector3df NewPos, f32 oneOverMassConst = 1.0f, f32 radiusConst = 1.0f, int aliveTime = 60 );
    virtual ~CEffect();

    virtual void Think();           
    virtual void Reset();

  protected:
    f32 fScale;
    ITimebasedInterpolator* interpolator;
};

#endif