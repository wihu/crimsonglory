#include "vehicle.h"

////////////////////////////////////////////
// CVehicle 
////////////////////////////////////////////

CVehicle::CVehicle()
{
	Reset();
}

CVehicle::CVehicle( const c8* modelFilename, BodyType bodyType, vector3df vScale, float fMass )
{
	Reset();
	assembleVehiclePhysics( modelFilename, bodyType, vScale, fMass );
}

CVehicle::~CVehicle()
{
	int i;

 	for (i=0; i < wheels.size(); i++)
		delete wheels[i];
	wheels.clear();
}

void CVehicle::Reset()
{
	//CControllable::Reset();
	CNewtonNode::Reset();

	bZombie = false;
	vMountPos = vector3df( 0.0f, 0.0f, 0.0f );
}


void CVehicle::Think()
{
	CControllable::Think();
}

void CVehicle::PhysicsVehicleWheelUpdate(const NewtonJoint *vehicle)
{
    void* wheelid = 0;
    for (wheelid = NewtonVehicleGetFirstTireID(vehicle); wheelid; wheelid = NewtonVehicleGetNextTireID(vehicle, wheelid))
    {
        CVehicleWheel* wheel = (CVehicleWheel*) NewtonVehicleGetTireUserData(vehicle, wheelid);
        wheel->setTirePhysics(vehicle, wheelid);
    }

}


void CVehicle::PhysicsTransform(const dFloat* matrix)
{
	static matrix4 mat;
	memcpy(mat.M, matrix, sizeof(float)*16);
	node->setPosition( mat.getTranslation() );
    node->setRotation( mat.getRotationDegrees() );

	transformWheels(vehicle, &mat );
}

void CVehicle::assembleVehiclePhysics( const c8* modelFilename, BodyType bodyType, vector3df vScale, float fMass )
{
	assemblePhysics( modelFilename, bodyType, vScale, fMass );

	dVector updir = dVector(0.0f, -1.0f, 0.0f);
	vehicle = NewtonConstraintCreateVehicle (WORLD.GetPhysics()->nWorld, &updir[0], this->body);

    NewtonVehicleSetTireCallback(vehicle, PhysicsVehicleWheelUpdate);

    NewtonBodySetAutoFreeze(this->body, 0);

	setUserData();
}

CVehicleWheel* CVehicle::addWheel( const c8* modelFilename, tireSetup setup, vector3df vPos, vector3df vScale )
{
	CVehicleWheel* wheel = new CVehicleWheel( vehicle, wheels.size(), modelFilename, setup, vPos, vScale );
	wheels.push_back( wheel );

	return wheel;
}

void CVehicle::endVehicleConstruction()
{
	//NewtonVehicleBalanceTires( vehicle, WORLD.GetPhysics()->dGravity );
}

void CVehicle::transformWheels( NewtonJoint *vehicle, matrix4* mat )
{
	if (!vehicle)
		return;
    void* wheelid = 0;
	CVehicleWheel *wheel;
    for(wheelid = NewtonVehicleGetFirstTireID(vehicle); wheelid; wheelid = NewtonVehicleGetNextTireID(vehicle, wheelid))
    {
        wheel = NULL;
		wheel = (CVehicleWheel*) NewtonVehicleGetTireUserData(vehicle, wheelid);
		if (wheel)
		{
		   NewtonVehicleGetTireMatrix(vehicle, wheelid, &mat->M[0]);
		   wheel->node->setPosition(mat->getTranslation());
		   wheel->node->setRotation(mat->getRotationDegrees());
		}
    }
}

//void CVehicle::onAttached( CNewtonNode *what, void *data)
//{
//	//if (what->getType() == CHARACTER)
//	{
//		setControls( (CControls*) data );
//		what->setAttachmentPos( vMountPos );
//	}
//}
//
//void CVehicle::onUnAttached()
//{
//	//if (what->getType() == CHARACTER)
//	{
//		setControls( NULL );
//	}
//}

