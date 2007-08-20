// Log.h: interface for the CLog class.
//
//////////////////////////////////////////////////////////////////////

#if !defined(AFX_LOG_H__78348862_06C2_4FAA_B192_13F09596E2FD__INCLUDED_)
#define AFX_LOG_H__78348862_06C2_4FAA_B192_13F09596E2FD__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

#include "../Game/stdafx.h"
#include "singleton.h"

// Logger class
class CLog
{
  public:
    CLog( const char* logfile, bool printToStandardOutput = false );
    virtual ~CLog();

    virtual void Write( const char* msg, ... );
    //virtual void Write( int target, unsigned long msgID, ... );

  protected:
    std::ofstream Log;
    bool so;
};

#define APPLOG CAppLog::GetSingleton()

// Main App Log singleton
class CAppLog : public Singleton<CAppLog>, public CLog
{
  public:
    CAppLog( const char* logfile, bool printToStandardOutput = false ) : CLog( logfile, printToStandardOutput )
    {
    }
    virtual ~CAppLog()
    {
    }
};

#endif // !defined(AFX_LOG_H__78348862_06C2_4FAA_B192_13F09596E2FD__INCLUDED_)
