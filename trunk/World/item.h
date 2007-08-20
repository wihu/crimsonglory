#ifndef ITEM_H_INCLUDED
#define ITEM_H_INCLUDED

#include "../Engine/engine.h"
#include "actor.h"

///////////////////////////////////////////
// CItem
////////////////////////////////////////////

class CItem : public CActor
{
  public:
    CItem();
    CItem( const c8* modelFilename );
    virtual ~CItem();

    virtual void Reset();                           
    virtual void Think();
    virtual void Render();

  protected:
    void attachToParentNode( CNewtonNode* node, void* data );
    void unAttachFromParent();
};

#endif