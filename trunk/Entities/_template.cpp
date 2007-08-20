#include "buggy.h"
#include "../IrrConsole/console_vars.h"

////////////////////////////////////////////
// CBuggy 
////////////////////////////////////////////

CBuggy::CBuggy()
{
	Reset();
}

CBuggy::~CBuggy()
{
}

void CBuggy::Reset()
{
	CEntity::Reset();
}

void CBuggy::Think()
{
	CEntity::Think();
	//setDebugPos( IRR.smgr->getSceneCollisionManager()->getScreenCoordinatesFrom3DPosition( vCenter, WORLD.getIrrCamera() );
}

void CBuggy::Render()
{
	CEntity::Render();
}