#ifndef HYDROPLANE_H_INCLUDED
#define HYDROPLANE_H_INCLUDED

#include "../Engine/engine.h"
#include "../World/machine.h"
#include "../World/soundentity.h"
//#include "../Irrlicht/CAnimSprite.h"

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

	virtual void Unserialize( RakNet::BitStream &bt );

	f32 getEnginePower()				{ return fPower; }
	f32 getWingsLiftPower()				{ return fWingsLift; }
	f32 getWingsResistFactor()			{ return fWingsResist; }
	f32 getSteerPower()					{ return fSteerPower; }
	f32 getStabilizePower()				{ return fStabilizePower; }
	f32 getPlaneResistFactor()			{ return fPlaneResist; }

	vector3df vNMainAxis, vNMainAxisPerp;
	vector3df vAxisVelocity, vAxisVelocityPerp;
	s32 iRotTurn;

  protected:
	virtual void Load( const c8* filename );

	virtual void PhysicsControl();

	f32 fPower, fBrake;
	f32 fWingsLift, fWingsResist, fSteerPower, fStabilizePower, fPlaneResist;
	f32 fInitXAxisRotation;
	int iRotStep, iRotStepsNum;

	vector3df vAngDamp;

	vector3df vLastTrailPos;

	CSoundEntity *engineSound, *windSound;
	
	//CAnimSpriteSceneNode* propellerSprite;
	vector3df 

	String WeaponScriptName;
};

#endif