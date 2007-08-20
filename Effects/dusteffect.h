#ifndef DUSTEFFECT_H_INCLUDED
#define DUSTEFFECT_H_INCLUDED

#include "../Engine/engine.h"
#include "effect.h"
#include "../Irrlicht/CAnimSprite.h"

////////////////////////////////////////////
// CDustEffect 
////////////////////////////////////////////

class CDustEffect : public CEffect
{
  public:
    CDustEffect( vector3df OldPos, vector3df NewPos, f32 oneOverMassConst, f32 radiusConst, int aliveTime );
    virtual ~CDustEffect();

    virtual void Think();           
    virtual void Reset();

  protected:
    CAnimSpriteSceneNode* spritenode;
};

#endif