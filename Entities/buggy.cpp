#include "buggy.h"
// IRR.
#include "../irrlicht/IrrlichtTask.h"
// CONSOLE.
#include "../IrrConsole/console.h"
#include "../IrrConsole/console_vars.h"

#define BUGGY_TORQUE 50.0f
#define BUGGY_BRAKE 1.0f

////////////////////////////////////////////
// CBuggy 
////////////////////////////////////////////

CBuggy::CBuggy( const c8* scriptFilename )
{
	CBuggy::Reset();

	Load( scriptFilename );

	if ( (!node)||(!body) )
		bInvalidEntity = true;

	setDebugText( scriptFilename );

	setRotation( vector3df(0, ANGLE_FACE_LEFT, 0) );
}

CBuggy::~CBuggy()
{
}

void CBuggy::Reset()
{
	CVehicle::Reset();

	vAxisRotation.Y = ANGLE_FACE_LEFT;
	//bFix2DRot = false;
}

void CBuggy::Think()
{
	CVehicle::Think();

	// update the debug position
	setDebugPos( IRR.smgr->getSceneCollisionManager()->getScreenCoordinatesFrom3DPosition( getNodeCenter(), IRR.smgr->getActiveCamera() ) );

	GetWheel(0)->setTorque(0.0f);
	GetWheel(1)->setTorque(0.0f);
	GetWheel(2)->setBrakes(0.0f);
	GetWheel(3)->setBrakes(0.0f);
	GetWheel(2)->setSteer(0.0f);
	GetWheel(3)->setSteer(0.0f);
	vOldAxisRotation.Y = vAxisRotation.Y;

	if ( control )
	{
		if ( control->KeyPressed( irr::KEY_KEY_X ) ) // reverse
		{
			GetWheel(0)->setTorque(fTorque);
			GetWheel(1)->setTorque(fTorque);
		}
		if ( control->KeyPressed( irr::KEY_KEY_S ) ) // break
		{
			GetWheel(2)->setBrakes(fBrake);
			GetWheel(3)->setBrakes(fBrake);
		}
		if ( control->KeyPressed( irr::KEY_KEY_A ) ) // go right
		{
			GetWheel(0)->setTorque(-fTorque);
			GetWheel(1)->setTorque(-fTorque);

			if ( vAxisRotation.Y < ANGLE_FACE_RIGHT )
			{
				vAxisRotation.Y += fRotationRate;
				GetWheel(2)->setSteer(-1.0f);
				GetWheel(3)->setSteer(-1.0f);
			}
		}
		if ( control->KeyPressed( irr::KEY_KEY_D ) ) // go left
		{
			GetWheel(0)->setTorque(-fTorque);
			GetWheel(1)->setTorque(-fTorque);

			if ( vAxisRotation.Y > ANGLE_FACE_LEFT )
			{
				vAxisRotation.Y -= fRotationRate;
				GetWheel(2)->setSteer(1.0f);
				GetWheel(3)->setSteer(1.0f);
			}
		}

		if ( vOldAxisRotation.Y != vAxisRotation.Y )
		{
			//CONSOLE.addx( "vAxisRotation.Y: %f", vAxisRotation.Y);
			setRotation( vector3df(0, vAxisRotation.Y, 0), true, true, true );
		}

	}
}

void CBuggy::Render()
{
	CVehicle::Render();
}

void CBuggy::Load( const c8* filename )
{
	// load script file
	SCRIPT.RunFile( filename );

	String modelname, wheelmodelname, texname, wheeltexname;
	vector3df vScale;
	f32 fMass;
	int body;
	vector3df vWheelPos[4];
	f32 fWheelMass[4];
	f32 fWheelShock[4];
	f32 fWheelSpring[4];
	f32 fWheelLength[4];

    GMVARTOSTRING( "buggy_modelname", modelname ); 
    GMVARTOSTRING( "buggy_wheel_modelname", wheelmodelname );     
	GMVARTOSTRING( "buggy_texturename", texname );  
	GMVARTOSTRING( "buggy_wheel_texturename", wheeltexname ); 
    
  	GMVARTOVALUE( "buggy_modelscale_x", f32, vScale.X );
  	GMVARTOVALUE( "buggy_modelscale_y", f32, vScale.Y );
  	GMVARTOVALUE( "buggy_modelscale_z", f32, vScale.Z );  	  	

   	GMVARTOVALUE( "buggy_mass", f32, fMass );	
  	GMVARTOVALUE( "buggy_bodytype", int, body );
  	
	GMVARTOVALUE( "buggy_torque", f32, fTorque );	
	GMVARTOVALUE( "buggy_brake", f32, fBrake );	  
	GMVARTOVALUE( "buggy_rotationrate", f32, fRotationRate );	 

  	GMVARTOVALUE( "buggy_mountpos_x", f32, vMountPos.X );
  	GMVARTOVALUE( "buggy_mountpos_y", f32, vMountPos.Y );
  	GMVARTOVALUE( "buggy_mountpos_z", f32, vMountPos.Z );

	assembleVehiclePhysics( modelname.c_str(), (BodyType) body, vScale, fMass );
	node->setMaterialTexture(0, IRR.video->getTexture( texname.c_str() ));

	char buffer [50];
	tireSetup setup;
	for ( int i = 0; i < 4; i++ )
	{
		sprintf(buffer, "buggy_wheel%i_mass", i+1);
		GMVARTOVALUE( buffer, f32, fWheelMass[i] ); 
		sprintf(buffer, "buggy_wheel%i_position_x", i+1);
		GMVARTOVALUE( buffer, f32, vWheelPos[i].X ); 
		sprintf(buffer, "buggy_wheel%i_position_y", i+1);
		GMVARTOVALUE( buffer, f32, vWheelPos[i].Y ); 
		sprintf(buffer, "buggy_wheel%i_position_z", i+1);
		GMVARTOVALUE( buffer, f32, vWheelPos[i].Z ); 		
		sprintf(buffer, "buggy_wheel%i_suspensionshock", i+1);
		GMVARTOVALUE( buffer, f32, fWheelShock[i] ); 	
		sprintf(buffer, "buggy_wheel%i_suspensionspring", i+1);
		GMVARTOVALUE( buffer, f32, fWheelSpring[i] ); 	
		sprintf(buffer, "buggy_wheel%i_suspensionlength", i+1);
		GMVARTOVALUE( buffer, f32, fWheelLength[i] );

		setup.Mass = fWheelMass[i];
		setup.SuspensionShock = fWheelShock[i];
		setup.SuspensionSpring = fWheelSpring[i];
		setup.SuspensionLength = fWheelLength[i];
		CVehicleWheel *wheel = addWheel( wheelmodelname.c_str(), setup, vWheelPos[i], vScale );
		wheel->node->setMaterialTexture(0, IRR.video->getTexture( wheeltexname.c_str() ));
	}

	endVehicleConstruction();
}