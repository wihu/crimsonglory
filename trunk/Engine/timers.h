#ifndef TIMERS_H_INCLUDED
#define TIMERS_H_INCLUDED

class CTimerTask : public ITask
{
  public:
    bool Start();
    void Update();
    void Stop();
    AUTO_SIZE;
};


class ITickTimer : public IMMObject
{
  protected:
    int elapsedTicks, totalTicks;
    int loopsNum;
    CMMPointer<Functor> handler;

    static std::list<CMMPointer<ITickTimer> > timers;
  public:
    ITickTimer( Functor* h, int interval, int loops );
    virtual ~ITickTimer();

    bool bFreeze;

    virtual void Update();
    virtual void Kill();

    friend class CTimerTask;
    AUTO_SIZE;
};


#endif