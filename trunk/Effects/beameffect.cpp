#include "beameffect.h"

#include "../World/world.h"

////////////////////////////////////////////
// CBeamEffect 
////////////////////////////////////////////

bool bRegistered_CBeamEffect = FACTORY->Effects.Register<CBeamEffect>( "beam" );

CBeamEffect::CBeamEffect( vector3df OldPos, vector3df NewPos, f32 oneOverMassConst, f32 radiusConst, int aliveTime ) : CEffect( OldPos, NewPos, oneOverMassConst, radiusConst, aliveTime )
{
    Reset();

	this->OldPos = OldPos;
    this->Pos = NewPos;
    beam = NULL;

    fFade = 255.0f;
    interpolator = new CQuadraticTimeInterpolator( fFade, aliveTime, 255.0f, 170.0f, 0.0f );

    beam = new CBeamNode( IRR.smgr->getRootSceneNode(), IRR.smgr, -1, APP.useFile( "Sprites/fireball.bmp" ).c_str() ); 
    //beam->setMaterialType(video::EMT_TRANSPARENT_ALPHA_CHANNEL);EMT_TRANSPARENT_VERTEX_ALPHA
    beam->setMaterialType( EMT_TRANSPARENT_ADD_COLOR );
    beam->SetBeamScale( radiusConst );
}

CBeamEffect::~CBeamEffect()
{
    if ( ( IRR.smgr ) && ( beam ) )
    {
      beam->remove();
    }
}

void CBeamEffect::Reset()
{
    CEffect::Reset();
}

void CBeamEffect::Think()
{
    CEffect::Think();

    // set the rendering
    UpdateBeam();
}

void CBeamEffect::UpdateBeam()
{
    if ( beam )
    {
      beam->SetStartPoint( OldPos ); 
      beam->SetEndPoint( Pos ); 
      beam->SetBeamColorStart( SColor( 255, fFade, fFade, fFade ) );
      beam->SetBeamColorEnd( SColor( 255, fFade, fFade, fFade ) );
    }
}