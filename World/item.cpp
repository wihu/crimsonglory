#include "item.h"

#include "bullet.h"
#include "character.h"

////////////////////////////////////////////
// CItem 
////////////////////////////////////////////

//bool bRegistered_CItem = FACTORY->Actors.Register<CItem>( "item" );

CItem::CItem()
{
    Reset();
}

CItem::CItem( const c8* scriptFilename ) : CActor( scriptFilename )
{
    Reset();

    assemblePhysics( scriptFilename, BODY_HULL, vector3df( 0.1f, 0.1f, 0.1f ), vector3df( 0, 0, 0 ), 10.0f, true );
}

CItem::~CItem()
{
}

void CItem::Render()
{
    CActor::Render();
}

void CItem::Reset()
{
    CActor::Reset();

    type = NODECLASS_ITEM;
}


void CItem::Think()
{
    CActor::Think();

    if ( control )
    {
    }
}

void CItem::attachToParentNode( CNewtonNode* node, void* data )
{
    CNewtonNode::attachToParentNode( node );

    setControls( ( CControls * )data );
}

void CItem::unAttachFromParent()
{
    CNewtonNode::unAttachFromParent();

    setControls( NULL );
}