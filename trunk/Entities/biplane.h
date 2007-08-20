#ifndef BIPLANE_H_INCLUDED
#define BIPLANE_H_INCLUDED

#include "../Engine/engine.h"
#include "../World/entity.h"
#include "../World/vehicle.h"

#define BIPLANE_WHEELS 3

class CustomHinge;

////////////////////////////////////////////
// CBiplane 
////////////////////////////////////////////

class CBiplane : public CVehicle
{
  public:
    CBiplane( const c8* scriptFilename );
    virtual ~CBiplane();

	virtual void Reset();
	virtual void Think();
	virtual void Render();

	virtual void PhysicsControl();

	virtual void Load( const c8* filename );

	f32 getEnginePower()				{ return fPower; }
	f32 getWingsLiftPower()				{ return fWingsLift; }
	f32 getWingsResistFactor()			{ return fWingsResist; }
	f32 getSteerPower()					{ return fSteerPower; }
	f32 getStabilizePower()				{ return fStabilizePower; }
	f32 getPlaneResistFactor()			{ return fPlaneResist; }

	virtual void Zombify();

	vector3df vNMainAxis, vNMainAxisPerp;
	vector3df vAxisVelocity, vAxisVelocityPerp;
	s32 iRotTurn;

  private:
	void LoadZombie( matrix4 matrix );

	f32 fPower, fBrake;
	f32 fWingsLift, fWingsResist, fSteerPower, fStabilizePower, fPlaneResist;
	f32 fAngDamp, fLinDamp;
	f32 fInitXAxisRotation;
	int iRotStep, iRotStepsNum;

	f32 fRotationRate;
	vector3df vAxisRotation, vOldAxisRotation;

	CNewtonNode* p_wing;
	CustomHinge* joint_wing;
	CNewtonNode* p_wing2;
	CustomHinge* joint_wing2;
	CNewtonNode* p_tail;
	CustomHinge* joint_tail;
	CNewtonNode *p_wheels[BIPLANE_WHEELS];

	String configFilename;
};

#endif