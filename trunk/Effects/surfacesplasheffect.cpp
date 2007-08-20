#include "surfacesplasheffect.h"

#include "../Game/SingletonIncludes.h"


////////////////////////////////////////////
// CSurfaceSplashEffect 
////////////////////////////////////////////

bool bRegistered_CSurfaceSplashEffect = FACTORY->Effects.Register<CSurfaceSplashEffect>( "surfacesplash" );

CSurfaceSplashEffect::CSurfaceSplashEffect( vector3df OldPos, vector3df NewPos, f32 oneOverMassConst, f32 radiusConst, int aliveTime ) : CEffect( OldPos, NewPos, oneOverMassConst, radiusConst, aliveTime )
{
    Reset();

    fScale = radiusConst * 0.1f;
    interpolator = new CLinearTimeInterpolator( fScale, aliveTime, fScale, fScale * 3.0f );

    spritenode = new CAnimSpriteSceneNode( IRR.smgr->getRootSceneNode(), IRR.smgr, -1 ); 
    spritenode->Load( APP.useFile( "Sprites/bigsplash.png" ).c_str(), false, false ); 
    spritenode->setMaterialType( video::EMT_TRANSPARENT_ALPHA_CHANNEL );
	spritenode->setMaterialFlag( video::EMF_LIGHTING, (bool)IRR.useLighting );

    spritenode->setScale( vector3df( fScale, fScale, fScale ) );
    spritenode->setPosition( NewPos - vector3df( 0.0f, 0.1f, 0.0f ) );
    spritenode->setRotation( vector3df( 90, 0, 0 ) );
    spritenode->updateAbsolutePosition();
}

CSurfaceSplashEffect::~CSurfaceSplashEffect()
{
    if ( ( IRR.smgr ) && ( spritenode ) )
    {
      spritenode->remove();
    }
}

void CSurfaceSplashEffect::Reset()
{
    CEffect::Reset();
}

void CSurfaceSplashEffect::Think()
{
    CEffect::Think();

    if ( spritenode )
    {
      spritenode->setScale( vector3df( fScale, 3 * fScale, fScale ) );
    }
}   