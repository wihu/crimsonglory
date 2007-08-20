#ifndef CONTROLLABLE_H_INCLUDED
#define CONTROLLABLE_H_INCLUDED

#include "entity.h"
#include "controls.h"

class CControllable : public CEntity
{
  public:
    CControllable();
    virtual ~CControllable();

    virtual void Reset();

    void setControls( CControls* c );
    CControls* getControls()
    {
        return control;
    }

  protected:
    CControls* control;

  private:
};

#endif