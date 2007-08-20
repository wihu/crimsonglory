#include "world.h"
#include "../Engine/engine.h"
#include "../Game/SingletonIncludes.h"
#include "../IrrConsole/console_vars.h"

#include "../Newton/newton_physics.h"
#include "../Newton/newton_node.h"

#include "actor.h"
#include "map.h"
#include "controls.h"
#include "respawn.h"
#include "../Effects/effect.h"
#include "player.h"
#include "rules.h"

#define ANGLE_DIVIDE 3
//#define DEFAULT_CAMERA_FOV -PI / 1.09f
//#define DEFAULT_CAMERA_FOV 90.0f*180.0f/irr::core::GRAD_PI2
#define DEFAULT_CAMERA_FOV -100.0f*180.0f/irr::core::GRAD_PI2
#define DEFAULT_CAMERA_FAR 170000.0f
#define DEFAULT_CAMERA_OVERRIDEZ 1.2f
#define DEFAULT_CAMERA_POSLAG 3.0f
#define DEFAULT_CAMERA_TARGETLAG 5.0f

#define STARTRULES "Scripts/rules_dogfight.gm"

////////////////////////////////////
// CWorldTask                     //
////////////////////////////////////

CWorldTask::CWorldTask()
{
    newtonTask = new CNewton();
    ADDTASK( newtonTask, 55, true );
    newtonTask->Stop();

    players = new CPlayerManager();

    CONSOLE_VAR( "w_camera_poslag", f32, fCamPosLag, 3.0f, L"w_camera_poslag [real]. Ex. w_camera_poslag 3.0f", L"Lag of camera position movement. Higher value - more lag." );
    CONSOLE_VAR( "w_camera_targetlag", f32, fCamTargetLag, 5.0f, L"w_camera_targetlag [real]. Ex. w_camera_targetlag 5.0f", L"Lag of camera target movement. Higher value - more lag." );
    CONSOLE_VAR( "w_camera_distance", f32, fCamDistance, 15.0f, L"w_camera_distance [real]. Ex. w_camera_distance 15.0f", L"Default distance of camera to target." );
    CONSOLE_VAR( "w_camera_ortho", int, iCamOrtho, 0, L"w_camera_ortho [0/1]. Ex. w_camera_ortho 1", L"Sets the camera to orthogonal view." );
    CONSOLE_VAR( "w_camera_speedfactor", f32, fCamSpeedFactor, 0.5f, L"w_camera_speedfactor [real]. Ex. w_camera_speedfactor 0.5f", L"How the distance of camera is affected by targets speed." );
    CONSOLE_VAR( "w_camera_mountfactor", f32, fCamMountFactor, 500.0f, L"w_camera_mountfactor [real]. Ex. w_camera_mountfactor 500.0f", L"How the distance of camera is affected by event when target is mounted." );

    CONSOLE_VAR( "w_dayspeed", f32, fDayspeed, 500.0f, L"w_dayspeed [real]. Ex. w_dayspeed 500.0f", L"Speed of day/night cycle." );

    CONSOLE_VAR( "r_player_respawn_time", f32, playerRespawnTime, 2.0f, L"r_player_respawn_time [seconds]. Ex. r_player_respawn_time 1.5", L"Sets the default normal respawn time for the player." );
    CONSOLE_VAR( "r_waverespawn_time", f32, waveRespawnTime, 2.0f, L"r_waverespawn_time [seconds]. Ex. r_waverespawn_time 1.5", L"Sets the default wave respawn time." );
    CONSOLE_VAR( "r_actor_respawn_time", f32, actorRespawnTime, 2.0f, L"r_actor_respawn_time [seconds]. Ex. r_actor_respawn_time 1.5", L"Sets the default normal respawn time for stuff other than player." );
    //CONSOLE_VAR( "r_waverespawn_on", int, useWaveRespawn, 0,
    //                L"r_waverespawn_on [seconds]. Ex. r_waverespawn_on 1", L"Sets game to use wave respawning." );

    myControls.MapKeys( true );

    myPlayer = NULL;
    camera = NULL;
    map = NULL;
    rules = NULL;
}

CWorldTask::~CWorldTask()
{
    delete players;
    players = NULL;
}

bool CWorldTask::Start()
{
    newtonTask->Start();
    worldRender = new CWorldRender(); // renderable
    map = new CMap(); // entity
    map->setDebugText( "World Map" );
    rules = new CRules( STARTRULES ); // entity

    // init scene
    camera = new CCamera( this ); // must delete

    return true;
}

void CWorldTask::Update()
{
    PROFILE( "World task" );

    int i;

    for ( i = 0; i < Entitys.size(); i++ )
    {
      if ( Entitys[i]->ValidEntity() )
      {
        Entitys[i]->Think();
      }
    }
    for ( i = 0; i < Entitys.size(); i++ )
    {
      if ( Entitys[i]->bCanDie )
      {
        Entitys[i]->Die();
        i = 0;
      }
    }

    //if ((KERNEL.GetTicks() % 30) == 0)
    //{
    //  CWorldPart *p = new CWorldPart( "Textures/fireball.bmp", dimension2d<f32>(1.0f, 1.0f), vector3df(-10,0,0), 0.66f, 1, true, 180 );
    //  p->SetVelocity( vector3df( 0.4f, -0.4f, 0 ) );
    //}

    // TEMP:
    if ( camera )
    {
      camera->fPosLag = fCamPosLag;
      camera->fTargetLag = fCamTargetLag;
      camera->vOverride.Z = fCamDistance;
      camera->iOrtho = iCamOrtho;
      camera->fSpeedFactor = fCamSpeedFactor;
      camera->fMountFactor = fCamMountFactor;
    }
    if ( rules )
    {
      rules->actorRespawnTime = actorRespawnTime;
      rules->waveRespawnTime = waveRespawnTime;
      rules->playerRespawnTime = playerRespawnTime;
    }
}

void CWorldTask::Stop()
{
    int i;

    //  CONSOLE.add( "CONSOLE DOES NOT EXIST AT THIS POINT WHEN CLOSING" );
    map = NULL;
    rules = NULL;

    for ( i = 0; i < Entitys.size(); i++ )
    {
      delete Entitys[i];
    }
    Entitys.clear();

    delete camera;
    camera = NULL;

    newtonTask->Stop();

    if ( IRR.smgr )
    {
      IRR.smgr->clear();
    }

    worldRender->Kill();
}

void CWorldTask::AddEntity( CEntity* e )
{
    Entitys.push_back( e );
}

void CWorldTask::RemoveEntity( CEntity* e )
{
    Entitys.erase( Entitys.binary_search( e ) );
    delete e;
}

////////////////////////////////////
// CWorldRender                   //
////////////////////////////////////

void CWorldRender::Render()
{
    //   if ( APP.DebugMode )
    {
        WORLD.GetPhysics()->Render();

        for ( int i = 0; i < WORLD.GetEntitysNum(); i++ )
        {
          WORLD.GetEntity( i )->Render();
        }
        return;
    }
}

////////////////////////////////////////////
// CCamera 
////////////////////////////////////////////
CCamera::CCamera( CWorldTask* w )
{
    world = w;

    if ( APP.DebugMode )
    {
      IrrCamera = IRR.smgr->addCameraSceneNodeMaya( 0, -1500, 1000, 500 );
    }
    else
    {
      IrrCamera = IRR.smgr->addCameraSceneNode();
    }

    Reset();

	bSetCamera = false;

    //TEMP:
    //if (APP.DebugMode > 1)
    //  bInvalidEntity = true;
}

CCamera::~CCamera()
{
}

void CCamera::Reset()
{
    IRR.smgr->setActiveCamera( IrrCamera );
    target = NULL;
    bOverrideX = false;
    bOverrideY = false;
    bOverrideZ = true;
    vOverride.X = 0.0f;
    vOverride.Y = 0.0f;
    vOverride.Z = DEFAULT_CAMERA_OVERRIDEZ;
    vRealPos = vOverride;
    vRealTargetPos = vRealPos;
    fPosLag = DEFAULT_CAMERA_POSLAG;
    iOrtho = false;
    IrrCamera->setFOV( DEFAULT_CAMERA_FOV );
    IrrCamera->setFarValue( DEFAULT_CAMERA_FAR );
    fMountFactor = fSpeedFactor = 0.0f;
    spectateMoveFactor = 0.002f;

    //IrrCamera->setPosition( vector3df(0,0,DEFAULT_CAMERA_OVERRIDEZ) );
}

void CCamera::Think()
{
    //if ( IRR.smgr->getActiveCamera() != IrrCamera )
    //  IRR.smgr->setActiveCamera(IrrCamera);

    vector3df vP, vT;

    vOldPos = vRealPos;

    if ( !target )
    {
      // no target,stay
      // move target according to mouse
      if ( myControls.ActionKeyPressed( AK_FIRE ) )
      {
        vRealPos.X += ( myControls.mousePos.X - IRR.getScreenWidth() / 2 ) * spectateMoveFactor;
        vRealPos.Y += ( myControls.mousePos.Y - IRR.getScreenHeight() / 2 ) * spectateMoveFactor;
      }
      vP = vRealPos;

      if ( myControls.ActionKeyPressed( AK_FIRE2 ) )
      {
        vP.Z = vOverride.Z;
      }
      else
      {
        vP.Z = vOverride.Z + fMountFactor;
      }

     // vT = vRealTargetPos = vP;
	   vT = vRealTargetPos;
    }
    else
    {
      // follow target
      vector3df vM = myControls.mousePosWorld;
      //if (myControls.mousePosWorld.getLengthSQ() < 10000000000.0f )
      vP = ( target->getPosition() + vM ) / 2;
      //    else
      //        vP = ( target->getPosition() );


      //TEMP
      vP.Y -= 0.1f;

      f32 fInVehicle = 0.0f;
      if ( target->isAttached() )
      {
        fInVehicle = fMountFactor;
      }

      if ( bOverrideX )
      {
        vP.X = vOverride.X;
      }
      if ( bOverrideY )
      {
        vP.Y = vOverride.Y;
      }
      if ( bOverrideZ )
      {
        vP.Z = fSpeedFactor * abs( target->getVelocity().X ) + vOverride.Z + fInVehicle;
      }

      if ( target->isZombie() )
      {
        target = NULL;
      }
    }

    vector3df vPosDelta = vP - vRealPos;
    // don't poslag if spectator
    if ( target )
    {
      vPosDelta /= fPosLag;
      vPosDelta.Z /= fPosLag / 3;
    }
    vRealPos += vPosDelta;

	//if (target)
	{
		if ( iOrtho )
		{
		vRealPos.Z = DEFAULT_CAMERA_OVERRIDEZ;    
		vRealTargetPos = vRealPos;
		vRealTargetPos.Z = 0.0f;
		}
		else
		{
		vT = vP;
		vRealTargetPos += ( vT - vRealTargetPos ) / fTargetLag;
		vRealTargetPos.Z = 0.0f;
		}
	}

    //TEMP:
    //if (!APP.DebugMode)
    {
        //if (vRealPos.Y > -30.5f)
        //  vRealPos.Y = -30.5f;
        //if (vRealTargetPos.Y > -30.5f)
        //  vRealTargetPos.Y = -30.5f;
        //CONSOLE.addx( "vRealPos.Y %f", vRealPos.Y);
    }

	if (bSetCamera)
	{
		vRealPos = vSetPos;
		vRealTargetPos = vSetTargetPos;
	}

    IrrCamera->setPosition( vRealPos );
    IrrCamera->setTarget( vRealTargetPos );

    //create an orthogonal 
    //buildProjectionMatrixOrthoLH(width,height,near,far) 
    if ( iOrtho )
    {
      ProjMatrix.buildProjectionMatrixOrthoLH( -vOverride.Z * 16.0f, -vOverride.Z * 12.0f, -30.5f, 30.5f ); 
      IrrCamera->setProjectionMatrix( ProjMatrix );
    }

    if ( !APP.DebugMode )
    {
      IrrCamera->updateAbsolutePosition();
    }

    //target = NULL;
}
