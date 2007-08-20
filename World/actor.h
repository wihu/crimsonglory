#ifndef ACTOR_H_INCLUDED
#define ACTOR_H_INCLUDED

#include "../Engine/engine.h"
#include "controllable.h"
#include "../Newton/newton_physics.h"
#include "../Newton/newton_node.h"

// IRR.
#include "../irrlicht/IrrlichtTask.h"
// CONSOLE.
#include "../IrrConsole/console.h"
#include "../IrrConsole/console_vars.h"

#include "../RakNet/BitStream.h"


class CScreenText;

////////////////////////////////////////////
// CActor
////////////////////////////////////////////

class CActor : public CControllable, public CNewtonNode
{
  public:
    CActor();
    CActor( const c8* scriptFilename );
    virtual ~CActor();

    virtual void Reset();                           
    virtual void Think();
    virtual void Render();

    virtual void Respawn();

    bool isOnGround()
    {
        return bOnGround;
    }
    bool isStopped()
    {
        return bStopped;
    }

    int getTeam()
    {
        return team;
    }

    String getFactoryName()
    {
        return factoryName;
    }
    int getFactoryIndex()
    {
        return factoryIndex;
    }

    PlayerID getOwnersPlayerID()
    {
        return ownerPlayerID;
    }
    void setOwnersPlayerID( PlayerID pid )
    {
        ownerPlayerID = pid;
    }

    virtual void takeDamage( f32 fAmount, PlayerID fromWhoID );

    virtual void setDirection( f32 angle )
    {
    }
    vector3df vAxisRotation, vOldAxisRotation;

    virtual void Serialize( RakNet::BitStream& bt )
    {
        bt.Write( &loadedBitStream );
    }
    virtual void Unserialize( RakNet::BitStream& bt )
    {
    }

    int getHealth()
    {
        return fHealth;
    }

    virtual void setRespawnable( bool respawn )
    {
        bRespawn = respawn;
    }

    virtual void Broadcast();

    CScreenText* aboveText;

    // STATIC
    static CActor* getActorWithPlayerID( PlayerID pid );
    static array<CActor*> actorsList;

    // is network broadcasted!
    static CActor* CreateActor( const c8* classname, const c8* scriptname, int control, int camerafollow, vector3df vPos, const c8* debugname );


  protected:
    virtual void Load( const c8* filename )
    {
    }

    virtual bool onChildAttached( CNewtonNode* what, void* data );
    virtual void onChildUnAttached( CNewtonNode* what );

    virtual void LoadZombie( matrix4 matrix )
    {
    }

    virtual void LoadAttachment( const c8* classname, const c8* configfilename );

    virtual void ZombieDie();

    f32 fHealth;

    bool bOnGround, bStopped;
    f32 fRotationRate;

    String configFilename;
    String factoryName;
    int factoryIndex;
    RakNet::BitStream loadedBitStream;

    int team;
    PlayerID ownerPlayerID;

    // the player that inflicted the most recent damage to this actor, used for counting frags
    PlayerID recentDamageFromPlayerID;

    //will this actor respawn after death?
    bool bRespawn;
};

// CONFIG LOADING MACROS
#define LOAD_STRING( gmvarname )                                    \
{                                                                   \
String filename;                                                    \
GMVARTOSTRING( gmvarname, filename );                               \
loadedBitStream.Write( (int)filename.size() );                      \
loadedBitStream.Write( filename.c_str(), (int)filename.size() );    \
}                                                                   \

#define LOAD_VALUE( gmvarname, vartype )                            \
{                                                                   \
vartype var;                                                        \
GMVARTOVALUE( gmvarname, vartype, var );                            \
loadedBitStream.Write( var );                                       \
}                                                                   \

#define LOAD_VECTOR( gmvarnameX, gmvarnameY, gmvarnameZ )           \
{                                                                   \
vector3df vector;                                                   \
GMVARTOVALUE( gmvarnameX, f32, vector.X );                          \
GMVARTOVALUE( gmvarnameY, f32, vector.Y );                          \
GMVARTOVALUE( gmvarnameZ, f32, vector.Z );                          \
loadedBitStream.WriteVector( vector.X, vector.Y, vector.Z );        \
}                                                                   \

#define UNLOAD_STRING                                               \
bt.Read( stringLength );                                            \
inString = new char[ stringLength + 1 ];                            \
bt.Read( inString, stringLength );                                  \
inString[ stringLength ] = '\0';                                    \


#endif