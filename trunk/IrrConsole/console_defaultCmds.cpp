#include "console_defaultCmds.h"
#include "../App/app.h"

//=========================================================================================
//! loads a few default commands into the console
void IC_Console::loadDefaultCommands()
{
    IC_Command* cmd = 0;
    cmd = new IC_Command_ECHO();
    registerCommand( cmd );
    cmd = new IC_Command_HELP();
    registerCommand( cmd );
    cmd = new IC_Command_LIST();
    registerCommand( cmd );
    cmd = new IC_Command_QUIT();
    registerCommand( cmd );
    cmd = new IC_Command_VRESTART();
    registerCommand( cmd );
    //cmd = new IC_Command_LOADMODULE();
    //registerCommand( cmd );
}
//=========================================================================================

bool IC_Command_ECHO::invoke( const array<WideString>& args, IC_Dispatcher* pDispatcher, IC_MessageSink* pOutput )
{
    if ( args.size() > 1 )
    {
      WideString wstr = args[0];
      for ( u32 i = 1; i < args.size(); i++ )
      {
        wstr += L" ";
        wstr += args[i];
      }
      pOutput->add( wstr );
    }
    return true;
}
IC_Command_HELP::IC_Command_HELP() : IC_Command( L"help" )
{
    setUsage( L"help [cmd-name]. Ex. help cls" );
    addDescLine( L"If cmd-name is not supplied a list of commands is printed with usage" );
    addDescLine( L"any command has to be preceded with the backslash character to execute" );
    addDescLine( L"ex. \help" );
}
IC_Command_HELP::~IC_Command_HELP()
{
}
bool IC_Command_HELP::invoke( const array<WideString>& args, IC_Dispatcher* pDispatcher, IC_MessageSink* pOutput )
{
    if ( args.size() == 0 )
    {
      pDispatcher->printCommandList( pOutput, true );
    }
    else
    {
      WideString wstr = args[0];
      for ( u32 i = 1; i < args.size(); i++ )
      {
        wstr += L" ";
        wstr += args[i];
      }
      if ( pDispatcher->hasCommand( wstr ) )
      {
        pDispatcher->printCommandDesc( wstr, pOutput );
      }
      else
      {
        WideString msg = " No help available for command ";
        msg += wstr;
        pOutput->add( msg );
      }
    }
    return true;
}


bool IC_Command_LIST::invoke( const array<WideString>& args, IC_Dispatcher* pDispatcher, IC_MessageSink* pOutput )
{
    if ( args.size() > 0 )
    {
      pDispatcher->printCommandList( pOutput, true );
    }
    else
    {
      pDispatcher->printCommandList( pOutput, false );
    }
    return true;
}


bool IC_Command_QUIT::invoke( const array<WideString>& args, IC_Dispatcher* pDispatcher, IC_MessageSink* pOutput )
{
    APP.Shutdown();
    return true;
}

bool IC_Command_CLS::invoke( const array<WideString>& args, IC_Dispatcher* pDispatcher, IC_MessageSink* pOutput )
{
    pOutput->clearMessages();
    return true;
}
// doesn't work
bool IC_Command_VRESTART::invoke( const array<WideString>& args, IC_Dispatcher* pDispatcher, IC_MessageSink* pOutput )
{
    APP.Restart();
    return true;
}

//bool IC_Command_LOADMODULE::invoke( const array<WideString>& args, IC_Dispatcher* pDispatcher, IC_MessageSink* pOutput )
//{
//    if ( args.size() == 0 )
//    {
//        printDesc( pOutput );
//    }
//    else
//    {
//        APP.LoadGameDLL( wchar2char( args[0] ) );
//    }
//    return true;
//}