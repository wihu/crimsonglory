#ifndef _IrrConsole_vars_h_
#define _IrrConsole_vars_h_
#include "../Engine/engine.h"
#include "console_utils.h"
#include "console.h"
#include "../App/SingletonIncludes.h"

// macros rule...

#define GMVARTOVALUE(VarName, VarType, Variable) \
    { \
    std::stringstream str; \
    str.unsetf(std::ios::skipws); \
    str<<IC_StrConv::toString(SCRIPT.gmVarToValue(VarName)); \
    VarType res; \
    str>>res; \
    Variable = res; \
    } \

#define GMVARTOSTRING(VarName, Variable) \
    Variable = IC_StrConv::toString(SCRIPT.gmVarToValue(VarName)); \


// NUMBERS CONSOLE VARIABLE MACRO
//ex. CONSOLE_VAR( "dupa", f32, k, 5.111, L"Just do it", L"Dupny komand");

#define CONSOLE_VAR(CommandName, VarType, Variable, DefaultValue, Usage, Description) \
{ \
class ICConsoleNewCmd : public IC_Command \
{ \
public: \
    ICConsoleNewCmd(VarType *t): IC_Command(CommandName) \
                                { \
                                    setUsage(Usage); \
                                    addDescLine(Description); \
                                    dat = t; \
                                    (*dat) = DefaultValue; \
                                } \
    virtual ~ICConsoleNewCmd()        {}; \
    bool invoke(const array<WideString>& args, IC_Dispatcher* pDispatcher, IC_MessageSink* pOutput) \
                                { \
                                    if (args.size() > 0) \
                                    { \
                                        CONSOLE.addVariable(CommandName, args[0]); \
                                        std::stringstream str; \
                                        str.unsetf(std::ios::skipws); \
                                        str<<IC_StrConv::toString(args[0]); \
                                        VarType res; \
                                        str>>res; \
                                        (*dat) = res; \
                                    } \
                                    else \
                                    { \
                                        WideString s = " "; \
                                        s += CommandName; s += " = "; \
                                        std::stringstream str; \
                                        str.unsetf(std::ios::skipws); \
                                        str<<(*dat); \
                                        std::string res; \
                                        str>>res; \
                                        int len = res.size() + 1; \
                                        wchar_t* buf = new wchar_t[len]; \
                                        ::mbstowcs(buf,res.c_str(),len); \
                                        WideString wstr = buf; \
                                        delete[] buf; \
                                        s += wstr; \
                                        pOutput->add(s); \
                                        printDesc(pOutput); \
                                    } \
                                    return true; \
                                } \
private:\
    VarType *dat;\
}; \
CONSOLE.registerCommand(new ICConsoleNewCmd(&Variable)); \
if (SCRIPT.gmVarToValue(CommandName).size() > 0) \
{ \
    std::stringstream str; \
    str.unsetf(std::ios::skipws); \
    str<<IC_StrConv::toString(SCRIPT.gmVarToValue(CommandName)); \
    VarType res; \
    str>>res; \
    Variable = res; \
    CONSOLE.addVariable(CommandName, SCRIPT.gmVarToValue(CommandName)); \
} \
else Variable = DefaultValue; \
}\


// WIDESTRING CONSOLE VARIABLE MACRO
//ex. CONSOLE_VAR_S( "dupa", s, "fik", L"Just do it", L"Dupny komand")

#define CONSOLE_VAR_S(CommandName, Variable, DefaultValue, Usage, Description) \
{ \
class ICConsoleNewCmd : public IC_Command \
{ \
public: \
    ICConsoleNewCmd(WideString *t): IC_Command(CommandName) \
                                { \
                                    setUsage(Usage); \
                                    addDescLine(Description); \
                                    dat = t; \
                                    (*dat) = DefaultValue; \
                                } \
    virtual ~ICConsoleNewCmd()        {}; \
    bool invoke(const array<WideString>& args, IC_Dispatcher* pDispatcher, IC_MessageSink* pOutput) \
                                { \
                                    if (args.size() > 0) \
                                        (*dat) = args[0]; \
                                    else \
                                    { \
                                        WideString s = " "; \
                                        s += CommandName; s += " = "; s += (*dat); \
                                        pOutput->add(s); \
                                        printDesc(pOutput); \
                                    } \
                                    for (int i=1; i < args.size(); i++) \
                                    { \
                                        (*dat).append(L" "); \
                                        (*dat) += args[i]; \
                                    } \
                                    CONSOLE.addVariable(CommandName, getBracketString((*dat))); \
                                    return true; \
                                } \
private:\
    WideString *dat;\
}; \
CONSOLE.registerCommand( new ICConsoleNewCmd(&Variable)); \
if (SCRIPT.gmVarToValue(CommandName).size() > 0) \
    { \
        Variable = SCRIPT.gmVarToValue(CommandName); \
    } \
else Variable = DefaultValue; \
CONSOLE.addVariable(CommandName, getBracketString(Variable)); \
}\

inline WideString getBracketString( WideString CommandName )
{
    WideString b = "`"; b.append( CommandName ); b.append( "`" );
    return b;
}



// SCOLOR CONSOLE VARIABLE MACRO
//ex. 

#define CONSOLE_VAR_C( CommandName, Variable, DefaultValue, Usage, Description) \
{\
class ICConsoleNewCmd : public IC_Command \
{ \
public: \
    ICConsoleNewCmd(irr::video::SColor *t): IC_Command(CommandName) \
                                { \
                                    setUsage(Usage); \
                                    addDescLine(Description); \
                                    dat = t; \
                                    (*dat) = DefaultValue; \
                                } \
    virtual ~ICConsoleNewCmd()        {}; \
    bool invoke(const array<WideString>& args, IC_Dispatcher* pDispatcher, IC_MessageSink* pOutput) \
                                { \
                                    if (args.size() > 3) \
                                        { \
                                            (*dat).setAlpha(wchar2i(args[0])); \
                                            (*dat).setRed(wchar2i(args[1])); \
                                            (*dat).setGreen(wchar2i(args[2])); \
                                            (*dat).setBlue(wchar2i(args[3])); \
                                        } \
                                    else \
                                        { \
                                            WideString s = " "; \
                                            s += CommandName; s += " = "; \
                                            s += i2wchar((*dat).getAlpha()); s += " ";\
                                            s += i2wchar((*dat).getRed()); s += " ";\
                                            s += i2wchar((*dat).getGreen()); s += " ";\
                                            s += i2wchar((*dat).getBlue());\
                                            pOutput->add(s); \
                                            printDesc(pOutput); \
                                        } \
                                    WideString b = CommandName; b.append("_a"); \
                                    CONSOLE.addVariable( b, i2wchar((*dat).getAlpha()) ); \
                                    b = CommandName; b.append("_r"); \
                                    CONSOLE.addVariable( b, i2wchar((*dat).getRed()) ); \
                                    b = CommandName; b.append("_g"); \
                                    CONSOLE.addVariable( b, i2wchar((*dat).getGreen()) ); \
                                    b = CommandName; b.append("_b"); \
                                    CONSOLE.addVariable( b, i2wchar((*dat).getBlue()) ); \
                                    return true; \
                                } \
private:\
    irr::video::SColor *dat;\
}; \
CONSOLE.registerCommand(new ICConsoleNewCmd(&Variable)); \
Variable = getSColorFromScript(CommandName, DefaultValue);\
}\


inline irr::video::SColor getSColorFromScript( WideString CommandName, irr::video::SColor defaultColor )
{
    irr::video::SColor color;
    WideString b = CommandName; b.append( "_a" );
    char* c = wchar2char( b );
    if ( SCRIPT.gmVarToValue( c ).size() > 0 )
    {
      color.setAlpha( wchar2i( SCRIPT.gmVarToValue( c ) ) ); 
      CONSOLE.addVariable( b, i2wchar( color.getAlpha() ) );
    }
    else
    {
      color.setAlpha( defaultColor.getAlpha() ); 
      CONSOLE.addVariable( b, i2wchar( defaultColor.getAlpha() ) );
    }
    b = CommandName; b.append( "_r" ); 
    delete c; c = wchar2char( b );
    if ( SCRIPT.gmVarToValue( c ).size() > 0 )
    {
      color.setRed( wchar2i( SCRIPT.gmVarToValue( c ) ) ); 
      CONSOLE.addVariable( b, i2wchar( color.getRed() ) );
    }
    else
    {
      color.setRed( defaultColor.getRed() ); 
      CONSOLE.addVariable( b, i2wchar( defaultColor.getRed() ) );
    }

    b = CommandName; b.append( "_g" ); 
    delete c; c = wchar2char( b );
    if ( SCRIPT.gmVarToValue( c ).size() > 0 )
    {
      color.setGreen( wchar2i( SCRIPT.gmVarToValue( c ) ) ); 
      CONSOLE.addVariable( b, i2wchar( color.getGreen() ) );
    }
    else
    {
      color.setGreen( defaultColor.getGreen() ); 
      CONSOLE.addVariable( b, i2wchar( defaultColor.getGreen() ) );
    }

    b = CommandName; b.append( "_b" ); 
    delete c; c = wchar2char( b );
    if ( SCRIPT.gmVarToValue( c ).size() > 0 )
    {
      color.setBlue( wchar2i( SCRIPT.gmVarToValue( c ) ) ); 
      CONSOLE.addVariable( b, i2wchar( color.getBlue() ) );
    }
    else
    {
      color.setBlue( defaultColor.getBlue() ); 
      CONSOLE.addVariable( b, i2wchar( defaultColor.getBlue() ) );
    }

    return color;
}


#endif