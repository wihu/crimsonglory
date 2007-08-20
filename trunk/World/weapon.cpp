#include "weapon.h"

#include "bullet.h"
#include "character.h"

// SOUND.
#include "../FreeSL/SoundTask.h"

////////////////////////////////////////////
// CWeapon 
////////////////////////////////////////////

//bool bRegistered_CWeapon = FACTORY->Actors.Register<CWeapon>( "weapon" );

CWeapon::CWeapon()
{
    Reset();
}

CWeapon::CWeapon( const c8* modelFilename ) : CItem( modelFilename )
{
    Reset();
}

CWeapon::~CWeapon()
{
}

void CWeapon::Render()
{
    CItem::Render();
}

void CWeapon::Reset()
{
    CItem::Reset();

    type = NODECLASS_WEAPON;
    //bFix2DRot = false;

    bulletData.aliveTime = 100;
    bulletData.fMass = 110.01f;
    bulletData.fRadius = 0.1f;
    bulletData.fVelocity = 4.0f;
    bulletData.fHitPower = 0.0f;
    bulletData.textureName = "Textures/fireball.bmp";
    bulletData.beamColorStart = SColor( 255, 255, 255, 255 );
    bulletData.beamColorEnd = SColor( 255, 255, 255, 255 );

    vMuzzlePos = vector3df( 1, 0, 0 );

    canFireCount = 0;
    fireInterval = 10;
    boltPullback = triggerPressed = false;
    bAutoFire = true;
    bBoltAction = false;

    fireSoundName = "Sounds/m60_fire2.wav";
}


void CWeapon::Think()
{
    CItem::Think();

    if ( control )
    {
      if ( getControls()->ActionKeyPressed( AK_FIRE ) )
      {
        triggerPressed = true;
      }
      else
      {
        triggerPressed = false;
      }

      if ( getControls()->ActionKeyPressed( AK_RELOAD ) )
      {
        boltPullback = true;
      }

      if ( canFireCount > 0 )
      {
        if ( ( ( bAutoFire ) || ( !triggerPressed ) ) && ( ( !bBoltAction ) || ( ( bBoltAction ) && ( boltPullback ) ) ) )
        {
          canFireCount--;
        }
      }

      if ( triggerPressed )
      {
        if ( canFireCount == 0 )
        {
          vector3df vNewPosition;
          vector3df vNewVelocity( 1.0f, 0.0f, 0.0f );
          matrix4 m;
          m.setRotationDegrees( getRotation() ); 
          m.transformVect( vNewVelocity );
          vector3df vTransformMuzzle = vMuzzlePos;
          m.transformVect( vTransformMuzzle );

          vNewPosition = getPosition() - vTransformMuzzle;

          // cast ray to check if muzzle is in collision
          vRayStart = getPosition();
          vRayStart.Z = 0.0f;
          vRayEnd = vNewPosition;
          vRayEnd.Z = 0.0f;
          vColPoint = vRayEnd;
          fIParam = 999999.0f;
          NewtonWorldRayCast( WORLD.GetPhysics()->nWorld, &vRayStart.X, &vRayEnd.X, CWeapon::RayCastFilter, this );

          // the new position of the bullet is either at the muzzle or right before collison if muzzle was in collision
          FireBullet( vColPoint, -vNewVelocity, &bulletData );

          canFireCount = fireInterval;
          boltPullback = false;
        }
      }
    }
}

void CWeapon::PhysicsCollision( const NewtonMaterial* material, const NewtonContact* contact, NewtonBody* cbody )
{
    CNewtonNode::PhysicsCollision( material, contact, cbody );

    dVector point;
    dVector normal;
    dVector velocity;

    // Get the collision and normal
    NewtonMaterialGetContactPositionAndNormal( material, &point.m_x, &normal.m_x );

    // get the mass, check if static object
    dFloat mass, Ixx, Iyy, Izz;
    NewtonBodyGetMassMatrix( cbody, &mass, &Ixx, &Iyy, &Izz );
}

void CWeapon::FireBullet( vector3df NewPos, vector3df VelocityNormal, WeaponBulletData* bdata )
{
    vector3df vPos = NewPos + getTickVelocity();// + getVelocity() * NewtonToIrr;
    vector3df vVel = VelocityNormal* bdata->fVelocity + getTickVelocity();

    // find the character owner
    //CCharacter *owner = NULL;
    //CNewtonNode *nparent;
    //nparent = GetGreatestParent();
    //if (!nparent)
    //  CONSOLE.add("GetGreatestParent null");

    //if ( nparent->getType()  == NODECLASS_CHARACTER )
    //  owner = static_cast<CCharacter*>(nparent);

    PlayerID ownerID;
    CPlayer* player = WORLD.GetPlayers()->GetPlayerByControls( control );

    if ( player )
    {
      ownerID = player->playerID;
    }
    else
    {
      ownerID = UNASSIGNED_PLAYER_ID;
    }

    CBullet* p = new CBullet( vPos, vVel, ownerID, 1 / bdata->fMass, bdata->fRadius, bdata->aliveTime, bdata->fHitPower );

    p->beamTexture = bdata->textureName;
    //p->beam = new CBeamNode( IRR.smgr->getRootSceneNode( ), IRR.smgr, -1, bdata->textureName.c_str() ); 
    //p->beam->SetBeamScale( bdata->fRadius );
    //p->beam->SetBeamColorStart( bdata->beamColorStart ); 
    //p->beam->SetBeamColorEnd( bdata->beamColorEnd ); 

    SOUND.playSound( fireSoundName.c_str(), vPos, vector3df( 0, 0, 0 ) );
}

bool CWeapon::canFire()
{
    return !( canFireCount );
}

dFloat CWeapon::RayCastFilter( const NewtonBody* body, const dFloat* normal, int collisionID, void* userData, dFloat intersetParam )
{
    CWeapon* weapon = ( CWeapon* )userData;
    if ( intersetParam < weapon->fIParam )
    {
      weapon->fIParam = intersetParam;
      //get point of collision
      weapon->vColPoint = weapon->vRayStart + ( weapon->vRayEnd - weapon->vRayStart ) * intersetParam * 0.9f;
      //CONSOLE_VECTOR(weapon->vColPoint);
    }

    return intersetParam;
}