#include "vehicle_wheel.h"
#include "../Newton/newton_node.h" // 

////////////////////////////////////////////
// CNewton 
////////////////////////////////////////////

CVehicleWheel::CVehicleWheel()
{
}

CVehicleWheel::CVehicleWheel( const NewtonJoint* vehicle, int tyreid, const c8* modelFilename, tireSetup setup, vector3df vPos, vector3df vScale )
{
	assembleWheelPhysics( vehicle, tyreid, modelFilename, vPos, vScale, setup );
}

CVehicleWheel::~CVehicleWheel()
{
	if ( IRR.smgr )
		node->remove();
}

void CVehicleWheel::assembleWheelPhysics( const NewtonJoint* vehicle, int tyreid, const c8* modelFilename, vector3df vPos, vector3df vScale, tireSetup setup )
{
	// load scenenode
	vector3df vSize;
	IMesh* dummyMesh;
	node = loadNode_returnSize( modelFilename, vScale, 1.0f, vSize, dummyMesh );

    steerangle = 0.0f;
    torque = 0.0f;
    brakes = 0.0f;
    radius = vScale.X * vSize.X / 2; // these are the same values as the 3d mesh, ie the wheel meshes that we loaded in main() are
    width = vScale.Y * vSize.Z;  // cylinders of radius 2 and width 1 made in milkshape.

	// create the tyre's position (these coords were passed to us from assembleVehiclePhysics)
    matrix4 tyrepos;
    tyrepos.setTranslation( vPos );

    // the tire will spin around the lateral axis of the same tire space
	dVector tirePin (0.0, 0.0, 1.0f);

    
	// add the tire and set this as the user data
    // nb NULL should be the parent vehicle joint

	NewtonVehicleAddTire(vehicle, &tyrepos.M[0], &tirePin[0], setup.Mass, width, radius, 
						 setup.SuspensionShock, setup.SuspensionSpring, setup.SuspensionLength, this, tyreid);

}

void CVehicleWheel::setSteer(f32 newsteerangle)
{
    steerangle = newsteerangle;
}

void CVehicleWheel::setTorque(f32 newtorque)
{
	torque = newtorque;
}

void CVehicleWheel::setBrakes(f32 newbrakes)
{
    brakes = newbrakes;
}


void CVehicleWheel::setTirePhysics(const NewtonJoint *vehicle, void* id)
{
	f32 omega;
	f32 speed;
	f32 brakeAcceleration;


	dFloat currSteerAngle;

	currSteerAngle = NewtonVehicleGetTireSteerAngle (vehicle, id);
	NewtonVehicleSetTireSteerAngle (vehicle, id, currSteerAngle +  (steerangle - currSteerAngle) * 0.25f);

    // this is pretty much all from the newton vehicle demo - not quite sure what it's all doing i'm afraid

	// get the tire angular velocity
    // NULL should be the parent's vehicle joint
	omega = NewtonVehicleGetTireOmega (vehicle, id);

    
	// add some viscuos damp to the tire torque (this prevent out of control spin
    //printf("torque = %f\n", torque);
	NewtonVehicleSetTireTorque (vehicle, id, torque - 0.1f * omega);

	// calculate the tire speed at the contact
	// set the max side slip speed as a fraction of the tire spm
	speed = radius * omega;
	NewtonVehicleSetTireMaxSideSleepSpeed (vehicle, id, speed * 0.1f);

	// The side slip is usually propostinal to the tire longitudilnal speed, and tire load
	NewtonVehicleSetTireSideSleepCoeficient (vehicle, id, speed * 0.05f);

    
	// if brake are applyed ....
	if (brakes > 0.0f) 
	{
		// ask Newton for the precise acceleration needed to stop the tire
		brakeAcceleration = NewtonVehicleTireCalculateMaxBrakeAcceleration (vehicle, id);

		// tell Newton you want this tire stoped but only if the trque need it is less than 
		// the brakes pad can withstand (assume max brake pad torque is 500 newton * meter)
		NewtonVehicleTireSetBrakeAcceleration (vehicle, id, brakeAcceleration, 500.0f * brakes);

		// set some side slipe as funtion of the linear speed 
		speed = NewtonVehicleGetTireLongitudinalSpeed (vehicle, id);
		NewtonVehicleSetTireMaxSideSleepSpeed (vehicle, id, speed * 0.1f);
	}
	// Reset the brakes
	brakes = 0.0f;
}

//void CVehicleWheel::rigupPhysics(NewtonJoint *vehicle, vector3df vPos, int tyreid)
//{
//	f32 tireMass; 
//	f32 tireSuspensionShock;
//	f32 tireSuspensionSpring; 
//	f32 tireSuspensionLength;
//
//
//	// set tire values
//	tireMass = 2.0f; 
//	tireSuspensionShock = 1.0f;
//	tireSuspensionSpring = 1.0f; 
//	tireSuspensionLength = 0.05f;
//
//	// create the tyre's position (these coords were passed to us from assembleVehiclePhysics)
//    matrix4 tyrepos;
//    tyrepos.setTranslation(vPos);
//
//    // the tire will spin around the lateral axis of the same tire space
//	dVector tirePin (0.0, 0.0, 1.0f);
//
//    
//	// add the tire and set this as the user data
//    // nb NULL should be the parent vehicle joint
//
//    NewtonVehicleAddTire (vehicle, &tyrepos.M[0], &tirePin[0], tireMass, width, radius, 
//						  tireSuspensionShock, tireSuspensionSpring, tireSuspensionLength, wheelpointer, tyreid);
//
//}