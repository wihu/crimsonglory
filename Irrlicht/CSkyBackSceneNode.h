// Copyright (C) 2002-2005 Nikolaus Gebhardt
// This file is part of the "Irrlicht Engine".
// For conditions of distribution and use, see copyright notice in irrlicht.h

#ifndef __C_SKY_BACK_SCENE_NODE_H_INCLUDED__
#define __C_SKY_BACK_SCENE_NODE_H_INCLUDED__

#include "../Engine/engine.h"

namespace irr
{
    namespace scene
    {
        // Skybox, rendererd with zbuffer turned off, before all other nodes.
        class CSkyBackSceneNode : public ISceneNode
        {
          public:

            //! constructor
            CSkyBackSceneNode( video::ITexture* tex, ISceneNode* parent, ISceneManager* mgr, s32 id );

            //! destructor
            virtual ~CSkyBackSceneNode();

            virtual void OnPreRender();

            //! renders the node.
            virtual void render();

            //! returns the axis aligned bounding box of this node
            virtual const core::aabbox3d<f32>& getBoundingBox() const;

            virtual video::SMaterial& getMaterial( s32 i );

            //! returns amount of materials used by this scene node.
            virtual s32 getMaterialCount();

          private:

            core::aabbox3d<f32> Box;
            u16 Indices[6];
            video::S3DVertex Vertices[4];
			video::S3DVertex VerticesR[4];
            video::SMaterial Material;
        };
    } // end namespace scene
} // end namespace irr

#endif

