#include "watercircleeffect.h"

#include "../Game/SingletonIncludes.h"


////////////////////////////////////////////
// CWaterCircleEffect 
////////////////////////////////////////////

bool bRegistered_CWaterCircleEffect = FACTORY->Effects.Register<CWaterCircleEffect>( "watercircle" );

CWaterCircleEffect::CWaterCircleEffect( vector3df OldPos, vector3df NewPos, f32 oneOverMassConst, f32 radiusConst, int aliveTime ) : CEffect( OldPos, NewPos, oneOverMassConst, radiusConst, aliveTime )
{
    Reset();

    fScale = radiusConst * 0.1f;
    interpolator = new CLinearTimeInterpolator( fScale, aliveTime, fScale, fScale * 100.0f );

    spritenode = new CAnimSpriteSceneNode( IRR.smgr->getRootSceneNode(), IRR.smgr, -1 ); 
    spritenode->Load( APP.useFile( "Sprites/wcircle.png" ).c_str(), false, false ); 
    spritenode->setMaterialType( video::EMT_TRANSPARENT_ADD_COLOR );
	spritenode->setMaterialFlag( video::EMF_LIGHTING, (bool)IRR.useLighting );

    spritenode->setScale( vector3df( fScale, fScale, fScale ) );
    spritenode->setPosition( NewPos - vector3df( 0.0f, 0.1f, 0.0f ) );
    spritenode->setRotation( vector3df( 90, 0, 0 ) );
    spritenode->updateAbsolutePosition();
}

CWaterCircleEffect::~CWaterCircleEffect()
{
    if ( ( IRR.smgr ) && ( spritenode ) )
    {
      spritenode->remove();
    }
}

void CWaterCircleEffect::Reset()
{
    CEffect::Reset();
}

void CWaterCircleEffect::Think()
{
    CEffect::Think();

    if ( spritenode )
    {
      spritenode->setScale( vector3df( fScale, 3 * fScale, fScale ) );
    }
}   