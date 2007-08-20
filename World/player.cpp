#include "player.h"

#include "../Game/SingletonIncludes.h"

#include "rules.h"
#include "actor.h"

////////////////////////////////////////////
// CPlayer 
////////////////////////////////////////////

CPlayer::CPlayer( PlayerID pID )
{
    playerID = pID;
    //  bCustomControls = localPlayer;

    //if (!localPlayer)
    //{
    //  controls = new CControls();
    //  bCustomControls = true;
    //}
    //else
    //{
    //  controls = &myControls;
    //  bCustomControls = false;
    //}

    controls = new CControls();
    bCustomControls = true;

    //kills = 0;
    //deaths = 0;
    //score = 0;
    //ping = 0;
    stats.set_used( 4 );
    ResetStats();

    info.name = "Unnamed player";
    info.team = 0;
}

CPlayer::~CPlayer()
{
    if ( bCustomControls )
    {
      delete controls;
    }

    stats.clear();
}

void CPlayer::ResetStats()
{
    for ( int i = 0; i < stats.size(); i++ )
    {
      stats[i] = 0;
    }
}

void CPlayer::setControls( CControls* c )
{
    bCustomControls = false;
    // FUCKED   delete controls;
    controls = c;
}


////////////////////////////////////////////
// CPlayerManager
////////////////////////////////////////////

CPlayerManager::CPlayerManager()
{
}

CPlayerManager::~CPlayerManager()
{
    for ( int i = 0; i < Players.size(); i++ )
    {
      delete Players[i];
    }
    Players.clear();
}

CPlayer* CPlayerManager::GetPlayer( PlayerID playerID )
{
    //CONSOLE.addx( "GetPlayer num %i", Players.size() );
    for ( int i = 0; i < Players.size(); i++ )
    {
      if ( Players[i]->playerID == playerID )
      {
        return Players[i];
      }
    }

    return NULL;
}

CPlayer* CPlayerManager::GetPlayerByControls( CControls* c )
{
    //CONSOLE.addx( "GetPlayerByControls num %i", Players.size() );
    for ( int i = 0; i < Players.size(); i++ )
    {
      if ( Players[i]->getControls() == c )
      {
        return Players[i];
      }
    }

    return NULL;
}


CPlayer* CPlayerManager::AddPlayer( PlayerID playerID )
{
    CPlayer* p = 0;
    if ( APP.DebugMode )
    {
      CONSOLE.add( "CPlayerManager::AddPlayer" );
    }
    // check if is here already
    p = GetPlayer( playerID );
    if ( !p )
    {
      p = new CPlayer( playerID );
      if ( APP.DebugMode )
      {
        CONSOLE.add( "AddPlayer" );
      }
      Players.push_back( p );
    }
    else
    {
      if ( APP.DebugMode )
      {
        CONSOLE.add( "AddPlayer - duplicate" );
      }
    }


    return p;
}

void CPlayerManager::RemovePlayer( PlayerID playerID )
{
    //  if (APP.DebugMode)
    //      CONSOLE.add( "CPlayerManager::RemovePlayer" );
    CPlayer* p = GetPlayer( playerID );
    if ( !p )
    {
      //        if (APP.DebugMode)
      //            CONSOLE.add( "Player already removed" );
      return;
    }

    if ( WORLD.GetRules() )
    {
      WORLD.GetRules()->OnPlayerLeave( p );
    }

    if ( WORLD.myPlayer == p )
    {
      WORLD.myPlayer = NULL;
    }

    for ( int i = 0; i < CActor::actorsList.size(); i++ )
    {
      if ( CActor::actorsList[i]->getControls() == p->getControls() )
      {
        CActor::actorsList[i]->setControls( NULL );
      }
    }


    int i = -1;
    i = Players.binary_search( p );
    if ( i > -1 )
    {
      Players.erase( i );
      delete p;
    }

    //  if (APP.DebugMode)
    //      CONSOLE.add( "RemovePlayer" );
}

void CPlayerManager::DumpToConsole()
{
    CONSOLE.add( "Listing players: " );
    String debug;
    for ( int i = 0; i < Players.size(); i++ )
    {
      //TEMP: take ping 
      if ( NET.rakServer->IsActive() )
      {
        Players[i]->stats[PS_PING] = NET.rakServer->GetLastPing( Players[i]->playerID );
      }

      CONSOLE.addx( "%i %s - score(%i) kills(%i) deaths(%i) ping(%i) playerID(%u:%u)", i, Players[i]->info.name.c_str(), Players[i]->stats[0], Players[i]->stats[1], Players[i]->stats[2], Players[i]->stats[3], Players[i]->playerID.binaryAddress, Players[i]->playerID.port );
    }
}

String CPlayerManager::getPlayerName( PlayerID playerID )
{
    String name = "???";
    CPlayer* player = 0;
    player = GetPlayer( playerID );
    if ( player )
    {
      name = player->info.name.c_str();
    }

    return name;
}
