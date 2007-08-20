#ifndef SCRIPT_H_INCLUDED
#define SCRIPT_H_INCLUDED

#include "../Engine/engine.h"

#include "gmThread.h"
#include "gmDebugger.h"
#include "gmCallScript.h"

#include "../Engine/singleton.h"

#define SCRIPT CScript::GetSingleton()

class CScript : public Singleton<CScript>
{
  public:
    CScript();
    virtual ~CScript();

    virtual bool Run();
    virtual bool RunString( WideString a_string );
    virtual bool RunFile( WideString a_fileName );

    virtual void SaveGlobalsToFile( WideString filename );

    // return a string value of the gmMachine global variable
    virtual WideString gmVarToValue( const char* varName );

    void gmTableToStringArray( gmTableObject* a_table, array<String>& arr );
    void gmTableToStringArray( const char* a_globalTableName, array<String>& arr );

    gmMachine* machine;

  protected:

  private:
    void LogErrors( WideString header );

    // for script executing
    int deltaTime, curTime, lastTime;
    int errors;
};

#endif