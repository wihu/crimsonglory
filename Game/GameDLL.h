#ifndef GAMEDLL_H_INCLUDED
#define GAMEDLL_H_INCLUDED

#include "../Engine/engine.h"
#include "../App/app.h"

#define DLLEXPORT extern "C" __declspec(dllexport)

DLLEXPORT bool GameDLLInit( ModuleImportExport ie, int argc, char* argv[] );


class CKernel;
class CIrrlichtTask;
class CScriptTask;
class CTriggerTask;
class CTimerTask;
class CSoundEngine;
class CNetworkTask;
class CRender;
class CGameServer;
class CGameClient;
class CScript;
class IC_Console;
class CWorldTask;

#define GAME CGame::GetSingleton()

class CGame : public Singleton<CGame>, public ICApplication
{
  public:
    //singleton pointers
    CScript* script;
    IC_Console* console;
    CKernel* kernel;
    CIrrlichtTask* irrlicht;
    CSoundEngine* sound;
    CRender* renderer;

    virtual void Run( int argc, char* argv[] );
    virtual void Shutdown();
    virtual void Restart();

    bool AddMainTasks();

    // if already created destroys first
    void CreateNewWorld( const c8* fileName );
    bool worldLoaded;

    int goalTicks;
    bool bPrecache;
    bool bShaderWater;
    int shaderWaterDetail;

  private:
    // engine tasks
    CMMPointer<CIrrlichtTask> irrlichtTask;
    CMMPointer<CScriptTask> scriptTask;
    CMMPointer<CInterpolatorTask> interpolatorsTask;
    CMMPointer<CTriggerTask> triggersTask;
    CMMPointer<CTimerTask> timersTask;
    CMMPointer<CSoundEngine> soundTask;
    CMMPointer<CNetworkTask> networkTask;
    // game tasks
    CMMPointer<CWorldTask> worldTask;

    bool tasksAdded;
};

#endif