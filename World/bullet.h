#ifndef BULLET_H_INCLUDED
#define BULLET_H_INCLUDED

#include "../Engine/engine.h"
#include "projectile.h"

////////////////////////////////////////////
// CBullet 
////////////////////////////////////////////

class CBullet : public CProjectile
{
  public:
    CBullet( vector3df NewPos = vector3df( 0, 0, 0 ), vector3df NewVelocity = vector3df( 0, 0, 0 ), PlayerID ownerPID = UNASSIGNED_PLAYER_ID, f32 oneOverMassConst = 1.0f, f32 radiusConst = 1.0f, int aliveTime = -1, f32 power = 0.0f );
    virtual ~CBullet();

    virtual void Think();           
    virtual void Reset();

    virtual void OnHitLevel( vector3df vImpactVel, f32 fImpactMass );
    virtual void OnHitNewtonNode( CNewtonNode* hitnode, vector3df vImpactVel, f32 fImpactMass );

  protected:
};

#endif