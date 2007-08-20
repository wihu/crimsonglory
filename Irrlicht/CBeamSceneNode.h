#ifndef BEAMNODE_H_INCLUDED
#define BEAMNODE_H_INCLUDED

#include "../Engine/engine.h"

namespace irr
{
    namespace scene
    {
        // code by cdbarrett from the Irrlicht forums
        // Nov 17, 2005

        // Just a simple wrapper :D 
        struct IrrQuad
        {
            video::S3DVertex verts[4];
        }; 

        class CBeamNode : public scene::ISceneNode
        {
          private: 
            // The beam material. 
            video::SMaterial material; 

            // Start/End Points 
            core::vector3df vStart; 
            core::vector3df vEnd; 

            // Bounding Box 
            core::aabbox3d<f32> Box; 

            // Size of the beam 
            float flScale; 


            void DrawQuad( IrrQuad& quad )
            {
                u16 indices[] =
                {
                  0, 2, 3, 2, 1, 3, 1, 0, 3, 2, 0, 1
                }; 
                video::IVideoDriver* driver = SceneManager->getVideoDriver(); 
                driver->setMaterial( material ); 
                driver->drawIndexedTriangleList( &quad.verts[0], 4, &indices[0], 4 );
            } 

            // Thanks to whoever wrote this little function :) 
            core::vector3df getTargetAngle( core::vector3df v, core::vector3df r )
            {
                //v -current node position 
                //r -target node position 

                core::vector3df angle; 
                float x, y, z; 
                x = r.X - v.X; 
                y = r.Y - v.Y; 
                z = r.Z - v.Z; 

                //angle in X-Z plane 
                angle.Y = atan2( x, z ); 
                angle.Y *= ( 180 / 3.14 ); //converting from rad to degrees 

                //just making sure angle is somewhere between 0-360 degrees 
                if ( angle.Y < 0 )
                {
                  angle.Y += 360;
                } 
                if ( angle.Y >= 360 )
                {
                  angle.Y -= 360;
                } 

                //angle in Y-Z plane while Z and X axes are already rotated around Y 
                float z1 = sqrt( x* x + z* z ); 

                angle.X = atan2( z1, y ); 
                angle.X *= ( 180 / 3.14 ); //converting from rad to degrees 
                angle.X -= 90; 

                //just making sure angle is somewhere between 0-360 degrees 
                if ( angle.X < 0 )
                {
                  angle.X += 360;
                } 
                if ( angle.X >= 360 )
                {
                  angle.X -= 360;
                } 

                return angle;
            } 

          public: 

            CBeamNode( scene::ISceneNode* parent, scene::ISceneManager* mgr, s32 id = -1, const char* szBeam = "" ) : scene::ISceneNode( parent, mgr, id )
            {
                // Setup the beam material 
                material.Wireframe = false; 
                material.Lighting = false; 
                material.MaterialType = EMT_TRANSPARENT_VERTEX_ALPHA  ; //video::EMT_TRANSPARENT_ADD_COLOR; 
                if ( szBeam != "" )
                {
                  material.Texture1 = mgr->getVideoDriver()->getTexture( szBeam );
                } 

                // Default to 32 units for the scale 
                flScale = 32.0; 

                // Default to white 
                beamColor.set( 255, 255, 255, 255 ); 
                beamColor2.set( 255, 255, 255, 255 ); 

                AutomaticCullingEnabled = false;
            } 

            virtual void OnPreRender()
            {
                if ( IsVisible )
                {
                  SceneManager->registerNodeForRendering( this ); 
                  //setPosition( vStart );
                }
            } 

            virtual void render()
            {
                SceneManager->getVideoDriver()->setTransform( irr::video::ETS_WORLD, AbsoluteTransformation ); 

                // Figure out quads based on start/end points. 
                core::matrix4 m; 
                m.setRotationDegrees( getTargetAngle( vStart, vEnd ) ); 
                core::vector3df vUp( 0, -1, 0 ); 
                core::vector3df vRight( -1, 0, 0 ); 
                m.transformVect( vRight ); 
                m.transformVect( vUp ); 


                // Draw the first cross 
                static IrrQuad beam; 
                beam.verts[0] = video::S3DVertex( vStart + vUp * flScale, core::vector3df( 1, 1, 0 ), beamColor, core::vector2df( 0, 1 ) ); 
                beam.verts[1] = video::S3DVertex( vStart + vUp * -flScale, core::vector3df( 1, 0, 0 ), beamColor, core::vector2df( 1, 1 ) ); 
                beam.verts[2] = video::S3DVertex( vEnd + vUp * -flScale, core::vector3df( 0, 1, 1 ), beamColor2, core::vector2df( 1, 0 ) ); 
                beam.verts[3] = video::S3DVertex( vEnd + vUp * flScale, core::vector3df( 0, 0, 1 ), beamColor2, core::vector2df( 0, 0 ) ); 
                DrawQuad( beam ); 

                // Draw the second cross. 
                beam.verts[0] = video::S3DVertex( vStart + vRight * flScale, core::vector3df( 1, 1, 0 ), beamColor, core::vector2df( 0, 1 ) ); 
                beam.verts[1] = video::S3DVertex( vStart + vRight * -flScale, core::vector3df( 1, 0, 0 ), beamColor, core::vector2df( 1, 1 ) ); 
                beam.verts[2] = video::S3DVertex( vEnd + vRight * -flScale, core::vector3df( 0, 1, 1 ), beamColor2, core::vector2df( 1, 0 ) ); 
                beam.verts[3] = video::S3DVertex( vEnd + vRight * flScale, core::vector3df( 0, 0, 1 ), beamColor2, core::vector2df( 0, 0 ) ); 
                DrawQuad( beam );
            } 

            virtual const core::aabbox3d<f32>& getBoundingBox() const
            {
                return Box;
            } 

            virtual s32 getMaterialCount()
            {
                return 1;
            } 

            virtual video::SMaterial& getMaterial( s32 i )
            {
                return material;
            } 

            void SetStartPoint( core::vector3df pos )
            {
                vStart = pos;
            } 

            void SetEndPoint( core::vector3df pos )
            {
                vEnd = pos;
            } 

            void SetBeamScale( float size )
            {
                flScale = size;
            } 

            void SetBeamColorStart( video::SColor color )
            {
                beamColor = color;
            } 

            void SetBeamColorEnd( video::SColor color )
            {
                beamColor2 = color;
            } 

            // Beam color 
            video::SColor beamColor, beamColor2;
        };
    } // end namespace scene
} // end namespace irr

#endif
