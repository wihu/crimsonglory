#include "biplane.h"
// IRR.
#include "../irrlicht/IrrlichtTask.h"
// CONSOLE.
#include "../IrrConsole/console.h"
#include "../IrrConsole/console_vars.h"

#include "../Newton/CustomUniversal.h"
#include "../Newton/CustomHinge.h"


////////////////////////////////////////////
// CBiplane 
////////////////////////////////////////////

CBiplane::CBiplane( const c8* scriptFilename )
{
	CBiplane::Reset();

	Load( scriptFilename );

	if ( (!node)||(!body) )
		bInvalidEntity = true;

	configFilename = scriptFilename;

	setDebugText( scriptFilename );

	// set custom Newton data
	//CONSOLE.addx("CBiplane %i",  this);
//    NewtonBodySetUserData( body, this );
	//NewtonBodySetTransformCallback (body, CBiplane::PhysicsSetTransform);
	//NewtonBodySetForceAndTorqueCallback(body, CBiplane::PhysicsApplyForceAndTorque);

	NewtonBodySetLinearDamping( body, fLinDamp );
	vector3df vAngDamp( fAngDamp, fAngDamp, fAngDamp );
	NewtonBodySetAngularDamping( body, &vAngDamp.X );

	setRotation( vector3df(0, ANGLE_FACE_LEFT, 0) );
}

CBiplane::~CBiplane()
{
}

void CBiplane::Reset()
{
	CVehicle::Reset();

	vAxisRotation.X = 180;
	iRotTurn = 0;
	iRotStep = 0;
	vAxisRotation.Y = ANGLE_FACE_LEFT;
}

void CBiplane::Think()
{
	CVehicle::Think();

	// update the debug position
	setDebugPos( IRR.smgr->getSceneCollisionManager()->getScreenCoordinatesFrom3DPosition( getNodeCenter(), IRR.smgr->getActiveCamera() ) );

	if ( control )
	{
		GetWheel(0)->setTorque(0.0f);
		GetWheel(1)->setTorque(0.0f);
		GetWheel(0)->setBrakes(0.0f);
		GetWheel(1)->setBrakes(0.0f);
		GetWheel(2)->setBrakes(0.0f);
		vOldAxisRotation.X = vAxisRotation.X;
		vOldAxisRotation.Y = vAxisRotation.Y;

		// flip
		if ( iRotTurn == 1)
		{
			vAxisRotation.X += fRotationRate;
			iRotStep += 1;

			if ( iRotStep > iRotStepsNum-1 )
			{
				iRotTurn = 2;
				iRotStep = 0;
			}
		}

		if ( iRotTurn == 3)
		{
			vAxisRotation.X -= fRotationRate;
			iRotStep += 1;

			if ( iRotStep > iRotStepsNum-1 )
			{
				iRotTurn = 0;
				iRotStep = 0;
			}
		}

		if ( control->KeyPressed( irr::KEY_KEY_Q ) ) // detach all
		{
			Zombify();
			return;

			//delete joint_wing2;
			//joint_wing2 = NULL;
			//delete joint_wing;
			//joint_wing = NULL;
			//delete joint_tail;
			//joint_tail = NULL;

			//p_wing->bFix2DRot = false;
			//p_wing2->bFix2DRot = false;
			//p_tail->bFix2DRot = false;
		}
		//if ( control->KeyPressed( irr::KEY_KEY_E ) ) // detach tyres
		//{
		//	int wheelid = 0;
		//	for (wheelid = NewtonVehicleGetFirstTireID(vehicle); wheelid; wheelid = NewtonVehicleGetNextTireID(vehicle, wheelid))
		//	{
		//		NewtonVehicleRemoveTire( vehicle, wheelid );
		//		break;
		//	}
		//}
		if ( control->KeyPressed( irr::KEY_KEY_X ) ) // break
		{
			GetWheel(0)->setBrakes(fBrake);
			GetWheel(1)->setBrakes(fBrake);
		}
		if ( control->KeyPressed( irr::KEY_KEY_A ) ) // go right
		{
			if ( vAxisRotation.Y < ANGLE_FACE_RIGHT )
				vAxisRotation.Y += fRotationRate;
		}
		if ( control->KeyPressed( irr::KEY_KEY_D ) ) // go left
		{
			if ( vAxisRotation.Y > ANGLE_FACE_LEFT )
				vAxisRotation.Y -= fRotationRate;
		}
		if ( control->KeyPressed( irr::KEY_SPACE ) ) // flip
		{
			float fA = getRotation().X;
			int k = (int) fA % 180;
			float angle;
			if ( k != 0 )
			{
				fA -= k;
			}
			//CONSOLE.addx("getRotation %f", fA);

			if ( iRotStep == 0 )
			{
				if ( iRotTurn == 0 )
				{
					fInitXAxisRotation = fA;
					vAxisRotation.X = fInitXAxisRotation;				
					iRotTurn = 1;
					iRotStep = 0;
					iRotStepsNum = (int) 180 / fRotationRate;
				}
				if ( iRotTurn == 2 )
				{
					fInitXAxisRotation = fA;
					vAxisRotation.X = fInitXAxisRotation;				
					iRotTurn = 3;
					iRotStep = 0;
					iRotStepsNum = (int) 180 / fRotationRate;
				}
			}
			
		}

		if ( ( vOldAxisRotation.X != vAxisRotation.X ) || ( vOldAxisRotation.Y != vAxisRotation.Y ) )
		{
			setRotation( vector3df(vAxisRotation.X, vAxisRotation.Y, 0), true, false, false );
		}
		else
			setRotation( vector3df(vAxisRotation.X, 0, 0), true, false, false);

	}

}

void CBiplane::Render()
{
	CVehicle::Render();

	IRR.video->setTransform(ETS_WORLD, matrix4());

	SMaterial m; 
	m.Lighting = false; 
	m.BackfaceCulling = false;
	IRR.video->setMaterial(m);

	// draw main axis line
	vector3df vP1, vP2;
	vP1 = getPosition();
	vP2 = vP1 + vNMainAxis * 5.0f;
	IRR.video->draw3DLine(vP1,	vP2,  SColor(0,125,255,1));
	vP2 = vP1 + vNMainAxisPerp * 5.0f;
	IRR.video->draw3DLine(vP1,	vP2,  SColor(0,125,255,1));

	// draw main axis perp velocity
	vP2 = vP1 + vAxisVelocityPerp;
	IRR.video->draw3DLine(vP1,	vP2,  SColor(0,255,0,0));

	// draw main axis velocity
	vP2 = vP1 + vAxisVelocity;
	IRR.video->draw3DLine(vP1,	vP2,  SColor(0,10,10,255));

	// velocity
	vP2 = vP1 + vVelocity;
	IRR.video->draw3DLine(vP1,	vP2,  SColor(0,0,255,255));
}

void CBiplane::Load( const c8* filename )
{
	String modelname, texname;
	vector3df vScale;
	f32 fMass;
	int iBody;
	String wheelmodelname[BIPLANE_WHEELS], wheeltexturename[BIPLANE_WHEELS];
	vector3df vWheelPos[BIPLANE_WHEELS];
	f32 fWheelMass[BIPLANE_WHEELS];
	f32 fWheelShock[BIPLANE_WHEELS];
	f32 fWheelSpring[BIPLANE_WHEELS];
	f32 fWheelLength[BIPLANE_WHEELS];

	// load script file
	SCRIPT.RunFile( filename );
		
	GMVARTOSTRING( "biplane_modelname", modelname );  

	GMVARTOSTRING( "biplane_texturename", texname ); 
    
  	GMVARTOVALUE( "biplane_modelscale_x", f32, vScale.X );
  	GMVARTOVALUE( "biplane_modelscale_y", f32, vScale.Y );
  	GMVARTOVALUE( "biplane_modelscale_z", f32, vScale.Z );  	

	GMVARTOVALUE( "biplane_lineardamping", f32, fLinDamp );	
	GMVARTOVALUE( "biplane_angulardamping", f32, fAngDamp );	

   	GMVARTOVALUE( "biplane_mass", f32, fMass );	
  	GMVARTOVALUE( "biplane_bodytype", int, iBody );
  	
	GMVARTOVALUE( "biplane_enginepower", f32, fPower );	
	GMVARTOVALUE( "biplane_wingsliftpower", f32, fWingsLift );	
	GMVARTOVALUE( "biplane_wingsresistfactor", f32, fWingsResist );	
	GMVARTOVALUE( "biplane_steerpower", f32, fSteerPower );	
	GMVARTOVALUE( "biplane_stabilizepower", f32, fStabilizePower );	
	GMVARTOVALUE( "biplane_planeresistfactor", f32, fPlaneResist );	

	GMVARTOVALUE( "biplane_brake", f32, fBrake );	  
	GMVARTOVALUE( "biplane_rotationrate", f32, fRotationRate );	

  	GMVARTOVALUE( "biplane_mountpos_x", f32, vMountPos.X );
  	GMVARTOVALUE( "biplane_mountpos_y", f32, vMountPos.Y );
  	GMVARTOVALUE( "biplane_mountpos_z", f32, vMountPos.Z ); 

	assembleVehiclePhysics( modelname.c_str(), (BodyType) iBody, vScale, fMass );
	node->setMaterialTexture(0, IRR.video->getTexture( texname.c_str() ));

	// set wings, tail etc.  
	// I have no idea why this factor must be here
	//const float factor = 1.6875f;
	//dVector pivot, pin0;
	//p_wing = new CNewtonNode( "Models/sopwith/wing.ms3d", BODY_HULL, vScale, fMass / 100.0f);
	//p_wing->setPosition( vector3df(0.5, -5.4, 0) * NewtonToIrr / factor );
	//pivot = dVector(0.0, 0, 0);
	//pin0 = dVector(0, 1, 0);
	//joint_wing = new CustomHinge( pivot, pin0, p_wing->body, this->body );

	//p_wing2 = new CNewtonNode( "Models/sopwith/wing.ms3d", BODY_HULL, vScale, fMass / 100.0f);
	//p_wing2->setPosition( vector3df(0.5, 0.2, 0) * NewtonToIrr / factor );
	//pivot = dVector(0.0, 0, 0);
	//pin0 = dVector(0, 1, 0);
	//joint_wing2 = new CustomHinge( pivot, pin0, p_wing2->body, this->body );

	//p_tail = new CNewtonNode( "Models/sopwith/tail.ms3d", BODY_HULL, vScale, fMass / 100.0f);
	//p_tail->setPosition( vector3df(15.9, -2.7, 0) * NewtonToIrr / factor );
	//pivot = dVector(0.0, 0, 0);
	//pin0 = dVector(0, 1, 0);
	//joint_tail = new CustomHinge( pivot, pin0, p_tail->body, this->body );

	//p_wing->bFix2DRot = false;
	//p_wing2->bFix2DRot = false;
	//p_tail->bFix2DRot = false;


	// set tyres

	char buffer [50];
	tireSetup setup;
	for ( int i = 0; i < BIPLANE_WHEELS; i++ )
	{
		sprintf(buffer, "biplane_wheel%i_modelname", i+1);
		GMVARTOSTRING( buffer, wheelmodelname[i] ); 
		sprintf(buffer, "biplane_wheel%i_texturename", i+1);
		GMVARTOSTRING( buffer, wheeltexturename[i] ); 
		sprintf(buffer, "biplane_wheel%i_mass", i+1);
		GMVARTOVALUE( buffer, f32, fWheelMass[i] ); 
		sprintf(buffer, "biplane_wheel%i_position_x", i+1);
		GMVARTOVALUE( buffer, f32, vWheelPos[i].X ); 
		sprintf(buffer, "biplane_wheel%i_position_y", i+1);
		GMVARTOVALUE( buffer, f32, vWheelPos[i].Y ); 
		sprintf(buffer, "biplane_wheel%i_position_z", i+1);
		GMVARTOVALUE( buffer, f32, vWheelPos[i].Z ); 		
		sprintf(buffer, "biplane_wheel%i_suspensionshock", i+1);
		GMVARTOVALUE( buffer, f32, fWheelShock[i] ); 	
		sprintf(buffer, "biplane_wheel%i_suspensionspring", i+1);
		GMVARTOVALUE( buffer, f32, fWheelSpring[i] ); 	
		sprintf(buffer, "biplane_wheel%i_suspensionlength", i+1);
		GMVARTOVALUE( buffer, f32, fWheelLength[i] );

		setup.Mass = fWheelMass[i];
		setup.SuspensionShock = fWheelShock[i];
		setup.SuspensionSpring = fWheelSpring[i];
		setup.SuspensionLength = fWheelLength[i];
		CVehicleWheel *wheel = addWheel( wheelmodelname[i].c_str(), setup, vWheelPos[i], vScale );
		wheel->node->setMaterialTexture(0, IRR.video->getTexture( wheeltexturename[i].c_str() ));
	}

	endVehicleConstruction();	
}

void CBiplane::PhysicsControl()
{
	// calculate the main plane axis (line from tail to engine)
	matrix4 mat;
	NewtonBodyGetMatrix( body, &mat.M[0] ); 

	vNMainAxis = vector3df( -1.0f, 0.0f, 0.0f );
	mat.rotateVect( vNMainAxis );

	vNMainAxisPerp = vector3df( 0.0f, -1.0f, 0.0f );
	mat.rotateVect( vNMainAxisPerp );

	// calculate velocity
	vector3df vWingVelocity = WORLD.GetPhysics()->getPointVelocity( body, vector3df(0.1f, 3.0f, 0.0f) );
	vAxisVelocity = vWingVelocity * vNMainAxis;
	vAxisVelocityPerp = vWingVelocity * vNMainAxisPerp;

	vector3df forcePoint;

	// wings lift force
	//forcePoint = vector3df( -1.8f, -0.3f, 0.0f ); 
	vector3df vWingsLift;
	vWingsLift = vNMainAxisPerp * getWingsLiftPower() * vAxisVelocity.getLength() ;
	NewtonBodyAddForce(body, &vWingsLift.X);
	
	// wings resistance force
	vector3df vWingsResist;
	vWingsResist = vAxisVelocityPerp * getWingsResistFactor();
	if ( vVelocity.X > 0 )
		vWingsResist.X = -abs( vWingsResist.X );
	else
		vWingsResist.X = abs( vWingsResist.X );
	if ( vVelocity.Y > 0 )
		vWingsResist.Y = -abs( vWingsResist.Y );
	else
		vWingsResist.Y = abs( vWingsResist.Y );
	NewtonBodyAddForce(body, &vWingsResist.X);	
	//WORLD.GetPhysics()->addLocalForce( body, vWingsResist, forcePoint );

	// drag force
	vector3df vPlaneResist;
	vPlaneResist = -vVelocity  * getPlaneResistFactor();
	NewtonBodyAddForce(body, &vPlaneResist.X);

	if ( control )
	{
		int face;
		if ( iRotTurn == 2 )
			face = 1;
		else
			face = -1;

		if ( getControls()->KeyPressed( irr::KEY_SHIFT ) ) // engine
		{
			forcePoint = vector3df( -3.5f, 0.1f, 0.0f ); 
			
			vector3df vThrustMain = vNMainAxis;
			vThrustMain *= getEnginePower();
			vector3df vTorqueBack( 0.0f, 0.0f, getStabilizePower() *face * getEnginePower() );
			//vector3df vThrustMain = vector3df( getEnginePower(), 0.0f, 0.0f);

			//WORLD.GetPhysics()->addLocalForce( body, vThrustMain, forcePoint );
			NewtonBodyAddForce(body, &vThrustMain.X);
			NewtonBodySetTorque( body, &vTorqueBack.X);
		}

		if ( getControls()->KeyPressed( irr::KEY_KEY_S ) ) // tail up
		{
			forcePoint = vector3df( 16.5f, 2.1f, 0.0f ); 
			vector3df vSteerForce( 0.0f, 0.0f, 7.0f  * face * getSteerPower() );
			//WORLD.GetPhysics()->addLocalForce( body, vSteerForce, forcePoint );
			NewtonBodySetTorque( body, &vSteerForce.X);
		}
		if ( getControls()->KeyPressed( irr::KEY_KEY_W ) ) // tail down
		{
			forcePoint = vector3df( 16.5f, 2.1f, 0.0f ); 
			//vector3df vSteerForce( 0.0f, 0.0f, vAxisVelocity.getLength() * -face * getSteerPower() );
			vector3df vSteerForce( 0.0f, 0.0f, 7.0f * -face * getSteerPower() );
			//WORLD.GetPhysics()->addLocalForce( body, vSteerForce, forcePoint );
			NewtonBodySetTorque( body, &vSteerForce.X);
		}

	}

	vAxisVelocity = vWingsLift / 25;
	vAxisVelocityPerp = vWingsResist / 25;

//	p_wing->setPosition( mat.getTranslation() + vector3df(0, -5.4f * NewtonToIrr, 0)  );

	// set rotation of jointed parts
	//if (bZombie)
	//{
	//	p_wing->setPosition( mat.getTranslation() );
	//	p_wing2->setPosition( mat.getTranslation() );
	//	p_tail->setPosition( mat.getTranslation() );
	//if (joint_wing)
	//	p_wing->setRotation( mat.getRotationDegrees() );
	//if (joint_wing2)
	//	p_wing2->setRotation( mat.getRotationDegrees() );
	//if (joint_tail)
	//	p_tail->setRotation( mat.getRotationDegrees() );
	//}
}

void CBiplane::LoadZombie( matrix4 matrix )
{
	String modelnamebody, modelnamewing, modelnametail, texname;
	vector3df vScale, vWingPos, vWing2Pos, vTailPos;
	f32 fMass;
	int iBody;
	String wheelmodelname[BIPLANE_WHEELS], wheeltexturename[BIPLANE_WHEELS];
	vector3df vWheelPos[BIPLANE_WHEELS];
	f32 fWheelMass[BIPLANE_WHEELS];

    GMVARTOSTRING( "biplane_modelname_body", modelnamebody );   
	GMVARTOSTRING( "biplane_texturename", texname ); 
    GMVARTOSTRING( "biplane_modelname_wing", modelnamewing );   
    GMVARTOSTRING( "biplane_modelname_tail", modelnametail );   

  	GMVARTOVALUE( "biplane_modelscale_x", f32, vScale.X );
  	GMVARTOVALUE( "biplane_modelscale_y", f32, vScale.Y );
  	GMVARTOVALUE( "biplane_modelscale_z", f32, vScale.Z );  	
    
  	GMVARTOVALUE( "biplane_wing_position_x", f32, vWingPos.X );
  	GMVARTOVALUE( "biplane_wing_position_y", f32, vWingPos.Y );
  	GMVARTOVALUE( "biplane_wing_position_z", f32, vWingPos.Z );  	
  	GMVARTOVALUE( "biplane_wing2_position_x", f32, vWing2Pos.X );
  	GMVARTOVALUE( "biplane_wing2_position_y", f32, vWing2Pos.Y );
  	GMVARTOVALUE( "biplane_wing2_position_z", f32, vWing2Pos.Z );  
  	GMVARTOVALUE( "biplane_tail_position_x", f32, vTailPos.X );
  	GMVARTOVALUE( "biplane_tail_position_y", f32, vTailPos.Y );
  	GMVARTOVALUE( "biplane_tail_position_z", f32, vTailPos.Z );  

   	GMVARTOVALUE( "biplane_mass", f32, fMass );	
  	GMVARTOVALUE( "biplane_bodytype", int, iBody );

	assemblePhysics( modelnamebody.c_str(), (BodyType) iBody, vScale, fMass );
	node->setMaterialTexture(0, IRR.video->getTexture( texname.c_str() ));

	NewtonBodySetMatrix( body, &matrix.M[0] );
	NewtonBodySetVelocity( body, &vVelocity.X );
	NewtonBodySetOmega( body, &vOmega.X );

	// set wings, tail etc.  
	// I have no idea why this factor must be here
	vector3df vP;
	const float factor = 0.67;
	dVector pivot, pin0;
    matrix4 mat;

	// wing part	
	p_wing = new CNewtonNode( modelnamewing.c_str(), BODY_HULL, vScale, fMass / 100.0f);
	vP = vWingPos;
	matrix.rotateVect( vP );
    p_wing->setPosition( matrix.getTranslation() + vP * NewtonToIrr / factor );

	NewtonBodySetVelocity( p_wing->body, &vVelocity.X );
	NewtonBodySetOmega( p_wing->body, &vOmega.X );
	NewtonBodyGetMatrix( p_wing->body, &mat.M[0] ); 
	p_wing->PhysicsSetTransform( p_wing->body, &mat.M[0] );

	// wing2 part
	p_wing2 = new CNewtonNode(  modelnamewing.c_str(), BODY_HULL, vScale, fMass / 100.0f);
	vP = vWing2Pos;
	matrix.rotateVect( vP );
	p_wing2->setPosition( matrix.getTranslation() + vP * NewtonToIrr / factor );

	NewtonBodySetVelocity( p_wing2->body, &vVelocity.X );
	NewtonBodySetOmega( p_wing2->body, &vOmega.X );
	NewtonBodyGetMatrix( p_wing2->body, &mat.M[0] ); 
	p_wing->PhysicsSetTransform( p_wing2->body, &mat.M[0] );

	// tail part
	p_tail = new CNewtonNode(  modelnametail.c_str(), BODY_HULL, vScale, fMass / 100.0f);
	vP = vTailPos;
	matrix.rotateVect( vP );
	p_tail->setPosition( matrix.getTranslation() + vP * NewtonToIrr / factor );

	NewtonBodySetVelocity( p_tail->body, &vVelocity.X );
	NewtonBodySetOmega( p_tail->body, &vOmega.X );
	NewtonBodyGetMatrix( p_tail->body, &mat.M[0] ); 
	p_wing->PhysicsSetTransform( p_tail->body, &mat.M[0] );

	// wheels
	char buffer [50];
	tireSetup setup;
	for ( int i = 0; i < BIPLANE_WHEELS; i++ )
	{
		sprintf(buffer, "biplane_wheel%i_modelname", i+1);
		GMVARTOSTRING( buffer, wheelmodelname[i] ); 
		sprintf(buffer, "biplane_wheel%i_texturename", i+1);
		GMVARTOSTRING( buffer, wheeltexturename[i] ); 
		sprintf(buffer, "biplane_wheel%i_mass", i+1);
		GMVARTOVALUE( buffer, f32, fWheelMass[i] ); 
		sprintf(buffer, "biplane_wheel%i_position_x", i+1);
		GMVARTOVALUE( buffer, f32, vWheelPos[i].X ); 
		sprintf(buffer, "biplane_wheel%i_position_y", i+1);
		GMVARTOVALUE( buffer, f32, vWheelPos[i].Y ); 
		sprintf(buffer, "biplane_wheel%i_position_z", i+1);
		GMVARTOVALUE( buffer, f32, vWheelPos[i].Z ); 		

		p_wheels[i] = new CNewtonNode( wheelmodelname[i].c_str(), BODY_HULL, vScale, fWheelMass[i] );
		p_wheels[i]->node->setMaterialTexture(0, IRR.video->getTexture( wheeltexturename[i].c_str() ));

		matrix.rotateVect( vWheelPos[i] );
		p_wheels[i]->setPosition( matrix.getTranslation() + vWheelPos[i] );

		NewtonBodySetVelocity( p_wheels[i]->body, &vVelocity.X );
		NewtonBodySetOmega( p_wheels[i]->body, &vOmega.X );
		NewtonBodyGetMatrix( p_wheels[i]->body, &mat.M[0] ); 
		p_wheels[i]->PhysicsSetTransform( p_wheels[i]->body, &mat.M[0] );
		p_wheels[i]->bFix2DRot = false;
	}


	//pivot = dVector(0.0, 0, 0);
	//pin0 = dVector(0, 1, 0);
	//joint_tail = new CustomHinge( pivot, pin0, p_tail->body, this->body );

	vector3df vAngDamp( 0.01f, 0.01f, 0.01f );
	NewtonBodySetAngularDamping( body, &vAngDamp.X );

	bFix2DRot = false;
	p_wing->bFix2DRot = false;
	p_wing2->bFix2DRot = false;
	p_tail->bFix2DRot = false;
}

void CBiplane::Zombify()
{
	if (bZombie)
		return;

    matrix4 mat;
	NewtonBodyGetMatrix( body, &mat.M[0] ); 

	NewtonDestroyJoint( WORLD.GetPhysics()->nWorld, vehicle );
	NewtonDestroyBody( WORLD.GetPhysics()->nWorld, body );
	if ( IRR.smgr )
		node->remove();
 	for ( int i=0; i < wheels.size(); i++)
		delete wheels[i];
	wheels.clear();
	vehicle = NULL;

	bZombie = true;
	LoadZombie( mat );

	control = NULL;
}

