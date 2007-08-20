// Copyright (C) 2002-2005 Nikolaus Gebhardt
// This file is part of the "Irrlicht Engine".
// For conditions of distribution and use, see copyright notice in irrlicht.h

#include "CSkyBackSceneNode.h"

namespace irr
{
    namespace scene
    {
        //! constructor
        CSkyBackSceneNode::CSkyBackSceneNode( video::ITexture* tex, ISceneNode* parent, ISceneManager* mgr, s32 id ) : ISceneNode( parent, mgr, id )
        {
#ifdef _DEBUG
            setDebugName( "CSkyBackSceneNode" );
#endif

            AutomaticCullingEnabled = false;

            // create indices

            Indices[0] = 2;
            Indices[1] = 1; 
            Indices[2] = 0; 
            Indices[3] = 3; 
            Indices[4] = 2; 
            Indices[5] = 0; 

            // create material

            video::SMaterial mat;
            mat.Lighting = false;
            mat.ZBuffer = false;
            mat.ZWriteEnable = false;
            mat.BilinearFilter = true;

            f32 onepixel = 0.0f;

            if ( tex )
            {
              core::dimension2d<s32> dim = tex->getSize();
              onepixel = 1.0f / ( dim.Width * 1.5f );
            }


            f32 l = 1.0f;
            f32 z = 12.0f;
            f32 t = 1.0f - onepixel;
            f32 o = 0.0f + onepixel;

            Material = mat;
            Material.Texture1 = tex;
            Vertices[0] = video::S3DVertex( -l, l, -z, 0, 0, 1, video::SColor( 255, 255, 255, 255 ), o, t );
            Vertices[1] = video::S3DVertex( l, l, -z, 0, 0, 1, video::SColor( 255, 255, 255, 255 ), t, t );
            Vertices[2] = video::S3DVertex( l, -l, -z, 0, 0, 1, video::SColor( 255, 255, 255, 255 ), t, o );
            Vertices[3] = video::S3DVertex( -l, -l, -z, 0, 0, 1, video::SColor( 255, 255, 255, 255 ), o, o );

            VerticesR[0] = video::S3DVertex( 2*l + -l, l, -z, 0, 0, 1, video::SColor( 255, 255, 255, 255 ), o, t );
            VerticesR[1] = video::S3DVertex( 2*l + l, l, -z, 0, 0, 1, video::SColor( 255, 255, 255, 255 ), t, t );
            VerticesR[2] = video::S3DVertex( 2*l + l, -l, -z, 0, 0, 1, video::SColor( 255, 255, 255, 255 ), t, o );
            VerticesR[3] = video::S3DVertex( 2*l + -l, -l, -z, 0, 0, 1, video::SColor( 255, 255, 255, 255 ), o, o );
		}



        //! destructor
        CSkyBackSceneNode::~CSkyBackSceneNode()
        {
        }


        //! renders the node.
        void CSkyBackSceneNode::render()
        {
            video::IVideoDriver* driver = SceneManager->getVideoDriver();
            scene::ICameraSceneNode* camera = SceneManager->getActiveCamera();

            if ( !camera || !driver )
            {
              return;
            }

            core::matrix4 mat;
            mat.setTranslation( camera->getAbsolutePosition() );

            driver->setTransform( video::ETS_WORLD, mat );

            driver->setMaterial( Material );
            driver->drawIndexedTriangleList( Vertices, 4, Indices, 2 );
            driver->drawIndexedTriangleList( VerticesR, 4, Indices, 2 );

			//triangle3df triangle;
			//vector3df star, star2, star3;
			//for (int i=0; i < 1000; i++)
			//{
			//	star = vector3df( Vertices[0].Pos.X+i, Vertices[0].Pos.Y+i, Vertices[0].Pos.Z - 10.0f );
			//	star2 = vector3df( Vertices[1].Pos.X+i, Vertices[1].Pos.Y+i, Vertices[1].Pos.Z + 10.0f );
			//	star3 = vector3df( Vertices[2].Pos.X+i, Vertices[2].Pos.Y+i, Vertices[2].Pos.Z + 10.0f );
			//	triangle.set( star, star2, star3 );
			//	driver->draw3DTriangle( triangle, SColor(255, 255, 255, 255 ) );
			//}
        }



        //! returns the axis aligned bounding box of this node
        const core::aabbox3d<f32>& CSkyBackSceneNode::getBoundingBox() const
        {
            return Box;
        }


        void CSkyBackSceneNode::OnPreRender()
        {
            if ( IsVisible )
            {
              SceneManager->registerNodeForRendering( this, ESNRP_SKY_BOX );
            }

            ISceneNode::OnPreRender();
        }


        //! returns the material based on the zero based index i. To get the amount
        //! of materials used by this scene node, use getMaterialCount().
        //! This function is needed for inserting the node into the scene hirachy on a
        //! optimal position for minimizing renderstate changes, but can also be used
        //! to directly modify the material of a scene node.
        video::SMaterial& CSkyBackSceneNode::getMaterial( s32 i )
        {
            return Material;
        }


        //! returns amount of materials used by this scene node.
        s32 CSkyBackSceneNode::getMaterialCount()
        {
            return 1;
        }
    } // end namespace scene
} // end namespace irr

