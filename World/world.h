#ifndef WORLD_H_INCLUDED
#define WORLD_H_INCLUDED

#include "../Engine/engine.h"

class CNewton;
class CWorldRender;
class CMap;
class CEntity;
class CPhys_Skeleton;
class CPhys_Part;
class CNewtonNode;
class CCamera;
class CEntity;
class CActor;
class CEffect;
class CRespawn;
class CPlayer;
class CPlayerManager;
struct PlayerID;
class CRules;

////////////////////////////////////////////
// CWorldTask 
////////////////////////////////////////////

#define WORLD CWorldTask::GetSingleton()

class CWorldTask : public Singleton<CWorldTask>, public ITask
{
  public:
    CWorldTask();
    virtual ~CWorldTask();
    AUTO_SIZE;

    virtual bool Start();
    virtual void Update();
    virtual void Stop();

    CNewton* GetPhysics()
    {
        return newtonTask;
    }
    CMap* GetMap()
    {
        return map;
    }
    CCamera* GetCamera()
    {
        return camera;
    }
    CPlayerManager* GetPlayers()
    {
        return players;
    }
    CRules* GetRules()
    {
        return rules;
    }

    CEntity* GetEntity( s32 i )
    {
        if ( EntityExists( i ) )
        {
          return Entitys[i];
        }
        else
        {
          return NULL;
        }
    }
    s32 GetEntitysNum()
    {
        return Entitys.size();
    }
    bool EntityExists( s32 i )
    {
        if ( ( i < 0 ) || ( i > ( s32 )Entitys.size() - 1 ) )
        {
          return false;
        }
        else
        {
          return true;
        }
    }
    s32 GetEntityIndex( CEntity* Entity )
    {
        return Entitys.binary_search( Entity );
    }

    void AddEntity( CEntity* e );
    void RemoveEntity( CEntity* e );

    f32 getDaySpeed()
    {
        return fDayspeed;
    }

    // client stuff
    CPlayer* myPlayer;

  private:
    CMMPointer<CNewton> newtonTask;
    CMMPointer<CWorldRender> worldRender;
    CMap* map;
    CCamera* camera;
    CPlayerManager* players;
    CRules* rules;

    array<CEntity*> Entitys;

    // TEMP?
    f32 fCamPosLag, fCamTargetLag, fCamDistance, fCamSpeedFactor, fCamMountFactor;
    int iCamOrtho;
    f32 playerRespawnTime, waveRespawnTime, actorRespawnTime;
    //int useWaveRespawn;

	f32 fDayspeed;
};

////////////////////////////////////////////
// CWorldRender 
////////////////////////////////////////////
class CWorldRender : public CRenderable
{
  public:
    CWorldRender() : CRenderable( RENDER_UNDER_GUI )
    {
    }
    virtual ~CWorldRender()
    {
    }
    AUTO_SIZE;

    virtual void Render();

  private:
};

////////////////////////////////////////////
// CCamera 
////////////////////////////////////////////
class CCamera// : public CEntity

{
  public:
    CCamera( CWorldTask* w );
    virtual ~CCamera();

    ICameraSceneNode* getIrrCamera()
    {
        return IrrCamera;
    }

    virtual void Reset();
    virtual void Think();
    virtual void Render()
    {
    }

    virtual void setTarget( CNewtonNode* t )
    {
        target = t;
    }
    virtual CNewtonNode* getTarget()
    {
        return target;
    }

    virtual void setTargetPos(vector3df pos)
    {
        vSetTargetPos = pos;
		bSetCamera = true;
    }

    virtual void setPos(vector3df pos)
    {
        vSetPos = pos;
		bSetCamera = true;
    }

    bool bOverrideX, bOverrideY, bOverrideZ;
    vector3df vOverride;

    f32 fPosLag, fTargetLag, fSpeedFactor, fMountFactor;
    int iOrtho;

    f32 spectateMoveFactor;

  private:
    ICameraSceneNode* IrrCamera;
    CWorldTask* world;
    CNewtonNode* target;

    matrix4 ProjMatrix;

    vector3df vRealPos, vRealTargetPos;
    vector3df vOldPos;
    vector3df vSetPos, vSetTargetPos;

	bool bSetCamera;
};

////////////////////////////////////////////
// CFactories 
////////////////////////////////////////////

#define FACTORY Factories

class CFactories
{
  public:
    static CFactories* CFactories::Instance()
    {
        static CFactories inst;
        return &inst;
    }

    ObjectFactory<CActor *( const c8* ), String> Actors;
    ObjectFactory<CEffect *( vector3df, vector3df, f32, f32, int ), String> Effects;
};

// CFactories Singleton
static CFactories* Factories = CFactories::Instance();


#endif