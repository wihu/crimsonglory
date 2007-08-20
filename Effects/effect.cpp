#include "effect.h"

#include "../World/world.h"

////////////////////////////////////////////
// CEffect 
////////////////////////////////////////////

CEffect::CEffect()
{
    Reset();
}

CEffect::CEffect( vector3df OldPos, vector3df NewPos, f32 oneOverMassConst, f32 radiusConst, int aliveTime ) : CWorldPart( NewPos, oneOverMassConst, radiusConst, 1.0f, false, aliveTime )
{
    Reset();

    this->OldPos = OldPos;
    this->Pos = NewPos;

    fScale = radiusConst;
}

CEffect::~CEffect()
{
    if ( interpolator )
    {
      interpolator->Kill();
    }
}

void CEffect::Reset()
{
    CWorldPart::Reset();
    interpolator = NULL;
}

void CEffect::Think()
{
    CWorldPart::Think();

    if ( node )
    {
      node->setSize( dimension2d<f32>( -fScale, -fScale ) ); //the sprite is somehow inverted?
    }
}   