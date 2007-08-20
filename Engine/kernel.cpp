// Kernel.cpp: implementation of the CKernel class.
//
//////////////////////////////////////////////////////////////////////
#ifndef USING_ENGINUITY
#define USING_ENGINUITY
#endif
#include "../App/stdafx.h"
#include "engine.h"
#include "kernel.h"
#include "misc.h"

//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////

CKernel::CKernel()
{
    GoalTicks = DEFAULT_GOALTICKS;

    // Timing variables
    mainTickCounter = 0;
    TimeInMil = TimeInMilLast = TimePassed = 0;
    Seconds = SecondsLast = 0;
    Ticks = TicksPerSecond = 0;
    Frames = FramesPerSecond = 0;
    TickTime = TickTimeLast = 0;
    frc = fps = frp = 0;
}

CKernel::~CKernel()
{
}

int CKernel::Execute()
{
    int i, mainControl, loopTicks;

    while ( !taskList.empty() )
    {
      {
          PROFILE( "Kernel task loop" );

          Number27Timing();

          std::list<CMMPointer<ITask> >::iterator it, thisIt;

          // frame-rate independent tasks
          loopTicks = TickTime - TickTimeLast;
          for ( mainControl = 0; mainControl < loopTicks; mainControl++ )
          {
            Ticks += 1;
            mainTickCounter += 1;

            for ( it = taskList.begin(); it != taskList.end(); )
            {
              ITask* t = ( *it );
              ++it;
              if ( ( !t->canKill ) && ( t->framerate_independent ) )
              {
                t->Update();
              }
            }
          }

          // frame-rate dependent tasks
          for ( it = taskList.begin(); it != taskList.end(); )
          {
            ITask* t = ( *it );
            ++it;
            if ( ( !t->canKill ) && ( !t->framerate_independent ) )
            {
              t->Update();
            }
          }

          //loop again to remove dead tasks
          for ( it = taskList.begin(); it != taskList.end(); )
          {
            ITask* t = ( *it );
            thisIt = it; ++it;
            if ( t->canKill )
            {
              t->Stop();
              // 25-07-03 - updated this to use erase() rather than remove()
              taskList.erase( thisIt );
              t = 0;
            }
          }

          IMMObject::CollectGarbage();
      }
#ifdef PROFILER
      CProfileSample::Output();
#endif

      delay( 1 );
    }


    return 0;
}

bool CKernel::AddTask( const CMMPointer<ITask>& t )
{
    if ( !t->Start() )
    {
      return false;
    }

    //keep the order of priorities straight
    std::list<CMMPointer<ITask> >::iterator it;
    for ( it = taskList.begin(); it != taskList.end(); it++ )
    {
      CMMPointer<ITask>& comp = ( *it );
      if ( comp->priority > t->priority )
      {
        break;
      }
    }
    taskList.insert( it, t );
    return true;
}

void CKernel::SuspendTask( const CMMPointer<ITask>& t )
{
    //check that this task is in our list - we don't want to suspend a task that isn't running
    if ( std::find( taskList.begin(), taskList.end(), t ) != taskList.end() )
    {
      t->OnSuspend();
      taskList.remove( t );
      pausedTaskList.push_back( t );
    }
}

void CKernel::ResumeTask( const CMMPointer<ITask>& t )
{
    if ( std::find( pausedTaskList.begin(), pausedTaskList.end(), t ) != pausedTaskList.end() )
    {
      t->OnResume();
      pausedTaskList.remove( t );
      //keep the order of priorities straight
      std::list<CMMPointer<ITask> >::iterator it;
      for ( it = taskList.begin(); it != taskList.end(); it++ )
      {
        CMMPointer<ITask>& comp = ( *it );
        if ( comp->priority > t->priority )
        {
          break;
        }
      }
      taskList.insert( it, t );
    }
}

void CKernel::RemoveTask( const CMMPointer<ITask>& t )
{
    assert( t.isValid() && "Tried to remove a null task." );
    if ( std::find( taskList.begin(), taskList.end(), t ) != taskList.end() )
    {
      t->canKill = true;
      return;
    }
    //check the suspended list
    std::list<CMMPointer<ITask> >::iterator it;
    if ( ( it = std::find( pausedTaskList.begin(), pausedTaskList.end(), t ) ) != taskList.end() )
    {
      //kill the task immediately
      t->Stop();
      pausedTaskList.erase( it );
      return;
    }
}

void CKernel::KillAllTasks()
{
    std::list<CMMPointer<ITask> >::iterator it;
    for ( it = taskList.begin(); it != taskList.end(); it++ )
    {
      ( *it )->canKill = true;
    }
    //kill all paused tasks right now
    for ( it = pausedTaskList.begin(); it != pausedTaskList.end(); it++ )
    {
      ( *it )->Stop();
    }
    pausedTaskList.clear();
}

void CKernel::Number27Timing()
{
    TimeInMilLast = TimeInMil;
    TimeInMil = getPreciseTime();
    if ( TimeInMil - TimeInMilLast > 2000 )
    {
      TimeInMilLast = TimeInMil;
    }
    TimePassed = TimePassed + ( TimeInMil - TimeInMilLast );

    SecondsLast = Seconds;
    Seconds = TimePassed / 1000;
    if ( Seconds != SecondsLast )
    {
      TicksPerSecond = Ticks;
      Ticks = 0;
      FramesPerSecond = Frames;
      Frames = 0;
    }


    Frames += 1;
    TickTimeLast = TickTime;
    TickTime = TimePassed / ( 1000 / GoalTicks );
}