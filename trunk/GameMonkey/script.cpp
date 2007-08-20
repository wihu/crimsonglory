#include "script.h"

#include "gmSystemLib.h"
#include "gmmathlib.h"
#include "gmstringlib.h"
#include "gmarraylib.h"
#include "gmsystemlib.h"
#include "gmvector3lib.h"

#include "../IrrConsole/console.h"
#include "../Engine/misc.h"
#include "scriptFunctions.h"

CScript::CScript()
{
    machine = new gmMachine();

    gmBindMathLib( machine );
    gmBindStringLib( machine );
    gmBindArrayLib( machine );
    gmBindSystemLib( machine );
    gmBindVector3Lib( machine );

    deltaTime = 0;
    lastTime = getPreciseTime();
}

CScript::~CScript()
{
    delete machine;
}

bool CScript::RunString( WideString a_string )
{
    GM_ASSERT( this );

    int threadId = GM_INVALID_THREAD;

    char* ResultChar = new char[1024]; 
    wcstombs( ResultChar, a_string.c_str(), 1024 );

    errors = machine->ExecuteString( ResultChar, &threadId, true );
    if ( errors )
    {
      WideString a;
      a = "Script error: "; a.append( a_string );
      LogErrors( a );
    }

    deltaTime = 1;
    lastTime = getPreciseTime();

    delete ResultChar;

    return true;
}


bool CScript::RunFile( WideString a_fileName )
{
    FILE* scriptFile = NULL;
    char* fileString = NULL;
    int fileSize = 0;
    bool result = true;

    //a_fileName = APP.useFile( wide2string(a_fileName).c_str() ).c_str();
    a_fileName = wide2string( a_fileName ).c_str();

    GM_ASSERT( this );

    if ( APP.DebugMode )
    {
      WideString winfo = "Loading script file ";
      winfo += a_fileName;
      CONSOLE.add( winfo );
    }

    char* ResultChar = new char[1024]; 
    wcstombs( ResultChar, a_fileName.c_str(), 1024 );

    if ( !( scriptFile = fopen( ResultChar, "rb" ) ) )
    {
      return false;
    }

    fseek( scriptFile, 0, SEEK_END );
    fileSize = ftell( scriptFile );
    fseek( scriptFile, 0, SEEK_SET );
    fileString = new char[fileSize + 1];
    fread( fileString, fileSize, 1, scriptFile );
    fileString[fileSize] = 0; // Terminating null
    fclose( scriptFile );

    int threadId = GM_INVALID_THREAD;
    errors = machine->ExecuteString( fileString, &threadId, true, ResultChar );
    if ( errors )
    {
      WideString a;
      a = "Script error: "; a.append( a_fileName );
      LogErrors( a );
      result = false;
    }

    deltaTime = 1;
    lastTime = getPreciseTime();

    delete[] fileString;
    delete ResultChar;

    return result;
}

bool CScript::Run()
{
    if ( machine->Execute( deltaTime ) )
    {
      // Update delta time
      curTime = getPreciseTime();
      deltaTime = curTime - lastTime;
      lastTime = curTime;

      // Dump run time errors to output
      LogErrors( "Script error: runtime" );
    }

    return true;
}


void CScript::LogErrors( WideString header )
{
    bool first = true;
    const char* message;
    while ( ( message = machine->GetLog().GetEntry( first ) ) )
    {
      CONSOLE.add( header, COLOR_ERROR );
      CONSOLE.add( message, COLOR_ERROR );
    }
    machine->GetLog().Reset();
}

WideString CScript::gmVarToValue( const char* varName )
{
    gmTableObject* gtab = machine->GetGlobals();

    WideString wstr = "";

    if ( !gtab )
    {
      return wstr;
    } 

    gmVariable myVar = gtab->Get( machine, varName ); 

    if ( myVar.m_type == GM_NULL )
    {
      return wstr;
    }

    char retval[256]; 
    memset( retval, 0, 256 ); 

    // AsString
    myVar.AsStringWithType( machine, retval, 256 ); 

    std::string str = retval;

    int len = str.size() + 1; 
    wchar_t* buf = new wchar_t[len];     
    ::mbstowcs( buf, str.c_str(), len ); 
    wstr = buf; 
    delete[] buf; 

    return wstr;
}

void CScript::SaveGlobalsToFile( WideString filename )
{
}

// modified from Greg's GM forums post

void CScript::gmTableToStringArray( gmTableObject* a_table, array<String>& arr )
{
    if ( !a_table )
    {
      return;
    } 

    // Some operating memory for AsString() 
    const int bufferLen = 256; 
    char buffer[bufferLen]; 

    //if(!a_data) 
    //{ 
    //  a_data = Data::Create("", Data::TYPE_NODE); 
    //} 

    gmTableIterator it; 
    gmTableNode* curNode = a_table->GetFirst( it ); 
    while ( curNode )
    {
      int valueType = curNode->m_value.m_type; 
      //gmString keyString = curNode->m_key.AsString(machine, buffer, bufferLen); 
      // NOTE: keyString may just be "1","2" etc for indexed members, may want to ignore these 
      gmVariable* curVar = &curNode->m_value; 

      if ( valueType == GM_STRING )
      {
        gmStringObject* objString = ( gmStringObject* )( curVar->m_value.m_ref ); 
        arr.push_back( objString->GetString() );
        //Data* newData = a_data->Add(keyString, Data::TYPE_STRING); 
        // newData->SetString(objString->GetString()); 
      } 

      //if(valueType == GM_INT) 
      //{ 
      //  Data* newData = a_data->Add(keyString, Data::TYPE_INT); 
      //  newData->SetInt(curVar->m_value.m_int); 
      //} 
      //else if(valueType == GM_FLOAT) 
      //{ 
      //  Date* newData = a_data->Add(keyString, Data::TYPE_FLOAT); 
      //  newData->SetFloat(curVar->m_value.m_float); 
      //} 
      //else if(valueType == GM_STRING) 
      //{ 
      //  gmStringObject* objString = (gmStringObject*)(curVar->m_value.m_ref); 
      //  Data* newData = a_data->Add(keyString, Data::TYPE_STRING); 
      //  newData->SetString(objString->GetString()); 
      //} 
      //else if(valueType == GM_TABLE) 
      //{ 
      //  gmTableObject* objTable = (gmTableObject*)(curVar->m_value.m_ref); 
      //  Data* newData = a_data->Add(keyString, Data::TYPE_NODE); 
      //  gmTableToData(machine, objTable, newData); // Recurse for nested table 
      //} 
      //else 
      //{ 
      //  ASSERT("Invalid type for table conversion"); 
      //  return NULL; 
      //} 

      curNode = a_table->GetNext( it );
    } 
    // return a_data; 
} 


//Data* gmTableToData(gmMachine* machine, gmVariable* a_var) 
//{ 
//  if(a_var->m_type == GM_TABLE) 
//  { 
//    gmTableObject* table = (gmTableObject*)a_var->m_value.m_ref; 
//    return gmTableToData(machine, table, NULL); 
//  } 
//  return NULL; 
//} 


void CScript::gmTableToStringArray( const char* a_globalTableName, array<String>& arr )
{
    gmVariable foundVar = machine->GetGlobals()->Get( machine, a_globalTableName ); 
    if ( foundVar.m_type == GM_TABLE )
    {
      gmTableObject* table = ( gmTableObject* )foundVar.m_value.m_ref; 
      gmTableToStringArray( table, arr );
    }
}

///////////

void ScriptPrintCallback( gmMachine* a_machine, const char* a_string )
{
#ifdef _CLIENT
    CONSOLE.add( a_string, COLOR_SCRIPT );
#else
    printf( "%s\n", a_string );
#endif
}

gmPrintCallback gmMachine::s_printCallback = ScriptPrintCallback;




// ? - added so compiler is happy with GameMonkey

void gmDebuggerBreak( gmDebuggerSession* a_session, int a_threadId, int a_sourceId, int a_lineNumber )
{
}
void gmDebuggerRun( gmDebuggerSession* a_session, int a_threadId )
{
}
void gmDebuggerStop( gmDebuggerSession* a_session, int a_threadId )
{
}
void gmDebuggerSource( gmDebuggerSession* a_session, int a_sourceId, const char* a_sourceName, const char* a_source )
{
}
void gmDebuggerException( gmDebuggerSession* a_session, int a_threadId )
{
}
void gmDebuggerBeginContext( gmDebuggerSession* a_session, int a_threadId, int a_callFrame )
{
}
void gmDebuggerContextCallFrame( gmDebuggerSession* a_session, int a_callFrame, const char* a_functionName, int a_sourceId, int a_lineNumber, const char* a_thisSymbol, const char* a_thisValue, int a_thisId )
{
}
void gmDebuggerContextVariable( gmDebuggerSession* a_session, const char* a_varSymbol, const char* a_varValue, int a_varId )
{
}
void gmDebuggerEndContext( gmDebuggerSession* a_session )
{
}
void gmDebuggerBeginSourceInfo( gmDebuggerSession* a_session )
{
}
void gmDebuggerSourceInfo( gmDebuggerSession* a_session, int a_sourceId, const char* a_sourceName )
{
}
void gmDebuggerEndSourceInfo( gmDebuggerSession* a_session )
{
}
void gmDebuggerBeginThreadInfo( gmDebuggerSession* a_session )
{
}
void gmDebuggerThreadInfo( gmDebuggerSession* a_session, int a_threadId, int a_threadState )
{
}
void gmDebuggerEndThreadInfo( gmDebuggerSession* a_session )
{
}
void gmDebuggerError( gmDebuggerSession* a_session, const char* a_error )
{
}
void gmDebuggerMessage( gmDebuggerSession* a_session, const char* a_message )
{
}
void gmDebuggerAck( gmDebuggerSession* a_session, int a_response, int a_posNeg )
{
}
void gmDebuggerQuit( gmDebuggerSession* a_session )
{
}