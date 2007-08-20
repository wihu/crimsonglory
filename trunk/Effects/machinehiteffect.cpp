#include "machinehiteffect.h"

#include "../Game/SingletonIncludes.h"


////////////////////////////////////////////
// CMachineHitEffect 
////////////////////////////////////////////

bool bRegistered_CMachineHitEffect = FACTORY->Effects.Register<CMachineHitEffect>( "machinehit" );

CMachineHitEffect::CMachineHitEffect( vector3df OldPos, vector3df NewPos, f32 oneOverMassConst, f32 radiusConst, int aliveTime ) : CEffect( OldPos, NewPos, oneOverMassConst, radiusConst, aliveTime )
{
    Reset();

    fScale = radiusConst * 1.0f;
    interpolator = NULL;
    int frames = 8;

    spritenode = new CAnimSpriteSceneNode( IRR.smgr->getRootSceneNode(), IRR.smgr, -1 ); 
    spritenode->Load( APP.useFile( "Sprites/smallhit.png" ).c_str(), 0, 0, 256, 256 * frames, 256, 256, false, true ); 
    spritenode->setMaterialType( video::EMT_TRANSPARENT_ALPHA_CHANNEL );

    spritenode->setSpeed( aliveTime / frames );
    spritenode->setStartEndFrame( 0, frames );
    spritenode->setFrame( 0 );

    spritenode->setSize( dimension2d<f32>( fScale, fScale ) );
    spritenode->setPosition( NewPos );
    spritenode->updateAbsolutePosition();
}

CMachineHitEffect::~CMachineHitEffect()
{
    if ( ( IRR.smgr ) && ( spritenode ) )
    {
      spritenode->remove();
    }
}

void CMachineHitEffect::Reset()
{
    CEffect::Reset();
}

void CMachineHitEffect::Think()
{
    CEffect::Think();

    if ( spritenode )
    {
      spritenode->Update();
    }
}   