#ifndef SOLDIER_H_INCLUDED
#define SOLDIER_H_INCLUDED

#include "../Engine/engine.h"
#include "../World/character.h"

////////////////////////////////////////////
// CSoldier 
////////////////////////////////////////////

class CSoldier : public CCharacter
{
  public:
    CSoldier( const c8* configFilename );
    virtual ~CSoldier();

    virtual void Reset();
    virtual void Think();
    virtual void Render();

    virtual void Unserialize( RakNet::BitStream& bt );

  protected:
    virtual void Load( const c8* filename );

    virtual void PhysicsControl();

    f32 fRunForce, fJumpForce;

    // animations
    u16 anim_Idle, anim_Walk, anim_Run, anim_HoldWeapon, anim_RunBack, anim_Crouch, anim_BoltPull;

    vector3df vRunForce;
    f32 fMaxSpeed;
};

#endif