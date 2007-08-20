#include "engine.h"
#include "timers.h"

std::list<CMMPointer<ITickTimer> > ITickTimer::timers;

void ITickTimer::Kill()
{
    std::list<CMMPointer<ITickTimer> >::iterator it = ( std::find( timers.begin(), timers.end(), ( const ITickTimer* )this ) );
    if ( it != timers.end() )
    {
      ( *it ) = 0;
    }
}

bool CTimerTask::Start()
{
    return true;
}
void CTimerTask::Stop()
{
    ITickTimer::timers.clear();
}
void CTimerTask::Update()
{
    PROFILE( "Timer task" );
    std::list<CMMPointer<ITickTimer> >::iterator it, ite = ITickTimer::timers.end(), itT;
    for ( it = ITickTimer::timers.begin(); it != ite; it++ )
    {
      if ( ( *it ).isValid() )
      {
        ( *it )->Update();
      }
      else
      {
        itT = it;
        --it;
        ITickTimer::timers.erase( itT );
      }
    }
}

ITickTimer::ITickTimer( Functor* h, int interval, int loops )
{
    timers.push_back( this );
    bFreeze = false;
    handler = h;
    elapsedTicks = 0;
    totalTicks = interval;
    loopsNum = loops;
}

ITickTimer::~ITickTimer()
{
}

void ITickTimer::Update()
{
    if ( bFreeze )
    {
      return;
    }

    elapsedTicks += 1;

    if ( elapsedTicks > totalTicks )
    {
      ( *handler )();
      loopsNum -= 1;
      if ( loopsNum < 0 )
      {
        Kill();
      }
    }
}