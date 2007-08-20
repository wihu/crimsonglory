#ifndef HYDROPLANE_H_INCLUDED
#define HYDROPLANE_H_INCLUDED

#include "../Engine/engine.h"
#include "../World/machine.h"
#include "../World/soundentity.h"
#include "../Irrlicht/CAnimSprite.h"

////////////////////////////////////////////
// CHydroplane 
////////////////////////////////////////////

class CHydroplane : public CMachine
{
  public:
    CHydroplane( const c8* scriptFilename );
    virtual ~CHydroplane();

    virtual void Reset();
    virtual void Think();
    virtual void Render();

    virtual void Unserialize( RakNet::BitStream& bt );

    f32 getEnginePower()
    {
        return fPower;
    }
    f32 getWingsLiftPower()
    {
        return fWingsLift;
    }
    f32 getTailLiftPower()
    {
        return fTailLift;
    }
    f32 getWingsResistFactor()
    {
        return fWingsResist;
    }
    f32 getSteerPower()
    {
        return fSteerPower;
    }
    f32 getStabilizePower()
    {
        return fStabilizePower;
    }
    f32 getPlaneResistFactor()
    {
        return fPlaneResist;
    }
    f32 getTailResistFactor()
    {
        return fTailResist;
    }

    vector3df vNMainAxis, vNMainAxisPerp;
    vector3df vAxisVelocity, vAxisVelocityPerp;
    s32 iRotTurn;

  protected:
    virtual void Load( const c8* filename );

    virtual void PhysicsControl();

    f32 fPower, fBrake;
    f32 fWingsLift, fTailLift, fWingsResist, fSteerPower, fStabilizePower, fPlaneResist, fTailResist;
    f32 fInitXAxisRotation;
    int iRotStep, iRotStepsNum;

    f32 fThrustValue;

    vector3df vAngDamp;

    vector3df vLastTrailPos;

    CSoundEntity* engineSound, * windSound;

    CAnimSpriteSceneNode* propellerSprite;
    vector3df vPropPos, vPropRot;
    ISceneNode* wake;

	CAnimSpriteSceneNode* exhaustSprite[8];
    vector3df vExhaustPos[8], vExhaustRot[8];

    String WeaponScriptName;

    bool bThrustChange;
    f32 wakeFallCounter, wakeActiveCounter, lastWaterVelocity;
};

#endif