#ifndef SCREENTEXT_H_INCLUDED
#define SCREENTEXT_H_INCLUDED

#include "../Engine/engine.h"

#include "actor.h"
#include "entity.h"

//////////////////////////////
// CScreenText
//////////////////////////////

class CScreenText : public CEntity
{
  public:
    CScreenText( const char* newtext, CActor* attach, int timeon, position2d<s32> offset = position2d<s32>( 0, 0 ) );
    virtual ~CScreenText();
    AUTO_SIZE;

    virtual void Render();
    virtual void Think();

    void Kill();

  protected:
    WideString text;
    CActor* parent;
    position2d<s32> screenPos, posOffset;
    dimension2d<s32> textDim;
    IGUIFont* font;
    SColor fontColor, rectColor;
};

#endif