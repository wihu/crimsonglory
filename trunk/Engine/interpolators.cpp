#ifndef USING_ENGINUITY
#define USING_ENGINUITY
#endif
#include "engine.h"
#include "interpolators.h"

std::list<CMMPointer<IInterpolator> > IInterpolator::interpolators;

IInterpolator::IInterpolator( float& t ) : target( t )
{
    interpolators.push_back( this );
    bFreeze = false;
}

IInterpolator::~IInterpolator()
{
}

void IInterpolator::Kill()
{
    std::list<CMMPointer<IInterpolator> >::iterator it = ( std::find( interpolators.begin(), interpolators.end(), ( const IInterpolator* )this ) );
    if ( it != interpolators.end() )
    {
      ( *it ) = 0;
    }
}

bool CInterpolatorTask::Start()
{
    return true;
}
void CInterpolatorTask::Stop()
{
    IInterpolator::interpolators.clear();
}
void CInterpolatorTask::Update()
{
    PROFILE( "Interpolator task" );
    std::list<CMMPointer<IInterpolator> >::iterator it, ite = IInterpolator::interpolators.end(), itT;
    for ( it = IInterpolator::interpolators.begin(); it != ite; it++ )
    {
      if ( ( *it ).isValid() )
      {
        ( *it )->Update();
      }
      else
      {
        itT = it;
        --it;
        IInterpolator::interpolators.erase( itT );
      }
    }
}

ITimebasedInterpolator::ITimebasedInterpolator( float& targ, float time ) : IInterpolator( targ )
{
    elapsedTicks = 0;
    totalTicks = time;
}

void ITimebasedInterpolator::Update()
{
    if ( bFreeze )
    {
      return;
    }
    elapsedTicks += 1;
    Calculate();
    if ( elapsedTicks > totalTicks )
    {
      Kill();
    }
}

void ITimebasedInterpolator::Reset()
{
    elapsedTicks = 0;
}

CLinearTimeInterpolator::CLinearTimeInterpolator( float& targ, float time, float sV, float eV ) : ITimebasedInterpolator( targ, time )
{
    startVal = sV;
    endVal = eV;
    Calculate();
}

void CLinearTimeInterpolator::Calculate()
{
    float b = clamp( elapsedTicks / totalTicks, 0, 1 );
    target = startVal * ( 1 - b ) + endVal * b;
}

CQuadraticTimeInterpolator::CQuadraticTimeInterpolator( float& targ, float time, float sV, float mV, float eV ) : ITimebasedInterpolator( targ, time )
{
    startVal = sV;
    midVal = mV;
    endVal = eV;
}

void CQuadraticTimeInterpolator::Calculate()
{
    float b = clamp( elapsedTicks / totalTicks, 0, 1 ), a = 1 - b;
    target = startVal * a * a + midVal * 2 * a * b + endVal * b * b;
}

CCubicTimeInterpolator::CCubicTimeInterpolator( float& targ, float time, float sV, float mV1, float mV2, float eV ) : ITimebasedInterpolator( targ, time )
{
    startVal = sV;
    midVal1 = mV1;
    midVal2 = mV2;
    endVal = eV;
}

void CCubicTimeInterpolator::Calculate()
{
    float b = clamp( elapsedTicks / totalTicks, 0, 1 ), a = 1 - b;
    target = startVal * a * a * a + midVal1 * 3 * a * a * b + midVal2 * 3 * a * b * b + endVal * b * b * b;
}
