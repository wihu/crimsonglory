#ifndef GAME_H_INCLUDED
#define GAME_H_INCLUDED

#include "stdafx.h"
#include "../Engine/singleton.h"

#define ERROR_AUTOCONFIG_NOTLOADED 1;
#define ERROR_DLL_NOTLOADED 2;

class CAppLog;
class CNetwork;
class IProcessPacket;

class ICApplication
{
  public:
    WideString buildName;

    int startupError;

    //singleton pointers
    CAppLog* log;
    CNetwork* network;

    virtual void Run( int argc, char* argv[] ) = 0;
    virtual void Shutdown() = 0;
    virtual void Restart() = 0;
  private:
};

struct ModuleImportExport
{
    ICApplication* app;

    IProcessPacket* serverGameProcess;
    IProcessPacket* clientGameProcess;
};


#define APP CApplication::GetSingleton()
#ifndef _USRDLL
#define GAMEPTR if (APP.game) APP.game
#endif

class CApplication : public Singleton<CApplication>, public ICApplication
{
  public:
    int DebugMode;
    String modDir, appDir;

    ICApplication* game;

    virtual void Run( int argc, char* argv[] );
    virtual void Shutdown();
    virtual void Restart();

    virtual bool LoadGameDLL( const char* filename, int argc, char* argv[] );
    virtual bool ImportDLLPointers( ModuleImportExport ie );

    // use this whenever using a file to return the name, if file is not found file from default dir is returned, also changes \ to /
    virtual String useFile( const char* filename );
    irr::core::array<String> filesUsed;

  private:
    irr::core::array<HINSTANCE> modules;

    // system
    virtual String getDirectoryFromArgs( int argc, char* argv[] );
};

#endif