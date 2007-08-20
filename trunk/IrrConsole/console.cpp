#include "console.h"
#include "console_defaultCmds.h"
#include "../GameMonkey/script.h"
#include "../Engine/misc.h"
#include "../App/app.h"
#include "../Irrlicht/IrrlichtTask.h"
#include "../RakNet/GameClient.h"

#define COLOR_DARKEN_FACTOR 0.85f

//=========================================================================================
//! constructor
IC_MessageSink::IC_MessageSink()
{
}
//=========================================================================================
//! destructor
IC_MessageSink::~IC_MessageSink()
{
}
//=========================================================================================
//! log a UTF-8 message to the sink
void IC_MessageSink::logMessage_ANSI( irr::ELOG_LEVEL logLevel, const String message )
{
    logMessage( logLevel, IC_StrConv::toWideString( message ) );
}
//=========================================================================================
//! log a UTF-8 message to the sink at info log level
void IC_MessageSink::logInfo_ANSI( const String message )
{
    logMessage_ANSI( irr::ELL_INFORMATION, message );
}
//=========================================================================================
//! log a UTF-8 message to the sink at warning log level
void IC_MessageSink::logWarning_ANSI( const String message )
{
    logMessage_ANSI( irr::ELL_WARNING, message );
}
//=========================================================================================
//! log a UTF-8 message to the sink at error log level
void IC_MessageSink::logError_ANSI( const String message )
{
    logMessage_ANSI( irr::ELL_ERROR, message );
}
//=========================================================================================
//! log a UTF-16 message to the sink
void IC_MessageSink::logMessage( irr::ELOG_LEVEL logLevel, const WideString message )
{
    // WideString wstr = getLevelTag( logLevel );
    // wstr += message;
    add( message, getLevelTag( logLevel ) );
}
//=========================================================================================
//! log a UTF-16 message to the sink at info log level
void IC_MessageSink::logInfo( const WideString message )
{
    logMessage( irr::ELL_INFORMATION, message );
}
//=========================================================================================
//! log a UTF-16 message to the sink at warning log level
void IC_MessageSink::logWarning( const WideString message )
{
    logMessage( irr::ELL_INFORMATION, message );
}
//=========================================================================================
//! log a UTF-16 message to the sink at error log level
void IC_MessageSink::logError( const WideString message )
{
    logMessage( irr::ELL_INFORMATION, message );
}
//=========================================================================================
//! get a string for a log level
irr::video::SColor IC_MessageSink::getLevelTag( irr::ELOG_LEVEL logLevel )
{
    switch ( logLevel )
    {
      case irr::ELL_INFORMATION:
        return COLOR_INFO;
      case irr::ELL_WARNING:
        return COLOR_WARNING;
      case irr::ELL_ERROR:
        return COLOR_ERROR;
      default:
        return COLOR_INFO;
    }
}
//=========================================================================================
//! constructor
IC_Command::IC_Command( const WideString name ) : commandName( name )
{
}
//=========================================================================================
//! the destructor
IC_Command::~IC_Command()
{
}
//=========================================================================================
//! get command name
const WideString IC_Command::getName()
{
    return commandName;
}
//=========================================================================================
//! get the usage string for the command
const WideString IC_Command::getUsage()
{
    return commandUsage;
}
//=========================================================================================
//! get the description line count
const irr::u32 IC_Command::getDescLineCount()
{
    return commandDescLines.size();
}
//=========================================================================================
//! get the desc line at
const WideString IC_Command::getDescLine( const u32 index )
{
    return commandDescLines[index];
}
//=========================================================================================
//! print the usage to the message sink
void IC_Command::printUsage( IC_MessageSink* pOutput )
{
    WideString wstr = L"Command : ";
    wstr += getName();
    pOutput->add( wstr );
    wstr = L"usage : ";
    pOutput->add( wstr );
    wstr = L" ";
    wstr += getUsage();
    pOutput->add( wstr );
}
//=========================================================================================
//! print the description to the message sink
void IC_Command::printDesc( IC_MessageSink* pOutput )
{
    WideString wstr = L"Command : ";
    wstr += getName();
    pOutput->add( wstr );
    wstr = L"usage : ";
    pOutput->add( wstr );
    wstr = L" ";
    wstr += getUsage();
    pOutput->add( wstr );

    wstr = L"description :";
    pOutput->add( wstr );

    for ( u32 i = 0; i < commandDescLines.size(); i++ )
    {
      wstr = L" ";
      wstr += commandDescLines[i];
      pOutput->add( wstr );
    }
}
//=========================================================================================
//! set usage
void IC_Command::setUsage( const WideString usage )
{
    commandUsage = usage;
}
//=========================================================================================
//! append a line to the desc array
void IC_Command::addDescLine( const WideString line )
{
    commandDescLines.push_back( line );
}
//=========================================================================================
//! set descrition to ""
void IC_Command::clearDescLines()
{
    commandDescLines.clear();
}
//=========================================================================================
//! constructor
IC_Dispatcher::IC_Dispatcher()
{
}
//=========================================================================================
//! destructor
IC_Dispatcher::~IC_Dispatcher()
{
    deregisterAllCommands();
}
//=========================================================================================
//! register a command (the cmd->getName() should not already be registered else an exception is thrown)
void IC_Dispatcher::registerCommand( IC_Command* cmd )
{
    if ( cmd )
    {
      WideString name = cmd->getName();
      if ( !hasCommand( name ) )
      {
        commandTable[name] = cmd;
      }
      else
      {
        WideString wstr = L"Command ";
        wstr += name;
        wstr += " is already registered";
        //    throw IC_Error( wstr );
      }
    }
}
//=========================================================================================
//! check if a command is already registered
bool IC_Dispatcher::hasCommand( const WideString cmdName )
{
    map<WideString, IC_Command*>::iterator iter = commandTable.find( cmdName );
    return iter != commandTable.end();
}
//=========================================================================================
//! dispatch a command with the name and the supplied args
void IC_Dispatcher::dispatch( const WideString cmdName, const array<WideString>& args, IC_MessageSink* pOutput )
{
    map<WideString, IC_Command*>::iterator iter = commandTable.find( cmdName );
    if ( iter != commandTable.end() )
    {
      try
      {
        iter->second->invoke( args, this, pOutput );
        pOutput->add( L"" );
      }
      catch ( IC_Error& err )
      {
        WideString wstr = "error of type ";
        wstr += err.getType();
        wstr += " in invoking command [";
        wstr += cmdName;
        wstr += "]";
        pOutput->logError( wstr );
        pOutput->add( err.getMessage() );
      }
      //      catch(exception& ex)
      //      {
      //          WideString wstr = "error in invoking command [";
      //          wstr += cmdName;
      //          wstr += "]";
      //          pOutput->logError(wstr);
      //          pOutput->add(IC_StrConv::toWideString(ex.what()));
      //      }
    }
    else
    {
      WideString wstr = L"command [";
      wstr += cmdName;
      wstr += L"] is not a valid command";
      pOutput->logError( wstr );
      pOutput->add( L"for a list of messages type \"help\" or \"list\"" );
    }
}
//=========================================================================================
//! deregister (remove) a command
void IC_Dispatcher::deregisterCommand( const WideString cmdName )
{
    map<WideString, IC_Command*>::iterator iter = commandTable.find( cmdName );
    if ( iter != commandTable.end() )
    {
      delete iter->second;
      commandTable.erase( iter );
    }
}
//=========================================================================================
//! deregister all commands
void IC_Dispatcher::deregisterAllCommands()
{
    map<WideString, IC_Command*>::iterator iter = commandTable.begin();
    while ( iter != commandTable.end() )
    {
      delete iter->second;
      iter++;
    }
    commandTable.clear();
}
//=========================================================================================
//! get the list of registered commands
u32 IC_Dispatcher::getRegisteredCommands( array<WideString>& cmdNames )
{
    cmdNames.clear();
    map<WideString, IC_Command*>::iterator iter = commandTable.begin();
    while ( iter != commandTable.end() )
    {
      cmdNames.push_back( iter->first );
      iter++;
    }

    return cmdNames.size();
}
//=========================================================================================
//! print a list of commands
void IC_Dispatcher::printCommandList( IC_MessageSink* pOutput, bool bUsage )
{
    array<WideString> cmdNames;
    getRegisteredCommands( cmdNames );

    for ( u32 i = 0; i < cmdNames.size(); i++ )
    {
      //    cout << "Printing Command : " << cmdNames[i] << endl;
      if ( !bUsage )
      {
        WideString wstr = L"   ";
        wstr += cmdNames[i];
        pOutput->add( wstr );
      }
      else
      {
        printCommandUsage( cmdNames[i], pOutput );
        pOutput->add( L"" );
      }
    }
}
//=========================================================================================
//! print the command usage
void IC_Dispatcher::printCommandUsage( const WideString cmdName, IC_MessageSink* pOutput )
{
    IC_Command* cmd = commandTable[cmdName];
    if ( cmd )
    {
      cmd->printUsage( pOutput );
    }
}
//=========================================================================================
//! print detailed description
void IC_Dispatcher::printCommandDesc( const WideString cmdName, IC_MessageSink* pOutput )
{
    IC_Command* cmd = commandTable[cmdName];
    if ( cmd )
    {
      cmd->printDesc( pOutput );
    }
}
//=========================================================================================
//! the tilde (~/`) key
//const wchar_t IC_Console::IC_KEY_TILDE = 0xc0;
//=========================================================================================
//! constructor
IC_Console::IC_Console() : guiFont( 0 ), consoleHistoryIndex( 0 ), consoleRect( 0, 0, 0, 0 ), bVisible( false )
{
    consolelog = NULL;
    //register common commands
    loadDefaultCommands();

    bInited = false;
}
//=========================================================================================
//! destructor
IC_Console::~IC_Console()
{
    if ( consolelog )
    {
      delete consolelog;
    }
}
//=========================================================================================
//! get the console config reference
IC_ConsoleConfig& IC_Console::getConfig()
{
    return consoleConfig;
}
//=========================================================================================
//! (re)initialize the console with current config
void IC_Console::initializeConsole( irr::gui::IGUIEnvironment* guienv, const irr::core::dimension2d<s32>& screenSize, const char* logFileName, bool standardOutput )
{
    //addx( COLOR_ERROR, "Could not load console font '%s'",  logFileName );
    if ( strlen( logFileName ) > 1 )
    {
      consolelog = new CLog( logFileName, standardOutput );
    }

    //load the font
    guiFont = guienv->getFont( APP.useFile( wide2string( consoleConfig.fontName ).c_str() ).c_str() );
    if ( guiFont == 0 )
    {
      guiFont = guienv->getBuiltInFont();
      addx( COLOR_ERROR, "Could not load console font '%s'", wide2string( consoleConfig.fontName ).c_str() );
    }
    //cout<<"font loaded!!"<<endl;

    //calculate the console rectangle
    calculateConsoleRect( screenSize );

    //append a message
    //add( L"IrrConsole (re)initialized" );

    //resize message array
    resizeMessages( 1 );

    consoleOffset = typePlace = 0;

    bInited = true;
    inPrompt = true;
}
//=========================================================================================
//! resize the message count
void IC_Console::resizeMessages( int scrollAmount )
{
    u32 maxLines = 0;
    u32 lineHeight = 0;
    s32 fontHeight = 0;
    if ( calculateLimits( maxLines, lineHeight, fontHeight ) )
    {
      u32 messageCount = consoleMessages.size();
      if ( messageCount > maxLines )
      {
        //consoleMessages.erase( 0, messageCount - maxLines );
        //consoleMessagesColors.erase( 0, messageCount - maxLines );
        consoleOffset += scrollAmount;
        if ( consoleMessages.size() - consoleOffset > maxLines )
        {
          consoleOffset -= scrollAmount;
        }
      }
    }
}
//=========================================================================================
//! should console be visible
bool IC_Console::isVisible()
{
    return bVisible;
}
//=========================================================================================
//! set the visibility of the console
void IC_Console::setVisible( bool bV )
{
    bVisible = bV;
}
//=========================================================================================
//! toggle the visibility of the console
void IC_Console::toggleVisible()
{
    setVisible( !isVisible() );
}
//=========================================================================================
//! add a UTF-16 message to the sink
void IC_Console::add( const WideString message, irr::video::SColor color )
{
    array<WideString> text;
    breakText( message, text );
    for ( int i = 0; i < text.size(); i++ )
    {
      consoleMessages.push_back( text[i] );
      consoleMessagesColors.push_back( color );
    }

    resizeMessages( i );
    // add to client log

    if ( consolelog )
    {
      char* c = wchar2char( message );
      consolelog->Write( c );
      delete c;
    }
    //if ( !bInited )
    //  cout << c << "\n";
}
//=========================================================================================
void IC_Console::add( const WideString message )
{
    add( message, getConfig().fontColor );
}
//=========================================================================================
void IC_Console::addx( const char* msg, ... )
{
    va_list args; va_start( args, msg );
    char szBuf[1024];
    vsprintf( szBuf, msg, args );

    WideString wstr = szBuf;
    add( wstr );

    va_end( args );
}
//=========================================================================================
void IC_Console::addx( irr::video::SColor color, const char* msg, ... )
{
    va_list args; va_start( args, msg );
    char szBuf[1024];
    vsprintf( szBuf, msg, args );

    WideString wstr = szBuf;
    add( wstr, color );

    va_end( args );
}
//=========================================================================================
//! clear all the messages in the sink
void IC_Console::clearMessages()
{
    consoleMessages.clear();
    consoleMessagesColors.clear();
}
//=========================================================================================
//! render the console (it internally checks if the console is visible)
void IC_Console::renderConsole( irr::gui::IGUIEnvironment* guienv, irr::video::IVideoDriver* videoDriver, const u32 deltaMillis )
{
    // render only if the console is visible
    if ( ( isVisible() ) && ( bInited ) )
    {
      //if bg is to be drawn fill the console bg with color
      if ( consoleConfig.bShowBG )
      {
        //draw the bg as per configured color
        videoDriver->draw2DRectangle( consoleConfig.bgColor, consoleRect );
      }
      //we calculate where the message log shall be printed and where the prompt shall be printed
      rect<s32> textRect, shellRect;
      calculatePrintRects( textRect, shellRect );


      //now, render the messages
      u32 maxLines, lineHeight;
      s32 fontHeight;
      if ( !calculateLimits( maxLines, lineHeight, fontHeight ) )
      {
        return;
      }
      //calculate the line rectangle
      rect<s32> lineRect( textRect.UpperLeftCorner.X, textRect.UpperLeftCorner.Y, textRect.LowerRightCorner.X, textRect.UpperLeftCorner.Y + lineHeight );
      //for ( u32 i = 0; i < consoleMessages.size(); i++ )
      //{
      //    //we draw each line with the configured font and color vertically centered in the rectangle
      //    guiFont->draw( consoleMessages[i].c_str(), lineRect, consoleMessagesColors[i], false, true );

      //    //update line rectangle
      //    lineRect.UpperLeftCorner.Y += lineHeight;
      //    lineRect.LowerRightCorner.Y += lineHeight;
      //}
      if ( consoleOffset + maxLines <= consoleMessages.size() )
      {
        for ( u32 i = consoleOffset; i < consoleOffset + maxLines; i++ )
        {
          //we draw each line with the configured font and color vertically centered in the rectangle
          guiFont->draw( consoleMessages[i].c_str(), lineRect, consoleMessagesColors[i], false, true );

          //update line rectangle
          lineRect.UpperLeftCorner.Y += lineHeight;
          lineRect.LowerRightCorner.Y += lineHeight;
        }
      }
      else
      {
        for ( u32 i = 0; i < consoleMessages.size(); i++ )
        {
          //we draw each line with the configured font and color vertically centered in the rectangle
          guiFont->draw( consoleMessages[i].c_str(), lineRect, consoleMessagesColors[i], false, true );

          //update line rectangle
          lineRect.UpperLeftCorner.Y += lineHeight;
          lineRect.LowerRightCorner.Y += lineHeight;
        }
      }

      // if console is scrolled
      u32 k = consoleMessages.size() - consoleOffset;
      if ( k > maxLines )
      {
        WideString wstr = L"^ ";
        wstr += i2wchar( k - maxLines );
        wstr += L" ^^^^^^^^^^^^^^^^^^^^^^ ";
        guiFont->draw( wstr.c_str(), lineRect, COLOR_CRAZY, false, true );
      }

      //now, render the prompt
      if ( inPrompt )
      {
        WideString shellText = consoleConfig.prompt;
        shellText += L">";

        WideString left = shellText;
        left += currentCommand.subString( 0, typePlace );

        shellText += currentCommand;

        //draw the prompt string
        array<WideString> text;

        breakText( shellText, text );
        lineRect = shellRect;
        SColor color( consoleConfig.fontColor.getAlpha(), consoleConfig.fontColor.getRed() * COLOR_DARKEN_FACTOR, consoleConfig.fontColor.getGreen() * COLOR_DARKEN_FACTOR, consoleConfig.fontColor.getBlue() * COLOR_DARKEN_FACTOR );
        for ( int i = 0; i < text.size(); i++ )
        {
          guiFont->draw( text[i].c_str(), lineRect, color, false, true );
          //update shell line rectangle
          lineRect.UpperLeftCorner.Y += lineHeight;
          lineRect.LowerRightCorner.Y += lineHeight;
        }

        breakText( left, text );
        lineRect = shellRect;
        for ( int i = 0; i < text.size(); i++ )
        {
          guiFont->draw( text[i].c_str(), lineRect, consoleConfig.fontColor, false, true );
          //update shell line rectangle
          lineRect.UpperLeftCorner.Y += lineHeight;
          lineRect.LowerRightCorner.Y += lineHeight;
        }

        //for blinking cursor
        static u32 blinkCounter = 0;
        blinkCounter ++;
        if ( blinkCounter > 300 )
        {
          blinkCounter = 0;
        }
        else if ( blinkCounter <= 150 )
        {
          lineRect.UpperLeftCorner.Y -= lineHeight;
          lineRect.LowerRightCorner.Y -= lineHeight;
          lineRect.UpperLeftCorner.X += guiFont->getDimension( text[text.size() - 1].c_str() ).Width;
          lineRect.LowerRightCorner.X += guiFont->getDimension( L"_" ).Width;
          guiFont->draw( L"_", lineRect, consoleConfig.fontColor, false, true );
        }

        //inPrompt = false;
      }
    }
}
//=========================================================================================
//! handles a key press when console is active/visible
void IC_Console::handleKeyPress( wchar_t keyChar, irr::EKEY_CODE keyCode, bool bShiftDown, bool bControlDown, IrrlichtDevice* device )
{
    //  inPrompt = true;

    if ( keyCode == irr::KEY_RETURN )
    {
      addToHistory( currentCommand );
      handleCommandString( currentCommand );
      currentCommand = L"";
      consoleHistoryIndex = 0;
      typePlace = 0;
      inPrompt = false;
    }
    else if ( keyCode == irr::KEY_BACK || ( keyCode == irr::KEY_KEY_H && bControlDown ) )
    {
      if ( ( currentCommand.size() > 0 ) && ( typePlace > 0 ) )
      {
        WideString left, right, astr;
        left = currentCommand.subString( 0, typePlace - 1 );
        right = currentCommand.subString( typePlace, currentCommand.size() - typePlace );
        currentCommand = left + right;
        typePlace--;
      }
    }
    else if ( keyCode == irr::KEY_DELETE )
    {
      if ( typePlace < currentCommand.size() )
      {
        WideString left, right, astr;
        left = currentCommand.subString( 0, typePlace );
        right = currentCommand.subString( typePlace + 1, currentCommand.size() - typePlace );
        currentCommand = left + right;
      }
    }
    else if ( keyCode == irr::KEY_UP )
    {
      if ( consoleHistory.size() > 0 )
      {
        s32 index = consoleHistory.size() - 1 - consoleHistoryIndex;
        if ( index >= 0 && index < consoleHistory.size() )
        {
          consoleHistoryIndex++;
          currentCommand = consoleHistory[index];
        }
        else
        {
          consoleHistoryIndex = 0;
        }
      }
      else
      {
        consoleHistoryIndex = 0;
      }
      typePlace = currentCommand.size();
    }
    else if ( keyCode == irr::KEY_DOWN )
    {
      if ( consoleHistory.size() > 0 )
      {
        s32 index = consoleHistory.size() - consoleHistoryIndex;
        if ( index >= 0 && index < consoleHistory.size() )
        {
          consoleHistoryIndex--;
          currentCommand = consoleHistory[index];
        }
        else
        {
          consoleHistoryIndex = consoleHistory.size() - 1;
        }
      }
      else
      {
        consoleHistoryIndex = 0;
      }
      typePlace = currentCommand.size();
    }
    else if ( keyCode == irr::KEY_LEFT )
    {
      if ( typePlace > 0 )
      {
        typePlace--;
      }
    }
    else if ( keyCode == irr::KEY_RIGHT )
    {
      if ( typePlace < currentCommand.size() )
      {
        typePlace++;
      }
    }
    else if ( keyCode == irr::KEY_HOME )
    {
      typePlace = 0;
    }
    else if ( keyCode == irr::KEY_END )
    {
      typePlace = currentCommand.size();
    }
    else if ( keyCode == irr::KEY_TAB )
    {
      tabComplete();
      typePlace = currentCommand.size();
    }
    else if ( ( keyCode == irr::KEY_KEY_V && bControlDown ) || ( keyCode == irr::KEY_INSERT && bShiftDown ) )
    {
      WideString left, right, astr;
      left = currentCommand.subString( 0, typePlace );
      right = currentCommand.subString( typePlace, currentCommand.size() - typePlace );
      astr = device->getOSOperator()->getTextFromClipboard();
      currentCommand = left + astr + right;
      //typePlace += astr.size();
    }
    else if ( keyCode == irr::KEY_PRIOR )
    {
      if ( consoleOffset > 0 )
      {
        consoleOffset--;
      }
    }
    else if ( keyCode == irr::KEY_NEXT )
    {
      u32 maxLines, lineHeight;
      s32 fontHeight;
      if ( !calculateLimits( maxLines, lineHeight, fontHeight ) )
      {
        return;
      }

      if ( consoleMessages.size() - consoleOffset > maxLines )
      {
        consoleOffset++;
      }
    }
    else if ( keyChar )
    {
      wchar_t buf[2];
      buf[0] = keyChar;
      buf[1] = 0;
      WideString astr = buf;
      if ( bShiftDown )
      {
        astr.make_upper();
      }

      WideString left, right;
      left = currentCommand.subString( 0, typePlace );
      right = currentCommand.subString( typePlace, currentCommand.size() - typePlace );
      currentCommand = left + astr + right;
      typePlace++;
    }

    //  if (currentCommand == L"")
    //inPrompt = false;
}
//=========================================================================================
//! handle the current command string
void IC_Console::handleCommandString( WideString& wstr )
{
    //add to console
    add( wstr );

    //check if it is a command
    if ( wstr.size() > 0 && wstr[0] == ( wchar_t )'\\' )
    {
      WideString cmdLine = wstr.subString( 1, wstr.size() - 1 );
      //  cout << "Command Received : " << cmdLine << endl;

      //append the message
      //WideString msg = L">> Command : ";
      //msg += cmdLine;
      //add(msg);

      //parsing logic

      array<WideString> args;
      WideString cmdName;
      IC_CmdLineParser parser( cmdLine );
      if ( parser.parse( cmdName, args ) )
      {
        dispatch( cmdName, args, this );
      }
      else
      {
        WideString errorMessage = L"The command syntax is incorrect or it could not be parsed";
        logError( errorMessage );
      }
    }
    else
    {
      // EXECUTE SCRIPT
      SCRIPT.RunString( wstr );
    }
}
//=========================================================================================
//! add to history and readjust history
void IC_Console::addToHistory( WideString& wstr )
{
    if ( consoleHistory.size() >= consoleConfig.commandHistorySize )
    {
      consoleHistory.erase( 0, 1 );
    }
    consoleHistory.push_back( wstr );
}
//=========================================================================================
//! calculate the whole console rect
void IC_Console::calculateConsoleRect( const irr::core::dimension2d<s32>& screenSize )
{
    if ( consoleConfig.dimensionRatios.X == 0 || consoleConfig.dimensionRatios.Y == 0 )
    {
      consoleRect = rect<s32>( 0, 0, 0, 0 );
    }
    else
    {
      //calculate console dimension
      dimension2d<s32> consoleDim = screenSize;
      consoleDim.Width = ( s32 )( ( f32 )consoleDim.Width * consoleConfig.dimensionRatios.X );
      consoleDim.Height = ( s32 )( ( f32 )consoleDim.Height * consoleConfig.dimensionRatios.Y );

      //set vertical alignment
      if ( consoleConfig.valign == VAL_TOP )
      {
        consoleRect.UpperLeftCorner.Y = 0;
      }
      else if ( consoleConfig.valign == VAL_BOTTOM )
      {
        consoleRect.UpperLeftCorner.Y = screenSize.Height - consoleDim.Height;
      }
      else if ( consoleConfig.valign == VAL_MIDDLE )
      {
        consoleRect.UpperLeftCorner.Y = ( screenSize.Height - consoleDim.Height ) / 2;
      }

      //set horizontal alignment
      if ( consoleConfig.halign == HAL_LEFT )
      {
        consoleRect.UpperLeftCorner.X = 0;
      }
      else if ( consoleConfig.halign == HAL_RIGHT )
      {
        consoleRect.UpperLeftCorner.X = screenSize.Width - consoleDim.Width;
      }
      else if ( consoleConfig.halign == HAL_CENTER )
      {
        consoleRect.UpperLeftCorner.X = ( screenSize.Width - consoleDim.Width ) / 2;
      }

      //set the lower right corner stuff
      consoleRect.LowerRightCorner.X = consoleRect.UpperLeftCorner.X + consoleDim.Width;
      consoleRect.LowerRightCorner.Y = consoleRect.UpperLeftCorner.Y + consoleDim.Height;
    }
}
//=========================================================================================
//! calculate the messages rect and prompt / shell rect
void IC_Console::calculatePrintRects( rect<s32>& textRect, rect<s32>& shellRect )
{
    u32 maxLines, lineHeight;
    s32 fontHeight;
    if ( calculateLimits( maxLines, lineHeight, fontHeight ) )
    {
      //the shell rect
      shellRect.LowerRightCorner.X = consoleRect.LowerRightCorner.X;
      shellRect.LowerRightCorner.Y = consoleRect.LowerRightCorner.Y;
      shellRect.UpperLeftCorner.X = consoleRect.UpperLeftCorner.X + getConfig().indent;
      shellRect.UpperLeftCorner.Y = consoleRect.LowerRightCorner.Y - lineHeight;

      //calculate text rect
      textRect.UpperLeftCorner.X = consoleRect.UpperLeftCorner.X + getConfig().indent;
      textRect.UpperLeftCorner.Y = consoleRect.UpperLeftCorner.Y;
      shellRect.LowerRightCorner.X = consoleRect.LowerRightCorner.X;
      shellRect.LowerRightCorner.Y = shellRect.UpperLeftCorner.Y;
    }
    else
    {
      textRect = rect<s32>( 0, 0, 0, 0 );
      shellRect = rect<s32>( 0, 0, 0, 0 );
    }
}
//=========================================================================================
bool IC_Console::calculateLimits( u32& maxLines, u32& lineHeight, s32& fontHeight )
{
    u32 consoleHeight = consoleRect.getHeight();
    if ( guiFont != 0 && consoleHeight > 0 )
    {
      fontHeight = guiFont->getDimension( L"X" ).Height;
      lineHeight = fontHeight + consoleConfig.lineSpacing;
      maxLines = consoleHeight / lineHeight;
      if ( maxLines > 2 )
      {
        maxLines -= 2;
      }
      return true;
    }
    else
    {
      return false;
    }
}
//=========================================================================================
void IC_Console::tabComplete()
{
    if ( currentCommand.size() == 0 )
    {
      return;
    }
    else if ( currentCommand[0] != ( wchar_t )( '\\' ) )
    {
      return;
    }
    WideString ccStr = currentCommand.subString( 1, currentCommand.size() - 1 );
    array<WideString> names;
    getRegisteredCommands( names );
    for ( u32 i = 0; i < names.size(); i++ )
    {
      WideString thisCmd = names[i];
      if ( thisCmd.size() == ccStr.size() )
      {
        if ( thisCmd == ccStr )
        {
          return;
        }
      }
      else if ( thisCmd.size() > ccStr.size() )
      {
        if ( thisCmd.subString( 0, ccStr.size() ) == ccStr )
        {
          currentCommand = L"\\";
          currentCommand += thisCmd;

          return;
        }
      }
    }
}



void IC_Console::externalCommandString( WideString wstr )
{
    //addToHistory(wstr);
    //handleCommandString(wstr);
    WideString cmdLine = wstr;//.subString(1,wstr.size() - 1);
    //  cout << "Command Received : " << cmdLine << endl;

    array<WideString> args;
    WideString cmdName;
    IC_CmdLineParser parser( cmdLine );
    if ( parser.parse( cmdName, args ) )
    {
      dispatch( cmdName, args, this );
    }
    else
    {
      WideString errorMessage = L"The command syntax is incorrect or it could not be parsed";
      logError( errorMessage );
    }
}

void IC_Console::breakText( WideString Text, array<WideString>& BrokenText )
{
    BrokenText.clear();

    IGUIFont* font = guiFont;

    if ( !font )
    {
      return;
    }

    core::stringw line;
    core::stringw word;
    core::stringw whitespace;
    s32 size = Text.size();
    s32 length = 0;
    s32 elWidth = consoleRect.getWidth() - 6;
    wchar_t c;

    for ( s32 i = 0; i < size; ++i )
    {
      c = Text[i];
      bool lineBreak = false;

      if ( c == L'\n' )
      {
        lineBreak = true;
        c = ' ';
      }

      if ( c == L' ' || c == 0 || i == ( size - 1 ) )
      {
        if ( word.size() )
        {
          // here comes the next whitespace, look if 
          // we can break the last word to the next line.
          s32 whitelgth = font->getDimension( whitespace.c_str() ).Width;
          s32 worldlgth = font->getDimension( word.c_str() ).Width;

          if ( length + worldlgth + whitelgth > elWidth )
          {
            // break to next line
            length = worldlgth;
            BrokenText.push_back( line );
            line = word;
          }
          else
          {
            // add word to line
            line += whitespace;
            line += word;
            length += whitelgth + worldlgth;
          }

          word = L"";
          whitespace = L"";
        }

        whitespace += c;

        // compute line break
        if ( lineBreak )
        {
          line += whitespace;
          line += word;
          BrokenText.push_back( line );
          line = L"";
          word = L"";
          whitespace = L"";
          length = 0;
        }
      }
      else
      {
        // yippee this is a word..
        word += c;
      }
    }

    line += whitespace; 
    line += word; 
    BrokenText.push_back( line );
}

//=========================================================================================

void IC_MainConsole::addVariable( WideString name, WideString val )
{
    // irrlicht linear_reverse_search is so fcuked up!
    for ( int i = 0; i < consoleVars.size(); i++ )
    {
      if ( consoleVars[i] == name )
      {
        //consoleVars[i] = name;
        //printf("hell %i",i);
        consoleVarsVals[i] = val;
        return;
      }
    }

    consoleVars.push_back( name );
    consoleVarsVals.push_back( val );
}

void IC_MainConsole::SaveVariablesToFile( const char* filename )
{
    std::ofstream out( filename );

    for ( int i = 0; i < consoleVars.size(); i++ )
    {
      out << "global " << IC_StrConv::toString( consoleVars[i] ) << " = " << IC_StrConv::toString( consoleVarsVals[i] ) << ";\n";
    }

    out << "print(`Config loaded from '" << filename << "'`);\n";

    out.close();
}

//=========================================================================================


void IC_ChatConsole::setCustomRectangle( rect<s32> newRect )
{
    //valign = VAL_NONE;
    //halign = HAL_NONE;

    //dimension2d<s32> dim;
    //dim.Width = newRect.getWidth();
    //dim.Height = newRect.getWidth();
    //calculateConsoleRect( dim );
}

//=========================================================================================
//! handle the current command string
void IC_ChatConsole::handleCommandString( WideString& wstr )
{
    //add to console
    //add( wstr );

    CGameClient::SendChat( ( char* )IC_StrConv::toString( wstr ).c_str(), 0 );
}