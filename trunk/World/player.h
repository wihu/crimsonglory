#ifndef PLAYER_H_INCLUDED
#define PLAYER_H_INCLUDED

#include "../Engine/engine.h"

#include "../RakNet/network.h"
#include "controls.h"

#define PLAYERNAME_LENGTH 40

struct PlayerInfo
{
    String name;
    u16 team;
};

enum PlayerStats { PS_SCORE, PS_KILLS, PS_DEATHS, PS_PING, PS_NUM };

//struct PlayerStats
//{
//  u16 score;
//  u16 kills;
//  u16 deaths;
//    u16 ping;
//};

////////////////////////////////////////////
// CPlayer 
////////////////////////////////////////////

class CPlayer
{
  public:
    CPlayer( PlayerID pID );
    virtual ~CPlayer();

    void ResetStats();

    PlayerID playerID;
    array<s16> stats;
    PlayerInfo info;

    CControls* getControls()
    {
        return controls;
    }
    void setControls( CControls* c );

    String className;

  private:
    CControls* controls;
    bool bCustomControls;
};

////////////////////////////////////////////
// CPlayerManager
////////////////////////////////////////////

class CPlayerManager
{
  public:
    CPlayerManager();
    virtual ~CPlayerManager();

    CPlayer* GetPlayer( PlayerID playerID );    
    CPlayer* GetPlayerByControls( CControls* c );
    CPlayer* AddPlayer( PlayerID playerID );
    void RemovePlayer( PlayerID playerID );

    void DumpToConsole();

    String getPlayerName( PlayerID playerID );

  private:
    array<CPlayer*> Players;
};

#endif