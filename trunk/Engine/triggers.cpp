#ifndef USING_ENGINUITY
#define USING_ENGINUITY
#endif
#include "engine.h"
#include "triggers.h"

std::list<CMMPointer<ITrigger> > ITrigger::triggerList;

ITrigger::ITrigger( Functor* h, bool fo )
{
    handler = h;
    bFireOnce = fo;
    triggerList.push_back( this );
}

ITrigger::~ITrigger()
{
}

void ITrigger::Kill()
{
    std::list<CMMPointer<ITrigger> >::iterator it = std::find( triggerList.begin(), triggerList.end(), ( const ITrigger* )this );
    if ( it != triggerList.end() )
    {
      ( *it ) = 0;
    }
}

void ITrigger::Tick()
{
    if ( Test() )
    {
      ( *handler )();
      if ( bFireOnce )
      {
        Kill();
      }
    }
}

bool CTriggerTask::Start()
{
    return true;
}
void CTriggerTask::Stop()
{
    ITrigger::triggerList.clear();
}
void CTriggerTask::Update()
{
    PROFILE( "Trigger task" );
    std::list<CMMPointer<ITrigger> >::iterator it, ite = ITrigger::triggerList.end(), itT;
    for ( it = ITrigger::triggerList.begin(); it != ite; it++ )
    {
      if ( ( *it ).isValid() )
      {
        ( *it )->Tick();
      }
      else
      {
        itT = it;
        --it;
        ITrigger::triggerList.erase( itT );
      }
    }
}


