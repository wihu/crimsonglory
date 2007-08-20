#ifndef __CANIMSPRITE_H_INCLUDED__
#define __CANIMSPRITE_H_INCLUDED__

#include "../Engine/engine.h"
#include "../IrrConsole/console.h"

// source: http://www.irrforge.org/index.php/Playing_2D_Animation_with_Irrlicht
// modified by MM

namespace irr
{
    namespace scene
    {
        class CAnimSpriteSceneNode : public ISceneNode
        {
          private:
            core::aabbox3d<f32> Box; 
            video::S3DVertex Vertices[4];
            u16 Indices[12]; 
            video::SMaterial Material; 
            video::ITexture* Texture;
            f32 fWidth, fHeight;
            f32 fFrameWidth, fFrameHeight;
            s32 crntFrm, TotalFrm;
            s32 stepww, stephh;
            BOOL forward;
            DWORD time;
            DWORD oldtick;
            s32 startFrame, endFrame; 
            f32 xCoord, yCoord;
            matrix4 Ortho;
            core::dimension2d<f32> Size;
            bool billboard;
            vector3df vBPos;
          public: 
            CAnimSpriteSceneNode( ISceneNode* parent, ISceneManager* mgr, s32 id ) : ISceneNode( parent, mgr, id )
            {
                Material.Wireframe = false;
                Material.Lighting = true;

                u16 ind[] =
                {
                  0, 1, 3, 3, 1, 2, 1, 0, 2, 2, 0, 3
                };
                memcpy( Indices, ind, sizeof( u16 ) * 12 );

                IVideoDriver* driver = SceneManager->getVideoDriver();   
                dimension2d<s32> Screensize = driver->getScreenSize(); 

                Ortho( 0,0 ) = ( double )2/ ( double )Screensize.Width;
                Ortho( 1, 0 ) = 0;
                Ortho( 2, 0 ) = 0;
                Ortho( 3, 0 ) = 0;

                Ortho( 0, 1 ) = 0;
                Ortho( 1,1 ) = ( double )2/ ( double )Screensize.Height;
                Ortho( 2, 1 ) = 0;
                Ortho( 3, 1 ) = 0;

                Ortho( 0, 2 ) = 0;
                Ortho( 1, 2 ) = 0;
                Ortho( 2, 2 ) = 1;
                Ortho( 3, 2 ) = 0;

                Ortho( 0, 3 ) = 0;
                Ortho( 1, 3 ) = 0;
                Ortho( 2, 3 ) = 0;
                Ortho( 3, 3 ) = 1;
            }     

            // APP.useFile() used when using this function 
            virtual void Load( const char* filename, s32 frmWidth, s32 frmHeight, bool useClrKey, bool isBillboard )
            {
                IVideoDriver* driver = SceneManager->getVideoDriver();   
                dimension2d<s32> Screensize = driver->getScreenSize(); 
                float x = ( float )frmWidth / 2.0f;
                float y = ( float )frmHeight / 2.0f;
                Vertices[0] = S3DVertex( -x, -y, 0, 0, 0, 0, SColor( 255, 255, 255, 255 ), 0, 1 );
                Vertices[1] = S3DVertex( x, -y, 0, 0, 0, 0, SColor( 255, 255, 255, 255 ), 1, 1 ); 
                Vertices[2] = S3DVertex( x, y, 0, 0, 0, 0, SColor( 255, 255, 255, 255 ), 1, 0 );
                Vertices[3] = S3DVertex( -x, y, 0, 0, 0, 0, SColor( 255, 255, 255, 255 ), 0, 0 );

                Box.reset( Vertices[0].Pos );
                for ( s32 i = 1; i < 4; ++i )
                {
                  Box.addInternalPoint( Vertices[i].Pos );
                }

                Texture = driver->getTexture( filename );
                if ( useClrKey == true )
                {
                  driver->makeColorKeyTexture( Texture, position2d<s32>( 0, 0 ) );
                }
                Material.MaterialType = EMT_TRANSPARENT_ALPHA_CHANNEL;  
                Material.Texture1 = Texture;

                dimension2d<s32> size = Texture->getOriginalSize();
                fWidth = ( float )frmWidth / ( float )size.Width;
                fHeight = ( float )frmHeight / ( float )size.Height;
                crntFrm = 0;
                stepww = size.Width / frmWidth;
                stephh = size.Height / frmHeight;
                TotalFrm = ( s32 )( stepww * stephh );
                forward = TRUE;
                startFrame = 0; 
                endFrame = TotalFrm;
                xCoord = yCoord = 0.0;

                Vertices[0].TCoords.X = 0; 
                Vertices[0].TCoords.Y = fHeight;
                Vertices[1].TCoords.X = fWidth; 
                Vertices[1].TCoords.Y = fHeight;
                Vertices[2].TCoords.X = fWidth; 
                Vertices[2].TCoords.Y = 0;
                Vertices[3].TCoords.X = 0; 
                Vertices[3].TCoords.Y = 0;

                billboard = isBillboard;
                fFrameWidth = frmWidth;
                fFrameHeight = frmHeight;
            }

            virtual void Load( const char* filename, s32 Ax, s32 Ay, s32 Aw, s32 Ah, s32 frmWidth, s32 frmHeight, bool useClrKey = false, bool isBillboard = false )
            {
                IVideoDriver* driver = SceneManager->getVideoDriver();   
                dimension2d<s32> Screensize = driver->getScreenSize(); 
                float x = ( float )frmWidth / 2.0f;
                float y = ( float )frmHeight / 2.0f;
                Vertices[0] = S3DVertex( -x, -y, 0, 0, 0, 0, SColor( 255, 255, 255, 255 ), 0, 1 );
                Vertices[1] = S3DVertex( x, -y, 0, 0, 0, 0, SColor( 255, 255, 255, 255 ), 1, 1 ); 
                Vertices[2] = S3DVertex( x, y, 0, 0, 0, 0, SColor( 255, 255, 255, 255 ), 1, 0 );
                Vertices[3] = S3DVertex( -x, y, 0, 0, 0, 0, SColor( 255, 255, 255, 255 ), 0, 0 );

                Box.reset( Vertices[0].Pos );
                for ( s32 i = 1; i < 4; ++i )
                {
                  Box.addInternalPoint( Vertices[i].Pos );
                }

                Texture = driver->getTexture( filename );

                if ( !Texture )
                {
                  return;
                }

                if ( useClrKey == true )
                {
                  driver->makeColorKeyTexture( Texture, position2d<s32>( 0, 0 ) );
                }
                Material.MaterialType = EMT_TRANSPARENT_ALPHA_CHANNEL;  
                Material.Texture1 = Texture;

                dimension2d<s32> size = Texture->getOriginalSize();
                fWidth = ( float )frmWidth / ( float )size.Width;
                fHeight = ( float )frmHeight / ( float )size.Height;
                crntFrm = 0;
                stepww = Aw / frmWidth;
                stephh = Ah / frmHeight;
                TotalFrm = stepww * stephh;
                forward = TRUE;
                startFrame = 0; 
                endFrame = TotalFrm;
                xCoord = ( float )Ax / ( float )size.Width;
                yCoord = ( float )Ay / ( float )size.Height;

                Vertices[0].TCoords.X = xCoord + 0; 
                Vertices[0].TCoords.Y = yCoord + fHeight;
                Vertices[1].TCoords.X = xCoord + fWidth; 
                Vertices[1].TCoords.Y = yCoord + fHeight;
                Vertices[2].TCoords.X = xCoord + fWidth; 
                Vertices[2].TCoords.Y = yCoord + 0;
                Vertices[3].TCoords.X = xCoord + 0; 
                Vertices[3].TCoords.Y = yCoord + 0; 

                billboard = isBillboard;
                fFrameWidth = frmWidth;
                fFrameHeight = frmHeight;
            }

            virtual void Load( const char* filename, bool useClrKey, bool isBillboard )
            {
                IVideoDriver* driver = SceneManager->getVideoDriver();   
                dimension2d<s32> Screensize = driver->getScreenSize(); 

                Box.reset( Vertices[0].Pos );
                for ( s32 i = 1; i < 4; ++i )
                {
                  Box.addInternalPoint( Vertices[i].Pos );
                }

                Texture = driver->getTexture( filename );
                if ( useClrKey == true )
                {
                  driver->makeColorKeyTexture( Texture, position2d<s32>( 0, 0 ) );
                }
                Material.MaterialType = EMT_TRANSPARENT_ALPHA_CHANNEL;  
                Material.Texture1 = Texture;

                dimension2d<s32> size = Texture->getOriginalSize();

                float x = ( float )size.Width / 2.0f;
                float y = ( float )size.Height / 2.0f;
                Vertices[0] = S3DVertex( -x, -y, 0, 0, 0, 0, SColor( 255, 255, 255, 255 ), 0, 1 );
                Vertices[1] = S3DVertex( x, -y, 0, 0, 0, 0, SColor( 255, 255, 255, 255 ), 1, 1 ); 
                Vertices[2] = S3DVertex( x, y, 0, 0, 0, 0, SColor( 255, 255, 255, 255 ), 1, 0 );
                Vertices[3] = S3DVertex( -x, y, 0, 0, 0, 0, SColor( 255, 255, 255, 255 ), 0, 0 );

                fWidth = 1.0f;
                fHeight = 1.0f;
                crntFrm = 0;
                stepww = 1.0f;
                stephh = 1.0f;
                TotalFrm = ( s32 )( stepww * stephh );
                forward = TRUE;
                startFrame = 0; 
                endFrame = TotalFrm;
                xCoord = yCoord = 0.0;

                Vertices[0].TCoords.X = 0; 
                Vertices[0].TCoords.Y = fHeight;
                Vertices[1].TCoords.X = fWidth; 
                Vertices[1].TCoords.Y = fHeight;
                Vertices[2].TCoords.X = fWidth; 
                Vertices[2].TCoords.Y = 0;
                Vertices[3].TCoords.X = 0; 
                Vertices[3].TCoords.Y = 0;

                billboard = isBillboard;
                fFrameWidth = size.Width;
                fFrameHeight = size.Height;
            }

            virtual void PlayForward()
            {
                forward = TRUE;
            }
            virtual void PlayBackward()
            {
                forward = FALSE;
            } 
            virtual void setSpeed( int spd )
            {
                time = spd;
            }

            virtual void OnPreRender()
            {
                if ( IsVisible )
                {
                  SceneManager->registerNodeForRendering( this );
                  ISceneNode::OnPreRender();
                }
            }

            virtual void setFrame( int n )
            {
                crntFrm = n;
                if ( forward )
                {
                  crntFrm--;
                }
                else
                {
                  crntFrm++;
                } 

                float x = ( n % stepww ) * fWidth;
                float y = ( n / stepww ) * fHeight; 
                Vertices[0].TCoords.X = xCoord + x; 
                Vertices[0].TCoords.Y = yCoord + y + fHeight;
                Vertices[1].TCoords.X = xCoord + x + fWidth; 
                Vertices[1].TCoords.Y = yCoord + y + fHeight;
                Vertices[2].TCoords.X = xCoord + x + fWidth; 
                Vertices[2].TCoords.Y = yCoord + y;
                Vertices[3].TCoords.X = xCoord + x; 
                Vertices[3].TCoords.Y = yCoord + y;
            }

            virtual void Update()
            {
                if ( KERNEL.GetTicks() - oldtick > time )
                {
                  oldtick = KERNEL.GetTicks();
                  if ( forward )
                  {
                    crntFrm++; 
                    if ( crntFrm > endFrame - 1 )
                    {
                      crntFrm = startFrame;
                    }
                  }
                  else
                  {
                    crntFrm--;
                    if ( crntFrm < startFrame )
                    {
                      crntFrm = endFrame - 1;
                    }
                  } 

                  float x = ( crntFrm % stepww ) * fWidth;
                  float y = ( crntFrm / stepww ) * fHeight; 
                  Vertices[0].TCoords.X = xCoord + x; 
                  Vertices[0].TCoords.Y = yCoord + y + fHeight;
                  Vertices[1].TCoords.X = xCoord + x + fWidth; 
                  Vertices[1].TCoords.Y = yCoord + y + fHeight;
                  Vertices[2].TCoords.X = xCoord + x + fWidth; 
                  Vertices[2].TCoords.Y = yCoord + y;
                  Vertices[3].TCoords.X = xCoord + x; 
                  Vertices[3].TCoords.Y = yCoord + y;
                }
            }     

            virtual void setStartEndFrame( s32 st, s32 ed )
            {
                startFrame = st; 
                endFrame = ed;
            }

            virtual s32 GetMaxFrames()
            {
                return TotalFrm;
            }

            virtual void render();

            virtual const aabbox3d<f32>& getBoundingBox() const
            {
                return Box;
            }

            virtual s32 getMaterialCount()
            {
                return 1;
            }

            virtual SMaterial& getMaterial( s32 i )
            {
                return Material;
            } 

            virtual f32 getFrameWidth()
            {
                return fFrameWidth;
            }

            virtual f32 getFrameHeight()
            {
                return fFrameHeight;
            }

            virtual void setVertice( s32 num, vector3df vPos )
            {
                Vertices[num].Pos = vPos;

                // recalc billboard size and position
                if ( billboard )
                                    //if ( num == 3)
                {
                  vector3df vHor = ( Vertices[1].Pos - Vertices[0].Pos ) / fFrameWidth;
                  vector3df vVert = ( Vertices[1].Pos - Vertices[2].Pos ) / fFrameHeight;
                  Size.Width = vHor.getLength();
                  Size.Height = vVert.getLength();
                  vBPos = ( Vertices[0].Pos + Vertices[1].Pos + Vertices[2].Pos + Vertices[3].Pos ) / 4;
                  setPosition( vBPos );
                  //    CONSOLE_VECTOR( vBPos );
                  //vBPos = vector3df( 0, -14, 0);
                }
            }

            virtual void setSize( const core::dimension2d<f32>& size );
        };
    } // end namespace scene
} // end namespace irr

#endif

