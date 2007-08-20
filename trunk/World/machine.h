#ifndef MACHINE_H_INCLUDED
#define MACHINE_H_INCLUDED

#include "../Engine/engine.h"
#include "actor.h"
#include "../World/weapon.h"

////////////////////////////////////////////
// CMachine 
////////////////////////////////////////////

class CMachine : public CActor
{
  public:
    CMachine( const c8* scriptFilename );
    virtual ~CMachine();

    virtual void Reset();
    virtual void Think();
    virtual void Render();

    virtual void setDirection( f32 angle );

  protected:
    virtual void PhysicsCollision( const NewtonMaterial* material, const NewtonContact* contact, NewtonBody* cbody );

    virtual void LoadZombie( matrix4 matrix );

    virtual bool onChildAttached( CNewtonNode* what, void* data );
    virtual void onChildUnAttached( CNewtonNode* what );

    // TEMP?
    CWeapon* weapon;

    int maxSplits, propTimeout;
    f32 fSplitFactor;
};



#endif