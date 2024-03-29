// ProfileLogHandler.cpp: implementation of the CProfileLogHandler class.
//
//////////////////////////////////////////////////////////////////////

#ifndef USING_ENGINUITY
#define USING_ENGINUITY
#endif
#include "engine.h"
#include "profileLogHandler.h"

//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////

void CProfileLogHandler::BeginOutput( float tTime )
{
    APPLOG.Write( "  Min :   Avg :   Max :   Time:  # : Profile Name (this loop: %2.5f seconds, %3.1f FPS)", tTime, 60.0f / tTime );
    APPLOG.Write( "--------------------------------------------" );
}

void CProfileLogHandler::Sample( float fMin, float fAvg, float fMax, float tAvg, int callCount, std::string name, int parentCount )
{
    //char szBuf[100];
    //for(int i=0;i<parentCount;i++){szBuf[i]=' ';}
    //szBuf[parentCount]=0;
    char namebuf[256], indentedName[256];
    char avg[16], min[16], max[16], num[16], time[16];

    sprintf( avg, "%3.1f", fAvg );
    sprintf( min, "%3.1f", fMin );
    sprintf( max, "%3.1f", fMax );
    sprintf( time, "%3.1f", tAvg );
    sprintf( num, "%3d", callCount );

    strcpy( indentedName, name.c_str() );
    for ( int indent = 0; indent < parentCount; ++indent )
    {
      sprintf( namebuf, " %s", indentedName );
      strcpy( indentedName, namebuf );
    }

    APPLOG.Write( "%5s : %5s : %5s : %5s : %3s : %s", min, avg, max, time, num, indentedName );
}

void CProfileLogHandler::EndOutput()
{
    APPLOG.Write( "\n" );
}