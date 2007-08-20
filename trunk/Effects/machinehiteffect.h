#ifndef MACHINEHITEFFECT_H_INCLUDED
#define MACHINEHITEFFECT_H_INCLUDED

#include "../Engine/engine.h"
#include "effect.h"
#include "../Irrlicht/CAnimSprite.h"

////////////////////////////////////////////
// CMachineHitEffect 
////////////////////////////////////////////

class CMachineHitEffect : public CEffect
{
  public:
    CMachineHitEffect( vector3df OldPos, vector3df NewPos, f32 oneOverMassConst, f32 radiusConst, int aliveTime );
    virtual ~CMachineHitEffect();

    virtual void Think();           
    virtual void Reset();

  protected:
    CAnimSpriteSceneNode* spritenode;
};

#endif