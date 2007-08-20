#ifndef INTERPOLATORS_H_INCLUDED
#define INTERPOLATORS_H_INCLUDED

class CInterpolatorTask : public ITask
{
  public:
    virtual bool Start();
    virtual void Update();
    virtual void Stop();
    AUTO_SIZE;
};


class IInterpolator : public IMMObject
{
  protected:
    float& target;
    static std::list<CMMPointer<IInterpolator> > interpolators;
  public:
    IInterpolator( float& t );
    virtual ~IInterpolator();

    bool bFreeze;

    virtual void Update() = 0;
    virtual void Kill();

    friend class CInterpolatorTask;
    AUTO_SIZE;
};


class ITimebasedInterpolator : public IInterpolator
{
  protected:
    float elapsedTicks, totalTicks;
    virtual void Calculate() = 0;
  public:
    virtual void Reset();
    virtual void Update();  
    ITimebasedInterpolator( float& targ, float time );
    AUTO_SIZE;
};

class CLinearTimeInterpolator : public ITimebasedInterpolator
{
  protected:
    float startVal, endVal;
    virtual void Calculate();
  public:
    CLinearTimeInterpolator( float& targ, float time, float sV, float eV );
    AUTO_SIZE;
};

class CQuadraticTimeInterpolator : public ITimebasedInterpolator
{
  protected:
    float startVal, midVal, endVal;
    virtual void Calculate();
  public:
    CQuadraticTimeInterpolator( float& targ, float time, float sV, float mV, float eV );
    AUTO_SIZE;
};

class CCubicTimeInterpolator : public ITimebasedInterpolator
{
  protected:
    float startVal, midVal1, midVal2, endVal;
    virtual void Calculate();
  public:
    CCubicTimeInterpolator( float& targ, float time, float sV, float mV1, float mV2, float eV );
    AUTO_SIZE;
};

#endif