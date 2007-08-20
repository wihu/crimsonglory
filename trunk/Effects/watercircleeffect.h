#ifndef WATERCIRCLEEFFECT_H_INCLUDED
#define WATERCIRCLEEFFECT_H_INCLUDED

#include "../Engine/engine.h"
#include "effect.h"
#include "../Irrlicht/CAnimSprite.h"

////////////////////////////////////////////
// CWaterCircleEffect 
////////////////////////////////////////////

class CWaterCircleEffect : public CEffect
{
  public:
    CWaterCircleEffect( vector3df OldPos, vector3df NewPos, f32 oneOverMassConst, f32 radiusConst, int aliveTime );
    virtual ~CWaterCircleEffect();

    virtual void Think();           
    virtual void Reset();

  protected:
    CAnimSpriteSceneNode* spritenode;
};

#endif