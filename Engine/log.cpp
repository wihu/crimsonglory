// Log.cpp: implementation of the CLog class.
//
//////////////////////////////////////////////////////////////////////

#include "log.h"

//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////

CLog::CLog( const char* logfile, bool printToStandardOutput )
{
    Log.open( logfile );
    so = printToStandardOutput;
}

CLog::~CLog()
{
    Log.close();
}


void CLog::Write( const char* msg, ... )
{
    va_list args; va_start( args, msg );
    char szBuf[1024];
    vsprintf( szBuf, msg, args );

    // standard output
    if ( so )
    {
      printf( szBuf );
      printf( "\n" );
    }

    Log << szBuf << "\n";
#ifdef _DEBUG
    Log.flush();
#endif

    va_end( args );
}


//#endif
