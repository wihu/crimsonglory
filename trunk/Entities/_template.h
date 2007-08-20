#ifndef BUGGY_H_INCLUDED
#define BUGGY_H_INCLUDED

#include "../Engine/engine.h"
#include "../World/entity.h"
#include "../World/controllable.h"


////////////////////////////////////////////
// CBuggy 
////////////////////////////////////////////

class CBuggy : public CEntity
{
  public:
    CBuggy();
    virtual ~CBuggy();

	virtual void Reset();
	virtual void Think();
	virtual void Render();

  private:

};

#endif