#include "controllable.h"

CControllable::CControllable() : CEntity()
{
    Reset();
}

CControllable::~CControllable()
{
}

void CControllable::Reset()
{
    //CEntity::Reset();
    control = NULL;
    //printf("CControllable::Reset()\n");
}

void CControllable::setControls( CControls* c )
{
    control = c;
}