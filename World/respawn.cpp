#include "respawn.h"

#include "../Game/SingletonIncludes.h"

#include "world.h"
#include "actor.h"
#include "player.h"
#include "rules.h"
#include "../RakNet/network.h"
#include "../RakNet/GameServer.h"

////////////////////////////////////////////
// CRespawnQueueActor 
////////////////////////////////////////////

CRespawnQueueActor::CRespawnQueueActor( String actorClassName, String actorConfigFileName, int time, CPlayer* p )
{
    actorName = actorClassName;
    respawnTime = time;
    player = p;
    if ( respawnTime < 0 )
    {
      waveRespawn = true;
    }
    else
    {
      waveRespawn = false;
    }
    actorConfigFile = actorConfigFileName;
    if ( player )
    {
      team = player->info.team;
    }
    else
    {
      team = 0;
    }
}

CRespawnQueueActor::~CRespawnQueueActor()
{
}

// ACTUAL RESPAWN OF ACTOR
void CRespawnQueueActor::Respawn( CRespawnPoint* point )
{
    CActor* a = 0;
    a = FACTORY->Actors.Create( actorName.c_str(), actorConfigFile.c_str() );
    if ( !a )
    {
      CONSOLE.addx( "Incorrect actor name on respawn: %s", actorName.c_str() );
      return;
    }
    if ( point )
    {
      a->setPosition( point->vPosition );
      a->setDirection( point->fDirection );
    }

    if ( player )
    {
      a->setOwnersPlayerID( player->playerID );
      a->setControls( player->getControls() );
      a->setDebugText( player->className );
    }
    else
    {
      a->setDebugText( a->getFactoryName() );
    }

    bool sent = false;
    // load parent attachment
    if ( point )
    {
      if ( ( point->parentActorName.size() > 1 ) && ( point->parentActorScriptName.size() > 1 ) )
      {
        CActor* pa = 0;
        pa = FACTORY->Actors.Create( point->parentActorName.c_str(), point->parentActorScriptName.c_str() );
        pa->setPosition( point->vPosition );
        pa->setDirection( point->fDirection ); 
        pa->setRespawnable( false );
        a->attachToParentNode( pa, a->getControls() );

        pa->Broadcast();
        sent = true;
      }
    }

    // send the actor to all players
    if ( !sent )
    {
      CGameServer::BroadcastActor( a );
    }


    WORLD.GetRules()->OnActorRespawn( a, point );
}

////////////////////////////////////////////
// CRespawnPoint 
////////////////////////////////////////////

CRespawnPoint::CRespawnPoint( vector3df vPos, String actorClassName, int teamId, f32 direction, String parentActorClassName, String parentActorScriptName )
{
    vPosition = vPos;
    actorName = actorClassName;
    team = teamId;
    radius = 0.5f;
    fDirection = direction;
    parentActorName = parentActorClassName;
    this->parentActorScriptName = parentActorScriptName;
}

CRespawnPoint::~CRespawnPoint()
{
}

////////////////////////////////////////////
// CRespawn 
////////////////////////////////////////////

CRespawn::CRespawn()
{
    Reset();
}

CRespawn::~CRespawn()
{
    int i;
    for ( i = 0; i < points.size(); i++ )
    {
      delete points[i];
    }
    points.clear();

    for ( i = 0; i < queue.size(); i++ )
    {
      delete queue[i];
    }
    queue.clear();
}

void CRespawn::Reset()
{
}

void CRespawn::Think()
{
    int i;
    for ( i = 0; i < queue.size(); i++ )
    {
      if ( !queue[i]->waveRespawn )
      {
        if ( queue[i]->respawnTime > 0 )
        {
          queue[i]->respawnTime--;
        }
      }
      else
      {
      }
    }

    for ( i = 0; i < queue.size(); i++ )
    {
      if ( !queue[i]->waveRespawn )
      {
        if ( queue[i]->respawnTime < 1 )
        {
          queue[i]->Respawn( FindSpawnPoint( queue[i] ) );
          delete queue[i];
          queue.erase( i );
          i = 0;
        }
      }
      else
      {
      }
    }
}

void CRespawn::Render()
{
    position2d<s32> pos;
    for ( int i = 0; i < points.size(); i++ )
    {
      WideString wstr = "(S) ";
      wstr += points[i]->getActorName().c_str();
      pos = IRR.smgr->getSceneCollisionManager()->getScreenCoordinatesFrom3DPosition( points[i]->getPosition(), IRR.smgr->getActiveCamera() );
      IRR.gui->getBuiltInFont()->draw( wstr.c_str(), core::rect<s32>( pos.X, pos.Y, pos.X + 100, pos.Y + 50 ), irr::video::SColor( 255, 15, 85, 10 ), false, true );
    }

    // draw 3d stuff
    IRR.video->setTransform( ETS_WORLD, matrix4() );

    SMaterial m; 
    m.Lighting = false; 
    m.BackfaceCulling = false;
    IRR.video->setMaterial( m );
    vector3df vP;
    for ( int i = 0; i < points.size(); i++ )
    {
      vP = points[i]->getPosition();
      IRR.video->draw3DBox( aabbox3df( vP - vector3df( points[i]->radius, points[i]->radius, points[i]->radius ), vP + vector3df( points[i]->radius, points[i]->radius, points[i]->radius ) ), SColor( 255, 105, 22, 90 ) );
    }
}

void CRespawn::AddPoint( vector3df vPos, String actorClassName, int teamId, f32 direction, String parentActorClassName, String parentActorScriptName )
{
    CRespawnPoint* p = new CRespawnPoint( vPos, actorClassName, teamId, direction, parentActorClassName, parentActorScriptName );
    points.push_back( p );
}

void CRespawn::AddToQueue( String actorClassName, String actorConfigfile, int time, CPlayer* p )
{
    CRespawnQueueActor* q = new CRespawnQueueActor( actorClassName, actorConfigfile, time, p );
    queue.push_back( q );
}

CRespawnPoint* CRespawn::FindSpawnPoint( CRespawnQueueActor* qactor )
{
    temppoints.clear();
    for ( int i = 0; i < points.size(); i++ )
    {
      if ( ( points[i]->getActorName() == qactor->actorName ) && ( points[i]->getTeam() == qactor->team ) )
      {
        temppoints.push_back( points[i] );
      }
    }

    if ( temppoints.size() > 0 )
    {
      return PickRandomPoint();
    }


    CONSOLE.addx( COLOR_WARNING, "No respawn point found for %s", qactor->actorName );
    return NULL;
}

int CRespawn::PickPointWithNoCollison( int count )
{
    int pick = random( temppoints.size() );

    vector3df vSpan;
    if ( count < 20 )
    {
      for ( int i = 0; i < CActor::actorsList.size(); i++ )
      {
        vSpan = CActor::actorsList[i]->getBoundingBox().getExtent() / 2;
        if ( ( CActor::actorsList[i]->getPosition() - temppoints[pick]->getPosition() ).getLength() < vSpan.getLength() )
        {
          pick = PickPointWithNoCollison( count + 1 );
          //CONSOLE.add("No");
          break;
        }
      }
    }

    return pick;
}

CRespawnPoint* CRespawn::PickRandomPoint()
{
    int pick = PickPointWithNoCollison( 0 );

    return temppoints[pick];
}