#include "soldier.h"

#include "../IrrConsole/console_vars.h"

#include "../World/world.h"

////////////////////////////////////////////
// CSoldier 
////////////////////////////////////////////

#define CMENU_DISMOUNT 0
#define CMENU_DROPWEAPON 1
#define CMENU_DROPITEM 2

bool bRegistered_CSoldier = FACTORY->Actors.Register<CSoldier>( "soldier" );

CSoldier::CSoldier( const c8* configFilename ) : CCharacter( configFilename )
{
    Reset();
    factoryName = "soldier";

    if ( configFilename != "" )
    {
      //CONSOLE.addx( "soldier");
      Load( configFilename );
      Unserialize( loadedBitStream );
    }
}

CSoldier::~CSoldier()
{
}

void CSoldier::Reset()
{
    CCharacter::Reset();
}

void CSoldier::Think()
{
    CCharacter::Think();

    if ( control )
    {
      // context menu handling
      s32 menusel = control->popContextMenuSelection();
      switch ( menusel )
      {
        case CMENU_DISMOUNT :
          unAttachFromParent();
          break;
        case CMENU_DROPWEAPON :
          lastDropped = DropChild( NODECLASS_WEAPON );
          break;
        case CMENU_DROPITEM :
          lastDropped = DropChild( NODECLASS_ITEM );
          break;
      }

      //if ( control->isContextMenuActive() )
      //{
      //    control->PositionContextMenu( 
      //        IRR.smgr->getSceneCollisionManager()->getScreenCoordinatesFrom3DPosition( getPosition(), IRR.smgr->getActiveCamera() ) );
      //}

      if ( control->ActionKeyPressed( AK_MOUNT ) )
      {
      }

      if ( control->ActionKeyPressed( AK_DISMOUNT ) )
      {
        if ( ( parentAttachment ) || ( childAttachments.size() ) )
        {
          control->NewContextMenu( control->mousePos );

          if ( parentAttachment )
          {
            control->AddContextMenuItem( "Dismount", CMENU_DISMOUNT );
          }
          if ( isChildAttached( NODECLASS_WEAPON ) )
          {
            control->AddContextMenuItem( "Drop Weapon", CMENU_DROPWEAPON );
          }
          if ( isChildAttached( NODECLASS_ITEM ) )
          {
            control->AddContextMenuItem( "Drop Item", CMENU_DROPITEM );
          }

          if ( control->getContextMenuItemsCount() == 1 )
          {
            control->setContextMenuFirstItemSelection();
          }
        }
      }

      if ( control->ActionKeyPressed( AK_DROP ) ) // selfkill
      {
        //bZombie = true;
        takeDamage( fHealth, ownerPlayerID );
        return;
      }

      if ( parentAttachment ) //attached
      {
        morphAnimation( anim_Crouch, 0.2f );
        clearAnimation( anim_Idle, 0.2f );
        clearAnimation( anim_Run, 0.02f );

        if ( weapon )
        {
          weapon->setControls( NULL );
        }
      }
      else
      {
        clearAnimation( anim_Crouch, 0.2f );
      }
    }
}

void CSoldier::Render()
{
    CCharacter::Render();

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

      // draw run force
      vP2 = vP1 + vRunForce / 100.0f;
      IRR.video->draw3DLine( vP1, vP2, SColor( 0, 255, 0, 0 ) );
    }
}

// !!! does not get in here if attached to sth
void CSoldier::PhysicsControl()
{
    bStopped = true;

    if ( control )
    {
      if ( !parentAttachment ) //unattached
      {
        vOldAxisRotation.Y = vAxisRotation.Y;

        f32 fSpeed = fVelocity;

        if ( !bOnGround )
        {
          vGroundNormal = vector3df( 0.0f, -1.0f, 0.0f );
        }

        if ( weapon )
        {
          weapon->setControls( control );

          if ( getControls()->ActionKeyPressed( AK_RELOAD ) )
          {
            //  if (!weapon->canFire())
            //      morphAnimation(anim_BoltPull, 0.5f);
          }

          //    if (weapon->canFire())
          //        clearAnimation(anim_BoltPull, 0.5f);
        }

        //  CONSOLE_FLOAT(fSpeed);

        //if (bOnGround)
        //  CONSOLE.add("if (bOnGround)");
        //else
        //  CONSOLE.add("xxxxxxxxxx");

        //if ( getControls()->ActionKeyPressed( AK_MOVE_DOWN ) )
        //{
        //  setCollisionSize( vector3df(1.0f, 1.0f, 0.55f) );
        //}
        //else
        //  if ( vHullScale.Z != 1.0f )
        //  {
        //      setCollisionSize( vector3df(1.0f, 1.0f, 1.0f) );
        //  }

        if ( getControls()->ActionKeyPressed( AK_FIRE2 ) )
        {
          dVector force( 0, fJumpForce, 0 );
          NewtonBodyAddForce( body, &force.m_x );
		  if (KERNEL.GetTicks() % 8 == 0)
		  {
			  vector3df jetPos = getPosition();
			  jetPos.Y += 1.0f;
			  FACTORY->Effects.Create( "machinehit", jetPos, jetPos, 0.0f, 2.5f, 30 );
		  }
        }

        if ( getControls()->ActionKeyPressed( AK_MOVE_LEFT ) )
        {
          bStopped = false;
          //vector3df force(-fRunForce, 0, 0);

          vRunForce = vGroundNormal * fRunForce;
          vRunForce.rotateXYBy( -90, vector3df( 0, 0, 0 ) );
          vRunForce.X = -abs( vRunForce.X );

          //if (bOnGround)
          if ( ( fSpeed < fMaxSpeed ) || ( !bOnGround ) )
          {
            NewtonBodyAddForce( body, &vRunForce.X );
          }

          ///if (control->mousePosWorld.X > getPosition().X)
          ///   changeAnimation(anim_RunBack, 1.35f);
          ///else
          ///   changeAnimation(anim_Run, 1.35f);

          morphAnimation( anim_Run, 0.6f );
        }

        if ( getControls()->ActionKeyPressed( AK_MOVE_RIGHT ) )
        {
          bStopped = false;

          vRunForce = vGroundNormal * fRunForce;
          vRunForce.rotateXYBy( 90, vector3df( 0, 0, 0 ) );
          vRunForce.X = abs( vRunForce.X );

          //if (bOnGround)
          if ( ( fSpeed < fMaxSpeed ) || ( !bOnGround ) )
          {
            NewtonBodyAddForce( body, &vRunForce.X );
          }

          ///if (control->mousePosWorld.X > getPosition().X)
          ///   morphAnimation(anim_Run, 1.35f);
          ///else
          ///   morphAnimation(anim_RunBack, 1.35f);

          morphAnimation( anim_Run, 0.6f );
        }


        if ( ( !getControls()->ActionKeyPressed( AK_MOVE_LEFT ) ) && ( !getControls()->ActionKeyPressed( AK_MOVE_RIGHT ) ) )
        {
          clearAnimation( anim_Run, 0.1f );
        }

        if ( getControls()->ActionKeyPressed( AK_MOVE_DOWN ) )
        {
          morphAnimation( anim_Crouch, 0.2f );
          clearAnimation( anim_Idle, 0.2f );
        }
        else
        {
          clearAnimation( anim_Crouch, 0.2f );
        }

        if ( control->mousePosWorld.X < getPosition().X )
        {
          if ( vAxisRotation.Y > ANGLE_FACE_LEFT )
          {
            vAxisRotation.Y -= fRotationRate;
          }
          else
          {
            vAxisRotation.Y = ANGLE_FACE_LEFT - 0.001f;
          } //HACK: don't know what's going with the rotations
        }
        else if ( vAxisRotation.Y < ANGLE_FACE_RIGHT )
        {
          vAxisRotation.Y += fRotationRate;
        }

        if ( bStopped )
        {
          morphAnimation( anim_Idle, 0.1f );
        }
        else
        {
          clearAnimation( anim_Idle, 0.1f );
        }

        morphAnimation( anim_HoldWeapon, 0.1f );
      }
    }

    //model->getCurrentModel()->getMixer()->clearCycle(model->getCurrentAnimID(), 0.01f);
    //changeAnimation(anim_Run, 0.01f, 0.01f);
    //changeAnimation(anim_BoltPull, 0.01f);

    bOnGround = false;
}


void CSoldier::Load( const c8* filename )
{
    String modelname;
    f32 fMass, fBuoyancyFactor;
    vector3df vScale;
    int wepPos, itemPos;

    // load script file
    SCRIPT.RunFile( filename );

    LOAD_STRING( "soldier_modelconfig" );
    LOAD_VALUE( "soldier_mass", f32 );
    LOAD_VALUE( "soldier_buoyancyfactor", f32 );
    LOAD_VECTOR( "soldier_scalecollision_x", "soldier_scalecollision_y", "soldier_scalecollision_z" );
    LOAD_VALUE( "soldier_runforce", f32 );
    LOAD_VALUE( "soldier_jumpforce", f32 );
    LOAD_VALUE( "soldier_rotationrate", f32 );
    LOAD_VALUE( "soldier_animation_speedfactor", f32 );
    LOAD_VALUE( "soldier_animation_bone2mouse", u16 );
    LOAD_VALUE( "soldier_animation_bonehead", u16 );
    LOAD_VALUE( "soldier_animation_boneLfoot", u16 );
    LOAD_VALUE( "soldier_animation_boneRfoot", u16 );
    LOAD_VALUE( "soldier_anim_idle", u16 );
    LOAD_VALUE( "soldier_anim_walk", u16 );
    LOAD_VALUE( "soldier_anim_holdweapon", u16 );
    LOAD_VALUE( "soldier_anim_run", u16 );
    LOAD_VALUE( "soldier_anim_runback", u16 );
    LOAD_VALUE( "soldier_anim_crouch", u16 );
    LOAD_VALUE( "soldier_anim_boltpull", u16 );
    LOAD_VALUE( "soldier_weapon_hand", u16 );
    LOAD_VALUE( "soldier_item_place", u16 );
    LOAD_VALUE( "soldier_lineardamping", f32 );
    LOAD_VALUE( "soldier_maxspeed", f32 );

    //  CONSOLE.addx( "loadedBitStream %i", loadedBitStream.GetNumberOfBytesUsed() );
}

void CSoldier::Unserialize( RakNet::BitStream& bt )
{
    f32 fMass, fBuoyancyFactor;
    vector3df vScale;
    u16 wepPos, itemPos;
    int stringLength = 0;
    char* inString = NULL;

    UNLOAD_STRING
    String configname = inString;   delete[] inString; inString = NULL;
    bt.Read( fMass );
    bt.Read( fBuoyancyFactor );
    bt.ReadVector( vScale.X, vScale.Y, vScale.Z );
    bt.Read( fRunForce );
    bt.Read( fJumpForce );
    bt.Read( fRotationRate );
    bt.Read( fAnimSpeedFactor );
    bt.Read( bone2MouseBoneID );
    bt.Read( headBoneID );
    bt.Read( LFootBoneID );
    bt.Read( RFootBoneID );
    bt.Read( anim_Idle );
    bt.Read( anim_Walk );
    bt.Read( anim_HoldWeapon );
    bt.Read( anim_Run );
    bt.Read( anim_RunBack );
    bt.Read( anim_Crouch );
    bt.Read( anim_BoltPull );
    bt.Read( wepPos );
    bt.Read( itemPos );
    bt.Read( fLinDamp );
    bt.Read( fMaxSpeed );

    addMountPlace( vector3df( wepPos, 0, 0 ), vector3df( 0, 0, 0 ), NODECLASS_WEAPON );
    addMountPlace( vector3df( itemPos, 0, 0 ), vector3df( 0, 0, 0 ), NODECLASS_ITEM );

    node = new CCal3DSceneNode( configname.c_str(), IRR.smgr->getRootSceneNode(), IRR.smgr, -1 ); 

    vector3df vExchangeSize;
    aabbox3d<f32> box = node->getBoundingBox();
    vector3df edges[8];
    box.getEdges( edges );
    vExchangeSize.Y = vScale.X * ( edges[3] - edges[1] ).getLength();
    vExchangeSize.X = vScale.Y * ( edges[0] - edges[1] ).getLength();
    vExchangeSize.Z = vScale.Z * ( edges[5] - edges[1] ).getLength();
    vExchangeSize *= 1;

    makeCharacterBody( vExchangeSize, fMass, fBuoyancyFactor );

    ///
    if ( ( !node ) || ( !body ) )
    {
      bInvalidEntity = true;
      return;
    }

    NewtonBodySetLinearDamping( body, fLinDamp );

    model = ( CCal3DSceneNode * )node;
    model->getCurrentModel()->getMesh( 0 )->getSubmesh( 0 )->setCoreMaterialId( team );

    //node->setRotation(vector3df(90,90,0));
    setLOD( 1.0f );
    //bFix2DRot = false;
    changeAnimation( anim_Idle );
    clearAnimation( anim_Idle, 1.0f );

    bonesNum = model->getCoreModel()->getCoreSkeleton()->getVectorCoreBone().size();
}

