#include "hydroplane.h"
#include "../World/calc.h"


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
    engineSound->setVolume( 1.0f );
    windSound = new CSoundEntity( "Sounds/wind_control_room3_loop.wav", this );
    windSound->setLooping( true );
    windSound->setVolume( 0.0f );
    windSound->play();

    //CONSOLE.add( APP.useFile( "Sounds/wind_control_room3_loop.wav" ).c_str() );
    //CONSOLE.add( "asdasd" );
}

CHydroplane::~CHydroplane()
{
    //  CONSOLE.add( "~CHydroplane" );
    if ( IRR.smgr )
    {
      wake->remove();
	  propellerSprite->remove();
    }
}

void CHydroplane::Reset()
{
    CMachine::Reset();

    iRotTurn = 0;
    iRotStep = 0;
    fThrustValue = 0.0f;
    wakeActiveCounter = 1.0f;
}

void CHydroplane::Think()
{
    CMachine::Think();

    if ( bZombie )
    {
      return;
    }

    propellerSprite->Update();
	for (int i=0; i < 8; i++)
	{
		if (fThrustValue == 0.0f)
			exhaustSprite[i]->setVisible( false );
		else
		{
			exhaustSprite[i]->setVisible( true );
			exhaustSprite[i]->Update();
		}
	}

   // vector3df vNewRot = vPropPos;
    //transformMatrix.rotateVect( vNewRot );
  //  propellerSprite->setPosition( getPosition() + vNewRot );
   // propellerSprite->setRotation( vector3df( vPropRot.X, vPropRot.Y + getRotation().Y, vPropRot.Z + getRotation().Z ) );

    // wake

    if ( wake->isVisible )
    {
      if ( vVelocity.X < 0.0f )
      {
        wake->setPosition( vector3df( getPosition().X - 3.0f, vWaterEntryPos.Y, 1.6f ) );
      }
      else
      {
        wake->setPosition( vector3df( getPosition().X + 3.0f, vWaterEntryPos.Y, 1.6f ) );
      }
    }

    f32 headingsign = vVelocity.X <0.0f ? -1.0f : vVelocity.X> 0.0f ? 1.0f : 0.0f;

    if ( ( isInWater() ) && ( !isUnderWater() ) )
    {
      if ( abs( vVelocity.X ) > 6.0f )
      {
        wake->setVisible( true );
      }
      else
      {
        wake->setVisible( false );
      }

      wake->setScale( vector3df( node->getScale().X * vVelocity.X / 20.0f, node->getScale().Y * vVelocity.X / -30.0f * -headingsign, node->getScale().Z * ( vVelocity.X / -18.0f ) * wakeActiveCounter ) );
      wakeFallCounter = vVelocity.X / 30.0f;
      lastWaterVelocity = vVelocity.X;
      if ( wakeActiveCounter <= 1.0f )
      {
        wakeActiveCounter *= 1.03f;
      }
    }
    else
    {
      if ( wakeActiveCounter >= 0.1f )
      {
        wakeActiveCounter *= 0.5f;
      }

      if ( abs( wakeFallCounter ) <= 0.2f )
      {
        wake->setVisible( false );
      }
      else
      {
        wakeFallCounter *= 0.98f;
        lastWaterVelocity *= 1.001f;

        wake->setScale( vector3df( node->getScale().X * lastWaterVelocity / 20.0f, node->getScale().Y * ( wakeFallCounter )*headingsign, node->getScale().Z * ( lastWaterVelocity / -18.0f ) ) );
      }
    }


    if ( control )
    {
      vOldAxisRotation = vAxisRotation;

      // flip
      if ( iRotTurn == 1 )
      {
        vAxisRotation.X += fRotationRate;
        iRotStep += 1;

        if ( iRotStep > iRotStepsNum - 1 )
        {
          iRotTurn = 2;
          iRotStep = 0;
        }
      }

      if ( iRotTurn == 3 )
      {
        vAxisRotation.X += fRotationRate;
        iRotStep += 1;

        if ( iRotStep > iRotStepsNum - 1 )
        {
          iRotTurn = 0;
          iRotStep = 0;
        }
      }

      // set flip rotation
      setRotation( vector3df( vAxisRotation.X, 0, 0 ), true, false, false );

      // get keys

      if ( getControls()->ActionKeyPressed( AK_THRUST ) ) // engine thrust
      {
        if ( !bThrustChange )
        {
          if ( fThrustValue == 0.0f )
          {
            fThrustValue = 1.0f;
          }
          else
          {
            fThrustValue = 0.0f;
          }
        }
        bThrustChange = true;
      }
      else
      {
        bThrustChange = false;
      }

      if ( control->ActionKeyPressed( AK_BRAKE ) ) // detach all
      {
        bZombie = true;
        return;
      }
      //if ( control->ActionKeyPressed( AK_MOVE_RIGHT ) ) // go right
      //{
      //    if ( vAxisRotation.Y < ANGLE_FACE_RIGHT )
      //        vAxisRotation.Y += fRotationRate;
      //}
      //if ( control->ActionKeyPressed( AK_MOVE_LEFT ) ) // go left
      //{
      //    if ( vAxisRotation.Y > ANGLE_FACE_LEFT )
      //        vAxisRotation.Y -= fRotationRate;
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
          //    fA -= k;
          //}
          //CONSOLE.addx("getRotation %f", fA);
          if ( iRotTurn == 0 )
          {
            fInitXAxisRotation = fA;
            vAxisRotation.X = fInitXAxisRotation;               
            iRotTurn = 1;
            iRotStep = 0;
            iRotStepsNum = ( int )180 / fRotationRate;
          }
          if ( iRotTurn == 2 )
          {
            fInitXAxisRotation = fA;
            vAxisRotation.X = fInitXAxisRotation;               
            iRotTurn = 3;
            iRotStep = 0;
            iRotStepsNum = ( int )180 / fRotationRate;
          }
        }
      }
    }
    else
    {
      if ( !isInWater() )
      {
        //NewtonBodySetLinearDamping( body, 0.00001f );
        //vector3df vAng(0.000001f, 0.000001f, 0.000001f );
        //NewtonBodySetAngularDamping( body, &vAng.X );
      }
      engineSound->pause();
    }

    windSound->setVolume( fVelocity / WINDSOUND_FACTOR );
}

void CHydroplane::Render()
{
    CMachine::Render();

    if ( APP.DebugMode )
    {
      IRR.video->setTransform( ETS_WORLD, matrix4() );

      SMaterial m; 
      m.Lighting = false; 
      m.BackfaceCulling = false;
      IRR.video->setMaterial( m );

      // draw main axis line
      vector3df vP1, vP2;
      vP1 = getPosition();
      vP2 = vP1 + vNMainAxis * 5.0f;
      IRR.video->draw3DLine( vP1, vP2, SColor( 0, 125, 255, 1 ) );
      vP2 = vP1 + vNMainAxisPerp * 5.0f;
      IRR.video->draw3DLine( vP1, vP2, SColor( 0, 125, 255, 1 ) );

      // draw main axis perp velocity
      vP2 = vP1 + vAxisVelocityPerp;
      IRR.video->draw3DLine( vP1, vP2, SColor( 0, 255, 0, 0 ) );

      // draw main axis velocity
      vP2 = vP1 + vAxisVelocity;
      IRR.video->draw3DLine( vP1, vP2, SColor( 0, 10, 10, 255 ) );

      // velocity
      vP2 = vP1 + vVelocity;
      IRR.video->draw3DLine( vP1, vP2, SColor( 0, 0, 255, 255 ) );

      //text
      IRR.gui->getBuiltInFont()->draw( L"Hydroplane: ", core::rect<s32>( 10, IRR.getScreenHeight() - 406, IRR.getScreenWidth(), IRR.getScreenHeight() ), irr::video::SColor( 255, 165, 15, 65 ), false, true );

      WideString sVel = "Velocity: ";
      sVel += f2wchar( vVelocity.getLength() );
      IRR.gui->getBuiltInFont()->draw( sVel.c_str(), core::rect<s32>( 10, IRR.getScreenHeight() - 392, IRR.getScreenWidth(), IRR.getScreenHeight() ), irr::video::SColor( 255, 165, 15, 65 ), false, true );
      sVel = "Position: [ "; sVel += f2wchar( getPosition().X );
      sVel += "  ,  "; sVel += f2wchar( getPosition().Y ); sVel += " ]";
      IRR.gui->getBuiltInFont()->draw( sVel.c_str(), core::rect<s32>( 10, IRR.getScreenHeight() - 378, IRR.getScreenWidth(), IRR.getScreenHeight() ), irr::video::SColor( 255, 165, 15, 65 ), false, true );
    }
}

void CHydroplane::PhysicsControl()
{
    if ( bInvalidEntity )
    {
      return;
    }

    // calculate the main plane axis (line from tail to engine)
    matrix4 mat;
    NewtonBodyGetMatrix( body, &mat.M[0] ); 

    matrix4 rotMat;
    rotMat.setRotationDegrees( vAxisRotation );

    vNMainAxis = vector3df( -1.0f, 0.0f, 0.0f );
    mat.rotateVect( vNMainAxis );

    vNMainAxisPerp = vector3df( 0.0f, -1.0f, 0.0f );
    mat.rotateVect( vNMainAxisPerp );

    // velocity is the relative wind (since there is no real wind)
    vector3df vNVelocity = vVelocity;
    vNVelocity.normalize();

    // calculate wing velocity
    //vector3df vWingVelocity = WORLD.GetPhysics()->getPointVelocity( body, vector3df( -3.0f, 27.7f, 0.0f) );
    vector3df vWingVelocity = vVelocity;
    vAxisVelocity = vWingVelocity * vNMainAxis;
    vAxisVelocityPerp = vWingVelocity * vNMainAxisPerp;

    // *** WINGS LIFT force
    vector3df vWingsLift;
    f32 fK0 = ( vAxisVelocity.getLength() );
    vWingsLift = vNVelocity * getWingsLiftPower() * fK0;
    vWingsLift.rotateXYBy( rotMat.getRotationDegrees().X + 90, vector3df( 0, 0, 0 ) );
    NewtonBodyAddForce( body, &vWingsLift.X );

    // *** DRAG force
    vector3df vWingsResist;
    f32 fK1 = vAxisVelocityPerp.getLength();
    f32 fK2 = fVelocity;

    if ( bInWater )
    {
      fK1 *= 2.0f;
      fK2 *= 2.0f;
    }

    vWingsResist = -vNVelocity * getWingsResistFactor() * fK1 * fK1; // wings
    vWingsResist -= vNVelocity * getPlaneResistFactor() * fK2 * fK2; // plane
    NewtonBodyAddForce( body, &vWingsResist.X );    

    // *** tail drag from rotating
    fK0 = -3.0f;
    if ( bInWater )
    {
      fK0 *= 3.0f;
    }
    vector3df vTailDrag( 0.0f, 0.0f, vOmega.Z* getTailResistFactor() * fK0 );
    NewtonBodyAddTorque( body, &vTailDrag.X );

    // *** tail drag from speed

    // check the relation between axis and velocity
    vector3df vCenter = getPosition();
    vector3df vA = vCenter + vNMainAxis* fVelocity;
    plane3df axisPlane( vCenter, vA, vector3df( vCenter.X, vCenter.Y, vCenter.Z + fVelocity ) );
    float rel = 1.0f;
    if ( axisPlane.classifyPointRelation( vCenter + vVelocity ) == ISREL3D_BACK )
    {
      rel *= -1.0f;
    }
    //if ( vVelocity.Y > 0.0f ) // a little hack taking advantage of the 2d nature of the game, I don't have the brains now to think of a nice solution
    //  fK1 *= -1;

    vector3df vTorqueBack( 0.0f, 0.0f, getStabilizePower() * fK1* rel );
    NewtonBodyAddTorque( body, &vTorqueBack.X );


    // *** TAIL WINGS LIFT force

    // calculate tail wing velocity
    //vector3df vTailWingVelocity = WORLD.GetPhysics()->getPointVelocity( body, vector3df( 88.0f, 30.5f, 0.0f) );
    vector3df vTailWingVelocity = vVelocity;
    vector3df vTailVelocity = vTailWingVelocity* vNMainAxis;
    vector3df vTailVelocityPerp = vTailWingVelocity* vNMainAxisPerp;

    fK0 = vTailWingVelocity.getLength();

    vTorqueBack = vector3df( 0.0f, 0.0f, -getTailLiftPower() * fK0 );
    rotMat.transformVect( vTorqueBack );
    NewtonBodyAddTorque( body, &vTorqueBack.X );

    //NewtonBodyAddForce(body, &vTailWingLift.X);

    //vector3df vTorqueBack( 0.0f, 0.0f, getStabilizePower() * getEnginePower() );
    //rotMat.transformVect( vTorqueBack );

    // *** water stabilize
    if ( bInWater )
    {
      vector3df vWaterTorque( 0.0f, 0.0f, 9600.0f );
      rotMat.transformVect( vWaterTorque );
      NewtonBodyAddTorque( body, &vWaterTorque.X );

      vector3df vSkiLift( 0.0f, -3500.0f, 0.0f );
      //NewtonBodyAddForce(body, &vSkiLift.X);      
      //WORLD.GetPhysics()->addLocalForce( body, vector3df( 0.0f, -1110.0f, 0.0f ), vector3df( -13.0f, -38.0f, 0.0f ) );
      //WORLD.GetPhysics()->addLocalForce( body, vector3df( 0.0f, -510.0f, 0.0f ), vector3df( 47.0f, -38.0f, 0.0f ) );
    }


    //CONSOLE_VECTOR( vOmega );


    // ENGINE force
    if ( fThrustValue > 0.0f ) // ---------- engine ON
    {
      vector3df vThrustMain = vNMainAxis;
      vThrustMain *= getEnginePower() * fThrustValue;
      NewtonBodyAddForce( body, &vThrustMain.X );

      if ( KERNEL.GetTicks() % 15 == 0 )
      {
        FACTORY->Effects.Create( "beam", vLastTrailPos, getPosition(), 0.0f, 1.0f, 60 );
        vLastTrailPos = getPosition();
      }

      if ( !engineSound->isPlaying() )
      {
        engineSound->play();
      }

      propellerSprite->setSpeed( 3 );
      propellerSprite->setStartEndFrame( 0, 10 );
    }
    else                                                // ---------- engine OFF
    {
      vLastTrailPos = getPosition();
      if ( !isInWater() )
      {
        //NewtonBodySetLinearDamping( body, 0.000000001f );
        //vector3df vAng(0.0000001f, 0.0000001f, 0.0000001f );
        //NewtonBodySetAngularDamping( body, &vAng.X );
      }

      engineSound->pause();

      propellerSprite->setSpeed( 7 );
      propellerSprite->setStartEndFrame( 12, 13 );
    }

    if ( control )
    {
      //f32 mouseAngle = getTargetAngle( getPosition(), myControls.mousePosWorld ).Y;
      //f32 planeAngle = getTargetAngle( getPosition(), getPosition() + vNMainAxis ).Y + PI/2;

      //if ( planeAngle > mouseAngle ) // tail up
      //{
      //    f32 steerFactor = 5.0f + vAxisVelocity.getLength();
      //    vector3df vSteerForce( 0.0f, 0.0f, steerFactor * getSteerPower() );
      //    rotMat.transformVect( vSteerForce );
      //    NewtonBodySetTorque( body, &vSteerForce.X);
      //}
      //if ( planeAngle < mouseAngle ) // tail down
      //{
      //    f32 steerFactor = 5.0f + vAxisVelocity.getLength();
      //    vector3df vSteerForce( 0.0f, 0.0f, - steerFactor * getSteerPower() );
      //    rotMat.transformVect( vSteerForce );
      //    NewtonBodySetTorque( body, &vSteerForce.X);
      //}

      if ( getControls()->ActionKeyPressed( AK_MOVE_DOWN ) ) // tail up
      {
        f32 steerFactor = 0.0f + sqrt( vTailVelocity.getLength() );
        vector3df vSteerForce( 0.0f, 0.0f, steerFactor* getSteerPower() );
        rotMat.transformVect( vSteerForce );
        NewtonBodySetTorque( body, &vSteerForce.X );
      }
      if ( getControls()->ActionKeyPressed( AK_MOVE_UP ) ) // tail down
      {
        f32 steerFactor = 0.0f + sqrt( vTailVelocity.getLength() );
        vector3df vSteerForce( 0.0f, 0.0f, -steerFactor* getSteerPower() );
        rotMat.transformVect( vSteerForce );
        NewtonBodySetTorque( body, &vSteerForce.X );
      }
    }

    // for debug render
    vAxisVelocity = vWingsLift / 500;
    vAxisVelocityPerp = vWingsResist / 500;

    //  p_wing->setPosition( mat.getTranslation() + vector3df(0, -5.4f * NewtonToIrr, 0)  );

    // set rotation of jointed parts
    //if (bZombie)
    //{
    //  p_wing->setPosition( mat.getTranslation() );
    //  p_wing2->setPosition( mat.getTranslation() );
    //  p_tail->setPosition( mat.getTranslation() );
    //if (joint_wing)
    //  p_wing->setRotation( mat.getRotationDegrees() );
    //if (joint_wing2)
    //  p_wing2->setRotation( mat.getRotationDegrees() );
    //if (joint_tail)
    //  p_tail->setRotation( mat.getRotationDegrees() );
    //}
}


void CHydroplane::Load( const c8* filename )
{
    // load script file
    SCRIPT.RunFile( filename );

    LOAD_STRING( "hydroplane_modelname" );
    LOAD_STRING( "hydroplane_texturename" );
    LOAD_STRING( "hydroplane_weaponscript" );
    LOAD_STRING( "hydroplane_propeller_sprite" );
    LOAD_VALUE( "hydroplane_propeller_sprite_size", int );
    LOAD_VECTOR( "hydroplane_modelscale_x", "hydroplane_modelscale_y", "hydroplane_modelscale_z" );
    LOAD_VECTOR( "hydroplane_modeloffset_x", "hydroplane_modeloffset_y", "hydroplane_modeloffset_z" );
    LOAD_VECTOR( "hydroplane_propeller_position_x", "hydroplane_propeller_position_y", "hydroplane_propeller_position_z" );
    LOAD_VALUE( "hydroplane_propellerscale", f32 );
    LOAD_VECTOR( "hydroplane_exhaust1_position_x", "hydroplane_exhaust1_position_y", "hydroplane_exhaust1_position_z" );
    LOAD_VECTOR( "hydroplane_exhaust2_position_x", "hydroplane_exhaust2_position_y", "hydroplane_exhaust2_position_z" );
    LOAD_VECTOR( "hydroplane_exhaust3_position_x", "hydroplane_exhaust3_position_y", "hydroplane_exhaust3_position_z" );
    LOAD_VECTOR( "hydroplane_exhaust4_position_x", "hydroplane_exhaust4_position_y", "hydroplane_exhaust4_position_z" );
    LOAD_VECTOR( "hydroplane_exhaust1_rotation_x", "hydroplane_exhaust1_rotation_y", "hydroplane_exhaust1_rotation_z" );
    LOAD_VECTOR( "hydroplane_exhaust2_rotation_x", "hydroplane_exhaust2_rotation_y", "hydroplane_exhaust2_rotation_z" );
    LOAD_VECTOR( "hydroplane_exhaust3_rotation_x", "hydroplane_exhaust3_rotation_y", "hydroplane_exhaust3_rotation_z" );
    LOAD_VECTOR( "hydroplane_exhaust4_rotation_x", "hydroplane_exhaust4_rotation_y", "hydroplane_exhaust4_rotation_z" );
    LOAD_VALUE( "hydroplane_exhaustscale_x", f32 );
    LOAD_VALUE( "hydroplane_exhaustscale_y", f32 );
    LOAD_VALUE( "hydroplane_lineardamping", f32 );
    LOAD_VALUE( "hydroplane_angulardamping", f32 );
    LOAD_VALUE( "hydroplane_mass", f32 );
    LOAD_VALUE( "hydroplane_bodytype", int );
    LOAD_VALUE( "hydroplane_proptimeout", int );
    LOAD_VALUE( "hydroplane_maxpropsplits", int );
    LOAD_VALUE( "hydroplane_splitfactor", f32 );
    LOAD_VALUE( "hydroplane_enginepower", f32 );
    LOAD_VALUE( "hydroplane_wingsliftpower", f32 );
    LOAD_VALUE( "hydroplane_tailliftpower", f32 );
    LOAD_VALUE( "hydroplane_wingsresistfactor", f32 );
    LOAD_VALUE( "hydroplane_steerpower", f32 );
    LOAD_VALUE( "hydroplane_stabilizepower", f32 );
    LOAD_VALUE( "hydroplane_planeresistfactor", f32 );
    LOAD_VALUE( "hydroplane_tailresistfactor", f32 );
    LOAD_VALUE( "hydroplane_brake", f32 );
    LOAD_VALUE( "hydroplane_rotationrate", f32 );
    LOAD_VECTOR( "hydroplane_mountpos_x", "hydroplane_mountpos_y", "hydroplane_mountpos_z" );
    LOAD_VECTOR( "hydroplane_gunmountpos_x", "hydroplane_gunmountpos_y", "hydroplane_gunmountpos_z" );
    LOAD_VECTOR( "hydroplane_gunmountrot_x", "hydroplane_gunmountrot_y", "hydroplane_gunmountrot_z" );
}

void CHydroplane::Unserialize( RakNet::BitStream& bt )
{
    vector3df vScale, vMPos, vMPos2, vMRot, vColOffset;
    f32 fMass, fPropScale, fExhaustScaleX, fExhaustScaleY;
    int PropFrameSize = 0, PropFramesNum = 13;
    int stringLength = 0;
    char* inString = NULL;

    UNLOAD_STRING
    String modelname = inString;        delete[] inString; inString = NULL;
    UNLOAD_STRING
    String texname = inString;          delete[] inString; inString = NULL;
    UNLOAD_STRING WeaponScriptName = inString;      delete[] inString; inString = NULL;
    UNLOAD_STRING
    String PropellerName = inString;    delete[] inString; inString = NULL;
    bt.Read( PropFrameSize );
    bt.ReadVector( vScale.X, vScale.Y, vScale.Z );
    bt.ReadVector( vColOffset.X, vColOffset.Y, vColOffset.Z );
    bt.ReadVector( vPropPos.X, vPropPos.Y, vPropPos.Z );
    bt.Read( fPropScale );
	for (int i=0; i < 4; i++)
		bt.ReadVector( vExhaustPos[i].X, vExhaustPos[i].Y, vExhaustPos[i].Z );
	for (int i=0; i < 4; i++)
		bt.ReadVector( vExhaustRot[i].X, vExhaustRot[i].Y, vExhaustRot[i].Z );
    bt.Read( fExhaustScaleX );
    bt.Read( fExhaustScaleY );
    bt.Read( fLinDamp );
    bt.Read( fAngDamp );
    bt.Read( fMass );
    bt.Read( bodyType );
    bt.Read( propTimeout );
    bt.Read( maxSplits );
    bt.Read( fSplitFactor );
    bt.Read( fPower );
    bt.Read( fWingsLift );
    bt.Read( fTailLift );
    bt.Read( fWingsResist );
    bt.Read( fSteerPower );
    bt.Read( fStabilizePower );
    bt.Read( fPlaneResist );
    bt.Read( fTailResist );
    bt.Read( fBrake );
    bt.Read( fRotationRate );
    bt.ReadVector( vMPos.X, vMPos.Y, vMPos.Z );
    bt.ReadVector( vMPos2.X, vMPos2.Y, vMPos2.Z );
    bt.ReadVector( vMRot.X, vMRot.Y, vMRot.Z );

    assemblePhysics( modelname.c_str(), ( BodyType )bodyType, vScale, vColOffset, fMass );

    if ( ( !node ) || ( !body ) )
    {
      bInvalidEntity = true;
      return;
    }

    addMountPlace( vMPos, vector3df( 0, 0, 0 ), NODECLASS_CHARACTER );
    addMountPlace( vMPos2, vMRot, NODECLASS_WEAPON );

    node->setMaterialTexture( 0, IRR.video->getTexture( APP.useFile( texname.c_str() ).c_str() ) );

    NewtonBodySetLinearDamping( body, fLinDamp );
    vAngDamp = vector3df( fAngDamp, fAngDamp, fAngDamp );
    NewtonBodySetAngularDamping( body, &vAngDamp.X );

    // set custom wieght factor
    weightFactor = NewtonConvexCollisionCalculateVolume( NewtonBodyGetCollision( body ) ) * 1.0f;

    setDirection( ANGLE_FACE_LEFT );

    //propeller
    propellerSprite = new CAnimSpriteSceneNode( node, IRR.smgr, -1 ); 
    propellerSprite->Load( APP.useFile( PropellerName.c_str() ).c_str(), 0, 0, PropFrameSize, PropFrameSize * PropFramesNum, PropFrameSize, PropFrameSize, false ); 
    propellerSprite->setPosition( vPropPos );
    vPropRot = vector3df( 0, 80, 0 );
    propellerSprite->setRotation(vPropRot);   
    propellerSprite->setScale( vector3df( fPropScale, fPropScale, fPropScale ) );  
    propellerSprite->setMaterialType( video::EMT_TRANSPARENT_ALPHA_CHANNEL );
	propellerSprite->setMaterialFlag( video::EMF_LIGHTING, (bool)IRR.useLighting );
    propellerSprite->updateAbsolutePosition();
	propellerSprite->setSpeed(1);
	propellerSprite->setStartEndFrame( 12, 13 );
	propellerSprite->setFrame(12);

    //exhaust
	for (int i=4; i < 8; i++)
	{
		vExhaustPos[i] = vExhaustPos[i-4];
		vExhaustPos[i].Z = -vExhaustPos[i].Z;
		vExhaustRot[i] = vExhaustRot[i-4];
	}

	for (int i = 0; i < 8; i++)
	{
		exhaustSprite[i] = new CAnimSpriteSceneNode( node, IRR.smgr, -1 ); 
		exhaustSprite[i]->Load( APP.useFile( "Sprites/exhaust.png" ).c_str(), 0, 0, 64, 128, 64, 16, false ); 
		exhaustSprite[i]->setPosition( vExhaustPos[i] );
		exhaustSprite[i]->setRotation( vExhaustRot[i] );   
		exhaustSprite[i]->setScale( vector3df( fExhaustScaleX, fExhaustScaleY, 1.0f ) );  
		exhaustSprite[i]->setMaterialType( video::EMT_TRANSPARENT_ALPHA_CHANNEL );
		exhaustSprite[i]->setMaterialFlag( video::EMF_LIGHTING, false );
		exhaustSprite[i]->getMaterial(0).EmissiveColor.setBlue(255);
		exhaustSprite[i]->getMaterial(0).EmissiveColor.setRed(255);
		exhaustSprite[i]->getMaterial(0).EmissiveColor.setGreen(255);
		exhaustSprite[i]->getMaterial(0).EmissiveColor.setAlpha(255);

		exhaustSprite[i]->updateAbsolutePosition();
		exhaustSprite[i]->setSpeed(1);
		exhaustSprite[i]->setStartEndFrame( 0, 7 );
		exhaustSprite[i]->setFrame(0);
	}

	//wake
    IAnimatedMesh* mesh = IRR.smgr->getMesh( "Models/wake.ms3d" );
    wake = IRR.smgr->addAnimatedMeshSceneNode( mesh );
    wake->setMaterialTexture( 0, IRR.video->getTexture( APP.useFile( "Textures/wake.png" ).c_str() ) );
    wake->setMaterialFlag( EMF_BACK_FACE_CULLING, false );
    wake->setMaterialType( video::EMT_TRANSPARENT_ALPHA_CHANNEL );
	wake->setMaterialFlag( video::EMF_LIGHTING, (bool)IRR.useLighting );
    wake->setRotation( vector3df( 0, -90, 0 ) );
    wake->setScale( vScale );
    wake->setPosition( vector3df( 0, 0, 0 ) );
    wake->setVisible( false );
}

