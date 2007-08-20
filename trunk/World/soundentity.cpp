#include "soundentity.h"

////////////////////////////////////////////
// CSoundEntity
////////////////////////////////////////////

CSoundEntity::CSoundEntity( const char* strFile, CNewtonNode* ownerNode ) : CSoundObject( strFile )
{
    Reset();

    setDebugText( strFile );
    owner = ownerNode;
}

CSoundEntity::~CSoundEntity()
{
}

void CSoundEntity::Reset()
{
    owner = NULL;
}

void CSoundEntity::Think()
{
    if ( owner )
    {
      if ( owner->isZombie() )
      {
        owner = NULL;
        //CONSOLE.addx("Dropping sound entity: %s", getDebugText() );
        bCanDie = true;
        return;
      }

      vector3df vTemp = owner->getPosition() * IrrToSL;
      setPosition( &vTemp.X );
      vTemp = owner->getVelocity() * IrrToSL;
      setVelocity( &vTemp.X );
    }
}

void CSoundEntity::Render()
{
}