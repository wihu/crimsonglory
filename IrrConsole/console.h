#ifndef _IrrConsole_Console_h_
#define _IrrConsole_Console_h_
#include "../App/stdafx.h"
#include "../Engine/singleton.h"
#include "console_utils.h"
//=====================================================================================
//advance declarations
class IC_MessageSink;
class IC_Command;
class IC_Dispatcher;
class IC_Console;
struct IC_IC_ConsoleConfig;
using  std::map;
//=====================================================================================

#define COLOR_ERROR irr::video::SColor( 255, 245, 45, 45 ) 
#define COLOR_WARNING irr::video::SColor( 255, 245, 245, 45 ) 
#define COLOR_INFO irr::video::SColor( 255, 145, 145, 145 ) 
#define COLOR_CRAZY irr::video::SColor( 255, 245, 45, 145 ) 
#define COLOR_SCRIPT irr::video::SColor( 255, 145, 145, 225 ) 

#define CONSOLE_FLOAT(what) \
CONSOLE.addx("Float: %f", what); \

#define CONSOLE_INT(what) \
CONSOLE.addx("Int: %i", what); \

#define CONSOLE_VECTOR(what) \
CONSOLE.addx( "Vector: X=%f; Y=%f; Z=%f", what.X, what.Y, what.Z ); \

//=====================================================================================
//!the message sink interface
class IC_MessageSink
{
  public:
    //! constructor
    IC_MessageSink();
    //! destructor
    virtual ~IC_MessageSink();

    //! log a UTF-8 message to the sink
    void logMessage_ANSI( irr::ELOG_LEVEL logLevel, const String message );
    //! log a UTF-8 message to the sink at info log level
    void logInfo_ANSI( const String message );
    //! log a UTF-8 message to the sink at warning log level
    void logWarning_ANSI( const String message );
    //! log a UTF-8 message to the sink at error log level
    void logError_ANSI( const String message );

    //! log a UTF-16 message to the sink
    void logMessage( irr::ELOG_LEVEL logLevel, const WideString message );
    //! log a UTF-16 message to the sink at info log level
    void logInfo( const WideString message );
    //! log a UTF-16 message to the sink at warning log level
    void logWarning( const WideString message );
    //! log a UTF-16 message to the sink at error log level
    void logError( const WideString message );


    //! get a string for a log level
    virtual irr::video::SColor getLevelTag( irr::ELOG_LEVEL logLevel );
    //! add a UTF-16 message to the sink
    virtual void add( const WideString message ) = 0;
    virtual void add( const WideString message, irr::video::SColor color ) = 0;
    //! clear all the messages in the sink
    virtual void clearMessages() = 0;
    //! set the visibility
    virtual void toggleVisible() = 0;
};
//=====================================================================================
//! the command class
class IC_Command
{
  public:
    //! the destructor
    virtual ~IC_Command();

    //! invoke the command with supplied args
    virtual bool invoke( const array<WideString>& args, IC_Dispatcher* pDispatcher, IC_MessageSink* pOutput ) = 0;

    //! get command name
    const WideString getName();
    //! get the usage string for the command
    const WideString getUsage();
    //! get the description line count
    const irr::u32 getDescLineCount();
    //! get the desc line at
    const WideString getDescLine( const u32 index );

    //! print the usage to the message sink
    virtual void printUsage( IC_MessageSink* pOutput );
    //! print the description to the message sink
    virtual void printDesc( IC_MessageSink* pOutput );

  protected:
    //! constructor
    IC_Command( const WideString name );
    //! set usage
    void setUsage( const WideString usage );
    //! append a line to the desc array
    void addDescLine( const WideString line );
    //! set descrition to ""
    void clearDescLines();
  private:
    //! the name of the command
    WideString commandName;
    //! the usage string for the command
    WideString commandUsage;
    //! the description lines
    array<WideString> commandDescLines;
};
//=====================================================================================
//! the dispatcher interface
class IC_Dispatcher
{
  public:
    //! destructor
    virtual ~IC_Dispatcher();
    //! register a command (the cmd->getName() should not already be registered else an exception is thrown)
    virtual void registerCommand( IC_Command* cmd );
    //! check if a command is already registered
    bool hasCommand( const WideString cmdName );
    //! dispatch a command with the name and the supplied args
    void dispatch( const WideString cmdName, const array<WideString>& args, IC_MessageSink* pOutput );
    //! deregister (remove) a command
    void deregisterCommand( const WideString cmdName );
    //! get the list of registered commands
    u32 getRegisteredCommands( array<WideString>& cmdNames );
    //! deregister all commands
    void deregisterAllCommands();

    //! print a list of commands
    void printCommandList( IC_MessageSink* pOutput, bool bUsage );
    //! print the command usage
    void printCommandUsage( const WideString cmdName, IC_MessageSink* pOutput );
    //! print detailed description
    void printCommandDesc( const WideString cmdName, IC_MessageSink* pOutput );
  protected:
    //! constructor
    IC_Dispatcher();
  private:
    //! the map of command name to command pointer
    map<WideString, IC_Command*> commandTable;
};
//=====================================================================================
//! an alignment enumeration for vertical alignment
enum IC_VerticalAlignment { //! top
VAL_TOP = 0,
                            //! middle
VAL_MIDDLE = 1,
                            //! bottom
VAL_BOTTOM = 2, VAL_NONE = 3 };
//=====================================================================================
//! an alignment enumeration for horizontal alignment
enum IC_HorizontalAlignment { //! left
HAL_LEFT = 0,
                              //! center
HAL_CENTER = 1,
                              //! right
HAL_RIGHT = 2, HAL_NONE = 3 };
//=====================================================================================
//! the console config structure
struct IC_ConsoleConfig
{
  public:
    //! constructor
    IC_ConsoleConfig()
    {
        setDefaults();
    }
    //! set the defaults on the console config
    void setDefaults()
    {
        // TODO: this will be overwritten by console vars
        dimensionRatios.X = 1.0f;
        dimensionRatios.Y = 0.75f;
        lineSpacing = 0;
        indent = 111;
        valign = VAL_TOP;
        halign = HAL_LEFT;
        bShowBG = true;
        bgColor = irr::video::SColor( 250, 10, 10, 70 );
        fontColor = irr::video::SColor( 200, 200, 200, 200 );
        fontName = L"Fonts/console.bmp";
        prompt = L"cmd";
        commandHistorySize = 10;
        key_tilde = 0xc0;
    }

    //! this contains the Width and Height ratios to the main view port (0 - 1)
    vector2df dimensionRatios;

    //! this is the spacing between two lines in pixels (Default / Min : 2)
    u32 lineSpacing;

    //! this is the indentation for each line in pixels (Default / Min : 1)
    u32 indent;

    //! this is the alignment flag for the vertical placement of the console
    u32 valign;

    //! this is the alignment flag for the horizontal alignment of the console
    u32 halign;

    //! this is the flag indicating if the console BG should be drawn
    bool bShowBG;

    //! this is the color for the console BG
    irr::video::SColor bgColor;

    //! this is the font color for the console
    irr::video::SColor fontColor;

    //! this is the font name
    WideString fontName;

    //! this is the prompt string displayed as prompt$>
    WideString prompt;

    //! this is the command history length (defaults to 10)
    u32 commandHistorySize;

    //!key for opening/closing the console
    wchar_t key_tilde;
};
//=====================================================================================

//! A Quake Like console class
class IC_Console : public IC_Dispatcher, public IC_MessageSink
{
  public:
    //! constructor
    IC_Console();
    //! destructor
    virtual ~IC_Console();

    //! get the console config reference
    IC_ConsoleConfig& getConfig();
    //! (re)initialize the console with current config
    void initializeConsole( irr::gui::IGUIEnvironment* guienv, const irr::core::dimension2d<s32>& screenSize, const char* logFileName, bool standardOutput );

    //! loads a few default commands into the console
    void loadDefaultCommands();
    //! should console be visible
    bool isVisible();
    //! set the visibility of the console
    void setVisible( bool bV );
    //! toggle the visibility of the console
    void toggleVisible();

    //
    //  Message Sink implementation
    //
    //! add a UTF-16 message to the sink
    void add( const WideString message );
    void add( const WideString message, irr::video::SColor color );
    void addx( const char* msg, ... );
    void addx( irr::video::SColor color, const char* msg, ... );

    //! clear all the messages in the sink
    void clearMessages();

    //
    //  console rendering
    //

    //! render the console (it internally checks if the console is visible)
    void renderConsole( irr::gui::IGUIEnvironment* guienv, irr::video::IVideoDriver* videoDriver, const u32 deltaMillis );

    //
    //  console message handling
    //

    //! handles a key press when console is active/visible
    void handleKeyPress( wchar_t keyChar, irr::EKEY_CODE keyCode, bool bShiftDown, bool bControlDown, IrrlichtDevice* device );

    //!
    void externalCommandString( WideString wstr );

    bool inPrompt;


  protected:
    //! parses and handles the current command string
    virtual void handleCommandString( WideString& wstr );
    //! add a line to history
    void addToHistory( WideString& line );
    //! calculate the whole console rect
    void calculateConsoleRect( const irr::core::dimension2d<s32>& screenSize );
    //! calculate the messages rect and prompt / shell rect
    void calculatePrintRects( rect<s32>& textRect, rect<s32>& shellRect );
    //! calculate the various limits of the console
    bool calculateLimits( u32& maxLines, u32& lineHeight, s32& fontHeight );
    //! resize the message count
    void resizeMessages( int scrollAmount );
    //! do a tab completion
    virtual void tabComplete();

    void breakText( WideString Text, array<WideString>& BrokenText );

    //! console config data
    IC_ConsoleConfig consoleConfig;

    //! visibility flag
    bool bVisible;

    //! initialized in irrlicht?
    bool bInited;

    //! the font of the console
    irr::gui::IGUIFont* guiFont;

    //! the console rectangle
    irr::core::rect<s32> consoleRect;

    //! the console messages
    array<WideString> consoleMessages;
    //! the console messages
    array<irr::video::SColor> consoleMessagesColors;

    //! the command history
    array<WideString> consoleHistory;

    //! the history pointer / index
    u32 consoleHistoryIndex;

    //! the current command string
    WideString currentCommand;

    // consolelog
    CLog* consolelog;

    u32 typePlace;
    u32 consoleOffset;
};
//=====================================================================================


#define CONSOLE IC_MainConsole::GetSingleton()

class IC_MainConsole : public Singleton<IC_MainConsole>, public IC_Console
{
  public:
    //!
    virtual void addVariable( WideString name, WideString val );

    //!
    void SaveVariablesToFile( const char* filename );

  private:
    //! ME: console variables
    array<WideString> consoleVars;
    array<WideString> consoleVarsVals;
};

//=====================================================================================

class IC_ChatConsole : public IC_Console
{
  public:
    void setCustomRectangle( rect<s32> newRect );

    bool bActive;
  protected:
    virtual void handleCommandString( WideString& wstr );
};

#endif
