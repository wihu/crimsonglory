#ifndef VEHICLE_WHEEL_H_INCLUDED
#define VEHICLE_WHEEL_H_INCLUDED

#include "../Engine/engine.h"
#include "../Newton/newton_physics.h" // for loadNode_returnSize

////////////////////////////////////////////
// CVehicle 
////////////////////////////////////////////

struct tireSetup
{
	f32 Mass; 
	f32 SuspensionShock;
	f32 SuspensionSpring; 
	f32 SuspensionLength;
};

class CVehicleWheel
{
  public:
	CVehicleWheel();
    CVehicleWheel( const NewtonJoint* vehicle, int id, const c8* modelFilename, tireSetup setup, vector3df vPos = vector3df(0.0f, 0.0f, 0.0f), vector3df vScale = vector3df(1.0f, 1.0f, 1.0f) );
    virtual ~CVehicleWheel();

	void assembleWheelPhysics( const NewtonJoint* vehicle, int tyreid, const c8* modelFilename, vector3df vPos, vector3df vScale, tireSetup setup );

    void setSteer(f32 newsteerangle);
    void setTorque(f32 newtorque);
    void setBrakes(f32 newbrakes);

    void setTirePhysics(const NewtonJoint* vehicle, void* tyreid);

	ISceneNode* node;

  private:
    f32 steerangle;
    f32 radius;
    f32 width;
    f32 torque;
    f32 brakes;
};

#endif