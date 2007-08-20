#ifndef BEAMEFFECT_H_INCLUDED
#define BEAMEFFECT_H_INCLUDED

#include "../Engine/engine.h"

#include "effect.h"
#include "../Irrlicht/CBeamSceneNode.h"

////////////////////////////////////////////
// CBeamEffect 
////////////////////////////////////////////

class CBeamEffect : public CEffect
{
  public:
    CBeamEffect( vector3df OldPos, vector3df NewPos, f32 oneOverMassConst = 1.0f, f32 radiusConst = 1.0f, int aliveTime = 60 );
    virtual ~CBeamEffect();

    virtual void Think();           
    virtual void Reset();

    // NULL on creation, same as with CWorldPart::node
    CBeamNode* beam;

  protected:
    void UpdateBeam();

    f32 fFade;
};

#endif