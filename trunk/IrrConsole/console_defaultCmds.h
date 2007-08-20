#ifndef _IrrConsole_Default_Cmds_h_
#define _IrrConsole_Default_Cmds_h_
#include "includes.h"
#include "console_utils.h"
#include "console.h"


//
//  Default Command Set
//

CONSOLE_COMMAND( IC_Command_ECHO, "echo", "echo [string]. Ex. echo echo!", "This command echoes the given string to console." );
CONSOLE_COMMAND_BASE( IC_Command_HELP );
CONSOLE_COMMAND( IC_Command_LIST, "list", "list [command]. Ex. list cls", "Return the list of available commands." );
CONSOLE_COMMAND( IC_Command_QUIT, "quit", "quit", "Quit the game." );
CONSOLE_COMMAND( IC_Command_CLS, "cls", "cls", "Clears the console messages." );
CONSOLE_COMMAND( IC_Command_VRESTART, "v_restart", "v_restart", "Restarts the video engine." );
//CONSOLE_COMMAND( IC_Command_LOADMODULE, "loadmodule", "loadmodule [filename]. Ex. loadmodule Data/App.dll",
//                 "Loads a DLL game module." );


#endif