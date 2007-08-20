#include "scriptFunctions.h"
#include "../Game/GameDLL.h"
#include "../Game/SingletonIncludes.h"

#include "../World/controls.h"
#include "../World/player.h"
#include "../World/map.h"
#include "../World/bot.h"

#include "../RakNet/GameServer.h"

#include "../Entities/EntityIncludes.h"

//\\//\\//\\//\\//\\//\\//\\//\\//\\//\\//\\//\\//\\//\\//\\//\\//\\//\\

// SCRIPTBIND(gmTest, "Test");
int GM_CDECL gmTest( gmThread* a_thread )
{
    CONSOLE.add( "Test!" );
    return GM_OK;
}


// SCRIPTBIND(gmLoadModule, "LoadModule");
int GM_CDECL gmLoadModule( gmThread* a_thread )
{
    GM_CHECK_NUM_PARAMS( 1 ); 
    GM_CHECK_STRING_PARAM( strParam1, 0 ); 

    //    APP.LoadGameDLL( strParam1 );

    return GM_OK;
}

// SCRIPTBIND(gmConsole, "Console");
int GM_CDECL gmConsole( gmThread* a_thread )
{
    GM_CHECK_NUM_PARAMS( 1 ); 
    GM_CHECK_STRING_PARAM( strParam1, 0 ); 

    CONSOLE.externalCommandString( strParam1 );

    return GM_OK;
}

// SCRIPTBIND(gmQuit, "Quit");
int GM_CDECL gmQuit( gmThread* a_thread )
{
    APP.Shutdown();

    return GM_OK;
}

// SCRIPTBIND(gmConnect, "Connect");
int GM_CDECL gmConnect( gmThread* a_thread )
{
    GM_CHECK_NUM_PARAMS( 2 ); 
    GM_CHECK_STRING_PARAM( IP, 0 );     
    GM_CHECK_INT_PARAM( port, 1 );

    NET.Connect( IP, port );

    return GM_OK;
}

// SCRIPTBIND(gmDisconnect, "disconnect");
int GM_CDECL gmDisconnect( gmThread* a_thread )
{
    NET.Disconnect();

    return GM_OK;
}

// SCRIPTBIND( gmNewWorld, "newWorld");
int GM_CDECL gmNewWorld( gmThread* a_thread )
{
    GM_CHECK_NUM_PARAMS( 1 ); 
    GM_CHECK_STRING_PARAM( map, 0 );  

    GAME.CreateNewWorld( map );

    return GM_OK;
}


// SCRIPTBIND( gmAddActor, "addActor");
int GM_CDECL gmAddActor( gmThread* a_thread )
{
    GM_CHECK_NUM_PARAMS( 7 );
    GM_CHECK_STRING_PARAM( classname, 0 );
    GM_CHECK_STRING_PARAM( filename, 1 );
    GM_CHECK_INT_PARAM( respawn, 2 );
    GM_CHECK_INT_PARAM( camerafollow, 3 );
    GM_CHECK_INT_PARAM( posx, 4 );
    GM_CHECK_INT_PARAM( posy, 5 ); 
    GM_CHECK_STRING_PARAM( debugname, 6 );

    if ( respawn == 0 )
    {
      CActor::CreateActor( classname, filename, respawn, camerafollow, vector3df( posx, posy, 0 ), debugname );
    }
    else if ( WORLD.GetMap() )
    {
      WORLD.GetMap()->GetRespawn()->AddToQueue( classname, filename, 5, 0 );
    }

    return GM_OK;
}


// SCRIPTBIND( gmListEntities, "listEntities");
int GM_CDECL gmListEntities( gmThread* a_thread )
{
    GM_CHECK_NUM_PARAMS( 0 ); 

    CONSOLE.add( "Listing all world entities names:" );
    for ( int i = 0; i < WORLD.GetEntitysNum(); i++ )
    {
      if ( WORLD.GetEntity( i )->ValidEntity() )
      {
        CONSOLE.add( WORLD.GetEntity( i )->getDebugText() );
      }
      else
      {
        CONSOLE.addx( COLOR_ERROR, "Invalid: %s", WORLD.GetEntity( i )->getDebugText() );
      }
    }

    return GM_OK;
}


// SCRIPTBIND( gmStartServer, "startServer");
int GM_CDECL gmStartServer( gmThread* a_thread )
{
    if ( NET.StartServer() )
    {
      CONSOLE.add( "Server shutdown." );
    }
    else
    {
      CONSOLE.add( "Server activated." );
    }

    return GM_OK;
}

// SCRIPTBIND( gmListPlayers, "listPlayers");
int GM_CDECL gmListPlayers( gmThread* a_thread )
{
    WORLD.GetPlayers()->DumpToConsole();

    return GM_OK;
}

// SCRIPTBIND( gmStartEditor, "startEditor");
int GM_CDECL gmStartEditor( gmThread* a_thread )
{
    WORLD.GetMap()->GetEditor();

    return GM_OK;
}

// SCRIPTBIND( gmListUsedFiles, "listUsedFiles");
int GM_CDECL gmListUsedFiles( gmThread* a_thread )
{
    CONSOLE.add( "Listing files used in game:" );
    for ( int i = 0; i < APP.filesUsed.size(); i++ )
    {
      CONSOLE.add( APP.filesUsed[i].c_str() );
    }

    return GM_OK;
}

// SCRIPTBIND( gAddBot, "addBot");
int GM_CDECL gAddBot( gmThread* a_thread )
{
    new CBot();

    return GM_OK;
}

// SCRIPTBIND( gmNewMusic, "newMusic");
int GM_CDECL gmNewMusic( gmThread* a_thread )
{
    GM_CHECK_NUM_PARAMS( 6 );
    GM_CHECK_STRING_PARAM( filename, 0 );
    GM_CHECK_INT_PARAM( loop, 1 );
    GM_CHECK_INT_PARAM( loop_begin, 2 );
    GM_CHECK_INT_PARAM( loop_end, 3 );
    GM_CHECK_INT_PARAM( fadetime, 4 );
    GM_CHECK_FLOAT_PARAM( mv, 5 ); 

    SOUND.NewMusic( filename, ( bool )loop, loop_begin, loop_end, fadetime, mv );

    return GM_OK;
}

// SCRIPTBIND( gmSetCamera, "setCamera");
int GM_CDECL gmSetCamera( gmThread* a_thread )
{
    GM_CHECK_NUM_PARAMS( 6 );
    GM_CHECK_FLOAT_PARAM( cx, 0 );
    GM_CHECK_FLOAT_PARAM( cy, 1 );
    GM_CHECK_FLOAT_PARAM( cz, 2 );
    GM_CHECK_FLOAT_PARAM( tx, 3 );
    GM_CHECK_FLOAT_PARAM( ty, 4 );
    GM_CHECK_FLOAT_PARAM( tz, 5 );

	WORLD.GetCamera()->setPos( vector3df( cx, cy, cz ) );
	WORLD.GetCamera()->setTargetPos( vector3df( tx, ty, tz ) );

    return GM_OK;
}

//\\//\\//\\//\\//\\//\\//\\//\\//\\//\\//\\//\\//\\//\\//\\//\\//\\//\\

void BindScriptFunctions()
{
    // bind GM script functions
    SCRIPTBIND( gmTest, "test" );
    SCRIPTBIND( gmLoadModule, "loadModule" );
    SCRIPTBIND( gmConsole, "console" );
    SCRIPTBIND( gmQuit, "quit" );
    SCRIPTBIND( gmConnect, "connect" );
    SCRIPTBIND( gmDisconnect, "disconnect" );
    SCRIPTBIND( gmNewWorld, "newWorld" );
    SCRIPTBIND( gmStartServer, "startServer" );
    SCRIPTBIND( gmListPlayers, "listPlayers" );
    SCRIPTBIND( gmAddActor, "addActor" );
    SCRIPTBIND( gmListEntities, "listEntities" );
    SCRIPTBIND( gmStartEditor, "startEditor" );
    SCRIPTBIND( gmListUsedFiles, "listUsedFiles" );
    SCRIPTBIND( gAddBot, "addBot" );
    SCRIPTBIND( gmNewMusic, "newMusic" );
    SCRIPTBIND( gmSetCamera, "setCamera" );
}

