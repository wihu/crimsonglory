#include "bullet.h"

#include "../App/app.h"
// IRR.
#include "../irrlicht/IrrlichtTask.h"
// CONSOLE.
#include "../IrrConsole/console.h"

#include "world.h"
#include "prop.h"
#include "character.h"

////////////////////////////////////////////
// CBullet 
////////////////////////////////////////////

CBullet::CBullet( vector3df NewPos, vector3df NewVelocity, PlayerID ownerPID, f32 oneOverMassConst, f32 radiusConst, int aliveTime, f32 power ) : CProjectile( NewPos, NewVelocity, ownerPID, oneOverMassConst, radiusConst, aliveTime, power )
{
    Reset();
    setDebugText( "bullet" );
}

CBullet::~CBullet()
{
}

void CBullet::Reset()
{
    CProjectile::Reset();
}

void CBullet::Think()
{
    CProjectile::Think();

    FACTORY->Effects.Create( "beam", AltOldPos, Pos, 0.0f, radius, 5 );
}

void CBullet::OnHitLevel( vector3df vImpactVel, f32 fImpactMass )
{
    bCanDie = true;

    FACTORY->Effects.Create( "dust", Pos, Pos, 0.5f, radius * 8.0f, 80 );
}

void CBullet::OnHitNewtonNode( CNewtonNode* hitnode, vector3df vImpactVel, f32 fImpactMass )
{
    bCanDie = true;

    if ( ( hitnode->getType() == NODECLASS_MACHINE ) || ( hitnode->getType() == NODECLASS_CHARACTER ) )
    {
      CActor* actor = static_cast<CActor*>( hitnode );
      actor->takeDamage( fHitPower, ownerPlayerID );
    }

    if ( hitnode->getType() == NODECLASS_MACHINE )
    {
      FACTORY->Effects.Create( "machinehit", Pos, Pos, 0.0f, radius * 12.5f, 30 );
    }

    if ( hitnode->getType() == NODECLASS_CHARACTER )
    {
      FACTORY->Effects.Create( "gunsplash", Pos, Pos, 0.00001f, 5.0f * radius, 30 );
    }


    if ( hitnode->getType() == NODECLASS_PROP )
    {
      CProp* prop = static_cast<CProp*>( hitnode );

      prop->fHealth -= fHitPower;
      if ( prop->fHealth <= 0.0f )
      {
        prop->BreakAtPoint( Pos, vImpactVel );
      }

      FACTORY->Effects.Create( "machinehit", Pos, Pos, 0.0f, radius * 12.5f, 30 );
    }
}