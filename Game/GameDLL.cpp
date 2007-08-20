// GameDLL.cpp : Defines the entry point for the DLL application.
//

#include "GameDLL.h"
#include "../IrrConsole/console_vars.h"
#include "../GameMonkey/scriptGUIFunctions.h"
#include "../GameMonkey/scriptEditorFunctions.h"
#include "../Engine/misc.h"

//tasks
#include "../RakNet/NetworkTask.h"
#include "../Irrlicht/IrrlichtTask.h"
#include "../FreeSL/SoundTask.h"
#include "../GameMonkey/ScriptTask.h"
#include "../World/world.h"

#include "../RakNet/GameServer.h"
#include "../RakNet/GameClient.h"

#include "precache.h"

#define GAMEDLL_NAME L"Glory"
#define GAMEDLL_VERSION L"0.121a"

#define AUTOCONFIG_FILE "Scripts/autoconfig.gm"
#define GAMECONFIG_FILE "Scripts/config.gm"
#define PRECACHE_FILE "precache.txt"
#define AUTOSTART_FILE "Scripts/autostart.gm"

DLLEXPORT
bool GameDLLInit( ModuleImportExport ie, int argc, char* argv[] )
{
    // set engine singleton pointers
    CApplication::SetSingletonPtr( ( CApplication * )ie.app );
    CAppLog::SetSingletonPtr( ie.app->log );
    CNetwork::SetSingletonPtr( ie.app->network );

    CScript* script = new CScript();
    IC_MainConsole* console = new IC_MainConsole();

    //
    CONSOLE.add( "Game DLL initialized." );

    //change working directory to mod
    changeWorkingDir( APP.modDir );
    CONSOLE.addx( "Changing game working directory to '%s%s'", APP.appDir.c_str(), APP.modDir.c_str() );

    // create the Game
    new CGame();

    // create other objects
    CGameServer* serverProcess = new CGameServer();
#ifdef _CLIENT
    CGameClient* clientProcess = new CGameClient();
#endif

    // return singleton pointers
    ModuleImportExport ex;
    ex.app = CGame::GetSingletonPtr();
    ex.serverGameProcess = serverProcess;
#ifdef _CLIENT
    ex.clientGameProcess = clientProcess;
#endif
    APP.ImportDLLPointers( ex );

    // run it!
    CGame::GetSingleton().Run( argc, argv );

    // destroy created objects
    delete CGame::GetSingletonPtr();
    delete serverProcess;
#ifdef _CLIENT
    delete clientProcess;
#endif

    //change working directory to apps
    changeWorkingDir( APP.appDir );

    //clean up any remaining unreleased objects
    IMMObject::CollectRemainingObjects( ( bool )APP.DebugMode );

    delete script;
    delete console;

    return true;
}

static void SetupCVars()
{
    // APP
    CONSOLE_VAR( "g_debug", int, APP.DebugMode, 0, L"g_debug [0/1]. Ex. g_debug 1", L"Sets the level of debug information (0 - none)." );
    CONSOLE_VAR( "g_precache", bool, GAME.bPrecache, 0, L"g_precache [0/1]. Ex. g_precache 1", L"Are resources preloaded at game start?" );

    // GAME
    CONSOLE_VAR( "k_goalticks", int, GAME.goalTicks, 60, L"k_goalticks [ticks]. Ex. k_goalticks 60", L"Determines how many ticks per second the game engine is running." );

    // NET
    CONSOLE_VAR( "n_disconnectwait", int, NET.disconnectTime, 1000, L"n_disconnectwait [ms]. Ex. n_disconnectwait 1000", L"The time to wait before truly close the communication." );
    CONSOLE_VAR( "sv_maxplayers", int, NET.maxPlayers, 32, L"sv_maxplayers [1-255]. Ex. sv_maxplayers 32", L"Maximum number of players allowed ingame." );
    CONSOLE_VAR( "sv_port", int, NET.serverPort, 50008, L"sv_port [1-65000]. Ex. sv_port 50008", L"Server communication port, this is where it listens for incoming clients. Remember to open this port in your firewall." );
    CONSOLE_VAR( "cl_port", int, NET.clientPort, 50018, L"cl_port [1-65000]. Ex. cl_port 50018", L"Client communication port." );
    CONSOLE_VAR( "n_sleeptime", int, NET.threadSleepTime, 1, L"n_sleeptime [ms]. Ex. n_sleeptime 30", L"Network thread sleep time, can solve problems with cpu usage." );

    // EFFECTS
    CONSOLE_VAR( "e_waterreflection", bool, GAME.bShaderWater, 1, L"e_waterreflection [0/1]. Ex. e_waterreflection 1", L"Pixel shader water reflection effect" );
    CONSOLE_VAR( "e_waterreflection_detail", int, GAME.shaderWaterDetail, 1000, L"e_waterreflection_detail [integer]. Ex. e_waterreflection_detail 1000", L"Pixel shader water reflection polygon detail. The lower the more polygons in use. May crash if set too low.." );
}

void CGame::Run( int argc, char* argv[] )
{
    startupError = 0;
    buildName = APP.buildName;
    buildName += " "; 
    buildName += GAMEDLL_NAME; 
    buildName += " (build: "; 
    buildName += MakeBuildNumber( __DATE__ ); 
    buildName += " version: "; 
    buildName += GAMEDLL_VERSION;
    buildName += ")"; 

    InitRandomNumbers();

    // bind GM script functions
    BindScriptFunctions();
    BindScriptGUIFunctions();
    BindScriptEditorFunctions();

    //load autoconfig script
    if ( !SCRIPT.RunFile( AUTOCONFIG_FILE ) )
    {
      startupError = ERROR_AUTOCONFIG_NOTLOADED;
      CONSOLE.addx( COLOR_ERROR, "Could not load autoconfig file %s", AUTOCONFIG_FILE );
    }

    //setup critical variables
    SetupCVars();

    //set up the profiler with an output handler
    CProfileLogHandler profileLogHandler;
    CProfileSample::outputHandler = &profileLogHandler;

    //load game specific config
    SCRIPT.RunFile( GAMECONFIG_FILE );

    kernel = new CKernel();
    kernel->SetGoalTicks( goalTicks );

    tasksAdded = false;
    worldLoaded = false;
    AddMainTasks();

    // precache
    CPrecache* precache = new CPrecache();
    if ( bPrecache )
    {
      precache->LoadFromFile( PRECACHE_FILE );
    }

    // run main GUI script
    SCRIPT.RunFile( AUTOSTART_FILE );

    //main game loop
    KERNEL.Execute();

    //save final settings
    if ( startupError == 0 )
    {
      CONSOLE.SaveVariablesToFile( AUTOCONFIG_FILE );
      precache->SaveToFile( PRECACHE_FILE );
    }

    delete precache;

    //clean up singletons

    delete kernel;
}

void CGame::Shutdown()
{
    KERNEL.KillAllTasks();
}

void CGame::Restart()
{
    tasksAdded = false;
}

bool CGame::AddMainTasks()
{
    if ( tasksAdded )
    {
      return false;
    }

    // framerate independent
    scriptTask = new CScriptTask();
    ADDTASK( scriptTask, 10, true );

    networkTask = new CNetworkTask();
    ADDTASK( networkTask, 25, true );

    interpolatorsTask = new CInterpolatorTask();
    ADDTASK( interpolatorsTask, 30, true );

    triggersTask = new CTriggerTask();
    ADDTASK( triggersTask, 35, true );

    timersTask = new CTimerTask();
    ADDTASK( timersTask, 37, true );

    sound = soundTask = new CSoundEngine();
    ADDTASK( soundTask, 40, true );

    // framerate dependent (rendering etc.)
    irrlicht = irrlichtTask = new CIrrlichtTask();
    ADDTASK( irrlichtTask, 20, false );

    tasksAdded = true;
    return tasksAdded;
}

void CGame::CreateNewWorld( const c8* fileName )
{
    //if ( APP.DebugMode )
    CONSOLE.addx( "Creating world from file '%s'", fileName );
    if ( worldTask )
    {
      if ( APP.DebugMode )
      {
        CONSOLE.add( "Restarting world" );
      }

      worldTask->Stop();
      worldTask->Start();
      return;
    }
    worldTask = new CWorldTask();
    ADDTASK( worldTask, 50, true );

    worldLoaded = true;
}
