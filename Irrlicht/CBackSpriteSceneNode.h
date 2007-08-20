#ifndef __C_BACKSPRITE_SCENE_NODE_H_INCLUDED__
#define __C_BACKSPRITE_SCENE_NODE_H_INCLUDED__

#include "../Engine/engine.h"

namespace irr
{
    namespace scene
    {
        class CBackSpriteSceneNode : public ISceneNode
        {
          public:

            //! constructor
            CBackSpriteSceneNode( ISceneNode* parent, ISceneManager* mgr, s32 id, const core::vector3df& position, const core::dimension2d<f32>& size, int stripsNum );

            virtual ~CBackSpriteSceneNode();

            //! pre render event
            virtual void OnPreRender();

            //! render
            virtual void render();

            //! returns the axis aligned bounding box of this node
            virtual const core::aabbox3d<f32>& getBoundingBox() const;

            //! sets the size of the billboard
            virtual void setSize( const core::dimension2d<f32>& size );

            //! gets the size of the billboard
            virtual const core::dimension2d<f32>& getSize();

            virtual video::SMaterial& getMaterial( s32 i );

            //! returns amount of materials used by this scene node.
            virtual s32 getMaterialCount();

            //! Returns type of the scene node
            virtual ESCENE_NODE_TYPE getType()
            {
                return ESNT_BILLBOARD;
            }

          private:

            core::dimension2d<f32> Size;
            core::aabbox3d<f32> BBox;
            video::SMaterial Material;

            array<video::S3DVertex> vertices;
            array<u16> indices;

            u16 strips;
        };
    } // end namespace scene
} // end namespace irr

#endif

