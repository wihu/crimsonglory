#ifndef BOT_H_INCLUDED
#define BOT_H_INCLUDED

#include "entity.h"
#include "controls.h"
#include "../RakNet/network.h"
#include "player.h"
#include "actor.h"

enum BotStates { BOT_DEFAULT, BOT_SEEK_TARGET, BOT_IN_MACHINE };

class CBot : public CEntity
{
  public:
    CBot();
    virtual ~CBot();

    virtual void Reset();
    virtual void Think();
    virtual void Render();

    virtual void ChangeState();

  protected:
    PlayerID playerId;
    CPlayer* player;
    BotStates state;
    int seekType;
    CActor* target;
    CActor* myActor;

    CActor* FindTarget( int seeknodetype );
    CActor* FindMyActor();
    f32 getTargetDistance();

    // may not be up to date
    f32 targetDistance; 
    vector3df headingPos;
    vector3df vTargetDiff;

  private:
};

#endif