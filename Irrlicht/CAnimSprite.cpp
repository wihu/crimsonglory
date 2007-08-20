#include "CAnimSprite.h"
#include "IrrlichtTask.h"

void CAnimSpriteSceneNode::render()
{
    IVideoDriver* driver = SceneManager->getVideoDriver();
    ICameraSceneNode* camera = SceneManager->getActiveCamera();

    if ( billboard )
    {
      if ( !camera || !driver )
      {
        return;
      }

      // make billboard look to camera

      core::vector3df pos = getAbsolutePosition();

      core::vector3df campos = camera->getAbsolutePosition();
      core::vector3df target = camera->getTarget();
      core::vector3df up = camera->getUpVector();
      core::vector3df view = target - campos;
      view.normalize();

      core::vector3df horizontal = up.crossProduct( view );
      horizontal.normalize();

      if ( horizontal.getLength() == 0 )
      {
        horizontal.set( up.Y, up.X, up.Z );
        horizontal.normalize();
      }

      core::vector3df vertical = horizontal.crossProduct( view );

      view *= 1.0f;

      for ( s32 i = 0; i < 4; ++i )
      {
        Vertices[i].Normal = view;
      }

      horizontal *= 0.5f * Size.Width;
      vertical.normalize();
      vertical *= 0.5f * Size.Height;

      Vertices[0].Pos = pos + horizontal + vertical;
      Vertices[1].Pos = pos + horizontal - vertical;
      Vertices[2].Pos = pos - horizontal - vertical;
      Vertices[3].Pos = pos - horizontal + vertical;

      // draw

      core::matrix4 mat;
      driver->setTransform( video::ETS_WORLD, mat );
    }
    else
    {
      SceneManager->getVideoDriver()->setTransform( irr::video::ETS_WORLD, AbsoluteTransformation );
    }

	Material.Lighting = ( bool )IRR.useLighting;
    driver->setMaterial( Material );
    driver->drawIndexedTriangleList( &Vertices[0], 4, &Indices[0], 4 );
} 

//! sets the size of the billboard
void CAnimSpriteSceneNode::setSize( const core::dimension2d<f32>& size )
{
    Size = size;

    if ( Size.Width == 0.0f )
    {
      Size.Width = 1.0f;
    }

    if ( Size.Height == 0.0f )
    {
      Size.Height = 1.0f;
    }
}