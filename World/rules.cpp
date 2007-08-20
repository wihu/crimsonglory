#include "rules.h"

#include "../Game/SingletonIncludes.h"

#include "player.h"
#include "map.h"
#include "respawn.h"
#include "actor.h"
#include "../RakNet/GameServer.h"


#include "../IrrConsole/console_vars.h"

////////////////////////////////////////////
// CRules 
////////////////////////////////////////////

CRules::CRules( const c8* scriptFilename )
{
    SCRIPT.RunFile( scriptFilename );

    SCRIPT.gmTableToStringArray( "rules_PlayerClassActor", PlayerClassActors );
    SCRIPT.gmTableToStringArray( "rules_PlayerClassNames", PlayerClassNames );
    SCRIPT.gmTableToStringArray( "rules_PlayerClassScripts", PlayerClassScripts );
    GMVARTOVALUE( "rules_DefaultClass", int, defaultClass );
    GMVARTOVALUE( "rules_WaveRespawn", int, useWaveRespawn );
    GMVARTOVALUE( "rules_PointsOnDeath", int, deathPoints );
    GMVARTOVALUE( "rules_PointsOnKill", int, killPoints );
    GMVARTOVALUE( "rules_PointsOnSelfkill", int, selfkillPoints );
    GMVARTOVALUE( "rules_DamageModifier", f32, damageModifier );

    // override wave respawn console settings
    //int temp;
    //GMVARTOVALUE( "rules_WaveRespawn", int, temp );
    //WideString wstr = "console(`\\r_waverespawn_on ";
    //wstr += temp; wstr += "`);";
    //SCRIPT.RunString( wstr );
    //CONSOLE.add( wstr );
    //CONSOLE.add( wstr );
    //CONSOLE.add( wstr );
    //CONSOLE.add( wstr );
    //CONSOLE.add( wstr );
    //CONSOLE.add( wstr );

    CONSOLE.addx( "Loaded game rules from %s %i", scriptFilename, PlayerClassNames.size() );

    if ( APP.DebugMode )
    {
      for ( int i = 0; i < PlayerClassNames.size(); i++ )
      {
        CONSOLE.add( PlayerClassNames[i].c_str() );
      }
    }
}

CRules::~CRules()
{
}

void CRules::Think()
{
}

void CRules::OnNewPlayerJoin( CPlayer* player )
{
    if ( !player )
    {
      return;
    }
    CONSOLE.addx( "Player %s joined the game", player->info.name );

    //TEMP: player should choose the class
    player->className = PlayerClassNames[defaultClass];
    if ( WORLD.GetMap() )
    {
      WORLD.GetMap()->GetRespawn()->AddToQueue( PlayerClassActors[defaultClass], PlayerClassScripts[defaultClass], 5, player );
    }
}

void CRules::OnPlayerLeave( CPlayer* player )
{
    if ( !player )
    {
      return;
    }
    // death to the players actor
    CActor* actor = 0;
    actor = CActor::getActorWithPlayerID( player->playerID );
    if ( actor )
    {
      actor->takeDamage( actor->getHealth(), UNASSIGNED_PLAYER_ID );
      actor->setRespawnable( false );
    }
}

void CRules::OnPlayerQueue( String actorClassName, String actorConfigfile, CPlayer* p )
{
    if ( actorClassName == "hydroplane" )
    {
      return;
    }

    float time = 0;
    if ( p == NULL )
    {
      time = ( actorRespawnTime * GAME.goalTicks );
    }
    else
    {
      time = ( playerRespawnTime * GAME.goalTicks );
    }

    if ( WORLD.GetMap() )
    {
      WORLD.GetMap()->GetRespawn()->AddToQueue( actorClassName, actorConfigfile, ( int )time, p );
    }
}

void CRules::OnPlayerDie( CPlayer* victim, CPlayer* attacker )
{
    if ( victim )
    {
      if ( victim != attacker )
      {
        victim->stats[PS_SCORE] += deathPoints;
      }
      else
      {
        victim->stats[PS_SCORE] += selfkillPoints;
        //          CONSOLE.addx ("KILL %i", victim->stats[ PS_SCORE ] );
      }
      victim->stats[PS_DEATHS] += 1;
    }

    if ( attacker )
    {
      if ( victim != attacker )
      {
        attacker->stats[PS_SCORE] += killPoints;
        attacker->stats[PS_KILLS] += 1;
      }
      else
      {
      }
    }

    // message
    String name, name2;

    if ( victim )
    {
      name = victim->info.name;
    }
    else
    {
      name = "???";
    }//factoryName;

    if ( attacker )
    {
      name2 = attacker->info.name;
    }
    else
    {
      name2 = "???";
    }

    CONSOLE.addx( "%s destroyed by %s", name.c_str(), name2.c_str() );
}

void CRules::OnPlayerTakeDamage( CPlayer* victim, CPlayer* attacker, f32& fAmount )
{
    fAmount = fAmount * damageModifier;
}

void CRules::OnActorRespawn( CActor* actor, CRespawnPoint* point )
{
}