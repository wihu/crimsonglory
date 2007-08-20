#ifndef FLAG_H_INCLUDED
#define FLAG_H_INCLUDED

#include "../Engine/engine.h"
#include "../World/item.h"

///////////////////////////////////////////
// CItem
////////////////////////////////////////////

class CFlag : public CItem
{
  public:
    CFlag( const c8* scriptFilename );
    virtual ~CFlag();

    virtual void Reset();                           
    virtual void Think();
    virtual void Render();

    virtual void Unserialize( RakNet::BitStream& bt );

  protected:
    virtual void Load( const c8* filename );

    NewtonJoint* m_upVector;
};

#endif