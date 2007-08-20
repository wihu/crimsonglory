#include "splasheffect.h"

#include "../Game/SingletonIncludes.h"

////////////////////////////////////////////
// CSplashEffect 
////////////////////////////////////////////

bool bRegistered_CSplashEffect = FACTORY->Effects.Register<CSplashEffect>( "splash" );

CSplashEffect::CSplashEffect( vector3df OldPos, vector3df NewPos, f32 oneOverMassConst, f32 radiusConst, int aliveTime ) : CEffect( OldPos, NewPos, oneOverMassConst, radiusConst, aliveTime )
{
    Reset();

    fScale = radiusConst * 0.25f;
    interpolator = new CQuadraticTimeInterpolator( fScale, aliveTime, fScale, radiusConst * 1.75f, radiusConst * 1.0f );

    dimension2d<f32> dsize( fScale, fScale );
    node = IRR.smgr->addBillboardSceneNode( 0, dsize, NewPos );

    //  node->setMaterialType(video::EMT_TRANSPARENT_ADD_COLOR);
    node->setMaterialType( video::EMT_TRANSPARENT_ALPHA_CHANNEL );
   //bill node->setMaterialFlag( video::EMF_LIGHTING, true );
    node->setMaterialTexture( 0, IRR.video->getTexture( APP.useFile( "Sprites/sprycol.png" ).c_str() ) );   
    //CONSOLE_FLOAT(1.0f - radiusConst/70.0f);
}

CSplashEffect::~CSplashEffect()
{
}

void CSplashEffect::Reset()
{
    CEffect::Reset();
}

void CSplashEffect::Think()
{
    CEffect::Think();
}   