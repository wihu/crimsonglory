#ifndef PROJECTILE_H_INCLUDED
#define PROJECTILE_H_INCLUDED

#include "../Engine/engine.h"
#include "worldpart.h"
#include "../Irrlicht/CBeamSceneNode.h"
#include "player.h"

class CCharacter;

////////////////////////////////////////////
// CProjectile 
////////////////////////////////////////////

class CProjectile : public CWorldPart
{
  public:
    CProjectile( vector3df NewPos = vector3df( 0, 0, 0 ), vector3df NewVelocity = vector3df( 0, 0, 0 ), PlayerID ownerPID = UNASSIGNED_PLAYER_ID, f32 oneOverMassConst = 1.0f, f32 radiusConst = 1.0f, int aliveTime = -1, f32 power = 0.0f );
    virtual ~CProjectile();

    virtual void Think();           
    virtual void Reset();

    // when OnHit is called the part is on contact position and has correct response velocity set
    virtual void OnHit( const NewtonBody* body, vector3df vImpactVel, f32 fImpactMass );
    virtual void OnHitLevel( vector3df vImpactVel, f32 fImpactMass )
    {
    }
    virtual void OnHitNewtonNode( CNewtonNode* hitnode, vector3df vImpactVel, f32 fImpactMass )
    {
    }

    // NULL on creation, same as with CWorldPart::node
    CBeamNode* beam;
    String beamTexture;

    PlayerID ownerPlayerID;

    f32 fHitPower;

  protected:
};

#endif