#include "CCal3dSceneNode.h"

#include <fstream>
#include <vector>

#include "../Game/SingletonIncludes.h"

CFileCache<CCal3DModelCache> CCal3DSceneNode::coreModels;

//! constructor 
//! @param cf, configuration file 
//! @param parent, parent scene node 
//! @param mgr, scene manager 
//! @param id, scenenode id 
CCal3DSceneNode::CCal3DSceneNode( const char* cf, irr::scene::ISceneNode* parent, irr::scene::ISceneManager* mgr, irr::s32 id ) : irr::scene::ISceneNode( parent, mgr, id ), configFile( cf )
{
    m_scale = 1.0f; 
    m_blendTime = 0.05f; 
    m_lodLevel = 0.01f; 
    lastTick = 0; 
    m_bPaused = false; 
    bInitialized = false; 
    m_calCoreModel = NULL; 
    m_calModel = NULL; 

    mediaPath = configFile.substr( 0, configFile.find_last_of( "/" ) ); 

    material.Wireframe = false; 
    material.Lighting = ( bool )IRR.useLighting; 
    // material.MaterialType=irr::video::EMT_TRANSPARENT_VERTEX_ALPHA; 
    //  material.MaterialType=irr::video::EMT_SOLID; 
    material.ZWriteEnable = true; 
    material.BackfaceCulling = false; 

    AutomaticCullingEnabled = false;

    if ( APP.DebugMode > 1 )
    {
      setDebugDataVisible( true );
    } 

    init();
} 

//! destructur 
CCal3DSceneNode::~CCal3DSceneNode()
{
    //m_calModel->destroy(); 
    //m_calCoreModel->destroy(); 
    delete m_calModel; 
    //delete m_calCoreModel; 
    m_calCoreModel = NULL; 
    m_calModel = NULL;
} 

void CCal3DSceneNode::UpdateAnimation( float timeStepSec )
{
    if ( m_calCoreModel == NULL || m_calModel == NULL )
    {
      return;
    } 
    // calculate the amount of elapsed seconds 
    //float elapsedSeconds; 
    //elapsedSeconds = (float)(getPreciseTime() - lastTick) / 1000.0f; 

    // update the model if not paused 
    if ( !m_bPaused )
    {
      // check if the time has come to blend to the next animation 
      //if(m_calCoreModel->getCoreAnimationCount() > 1) 
      //{ 
      //    m_leftAnimationTime -= elapsedSeconds; 
      //    if(m_leftAnimationTime <= m_blendTime) 
      //    { 
      //        // get the next animation 
      //        m_currentAnimationId = (m_currentAnimationId + 1) % m_calCoreModel->getCoreAnimationCount(); 

      //        // fade in the new animation 
      //        m_calModel->getMixer()->executeAction(m_currentAnimationId, m_leftAnimationTime, m_blendTime); 

      //        // adjust the animation time left until next animation flip 
      //        m_leftAnimationTime = m_calCoreModel->getCoreAnimation(m_currentAnimationId)->getDuration() - m_blendTime; 
      //    } 
      //} 

      m_calModel->update( timeStepSec ); 

      CalVector p[8]; 
      m_calModel->getBoundingBox( false ).computePoints( p ); 
      Box.reset( p[0].x, p[0].y, p[0].z ); 
      for ( int i = 0; i < 8; ++i )
      {
        Box.addInternalPoint( p[i].x, p[i].y, p[i].z );
      }
    } 

    // update bones from special animation (physic etc.) 

    // current tick will be last tick next round 
    //lastTick = getPreciseTime(); 
} 

//////////////////////////////////////////////// 
// irrlicht rendering stuff 
//////////////////////////////////////////////// 

void CCal3DSceneNode::OnPreRender()
{
    if ( bInitialized && IsVisible )
    {
      SceneManager->registerNodeForRendering( this );
    } 

    irr::scene::ISceneNode::OnPreRender();
} 

void CCal3DSceneNode::render()
{
    if ( bInitialized )
    {
      irr::video::IVideoDriver* driver = SceneManager->getVideoDriver(); 


      driver->setTransform( irr::video::ETS_WORLD, AbsoluteTransformation ); 


      irr::video::S3DVertex tmp; 
      irr::scene::SMeshBuffer mb; 

      unsigned char meshColor[4]; // r g b a 


      // get the renderer of the model 
      CalRenderer* pCalRenderer; 
      pCalRenderer = m_calModel->getRenderer(); 
      pCalRenderer->setNormalization( true ); 

      if ( this->DebugDataVisible )
      {
        irr::video::SMaterial mat; 
        mat.Wireframe = false; 
        mat.Lighting = false; 
        driver->setMaterial( mat ); 
        driver->draw3DBox( Box ); 

        CalSkeleton* pCalSkeleton = m_calModel->getSkeleton(); 
        pCalSkeleton->calculateBoundingBoxes(); 
        std::vector<CalBone*>& vectorCoreBone = pCalSkeleton->getVectorBone(); 
        irr::core::aabbox3df b; 
        CalVector p[8]; 
        Vector3 v[8]; 


        for ( size_t boneId = 0; boneId < vectorCoreBone.size(); ++boneId )
        {
          CalBone* bone = vectorCoreBone[boneId]; 
          CalBoundingBox& calBoundingBox = bone->getBoundingBox(); 
          calBoundingBox.computePoints( p ); 

          for ( int i = 0; i < 8; ++i )
          {
            v[i].set( p[i].x, p[i].y, p[i].z );
          } 

          driver->setMaterial( mat ); 
          // draw the box 
          driver->draw3DLine( v[0], v[1], irr::video::SColor( 255, 0, 0, 255 ) ); 
          driver->draw3DLine( v[0], v[2], irr::video::SColor( 255, 0, 0, 255 ) ); 
          driver->draw3DLine( v[1], v[3], irr::video::SColor( 255, 0, 0, 255 ) ); 
          driver->draw3DLine( v[2], v[3], irr::video::SColor( 255, 0, 0, 255 ) ); 
          driver->draw3DLine( v[4], v[5], irr::video::SColor( 255, 0, 0, 255 ) ); 
          driver->draw3DLine( v[4], v[6], irr::video::SColor( 255, 0, 0, 255 ) ); 
          driver->draw3DLine( v[5], v[7], irr::video::SColor( 255, 0, 0, 255 ) ); 
          driver->draw3DLine( v[6], v[7], irr::video::SColor( 255, 0, 0, 255 ) ); 
          driver->draw3DLine( v[0], v[4], irr::video::SColor( 255, 0, 0, 255 ) ); 
          driver->draw3DLine( v[1], v[5], irr::video::SColor( 255, 0, 0, 255 ) ); 
          driver->draw3DLine( v[2], v[6], irr::video::SColor( 255, 0, 0, 255 ) ); 
          driver->draw3DLine( v[3], v[7], irr::video::SColor( 255, 0, 0, 255 ) ); 
          //printf("F: %f\n", v[0].X);
        }
      } 


      // begin the rendering loop 
      if ( pCalRenderer->beginRendering() )
      {
        // get the number of meshes 
        int meshCount; 
        meshCount = pCalRenderer->getMeshCount(); 

        // render all meshes of the model 
        int meshId; 
        for ( meshId = 0; meshId < meshCount; meshId++ )
        {
          // get the number of submeshes 
          int submeshCount; 
          submeshCount = pCalRenderer->getSubmeshCount( meshId ); 

          // render all submeshes of the mesh 
          int submeshId; 
          for ( submeshId = 0; submeshId < submeshCount; submeshId++ )
          {
            // select mesh and submesh for further data access 
            if ( pCalRenderer->selectMeshSubmesh( meshId, submeshId ) )
            {
              // set the material ambient color 
              pCalRenderer->getAmbientColor( &meshColor[0] ); 
              material.AmbientColor.set( meshColor[3], meshColor[0], meshColor[1], meshColor[2] ); 

              // set the material diffuse color 
              pCalRenderer->getDiffuseColor( &meshColor[0] ); 
              material.DiffuseColor.set( meshColor[3], meshColor[0], meshColor[1], meshColor[2] ); 

              // set the material specular color 
              pCalRenderer->getSpecularColor( &meshColor[0] ); 
              material.SpecularColor.set( meshColor[3], meshColor[0], meshColor[1], meshColor[2] ); 

              // set the material shininess factor 
              material.Shininess = pCalRenderer->getShininess(); 

              // get the transformed vertices of the submesh 
              static float meshVertices[3000][3]; 
              int vertexCount = 0; 
              // TODO:
              //if (KERNEL.GetTicks() % 3 == 0) //- make lod dependent?
              vertexCount = pCalRenderer->getVertices( &meshVertices[0][0] ); 

              // get the transformed normals of the submesh 
              static float meshNormals[3000][3]; 
              int normalsCount = 0;
              //    if (KERNEL.GetTicks() % 3 == 0)
              normalsCount = pCalRenderer->getNormals( &meshNormals[0][0] ); 

              // get the texture coordinates of the submesh 
              static float meshTextureCoordinates[3000][2]; 
              int textureCoordinateCount = 0; 
              textureCoordinateCount = pCalRenderer->getTextureCoordinates( 0, &meshTextureCoordinates[0][0] ); 

              // get the faces of the submesh 
              static CalIndex meshFaces[5000][3]; 
              int faceCount = 0;
              //if (KERNEL.GetTicks() % 12 == 0)
              faceCount = pCalRenderer->getFaces( &meshFaces[0][0] ); 

              if ( ( pCalRenderer->getMapCount() > 0 ) && ( textureCoordinateCount > 0 ) )
              {
                irr::video::ITexture* t = static_cast<irr::video::ITexture*>( pCalRenderer->getMapUserData( 0 ) ); 
                material.Texture1 = t;
              } 

              static S3DVertex vs[5000];
              for ( int i = 0; i < vertexCount; i++ )
              {
                vs[i].Pos.set( meshVertices[i][0], meshVertices[i][1], meshVertices[i][2] ); 
                vs[i].Normal.set( meshNormals[i][0], meshNormals[i][1], meshNormals[i][2] ); 
                vs[i].TCoords.set( meshTextureCoordinates[i][0], meshTextureCoordinates[i][1] ); 
                vs[i].Color = irr::video::SColor( 255, 255, 255, 255 );
              } 

              static u16 is[5000];
              for ( int i = 0; i < faceCount; i += 1 )
              {
                is[i * 3 + 0] = meshFaces[i][0]; 
                is[i * 3 + 1] = meshFaces[i][1]; 
                is[i * 3 + 2] = meshFaces[i][2];
              } 

              //mb.Vertices.clear(); 
              //mb.Vertices.reallocate(vertexCount);
              //for(int i=0;i<vertexCount;i++) 
              //{ 
              //    tmp.Pos.set(meshVertices[i][0],meshVertices[i][1],meshVertices[i][2]); 
              //    tmp.Normal.set(meshNormals[i][0],meshNormals[i][1],meshNormals[i][2]); 
              //    tmp.TCoords.set(meshTextureCoordinates[i][0],meshTextureCoordinates[i][1]); 
              //    tmp.Color=irr::video::SColor(255,255,255,255); 
              //    mb.Vertices.push_back(tmp); 
              //} 

              //mb.Indices.clear(); 
              //mb.Indices.reallocate(faceCount);
              //for(int i=0; i<faceCount; ++i)
              //{ 
              //    mb.Indices.push_back(meshFaces[i][0]); 
              //    mb.Indices.push_back(meshFaces[i][1]); 
              //    mb.Indices.push_back(meshFaces[i][2]); 
              //} 

              // draw 

              driver->setMaterial( material ); 
              //driver->drawIndexedTriangleList(mb.Vertices.const_pointer(),mb.Vertices.size(),mb.Indices.const_pointer(),faceCount); 
              driver->drawIndexedTriangleList( vs, vertexCount, is, faceCount ); 
              //driver->drawMeshBuffer(&mb); 

              //CONSOLE.addx("#Verts %d #Norm %d #Tex %d #Faces %d #Map %d", 
              //       vertexCount,normalsCount,textureCoordinateCount,faceCount, pCalRenderer->getMapCount()); 
            }
          }
        } 

        // end the rendering 
        pCalRenderer->endRendering();
      }
    }
} 

void CCal3DSceneNode::OnPostRender( irr::u32 timeMs )
{
    irr::scene::ISceneNode::OnPostRender( timeMs );
} 

const irr::core::aabbox3df& CCal3DSceneNode::getBoundingBox() const
{
    return Box;
} 

//////////////////////////////////////////////// 
// call 3d stuff 
//////////////////////////////////////////////// 


// load all textures 
bool CCal3DSceneNode::init()
{
    bool ok = true; 

    if ( coreModels.isFileLoaded( configFile.c_str() ) )
    {
      String ls = configFile.c_str();
      ls.make_lower();
      CCal3DModelCache* cache = 0;
      cache = coreModels.findFile( ls.c_str() );
      if ( cache )
      {
        ///load from cache
        m_calCoreModel = cache->m_calCoreModel;
        m_scale = cache->m_scale;
        for ( int i = 0; i < 255; i++ )
        {
          m_animationId[i] = cache->m_animationId[i];
        }
        //CONSOLE.add( "Model loaded already", COLOR_WARNING );
      }
      else
      {
        bInitialized = false; 
        return false;
      }
    }
    else
    {
      //CONSOLE.add( "Model loading", COLOR_WARNING );
      CCal3DModelCache* cache = PrecacheModel( configFile.c_str() );
      ///load from cache
      if ( cache )
      {
        m_calCoreModel = cache->m_calCoreModel;
        m_scale = cache->m_scale;
        for ( int i = 0; i < 255; i++ )
        {
          m_animationId[i] = cache->m_animationId[i];
        }
      }
      else
      {
        bInitialized = false; 
        return false;
      }
    }


    // create the model instance from the loaded core model 
    m_calModel = new CalModel( m_calCoreModel ); 
    //if(!m_calModel->create(&m_calCoreModel)) 
    //{ 
    //"CalError : %s",CalError::getLastErrorDescription().c_str() 
    //   return false; 
    //} 

    // attach all meshes to the model 
    int meshId; 
    for ( meshId = 0; meshId < m_calCoreModel->getCoreMeshCount(); meshId++ )
    {
      m_calModel->attachMesh( meshId );
    } 
    // set the material set of the whole model 
    m_calModel->setMaterialSet( 0 ); 

    // set initial animation state 
    if ( m_calCoreModel->getCoreAnimationCount() > 0 )
    {
      m_currentAnimationId = 0; 
      m_leftAnimationTime = m_calCoreModel->getCoreAnimation( m_currentAnimationId )->getDuration() - m_blendTime; 
      if ( m_calCoreModel->getCoreAnimationCount() > 1 )
      {
        m_calModel->getMixer()->executeAction( m_currentAnimationId, 0.0f, m_blendTime );
      }
      else
      {
        m_calModel->getMixer()->blendCycle( m_currentAnimationId, 1.0f, 0.0f );
      }
    }
    else
    {
      m_currentAnimationId = -1; 
      m_leftAnimationTime = -1.0f;
    } 

    m_calModel->setLodLevel( m_lodLevel ); 

    m_calModel->update( 0 ); 

    // calculate the irrlicht bounding box 
    CalVector p[8]; 
    m_calModel->getBoundingBox( true ).computePoints( p ); 
    Box.reset( p[0].x, p[0].y, p[0].z ); 
    for ( int i = 0; i < 8; ++i )
    {
      Box.addInternalPoint( p[i].x, p[i].y, p[i].z );
    } 

    //"Init done." 
    bInitialized = ok; 

    return ok;
}

//////////////////////////////////////////////// 
// more
//////////////////////////////////////////////// 

void CCal3DSceneNode::setLodLevel( float lodLevel )
{
    m_lodLevel = lodLevel;

    // set the new lod level in the cal model renderer
    m_calModel->setLodLevel( m_lodLevel );
}

void CCal3DSceneNode::executeAction( int animNum, float delayOut, float weightTarget, bool clearPrevious )
{
    //bool  executeAction (int id, float delayIn, float delayOut, float weightTarget=1.0f)
    if ( m_currentAnimationId == m_animationId[animNum] )
    {
      return;
    }

    if ( clearPrevious )
    {
      m_calModel->getMixer()->clearCycle( m_currentAnimationId, delayOut );
    }
    m_calModel->getMixer()->blendCycle( m_animationId[animNum], delayOut, weightTarget );
    m_currentAnimationId = m_animationId[animNum];

    //  m_currentAnimationId = m_animationId[animNum];
}

float CCal3DSceneNode::getLodLevel()
{
    return m_lodLevel;
}

void CCal3DSceneNode::clearAnim( int animNum, float delayOut )
{
    m_calModel->getMixer()->clearCycle( m_animationId[animNum], delayOut );
}

CCal3DModelCache* CCal3DSceneNode::PrecacheModel( const char* configFilename )
{
    CCal3DModelCache* cache = new CCal3DModelCache();
    std::string cf = configFilename;
    std::string mediaPath = cf.substr( 0, cf.find_last_of( "/" ) ); 

    if ( !cache->parseModelConfiguration( APP.useFile( configFilename ).c_str(), mediaPath ) )
    {
      //bInitialized=false; 
      return 0;
    }
    else
    {
      coreModels.addFile( configFilename, cache );
    }
    return cache;
}

//-----------------------------------------------------------------------------------

CCal3DModelCache::~CCal3DModelCache()
{
    delete m_calCoreModel;
}

//! load and parse the configuration file 
//! @param cf, configuration file name 
//! @return true on success, false otherwise 
bool CCal3DModelCache::parseModelConfiguration( const std::string& cf, const std::string& mediaPath )
{
    int animationCount = 0;

    std::string fn; 
    // open the model configuration file 
    std::ifstream file; 
    file.open( cf.c_str(), std::ios::in | std::ios::binary ); 
    if ( !file )
    {
      CONSOLE.addx( COLOR_ERROR, "Failed to open model configuration file '%s'.", cf.c_str() );
      return false;
    } 

    //CONSOLE.addx(  "Openiong model configuration file '%s'." ,cf.c_str() );

    // create a core model instance 
    m_calCoreModel = new CalCoreModel( "dummy" ); 
    //if(!m_calCoreModel->create("dummy")) 
    //{ 
    //   // "CalError : %s",CalError::getLastErrorDescription().c_str() 
    //   return false; 
    //} 

    // parse all lines from the model configuration file 
    int line; 
    for ( line = 1; ; line++ )
    {
      // read the next model configuration line 
      std::string strBuffer; 
      std::getline( file, strBuffer ); 

      // stop if we reached the end of file 
      if ( file.eof() )
      {
        break;
      } 

      // check if an error happend while reading from the file 
      if ( !file )
      {
        CONSOLE.addx( COLOR_ERROR, "Error while reading from the model configuration file '%s'.", cf.c_str() );
        return false;
      } 

      // find the first non-whitespace character 
      std::string::size_type pos; 
      pos = strBuffer.find_first_not_of( " \t" ); 

      // check for empty lines 
      if ( ( pos == std::string::npos ) || ( strBuffer[pos] == '\n' ) || ( strBuffer[pos] == '\r' ) || ( strBuffer[pos] == 0 ) )
      {
        continue;
      } 

      // check for comment lines 
      if ( strBuffer[pos] == '#' )
      {
        continue;
      } 

      // get the key 
      std::string strKey; 
      strKey = strBuffer.substr( pos, strBuffer.find_first_of( " =\t\n\r", pos ) - pos ); 
      pos += strKey.size(); 

      // get the '=' character 
      pos = strBuffer.find_first_not_of( " \t", pos ); 
      if ( ( pos == std::string::npos ) || ( strBuffer[pos] != '=' ) )
      {
        CONSOLE.addx( COLOR_ERROR, "%s  (%d): Invalid syntax.", cf.c_str(), line );
        return false;
      } 

      // find the first non-whitespace character after the '=' character 
      pos = strBuffer.find_first_not_of( " \t", pos + 1 ); 

      // get the data 
      std::string strData; 
      strData = strBuffer.substr( pos, strBuffer.find_first_of( "\n\r", pos ) - pos ); 

      //CONSOLE.addx( "CAL3D: %s", strKey.c_str() );

      // handle the model creation 
      if ( strKey == "scale" )
      {
        // set rendering scale factor 
        m_scale = float( atof( strData.c_str() ) );
      }
      else if ( strKey == "skeleton" )
      {
        // load core skeleton 
        fn.clear(); fn = mediaPath + "/" + strData; 

        fn = APP.useFile( fn.c_str() ).c_str();
        if ( APP.DebugMode )
        {
          CONSOLE.addx( "Loading skeleton '%s'...", fn.c_str() );
        }

        //"Loading skeleton '%s'...",fn.c_str() 
        if ( !m_calCoreModel->loadCoreSkeleton( fn ) )
        {
          CONSOLE.addx( COLOR_ERROR, "CalError (skeleton) : %s", CalError::getLastErrorDescription().c_str() );
          return false;
        }
      }
      else if ( strKey == "animation" )
      {
        //fn.clear(); fn=mediaPath+"/"+strData; 
        //// load core animation 
        ////"Loading animation '%s'...",fn.c_str() 
        //if(m_calCoreModel->loadCoreAnimation(fn) == -1) 
        //{ 
        //    //"CalError : %s",CalError::getLastErrorDescription().c_str() 
        //    return false; 
        //} 

        fn.clear(); fn = mediaPath + "/" + strData; 
        //CONSOLE.addx( "Loading animation '%s'...",fn.c_str() );
        fn = APP.useFile( fn.c_str() ).c_str();
        if ( APP.DebugMode )
        {
          CONSOLE.addx( "Loading animation '%s'...", fn.c_str() );
        }
        // load core mesh 
        //if (APP.DebugMode)
        //  CONSOLE.addx( "Loading mesh '%s'...",fn.c_str() );
        if ( animationCount > 255 )
        {
          return false;
        }
        m_animationId[animationCount] = m_calCoreModel->loadCoreAnimation( fn );
        if ( m_animationId[animationCount] == -1 )
        {
          CONSOLE.addx( COLOR_ERROR, "CalError (animation): %s", CalError::getLastErrorDescription().c_str() );
          return false;
        } 
        animationCount++;
      }
      else if ( strKey == "mesh" )
      {
        fn.clear(); fn = mediaPath + "/" + strData;

        fn = APP.useFile( fn.c_str() ).c_str();
        if ( APP.DebugMode )
        {
          CONSOLE.addx( "Loading mesh '%s'...", fn.c_str() );
        }

        // load core mesh
        if ( m_calCoreModel->loadCoreMesh( fn ) == -1 )
        {
          return false;
        }
      }
      else if ( strKey == "material" )
      {
        fn.clear(); fn = mediaPath + "/" + strData; 
        fn = APP.useFile( fn.c_str() ).c_str();
        // load core material 
        if ( APP.DebugMode )
        {
          CONSOLE.addx( "Loading material '%s'...", fn.c_str() );
        }
        if ( m_calCoreModel->loadCoreMaterial( fn ) == -1 )
        {
          CONSOLE.addx( COLOR_ERROR, "CalError (material): %s", CalError::getLastErrorDescription().c_str() );
          return false;
        }
      }
      else
      {
        // everything else triggers an error message, but is ignored 
        CONSOLE.addx( COLOR_ERROR, "%s  (%d): Invalid syntax.", cf.c_str(), line );
      }
    } 

    // explicitely close the file 
    file.close(); 

    ////////////////////////////////

    m_calCoreModel->scale( m_scale ); 

    // make one material thread for each material 
    // NOTE: this is not the right way to do it, but this viewer can't do the right 
    // mapping without further information on the model etc., so this is the only 
    // thing we can do here. 
    for ( int materialId = 0; materialId < m_calCoreModel->getCoreMaterialCount(); materialId++ )
    {
      // create the a material thread 
      m_calCoreModel->createCoreMaterialThread( materialId ); 

      // initialize the material thread 
      m_calCoreModel->setCoreMaterialId( materialId, 0, materialId );
    } 

    m_calCoreModel->getCoreSkeleton()->calculateBoundingBoxes( m_calCoreModel ); 

    // load all textures and store the texture id in the corresponding map in the material 
    for ( int materialId = 0; materialId < m_calCoreModel->getCoreMaterialCount(); materialId++ )
    {
      // get the core material 
      CalCoreMaterial* pCoreMaterial; 
      pCoreMaterial = m_calCoreModel->getCoreMaterial( materialId ); 

      // loop through all maps of the core material 
      int mapId; 
      for ( mapId = 0; mapId < pCoreMaterial->getMapCount(); mapId++ )
      {
        // get the filename of the texture 
        std::string fn = mediaPath + "/" + pCoreMaterial->getMapFilename( mapId ); 

        // load the texture from the file 
        irr::video::ITexture* texture = IRR.smgr->getVideoDriver()->getTexture( APP.useFile( fn.c_str() ).c_str() ); 
        if ( texture )
        {
          // store the texture id in the user data of the map 
          pCoreMaterial->setMapUserData( mapId, ( Cal::UserData )texture );
        }
        else
        {
          //ok=false; 
          //"Loading texture '%s' failed.",fn.c_str() 
        }
      }
    } 

    return true;
} 
