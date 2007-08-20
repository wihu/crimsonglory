#ifndef SCRIPTWEAPON_H_INCLUDED
#define SCRIPTWEAPON_H_INCLUDED

#include "../Engine/engine.h"
#include "../World/weapon.h"


////////////////////////////////////////////
// CScriptWeapon
////////////////////////////////////////////

class CScriptWeapon : public CWeapon
{
  public:
    CScriptWeapon( const c8* scriptFilename );
    virtual ~CScriptWeapon();

    virtual void Reset();                           
    virtual void Think();
    virtual void Render();

    virtual void Unserialize( RakNet::BitStream& bt );

  protected:
    virtual void Load( const c8* filename );
};

#endif