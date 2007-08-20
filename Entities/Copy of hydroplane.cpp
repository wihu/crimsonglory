#include "hydroplane.h"

////////////////////////////////////////////
// CHydroplane 
////////////////////////////////////////////

#define WINDSOUND_FACTOR 60.0f

bool bRegistered_CHydroplane = FACTORY->Actors.Register<CHydroplane>( "hydroplane" );

CHydroplane::CHydroplane( const c8* scriptFilename ) : CMachine( scriptFilename )
{
	Reset();

	factoryName = "hydroplane";

	if ( scriptFilename != "" ) // this is server
	{
		Load( scriptFilename );
		Unserialize( loadedBitStream );
		//attach weapon
		LoadAttachment( "scriptweapon", WeaponScriptName.c_str() );
	}

	engineSound = new CSoundEntity( "Sounds/machine_steady_fan_low.wav", this );
	engineSound->setLooping( true );
	engineSound->setVolume(1.0f);
	windSound = new CSoundEntity( "Sounds/wind_control_room3_loop.wav", this );
	windSound->setLooping( true );
	windSound->setVolume(0.0f);
	windSound->play();
}

CHydroplane::~CHydroplane()
{
}

void CHydroplane::Reset()
{
	CMachine::Reset();

	iRotTurn = 0;
	iRotStep = 0;
}

void CHydroplane::Think()
{
	CMachine::Think();

	propellerSprite->Update();

	if (bZombie)
		return;

	if ( control )
	{
		vOldAxisRotation = vAxisRotation;

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
			vAxisRotation.X += fRotationRate;
			iRotStep += 1;

			if ( iRotStep > iRotStepsNum-1 )
			{
				iRotTurn = 0;
				iRotStep = 0;
			}
		}

		// set flip rotation
		setRotation( vector3df(vAxisRotation.X, 0, 0), true, false, false);

		// get keys

		if ( control->ActionKeyPressed( AK_BRAKE ) ) // detach all
		{
			bZombie = true;
			return;
		}
		//if ( control->ActionKeyPressed( AK_MOVE_RIGHT ) ) // go right
		//{
		//	if ( vAxisRotation.Y < ANGLE_FACE_RIGHT )
		//		vAxisRotation.Y += fRotationRate;
		//}
		//if ( control->ActionKeyPressed( AK_MOVE_LEFT ) ) // go left
		//{
		//	if ( vAxisRotation.Y > ANGLE_FACE_LEFT )
		//		vAxisRotation.Y -= fRotationRate;
		//}
		if ( control->ActionKeyPressed( AK_FLIP ) ) // flip
		{
			if ( iRotStep == 0 )
			{
				float fA = getRotation().X;
				//CONSOLE.addx("AK_FLIP %f", fA);
				//int k = (int) fA % 180;
				//float angle;
				//if ( k != 0 )
				//{
				//	fA -= k;
				//}
				//CONSOLE.addx("getRotation %f", fA);
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


	}
	else
	{
		if (!isInWater())
		{
			NewtonBodySetLinearDamping( body, 0.000000001f );
			vector3df vAng(0.00001f, 0.00001f, 0.00001f );
			NewtonBodySetAngularDamping( body, &vAng.X );
		}
		engineSound->pause();
	}

	windSound->setVolume(fVelocity / WINDSOUND_FACTOR );

}

void CHydroplane::Render()
{
	CMachine::Render();

	if ( APP.DebugMode )
	{
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

		//text
		IRR.gui->getBuiltInFont()->draw( L"Hydroplane: ",
											core::rect<s32>( 10, IRR.getScreenHeight() - 406, IRR.getScreenWidth(),
															IRR.getScreenHeight() ), irr::video::SColor( 255, 165, 15, 65 ), false, true );

		WideString sVel = "Velocity: ";
		sVel += f2wchar( vVelocity.getLength() );
		IRR.gui->getBuiltInFont()->draw( sVel.c_str(),
											core::rect<s32>( 10, IRR.getScreenHeight() - 392, IRR.getScreenWidth(),
															IRR.getScreenHeight() ), irr::video::SColor( 255, 165, 15, 65 ), false, true );
		sVel = "Position: [ "; sVel += f2wchar( getPosition().X );
		sVel += "  ,  "; sVel += f2wchar( getPosition().Y ); sVel += " ]";
		IRR.gui->getBuiltInFont()->draw( sVel.c_str(),
											core::rect<s32>( 10, IRR.getScreenHeight() - 378, IRR.getScreenWidth(),
															IRR.getScreenHeight() ), irr::video::SColor( 255, 165, 15, 65 ), false, true );
	}
}

void CHydroplane::PhysicsControl()
{
	if (bInvalidEntity)
		return;

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
	//NewtonBodyAddForce(body, &vPlaneResist.X);

	if ( control )
	{
		matrix4 rotMat;
		rotMat.setRotationDegrees( vAxisRotation );

		if ( getControls()->ActionKeyPressed( AK_THRUST ) ) // engine
		{
			vector3df vThrustMain = vNMainAxis;
			vThrustMain *= getEnginePower();
			vector3df vTorqueBack( 0.0f, 0.0f, getStabilizePower() * getEnginePower() );
			rotMat.transformVect( vTorqueBack );
			//vector3df vThrustMain = vector3df( getEnginePower(), 0.0f, 0.0f);

			//WORLD.GetPhysics()->addLocalForce( body, vThrustMain, forcePoint );
			NewtonBodyAddForce(body, &vThrustMain.X);
			NewtonBodySetTorque( body, &vTorqueBack.X);

			if (KERNEL.GetTicks() % 10 == 0)
			{
				//FACTORY->Effects.Create( "beam", vLastTrailPos, getPosition(), 0.0f, 0.9f, 60 );
				vLastTrailPos = getPosition();
			}

			if ( !engineSound->isPlaying() )
				engineSound->play();
		}
		else
		{
			vLastTrailPos = getPosition();
			if (!isInWater())
			{
				NewtonBodySetLinearDamping( body, 0.000000001f );
				vector3df vAng(0.00001f, 0.00001f, 0.00001f );
				NewtonBodySetAngularDamping( body, &vAng.X );
			}

			engineSound->pause();
		}

		if ( getControls()->ActionKeyPressed( AK_MOVE_DOWN ) ) // tail up
		{
			f32 steerFactor = 5.0f + vAxisVelocity.getLength();
			vector3df vSteerForce( 0.0f, 0.0f, steerFactor * getSteerPower() );
			rotMat.transformVect( vSteerForce );
			NewtonBodySetTorque( body, &vSteerForce.X);
		}
		if ( getControls()->ActionKeyPressed( AK_MOVE_UP ) ) // tail down
		{
			f32 steerFactor = 5.0f + vAxisVelocity.getLength();
			vector3df vSteerForce( 0.0f, 0.0f, - steerFactor * getSteerPower() );
			rotMat.transformVect( vSteerForce );
			NewtonBodySetTorque( body, &vSteerForce.X);
		}

	}

	// for debug render
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

void CHydroplane::Load( const c8* filenamep )
{
	// load script file
	SCRIPT.RunFile( filenamep );
		
	LOAD_STRING( "hydroplane_modelname" );
	LOAD_STRING( "hydroplane_texturename" );
	LOAD_STRING( "hydroplane_weaponscript" );
	//LOAD_STRING( "hydroplane_propeller_sprite" );
	//LOAD_VALUE( "hydroplane_propeller_sprite_size", f32 );
	LOAD_VECTOR( "hydroplane_modelscale_x", "hydroplane_modelscale_y", "hydroplane_modelscale_z" );
	LOAD_VECTOR( "hydroplane_propeller_position_x", "hydroplane_propeller_position_y", "hydroplane_propeller_position_z" );
	LOAD_VALUE( "hydroplane_propellerscale", f32 );
	LOAD_VALUE( "hydroplane_lineardamping", f32 );
	LOAD_VALUE( "hydroplane_angulardamping", f32 );
	LOAD_VALUE( "hydroplane_mass", f32 );
	LOAD_VALUE( "hydroplane_bodytype", int );
	LOAD_VALUE( "hydroplane_proptimeout", int );
	LOAD_VALUE( "hydroplane_maxpropsplits", int );
	LOAD_VALUE( "hydroplane_splitfactor", f32 );
	LOAD_VALUE( "hydroplane_enginepower", f32 );
	LOAD_VALUE( "hydroplane_wingsliftpower", f32 );
	LOAD_VALUE( "hydroplane_wingsresistfactor", f32 );
	LOAD_VALUE( "hydroplane_steerpower", f32 );
	LOAD_VALUE( "hydroplane_stabilizepower", f32 );
	LOAD_VALUE( "hydroplane_planeresistfactor", f32 );
	LOAD_VALUE( "hydroplane_brake", f32 );
	LOAD_VALUE( "hydroplane_rotationrate", f32 );
	LOAD_VECTOR( "hydroplane_mountpos_x", "hydroplane_mountpos_y", "hydroplane_mountpos_z" );
	LOAD_VECTOR( "hydroplane_gunmountpos_x", "hydroplane_gunmountpos_y", "hydroplane_gunmountpos_z" );
	LOAD_VECTOR( "hydroplane_gunmountrot_x", "hydroplane_gunmountrot_y", "hydroplane_gunmountrot_z" );
}

void CHydroplane::Unserialize( RakNet::BitStream &bt )
{
	vector3df vScale, vMPos, vMPos2, vMRot, vPropPos;
	f32 fMass, fPropScale;
	int PropFrameSize;
	int stringLength = 0;
	char* inString = NULL;

	UNLOAD_STRING
	String modelname = inString; delete[] inString; inString = NULL;
	UNLOAD_STRING
	String texname = inString;	delete[] inString; inString = NULL;
	UNLOAD_STRING
	WeaponScriptName = inString;	delete[] inString; inString = NULL;
	//UNLOAD_STRING
	//String PropellerName = inString;	delete[] inString; inString = NULL;
	//bt.Read( PropFrameSize );
	bt.ReadVector( vScale.X, vScale.Y, vScale.Z );
	bt.ReadVector( vPropPos.X, vPropPos.Y, vPropPos.Z );
	bt.Read( fPropScale );
	bt.Read( fLinDamp );	
	bt.Read( fAngDamp );
	bt.Read( fMass );
	bt.Read( bodyType );
	bt.Read( propTimeout );
	bt.Read( maxSplits );
	bt.Read( fSplitFactor );
	bt.Read( fPower );
	bt.Read( fWingsLift );
	bt.Read( fWingsResist );
	bt.Read( fSteerPower );
	bt.Read( fStabilizePower );
	bt.Read( fPlaneResist );
	bt.Read( fBrake );
	bt.Read( fRotationRate );
	bt.ReadVector( vMPos.X, vMPos.Y, vMPos.Z );
	bt.ReadVector( vMPos2.X, vMPos2.Y, vMPos2.Z );
	bt.ReadVector( vMRot.X, vMRot.Y, vMRot.Z );

	assemblePhysics( modelname.c_str(), (BodyType) bodyType, vScale, fMass );

	if ( (!node)||(!body) )
	{
		bInvalidEntity = true;
		return;
	}

	addMountPlace( vMPos, vector3df(0,0,0), NODECLASS_CHARACTER );
	addMountPlace( vMPos2, vMRot, NODECLASS_WEAPON);

	node->setMaterialTexture(0, IRR.video->getTexture( APP.useFile( texname.c_str() ).c_str() ));

	NewtonBodySetLinearDamping( body, fLinDamp );
	vAngDamp = vector3df( fAngDamp, fAngDamp, fAngDamp );
	NewtonBodySetAngularDamping( body, &vAngDamp.X );

	setDirection( ANGLE_FACE_LEFT );

	//propeller
	//propellerSprite = new CAnimSpriteSceneNode( node, IRR.smgr, -1); 
	//propellerSprite->Load( PropellerName.c_str(), 0,0,PropFrameSize,PropFrameSize*13,PropFrameSize,PropFrameSize,true); 
	//propellerSprite->setSpeed(50);
 //   propellerSprite->setPosition(vPropPos);
 //   propellerSprite->setRotation(vector3df(0,90,0));   
 //   propellerSprite->setScale(vector3df(fPropScale,fPropScale,fPropScale));  
	//propellerSprite->setMaterialType(video::EMT_TRANSPARENT_ALPHA_CHANNEL);
}


