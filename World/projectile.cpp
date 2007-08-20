#include "projectile.h"

#include "../App/app.h"
// IRR.
#include "../irrlicht/IrrlichtTask.h"
// CONSOLE.
#include "../IrrConsole/console.h"

#include "world.h"
#include "character.h"

////////////////////////////////////////////
// CProjectile 
////////////////////////////////////////////

CProjectile::CProjectile( vector3df NewPos, vector3df NewVelocity, PlayerID ownerPID, f32 oneOverMassConst, f32 radiusConst, int aliveTime, f32 power ) : CWorldPart( NewPos, oneOverMassConst, radiusConst, 1.0f, true, aliveTime )
{
    Reset();

    setDebugText( "projectile" );

    SetVelocity( NewVelocity );

    beam = NULL;
    ownerPlayerID = ownerPID;
    fHitPower = power;
}

CProjectile::~CProjectile()
{
    if ( ( IRR.smgr ) && ( beam ) )
    {
      beam->remove();
    }
}

void CProjectile::Reset()
{
    CWorldPart::Reset();
}

void CProjectile::Think()
{
    CWorldPart::Think();

    // set the rendering and effects
    if ( beam )
    {
      beam->SetStartPoint( AltOldPos ); 
      beam->SetEndPoint( Pos );
    }
}

void CProjectile::OnHit( const NewtonBody* body, vector3df vImpactVel, f32 fImpactMass )
{
    dFloat mass;
    dFloat Ixx;
    dFloat Iyy;
    dFloat Izz;

    NewtonBodyGetMassMatrix( body, &mass, &Ixx, &Iyy, &Izz );

    if ( mass != 0.0f )
    {
      CNewtonNode* newtonNode = 0;
      newtonNode = ( CNewtonNode * )NewtonBodyGetUserData( body );
      if ( newtonNode )
      {
        OnHitNewtonNode( newtonNode, vImpactVel, fImpactMass );
      }

      vector3df vImpulse = vImpactVel* fImpactMass* fImpactMass* fImpactMass * 0.00001f; //TEMP: 0.00

      NewtonAddBodyImpulse( body, &vImpulse.X, &Pos.X );
    }
    else
    {
      OnHitLevel( vImpactVel, fImpactMass );
    }
}
