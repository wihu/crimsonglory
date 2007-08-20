#ifndef BUGGY_H_INCLUDED
#define BUGGY_H_INCLUDED

#include "../Engine/engine.h"
#include "../World/entity.h"
#include "../World/controllable.h"
#include "../World/vehicle.h"


////////////////////////////////////////////
// CBuggy 
////////////////////////////////////////////

class CBuggy : public CVehicle
{
  public:
    CBuggy( const c8* scriptFilename );
    virtual ~CBuggy();

	virtual void Reset();
	virtual void Think();
	virtual void Render();

	virtual void Load(const c8* filename);

  private:
	f32 fTorque, fBrake;
	bool bRotChange;

	f32 fRotationRate;
	vector3df vAxisRotation, vOldAxisRotation;
};

#endif