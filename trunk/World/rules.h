#ifndef RULES_H_INCLUDED
#define RULES_H_INCLUDED

#include "../Engine/engine.h"

#include "entity.h"
#include "../RakNet/network.h"

class CPlayer;
class CActor;
class CRespawnPoint;

////////////////////////////////////////////
// CRules 
////////////////////////////////////////////

class CRules : public CEntity
{
  public:
    CRules( const c8* scriptFilename );
    virtual ~CRules();

    virtual void Think();

    virtual void OnNewPlayerJoin( CPlayer* player );
    virtual void OnPlayerLeave( CPlayer* player );
    // on adding actor to respawn queue
    virtual void OnPlayerQueue( String actorClassName, String actorConfigfile, CPlayer* p );
    // when actor takes damage from another
    virtual void OnPlayerTakeDamage( CPlayer* victim, CPlayer* attacker, f32& fAmount );
    // when player dies, we know here who killed who
    virtual void OnPlayerDie( CPlayer* victim, CPlayer* attacker );
    // on actor respawn
    virtual void OnActorRespawn( CActor* actor, CRespawnPoint* point );

    f32 playerRespawnTime, waveRespawnTime, actorRespawnTime;
    int useWaveRespawn;

  private:
    array<String> PlayerClassActors;
    array<String> PlayerClassNames;
    array<String> PlayerClassScripts;

    int defaultClass;
    int killPoints, deathPoints, selfkillPoints;
    f32 damageModifier;
};

#endif