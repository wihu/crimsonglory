#ifndef VEHICLE_H_INCLUDED
#define VEHICLE_H_INCLUDED

#include "../Engine/engine.h"
#include "../Newton/newton_physics.h"
#include "../Newton/newton_node.h"
#include "vehicle_wheel.h"
#include "controllable.h"

////////////////////////////////////////////
// CVehicle 
////////////////////////////////////////////

class CVehicle : public CNewtonNode, public CControllable
{
  public:
	CVehicle();
    CVehicle( const c8* modelFilename, BodyType bodyType = BODY_BOX, vector3df vScale = vector3df(1.0f, 1.0f, 1.0f), float fMass = 1.0f );
    virtual ~CVehicle();

  protected:
	static void PhysicsVehicleWheelUpdate(const NewtonJoint *vehicle);
	static void transformWheels( NewtonJoint *vehicle, matrix4* mat );

	virtual void PhysicsTransform(const dFloat* matrix);

	virtual void Reset();
	virtual void Think();
	virtual void Render()				{};

	void assembleVehiclePhysics( const c8* modelFilename, BodyType bodyType, vector3df vScale, float fMass );

	CVehicleWheel* addWheel( const c8* modelFilename, tireSetup setup, vector3df vPos = vector3df(0.0f, 0.0f, 0.0f), vector3df vScale = vector3df(1.0f, 1.0f, 1.0f) );
	// call this after adding all wheels
	void endVehicleConstruction();

	CVehicleWheel* GetWheel(s32 i)					{ if (WheelExists(i)) return wheels[i]; else return NULL; }
	s32 GetWheelsNum()								{ return wheels.size(); }
	bool WheelExists(s32 i)							{ if ((i<0)||(i>(s32)wheels.size()-1)) return false; else return true;  }
	s32 GetWheelIndex(CVehicleWheel *Wheel)			{ return wheels.binary_search(Wheel); }

	virtual void Zombify()							{}

  protected:
	//virtual void onAttached( CNewtonNode *what, void *data );
	//virtual void onUnAttached();

	NewtonJoint *vehicle;
	array<CVehicleWheel*> wheels;

	vector3df vMountPos;
};

#endif