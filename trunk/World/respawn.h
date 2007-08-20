#ifndef RESPAWN_H_INCLUDED
#define RESPAWN_H_INCLUDED

#include "../Engine/engine.h"

class CPlayer;
class CRespawnPoint;

////////////////////////////////////////////
// CRespawnQueueActor 
////////////////////////////////////////////

class CRespawnQueueActor
{
  public:
    CRespawnQueueActor( String actorClassName, String actorConfigFileName, int time, CPlayer* p );
    virtual ~CRespawnQueueActor();

    void Respawn( CRespawnPoint* point );

    String actorName;
    String actorConfigFile;
    int respawnTime;
    bool waveRespawn;
    int team;

  protected:
    CPlayer* player;
};

////////////////////////////////////////////
// CRespawnPoint 
////////////////////////////////////////////

class CRespawnPoint
{
  public:
    CRespawnPoint( vector3df vPos, String actorClassName, int teamId, f32 direction, String parentActorClassName, String parentActorScriptName );
    virtual ~CRespawnPoint();

    vector3df getPosition()
    {
        return vPosition;
    }
    String getActorName()
    {
        return actorName;
    }
    int getTeam()
    {
        return team;
    }

    f32 radius;

    vector3df vPosition;

    //protected:
    String actorName;
    String parentActorName;
    String parentActorScriptName;
    int team;
    f32 fDirection;
};

////////////////////////////////////////////
// CRespawn 
////////////////////////////////////////////

class CRespawn
{
  public:
    CRespawn();
    virtual ~CRespawn();

    virtual void Reset();
    virtual void Think();
    virtual void Render();

    void AddPoint( vector3df vPos, String actorClassName, int teamId = 0, f32 direction = 0.0f, String parentActorClassName = "", String parentActorScriptName = "" );
    // time = -1; wave respawn
    void AddToQueue( String actorClassName, String actorConfigfile, int time, CPlayer* p = NULL );

    CRespawnPoint* GetPoint( s32 i )
    {
        if ( PointExists( i ) )
        {
          return points[i];
        }
        else
        {
          return NULL;
        }
    }
    s32 GetPointsNum()
    {
        return points.size();
    }
    bool PointExists( s32 i )
    {
        if ( ( i < 0 ) || ( i > ( s32 )points.size() - 1 ) )
        {
          return false;
        }
        else
        {
          return true;
        }
    }
    s32 GetPointIndex( CRespawnPoint* Point )
    {
        return points.binary_search( Point );
    }

  protected:
    friend class CEditor;
    friend class CMap;
    CRespawnPoint* CRespawn::FindSpawnPoint( CRespawnQueueActor* qactor );

    array<CRespawnPoint*> points;
    array<CRespawnQueueActor*> queue;

    // point picking
    int PickPointWithNoCollison( int count );
    CRespawnPoint* PickRandomPoint();
    array<CRespawnPoint*> temppoints;

    // wave respawn
};


#endif