#include "dusteffect.h"

#include "../Game/SingletonIncludes.h"


////////////////////////////////////////////
// CDustEffect 
////////////////////////////////////////////

bool bRegistered_CDustEffect = FACTORY->Effects.Register<CDustEffect>( "dust" );

CDustEffect::CDustEffect( vector3df OldPos, vector3df NewPos, f32 oneOverMassConst, f32 radiusConst, int aliveTime ) : CEffect( OldPos, NewPos, oneOverMassConst, radiusConst, aliveTime )
{
    Reset();

    fScale = radiusConst * 1.0f;
    interpolator = new CLinearTimeInterpolator( fScale, aliveTime, fScale, fScale * 2.0f );

    spritenode = new CAnimSpriteSceneNode( IRR.smgr->getRootSceneNode(), IRR.smgr, -1 ); 
    spritenode->Load( APP.useFile( "Sprites/dust.png" ).c_str(), 0, 0, 256, 256 * 4, 256, 256, false, true ); 
    spritenode->setMaterialType( video::EMT_TRANSPARENT_ALPHA_CHANNEL );
	spritenode->setMaterialFlag( video::EMF_LIGHTING, (bool)IRR.useLighting );

    //spritenode->setSpeed(4);
    //  spritenode->setStartEndFrame( 0, 3 );
    spritenode->setFrame( random( 4 ) );

    spritenode->setSize( dimension2d<f32>( fScale, fScale ) );
    spritenode->setPosition( NewPos );
    spritenode->updateAbsolutePosition();
}

CDustEffect::~CDustEffect()
{
    if ( ( IRR.smgr ) && ( spritenode ) )
    {
      spritenode->remove();
    }
}

void CDustEffect::Reset()
{
    CEffect::Reset();
}

void CDustEffect::Think()
{
    CEffect::Think();

    if ( spritenode )
    {
      spritenode->setSize( dimension2d<f32>( fScale, fScale ) );
      spritenode->Update();
    }
}   