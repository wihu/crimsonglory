#include "gunsplasheffect.h"

#include "../Game/SingletonIncludes.h"

////////////////////////////////////////////
// CGunSplashEffect 
////////////////////////////////////////////

bool bRegistered_CGunSplashEffect = FACTORY->Effects.Register<CGunSplashEffect>( "gunsplash" );

CGunSplashEffect::CGunSplashEffect( vector3df OldPos, vector3df NewPos, f32 oneOverMassConst, f32 radiusConst, int aliveTime ) : CEffect( OldPos, NewPos, oneOverMassConst, radiusConst, aliveTime )
{
    Reset();

    fStartScale = fScale = radiusConst;
    interpolator = new CQuadraticTimeInterpolator( fScale, aliveTime, fScale, radiusConst * 17.0f, radiusConst * 0.5f );

    dimension2d<f32> dsize( fScale, fScale );
    node = IRR.smgr->addBillboardSceneNode( 0, dsize, NewPos );

    node->setMaterialType( video::EMT_TRANSPARENT_ALPHA_CHANNEL );
    node->setMaterialTexture( 0, IRR.video->getTexture( APP.useFile( "Sprites/gunsplash.png" ).c_str() ) );
}

CGunSplashEffect::~CGunSplashEffect()
{
}

void CGunSplashEffect::Reset()
{
    CEffect::Reset();
}

void CGunSplashEffect::Think()
{
    CEffect::Think();

    if ( node )
    {
      node->setSize( dimension2d<f32>( -fStartScale, -fScale ) );
    }
}   