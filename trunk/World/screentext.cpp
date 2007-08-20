#include "screentext.h"

#include "world.h"

////////////////////////////////////////////
// CScreenText 
////////////////////////////////////////////

CScreenText::CScreenText( const char* newtext, CActor* attach, int timeon, position2d<s32> offset ) : CEntity()
{
    text = newtext;
    parent = attach;
    alive = timeon;
    font = IRR.guiFont;
    textDim = font->getDimension( text.c_str() );
    screenPos = posOffset = offset;
    fontColor = SColor( 45, 20, 20, 45 );
    rectColor = SColor( 255, 255, 240, 240 );
}

CScreenText::~CScreenText()
{
}

void CScreenText::Render()
{
    CEntity::Render();

    core::rect<s32> rect( screenPos.X - textDim.Width / 2 - 2, screenPos.Y - textDim.Height / 2 - 2, screenPos.X + textDim.Width / 2 + 2, screenPos.Y + textDim.Height / 2 + 2 );

    if ( parent )
    {
      screenPos = IRR.smgr->getSceneCollisionManager()->getScreenCoordinatesFrom3DPosition( parent->getPosition(), IRR.smgr->getActiveCamera() );
      rect.UpperLeftCorner += posOffset;
      rect.LowerRightCorner += posOffset;
    }

    IRR.video->draw2DRectangle( fontColor, rect );

    font->draw( text.c_str(), rect, rectColor, true, true );
}

void CScreenText::Think()
{
    CEntity::Think();

    if ( bCanDie )
    {
      if ( parent )
      {
        parent->aboveText = NULL;
      }
    }
}

void CScreenText::Kill()
{
    bCanDie = true;
    if ( parent )
    {
      parent->aboveText = NULL;
    }
}