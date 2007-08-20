#include "CBackSpriteSceneNode.h"
#include "IrrlichtTask.h"

namespace irr
{
    namespace scene
    {
        //! constructor
        CBackSpriteSceneNode::CBackSpriteSceneNode( ISceneNode* parent, ISceneManager* mgr, s32 id, const core::vector3df& position, const core::dimension2d<f32>& size, int stripsNum ) : ISceneNode( parent, mgr, id, position )
        {
#ifdef _DEBUG
            setDebugName( "CBackSpriteSceneNode" );
#endif

            setSize( size );

            strips = stripsNum;

            vertices.set_used( strips * 4 );
            indices.set_used( strips * 6 );


            for ( int i = 0; i < strips; i++ )
            {
              indices[0 + i * 6] = 0 + i * 4;
              indices[1 + i * 6] = 2 + i * 4;
              indices[2 + i * 6] = 1 + i * 4;
              indices[3 + i * 6] = 0 + i * 4;
              indices[4 + i * 6] = 3 + i * 4;
              indices[5 + i * 6] = 2 + i * 4;

              vertices[0 + i * 4].TCoords.set( 1.0f, 1.0f );
              vertices[0 + i * 4].Color.set( 0xffffffff );

              vertices[1 + i * 4].TCoords.set( 1.0f, 0.0f );
              vertices[1 + i * 4].Color.set( 0xffffffff );

              vertices[2 + i * 4].TCoords.set( 0.0f, 0.0f );
              vertices[2 + i * 4].Color.set( 0xffffffff );

              vertices[3 + i * 4].TCoords.set( 0.0f, 1.0f );
              vertices[3 + i * 4].Color.set( 0xffffffff );
            }

            setMaterialFlag( video::EMF_BACK_FACE_CULLING, false );
			setMaterialFlag( video::EMF_LIGHTING, ( bool )IRR.useLighting );
        }



        CBackSpriteSceneNode::~CBackSpriteSceneNode()
        {
        }



        //! pre render event
        void CBackSpriteSceneNode::OnPreRender()
        {
            if ( IsVisible )
            {
              SceneManager->registerNodeForRendering( this );
              ISceneNode::OnPreRender();
            }
        }


        //! render
        void CBackSpriteSceneNode::render()
        {
            video::IVideoDriver* driver = SceneManager->getVideoDriver();
            ICameraSceneNode* camera = SceneManager->getActiveCamera();

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

            pos.X = campos.X;

            core::vector3df horizontal = up.crossProduct( view );
            horizontal.normalize();

            if ( horizontal.getLength() == 0 )
            {
              horizontal.set( up.Y, up.X, up.Z );
              horizontal.normalize();
            }

            core::vector3df vertical = horizontal.crossProduct( view );

            view *= -1.0f;

            for ( s32 i = 0; i < 4 * strips; ++i )
            {
              vertices[i].Normal = view;
            }

            horizontal *= 0.5f * Size.Width;
            vertical.normalize();
            vertical *= 0.5f * Size.Height;

            f32 offset = ( strips* Size.Width ) / 2;
            if ( strips == 1 )
            {
              offset = 0;
            }

            for ( int i = 0; i < strips; i++ )
            {
              vertices[0 + i * 4].Pos = pos + horizontal + vertical;
              vertices[1 + i * 4].Pos = pos + horizontal - vertical;
              vertices[2 + i * 4].Pos = pos - horizontal - vertical;
              vertices[3 + i * 4].Pos = pos - horizontal + vertical;

              vertices[0 + i * 4].Pos.X += Size.Width * i - offset;
              vertices[1 + i * 4].Pos.X += Size.Width * i - offset;
              vertices[2 + i * 4].Pos.X += Size.Width * i - offset;
              vertices[3 + i * 4].Pos.X += Size.Width * i - offset;
            }

            // draw

            if ( DebugDataVisible )
            {
              driver->setTransform( video::ETS_WORLD, AbsoluteTransformation );
              video::SMaterial m;
              m.Lighting = false;
              driver->setMaterial( m );
              driver->draw3DBox( BBox, video::SColor( 0, 208, 195, 152 ) );
            }

            core::matrix4 mat;
            driver->setTransform( video::ETS_WORLD, mat );

            driver->setMaterial( Material );

            driver->drawIndexedTriangleList( &vertices[0], strips * 4, &indices[0], 2 * strips );
        }


        //! returns the axis aligned bounding box of this node
        const core::aabbox3d<f32>& CBackSpriteSceneNode::getBoundingBox() const
        {
            return BBox;
        }


        //! sets the size of the billboard
        void CBackSpriteSceneNode::setSize( const core::dimension2d<f32>& size )
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

            f32 avg = ( size.Width + size.Height ) / 6;
            BBox.MinEdge.set( -avg, -avg, -avg );
            BBox.MaxEdge.set( avg, avg, avg );
        }


        video::SMaterial& CBackSpriteSceneNode::getMaterial( s32 i )
        {
            return Material;
        }


        //! returns amount of materials used by this scene node.
        s32 CBackSpriteSceneNode::getMaterialCount()
        {
            return 1;
        }


        //! gets the size of the billboard
        const core::dimension2d<f32>& CBackSpriteSceneNode::getSize()
        {
            return Size;
        }
    } // end namespace scene
} // end namespace irr

