#include "../App/stdafx.h"

#include "engine.h"
#include "render.h"

std::list<CMMPointer<CRenderable> > CRenderable::renderables;

/////////////////
// CRenderable //
/////////////////

CRenderable::CRenderable( int renderLevel )
{
    level = renderLevel;
    visible = true;

    std::list<CMMPointer<CRenderable> >::iterator it, ite = renderables.end();
    for ( it = renderables.begin(); it != ite; it++ )
    {
      if ( ( *it ).isValid() )
      {
        if ( ( *it )->getRenderLevel() > level )
        {
          renderables.insert( it, this );
          return;
        }
      }
    }

    //first element
    renderables.push_back( this );
}

CRenderable::~CRenderable()
{
}

void CRenderable::Kill()
{
    std::list<CMMPointer<CRenderable> >::iterator it = ( std::find( renderables.begin(), renderables.end(), ( const CRenderable* )this ) ), itT;
    if ( it != renderables.end() )
    {
      ( *it ) = 0;
      //itT=it;
      //--it;
      //renderables.erase(itT);
    }
}


void CRenderable::RenderAll()
{
    PROFILE( "Render all" );

    std::list<CMMPointer<CRenderable> >::iterator it, ite = renderables.end(), itT;
    for ( it = renderables.begin(); it != ite; it++ )
    {
      if ( ( *it ).isValid() )
      {
        if ( ( *it )->getVisible() )
        {
          ( *it )->Render();
        }
      }
      else // for safety
      {
        itT = it;
        --it;
        renderables.erase( itT );
      }
    }
}