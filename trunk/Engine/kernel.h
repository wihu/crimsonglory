// Kernel.h: interface for the CKernel class.
//
//////////////////////////////////////////////////////////////////////

#if !defined(AFX_KERNEL_H__4E822B7D_1078_4F70_BC8F_3BB4F83ED0AF__INCLUDED_)
#define AFX_KERNEL_H__4E822B7D_1078_4F70_BC8F_3BB4F83ED0AF__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

#include "singleton.h"

#define DEFAULT_GOALTICKS 60

class ITask;

#define KERNEL CKernel::GetSingleton()

class CKernel : public Singleton<CKernel>
{
  public:
    CKernel();
    virtual ~CKernel();

    virtual int Execute();

    virtual bool AddTask( const CMMPointer<ITask>& t );
    virtual void SuspendTask( const CMMPointer<ITask>& t );
    virtual void ResumeTask( const CMMPointer<ITask>& t );
    virtual void RemoveTask( const CMMPointer<ITask>& t );
    virtual void KillAllTasks();

    int GetTicks()
    {
        return mainTickCounter;
    }

    void SetGoalTicks( int g )
    {
        GoalTicks = g; Number27Timing();
    }

  protected:
    std::list<CMMPointer<ITask> > taskList;
    std::list<CMMPointer<ITask> > pausedTaskList;

  private:
    //{********** NUMBER27's TIMING ROUTINES ***********}
    void Number27Timing();

    int mainTickCounter;
    int TimeInMil, TimeInMilLast;
    int TimePassed;
    int Seconds, SecondsLast;
    int Ticks, TicksPerSecond;
    int Frames, FramesPerSecond, TickTime, TickTimeLast;
    int GoalTicks;
    int frc, fps, frp;
};

class ITask : virtual public IMMObject
{
  public:
    ITask()
    {
        canKill = false;priority = 5000;
    }
    virtual bool Start() = 0;
    virtual void OnSuspend()
    {
    };
    virtual void Update() = 0;
    virtual void OnResume()
    {
    };
    virtual void Stop() = 0;

    bool canKill;
    long priority;
    bool framerate_independent;
};

#define ADDTASK(TaskName, TaskPriority, TaskFramerate_independent) \
    TaskName->priority = TaskPriority; \
    TaskName->framerate_independent = TaskFramerate_independent;\
    KERNEL.AddTask(CMMPointer<ITask>(TaskName));\

#endif // !defined(AFX_KERNEL_H__4E822B7D_1078_4F70_BC8F_3BB4F83ED0AF__INCLUDED_)
