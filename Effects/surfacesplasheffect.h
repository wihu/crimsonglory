#ifndef SURFACESPLASHEFFECT_H_INCLUDED
#define SURFACESPLASHEFFECT_H_INCLUDED

#include "../Engine/engine.h"
#include "effect.h"
#include "../Irrlicht/CAnimSprite.h"

////////////////////////////////////////////
// CSurfaceSplashEffect 
////////////////////////////////////////////

class CSurfaceSplashEffect : public CEffect
{
  public:
    CSurfaceSplashEffect( vector3df OldPos, vector3df NewPos, f32 oneOverMassConst, f32 radiusConst, int aliveTime );
    virtual ~CSurfaceSplashEffect();

    virtual void Think();           
    virtual void Reset();

  protected:
    CAnimSpriteSceneNode* spritenode;
};

#endif