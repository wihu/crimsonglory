#include "entity.h"
#include "world.h"
// GAME, APP.
#include "../Game/GameDLL.h"
// IRR.
#include "../irrlicht/IrrlichtTask.h"

////////////////////////////////////////////
// CEntity 
////////////////////////////////////////////

CEntity::CEntity()
{
    Reset();

    WORLD.AddEntity( this );
}

CEntity::~CEntity()
{
}

void CEntity::Reset()
{
    debugText = L"";
    bInvalidEntity = false;
    bCanDie = false;
    alive = -1;
}

void CEntity::Think()
{
    if ( alive > 0 )
    {
      alive--;
      if ( alive == 0 )
      {
        bCanDie = true;
      }
    }
}

void CEntity::Render()
{
    if ( ( APP.DebugMode ) && ( debugText.size() > 0 ) )
    {
      //debugScreenPos = IRR.smgr->getSceneCollisionManager()->getScreenCoordinatesFrom3DPosition( vCenter, WORLD.getIrrCamera() );
      WideString wstr;
      wstr = debugText.c_str();

      if ( bInvalidEntity )
      {
        wstr += L" - Invalid entity";
      }

      IRR.gui->getBuiltInFont()->draw( wstr.c_str(), core::rect<s32>( debugScreenPos.X, debugScreenPos.Y, debugScreenPos.X + 100, debugScreenPos.Y + 50 ), irr::video::SColor( 255, 255, 25, 25 ), false, true );
    }
}

void CEntity::Die()
{
    WORLD.RemoveEntity( this );
}
