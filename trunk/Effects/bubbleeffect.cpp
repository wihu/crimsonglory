#include "bubbleeffect.h"

#include "../Game/SingletonIncludes.h"

////////////////////////////////////////////
// CBubbleEffect 
////////////////////////////////////////////

bool bRegistered_CBubbleEffect = FACTORY->Effects.Register<CBubbleEffect>( "bubble" );

CBubbleEffect::CBubbleEffect( vector3df OldPos, vector3df NewPos, f32 oneOverMassConst, f32 radiusConst, int aliveTime ) : CEffect( OldPos, NewPos, oneOverMassConst, radiusConst, aliveTime )
{
    Reset();

    fScale = radiusConst;
    interpolator = NULL;

    dimension2d<f32> dsize( fScale, fScale );
    node = IRR.smgr->addBillboardSceneNode( 0, dsize, NewPos );

    node->setMaterialType( video::EMT_TRANSPARENT_ALPHA_CHANNEL );
    node->setMaterialFlag( video::EMF_LIGHTING, true );
    node->setMaterialTexture( 0, IRR.video->getTexture( APP.useFile( "Sprites/bubble.png" ).c_str() ) );
}

CBubbleEffect::~CBubbleEffect()
{
}

void CBubbleEffect::Reset()
{
    CEffect::Reset();
}

void CBubbleEffect::Think()
{
    CEffect::Think();
}   