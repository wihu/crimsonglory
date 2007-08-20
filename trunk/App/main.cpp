#include "stdafx.h"
#include "app.h"

#include "../RakNet/network.h"
#include "../Engine/log.h"
#include "misc.h"

#define ENGINE_NAME L"Crimson"
#define ENGINE_VERSION L"0.6"

#define GAME_MODULE "Game.dll"
#define DEFAULT_MODDIR "Base"

typedef bool ( __cdecl* cfunc )( ModuleImportExport, int, char*[] );

void CApplication::Run( int argc, char* argv[] )
{
    startupError = 0;
    buildName = ENGINE_NAME; 
    buildName += " (build: "; 
    buildName += MakeBuildNumber( __DATE__ ); 
    buildName += " version: "; 
    buildName += ENGINE_VERSION;
    buildName += ")"; 

    // get and set the working directory
    appDir = getDirectoryFromArgs( argc, argv );
    changeWorkingDir( appDir );

    //open logfiles
    log = new CAppLog( "Logs/app.txt", true );

    APPLOG.Write( wide2string( buildName ).c_str() );

    //create a couple of singletons
    network = new CNetwork();

    if ( argc > 1 )
    {
      modDir = argv[1];
    }
    else
    {
      modDir = DEFAULT_MODDIR;
    }
    modDir += "/";

    //load and run the game
    String dllFile = modDir;
    dllFile += GAME_MODULE;
    if ( !LoadGameDLL( dllFile.c_str(), argc, argv ) )
    {
      startupError = ERROR_DLL_NOTLOADED;
      APPLOG.Write( "Could not load game module %s", dllFile.c_str() );
    }
    // no script and console at this point

    for ( int i = 0; i < modules.size(); i++ )
    {
      if ( modules[i] )
      {
        FreeLibrary( ( HMODULE )modules[i] );
      }
    }

    //clean up singletons
    delete network;

    if ( startupError > 0 )
    {
      getchar();
    }
}

void CApplication::Shutdown()
{
    // close the game DLL
    GAMEPTR->Shutdown();
}

bool CApplication::LoadGameDLL( const char* filename, int argc, char* argv[] )
{
    HINSTANCE hLib = NULL;

    hLib = LoadLibrary( useFile( filename ).c_str() );

    if ( hLib )
    {
      modules.push_back( hLib );
      cfunc GameDLLInit = ( cfunc )GetProcAddress( ( HMODULE )hLib, "GameDLLInit" );
      ModuleImportExport ie;
      ie.app = CApplication::GetSingletonPtr();
      return GameDLLInit( ie, argc, argv );
    }

    return false;
}

bool CApplication::ImportDLLPointers( ModuleImportExport ie )
{
    game = ie.app;
    NET.serverGameProcess = ie.serverGameProcess;
    NET.clientGameProcess = ie.clientGameProcess;

    return true;
}

void CApplication::Restart()
{
    GAMEPTR->Restart();
}

//TODO: use callback when file not found?
String CApplication::useFile( const char* filename )
{
    String file;

    //if ( fileExists( filename ) )
    //  APPLOG.Write( "------------- '%s'  - %i", filename, fileExists2( filename ) );

    if ( fileExists2( filename ) )
    {
      file = filename;
      file.replace( '\\', '/' );
      if ( filesUsed.binary_search( file ) == -1 )
      {
        filesUsed.push_back( file );
        APPLOG.Write( "Using file '%s'", file.c_str() );
        //if ( (strstr(file.c_str(), ".gm")) == 0 )
        network->fileVerifier->AddFileForVerification( ( char* )file.c_str(), false );
      }
    }
    else
    {
      file = "../";
      file += DEFAULT_MODDIR;
      file += "/";
      file += filename;
      file.replace( '\\', '/' );
      if ( fileExists2( file.c_str() ) )
      {
        if ( filesUsed.binary_search( file ) == -1 )
        {
          filesUsed.push_back( file );
          APPLOG.Write( "File not found. Using default file '%s'", file.c_str() );

          //if ( (strstr(file.c_str(), ".gm")) == 0 )
          network->fileVerifier->AddFileForVerification( ( char* )file.c_str(), false );
        }
      }
      else
      {
        APPLOG.Write( "File not found '%s'", filename );
        //
      }
    }

    return file;
}

int main( int argc, char* argv[] )
{
    new CApplication();
    CApplication::GetSingleton().Run( argc, argv );
    delete CApplication::GetSingletonPtr();

    delete CAppLog::GetSingletonPtr();

    return 0;
}