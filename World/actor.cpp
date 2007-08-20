#include "actor.h"

#include "map.h"
#include "player.h"
#include "rules.h"
#include "../RakNet/GameServer.h"


////////////////////////////////////////////
// CActor 
////////////////////////////////////////////

array<CActor*> CActor::actorsList;

CActor::CActor()
{
    Reset();

    actorsList.push_back( this );
}

CActor::CActor( const c8* scriptFilename )
{
    Reset();

    setDebugText( scriptFilename );

    configFilename = scriptFilename;

    actorsList.push_back( this );
}


//CActor::CActor( const c8* modelFilename, BodyType bodyType, vector3df vScale, float fMass )
//{
//  Reset();
//
//  assemblePhysics( modelFilename, bodyType, vScale, fMass );
//
//    setUserData();
//}

CActor::~CActor()
{
    //printf( "a 1 size %i\n", actorsList.size() );
    actorsList.erase( actorsList.binary_search( ( CActor * )this ) );
    //printf( "a 2 size %i\n", actorsList.size() );
}

void CActor::Render()
{
    CControllable::Render();
}

void CActor::Reset()
{
    CControllable::Reset();
    CNewtonNode::Reset();

    vAxisRotation.X = 180;
    vAxisRotation.Y = ANGLE_FACE_LEFT;
    vOldAxisRotation = vAxisRotation;
    fRotationRate = 5.0f;
    bOnGround = bStopped = false;
    type = NODECLASS_ACTOR;
    fHealth = 100.0f;
    team = 0;
    ownerPlayerID = UNASSIGNED_PLAYER_ID;
    recentDamageFromPlayerID = UNASSIGNED_PLAYER_ID;
    bRespawn = true;
    aboveText = NULL;

    //printf("CActor::Reset()\n");
}


void CActor::Think()
{
    CControllable::Think();

    // update the debug position
    //if (APP.DebugMode)
    setDebugPos( IRR.smgr->getSceneCollisionManager()->getScreenCoordinatesFrom3DPosition( getPosition(), IRR.smgr->getActiveCamera() ) );

    ////    //HACK: should be after if (bZombie), here because of network disconnect crash

    // destroy the object if is zombie
    if ( bZombie )
    {
      Respawn();
      ZombieDie();
    }

    // DIEEeeeee.... if not dead yet
    if ( ( fHealth <= 0.0f ) && ( !bZombie ) )
    {
      bZombie = true;

      // get info on who killed who
      if ( ownerPlayerID != UNASSIGNED_PLAYER_ID )
      {
        CPlayer* player = WORLD.GetPlayers()->GetPlayer( ownerPlayerID );
        CPlayer* player2 = WORLD.GetPlayers()->GetPlayer( recentDamageFromPlayerID );

        WORLD.GetRules()->OnPlayerDie( player, player2 );
      }
    }
}

bool CActor::onChildAttached( CNewtonNode* what, void* data )
{
    if ( CNewtonNode::onChildAttached( what, data ) )
    {
      if ( what->getType() == NODECLASS_CHARACTER )
      {
        setControls( ( CControls * )data );
      }

      return true;
    }
    return false;
}

void CActor::onChildUnAttached( CNewtonNode* what )
{
    CNewtonNode::onChildUnAttached( what );

    if ( what->getType() == NODECLASS_CHARACTER )
    {
      setControls( NULL );
    }
}


void CActor::ZombieDie()
{
    //if (bCanDie)
    //  return;

    matrix4 mat;
    NewtonBodyGetMatrix( body, &mat.M[0] ); 
    LoadZombie( mat );

    control = NULL;

    bCanDie = true;
}

void CActor::LoadAttachment( const c8* classname, const c8* configfilename )
{
    CActor* a = FACTORY->Actors.Create( classname, configfilename );
    a->attachToParentNode( ( CNewtonNode * )NewtonBodyGetUserData( body ), control );
    a->setDebugText( configfilename );
    CONSOLE.addx( "LoadAttachment %s", configfilename );
}

void CActor::Respawn()
{
    if ( !bRespawn )
    {
      return;
    }

    bool cam = false;

    // CLIENT: if this is my player then set the cam
    if ( WORLD.myPlayer )
    {
      if ( ownerPlayerID == WORLD.myPlayer->playerID )
      {
        cam = true;
      }
    }

    CPlayer* player = NULL;
    if ( ownerPlayerID != UNASSIGNED_PLAYER_ID )
    {
      //CONSOLE.addx("CActor::Respawn %i", WORLD.GetPlayers()->GetPlayer( ownerPlayerID ) );
      player = WORLD.GetPlayers()->GetPlayer( ownerPlayerID );
      //if ( player == NULL )
      //    return;
    }

    //CONSOLE.addx("LOL %i", WORLD.GetPlayer( ownerPlayerID ) );

    //WORLD.GetMap()->GetRespawn()->AddToQueue( factoryName.c_str(), configFilename, player );
    WORLD.GetRules()->OnPlayerQueue( factoryName.c_str(), configFilename, player );
}

CActor* CActor::getActorWithPlayerID( PlayerID pid )
{
    for ( int i = 0; i < actorsList.size(); i++ )
    {
      if ( actorsList[i]->getOwnersPlayerID() == pid )
      {
        return actorsList[i];
      }
    }
    return NULL;
}

void CActor::takeDamage( f32 fAmount, PlayerID whoID )
{
    recentDamageFromPlayerID = whoID;

    if ( whoID != UNASSIGNED_PLAYER_ID )
    {
      if ( WORLD.GetRules() )
      {
        WORLD.GetRules()->OnPlayerTakeDamage( WORLD.GetPlayers()->GetPlayer( ownerPlayerID ), WORLD.GetPlayers()->GetPlayer( recentDamageFromPlayerID ), fAmount );
      }
    }

    fHealth -= fAmount;
}

CActor* CActor::CreateActor( const c8* classname, const c8* scriptname, int control, int camerafollow, vector3df vPos, const c8* debugname )
{
    CActor* a = 0;
    a = FACTORY->Actors.Create( classname, scriptname );
    if ( a )
    {
      a->setPosition( vPos );
      a->setDebugText( debugname );
      if ( camerafollow )
      {
        WORLD.GetCamera()->setTarget( a );
      }

      if ( NET.rakServer->IsActive() )
      {
        a->Broadcast();
      }
    }

    return a;
}

void CActor::Broadcast()
{
    // send the actor to all players
    CGameServer::BroadcastActor( this );

    //send children
    for ( int i = 0; i < childAttachments.size(); i++ )
    {
      if ( ( childAttachments[i]->getType() != NODECLASS_DEFAULT ) && ( childAttachments[i]->getType() != NODECLASS_PROP ) )
      {
        CGameServer::BroadcastActor( static_cast<CActor*>( childAttachments[i] ) );
      }
    }
}

