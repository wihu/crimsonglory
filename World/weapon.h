#ifndef WEAPON_H_INCLUDED
#define WEAPON_H_INCLUDED

#include "../Engine/engine.h"
#include "item.h"

struct WeaponBulletData
{
    f32 fMass, fVelocity;
    f32 fRadius;
    int aliveTime;
    f32 fHitPower;
    // visuals
    String textureName;
    SColor beamColorStart, beamColorEnd;
};

////////////////////////////////////////////
// CWeapon
////////////////////////////////////////////

class CWeapon : public CItem
{
  public:
    CWeapon();
    CWeapon( const c8* modelFilename );
    virtual ~CWeapon();

    virtual void Reset();                           
    virtual void Think();
    virtual void Render();

    virtual bool canFire();

  protected:
    virtual void PhysicsCollision( const NewtonMaterial* material, const NewtonContact* contact, NewtonBody* cbody );

    static dFloat RayCastFilter( const NewtonBody* body, const dFloat* normal, int collisionID, void* userData, dFloat intersetParam );

    virtual void FireBullet( vector3df NewPos, vector3df VelocityNormal, WeaponBulletData* bdata );

    vector3df vMuzzlePos;

    WeaponBulletData bulletData;

    int canFireCount, fireInterval;
    bool bAutoFire, bBoltAction;

    bool triggerPressed, boltPullback;

    // ray casting
    dFloat fIParam;
    vector3df vColPoint, vRayStart, vRayEnd;

    String fireSoundName;
};

#endif