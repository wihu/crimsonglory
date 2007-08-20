#include "map.h"
#include "../Game/SingletonIncludes.h"

#include "world.h"
#include "../Newton/newton_physics.h"
#include "boolblock.h"

#include "../Irrlicht/CSkyBackSceneNode.h"
#include "../Irrlicht/CLensFlaresSceneNode.h"
#include "../Irrlicht/CDMFLoader.h"
#include "../Irrlicht/CReflectedWater.h"
#include "../Irrlicht/CAnimSprite.h"
#include "../Irrlicht/CBackSpriteSceneNode.h"
#include "../Irrlicht/ATMOsphere.h"

#define WORLD_BOUND 17000.0f
#define WORLD_WIDTH 100.0f
#define PLANE_SIZE 5.0f
#define OPTCELL_WIDTH 300.0f
#define OPTCELL_HEIGHT 300.0f

#define WATER_FACTOR 200.0f

#define OPTIMIZE_COLLISION 1
#define OPTIMIZE2D_COLLISION
#define OPTIMIZE2D_COLLISION_DIF 2.0f

////////////////////////////////////////////
// CMap_Cloud 
////////////////////////////////////////////
CMap_Cloud::CMap_Cloud( aabbox3df box, int type )
{
    cloudbox = box;
    this->type = type;

    vector3df edges[8];
    box.getEdges( edges );

    vSize.Z = ( edges[3] - edges[1] ).getLength();
    vSize.Y = ( edges[0] - edges[1] ).getLength();
    vSize.X = ( edges[5] - edges[1] ).getLength();
    vSize.X *= 1.33f;
    vSize.Y *= 1.33f;
    vSize.Z *= 1.33f;

    vCenter = box.getCenter();

    // fill in the cloud with billboards
    vector3df vR, vBack;

    IBillboardSceneNode* billnode = IRR.smgr->addBillboardSceneNode( 0, dimension2d<f32>( vSize.X / 2.0f, vSize.Y / 2.0f ), vCenter );
    billnode->setMaterialFlag( video::EMF_LIGHTING, ( bool )IRR.useLighting );
    billnode->setMaterialType( video::EMT_TRANSPARENT_ADD_COLOR );
    //
    //  CONSOLE.addx("CLOUD TYPE: %i", type);
    switch ( type )
    {
      case CLOUD_1:
        billnode->setMaterialTexture( 0, IRR.video->getTexture( APP.useFile( "Textures/Clouds/cloud5.jpg" ).c_str() ) );
        break;
      case CLOUD_2:
        billnode->setMaterialTexture( 0, IRR.video->getTexture( APP.useFile( "Textures/Clouds/cloud6.jpg" ).c_str() ) );
        break;
      case CLOUD_3:
        billnode->setMaterialTexture( 0, IRR.video->getTexture( APP.useFile( "Textures/Clouds/cloud7.jpg" ).c_str() ) );
        break;
      case CLOUD_4:
        billnode->setMaterialTexture( 0, IRR.video->getTexture( APP.useFile( "Textures/Clouds/cloud8.jpg" ).c_str() ) );
        break;
      default:
        billnode->setMaterialTexture( 0, IRR.video->getTexture( APP.useFile( "Textures/Clouds/cloud6.jpg" ).c_str() ) );
    }
    nodes.push_back( billnode );

    //char buf[256];
    //vR = vector3df( 0.0f, 0.0f, -vSize.Z / 2.5f );
    //vBack = vCenter + vR;
    //vR = vector3df( 0.0f, 0.0f, vSize.Z / 4 );
    //for (int i = 0; i < 4; i++)
    //{
    //  billnode = IRR.smgr->addBillboardSceneNode( 0, dimension2d<f32>(vSize.X / 4.0f, vSize.Y / 4.0f), vBack );
    //  vBack += vR;
    //  billnode->setMaterialFlag(video::EMF_LIGHTING, false);
    //  billnode->setMaterialType(video::EMT_TRANSPARENT_ADD_COLOR);
    //  sprintf( buf, "Textures/Clouds/cloud%i.jpg", ((i+type) % 8) + 1 );
    //  billnode->setMaterialTexture(0, IRR.video->getTexture( APP.useFile( buf ).c_str() ) );
    //  nodes.push_back( billnode );
    //}

    //vR = vector3df( -vSize.X /4.0f, 0.0f, -vSize.Z / 3.0f );
    //vBack = vCenter + vR;
    //vR = vector3df( 0.0f, 0.0f, vSize.Z / 3 );
    //for (int i = 0; i < 4; i++)
    //{
    //  billnode = IRR.smgr->addBillboardSceneNode( 0, dimension2d<f32>(vSize.X / 4.0f, vSize.Y / 4.0f), vBack );
    //  vBack += vR;
    //  billnode->setMaterialFlag(video::EMF_LIGHTING, false);
    //  billnode->setMaterialType(video::EMT_TRANSPARENT_ADD_COLOR);
    //  sprintf( buf, "Textures/Clouds/cloud%i.jpg", ((i+2*type) % 8) + 1 );
    //  billnode->setMaterialTexture(0, IRR.video->getTexture( APP.useFile( buf ).c_str() ) );
    //  nodes.push_back( billnode );
    //}

    char buf[256];
    vR = vector3df( vSize.X / 5, 0.0f, -vSize.Z / 10 );
    for ( int i = 0; i < 4; i++ )
    {
      vR.rotateXYBy( 100, vector3df( 0, 0, 0 ) );
      vR.rotateYZBy( 35, vector3df( 0, 0, 0 ) );
      billnode = IRR.smgr->addBillboardSceneNode( 0, dimension2d<f32>( vSize.X / 4.0f, vSize.Y / 4.0f ), vCenter + vR );
      billnode->setMaterialFlag( video::EMF_LIGHTING, ( bool )IRR.useLighting );
      billnode->setMaterialType( video::EMT_TRANSPARENT_ADD_COLOR );
      sprintf( buf, "Textures/Clouds/cloud%i.jpg", ( ( i + type ) % 8 ) + 1 );
      billnode->setMaterialTexture( 0, IRR.video->getTexture( APP.useFile( buf ).c_str() ) );
      nodes.push_back( billnode );
    }

    vR = vector3df( vSize.X / 4.5f, 0.0f, -vSize.Z / 9 );
    vR.rotateXYBy( 13, vector3df( 0, 0, 0 ) );
    for ( int i = 0; i < 8; i++ )
    {
      vR.rotateXYBy( 60, vector3df( 0, 0, 0 ) );
      vR.rotateYZBy( 44, vector3df( 0, 0, 0 ) );
      billnode = IRR.smgr->addBillboardSceneNode( 0, dimension2d<f32>( vSize.X / 8.0f, vSize.Y / 8.0f ), vCenter + vR );
      billnode->setMaterialFlag( video::EMF_LIGHTING, ( bool )IRR.useLighting );
      billnode->setMaterialType( video::EMT_TRANSPARENT_ADD_COLOR );
      sprintf( buf, "Textures/Clouds/cloud%i.jpg", ( ( i + 2 * type ) % 8 ) + 1 );
      billnode->setMaterialTexture( 0, IRR.video->getTexture( APP.useFile( buf ).c_str() ) );
      nodes.push_back( billnode );
    }

    vR = vector3df( vSize.X / 9, 0.0f, vSize.Z / 5 );
    vR.rotateXYBy( 17, vector3df( 0, 0, 0 ) );
    for ( int i = 0; i < 4; i++ )
    {
      vR.rotateXYBy( 110, vector3df( 0, 0, 0 ) );
      vR.rotateYZBy( 43, vector3df( 0, 0, 0 ) );
      billnode = IRR.smgr->addBillboardSceneNode( 0, dimension2d<f32>( vSize.X / 4.0f, vSize.Y / 4.0f ), vCenter + vR );
      billnode->setMaterialFlag( video::EMF_LIGHTING, ( bool )IRR.useLighting );
      billnode->setMaterialType( video::EMT_TRANSPARENT_ADD_COLOR );
      sprintf( buf, "Textures/Clouds/cloud%i.jpg", ( ( i + 3 * type ) % 4 ) + 1 );
      billnode->setMaterialTexture( 0, IRR.video->getTexture( APP.useFile( buf ).c_str() ) );
      nodes.push_back( billnode );
    }

    vR = vector3df( vSize.X / 8, 0.0f, vSize.Z / 4 );
    for ( int i = 0; i < 4; i++ )
    {
      vR.rotateXYBy( 95, vector3df( 0, 0, 0 ) );
      vR.rotateYZBy( 29, vector3df( 0, 0, 0 ) );
      billnode = IRR.smgr->addBillboardSceneNode( 0, dimension2d<f32>( vSize.X / 8.0f, vSize.Y / 8.0f ), vCenter + vR );
      billnode->setMaterialFlag( video::EMF_LIGHTING, ( bool )IRR.useLighting );
      billnode->setMaterialType( video::EMT_TRANSPARENT_ADD_COLOR );
      sprintf( buf, "Textures/Clouds/cloud%i.jpg", ( ( i + 4 * type ) % 4 ) + 1 );
      billnode->setMaterialTexture( 0, IRR.video->getTexture( APP.useFile( buf ).c_str() ) );
      nodes.push_back( billnode );
    }
}

CMap_Cloud::~CMap_Cloud()
{
    if ( IRR.smgr )
    {
      for ( int i = 0; i < nodes.size(); i++ )
      {
        nodes[i]->remove();
      }
    }
}

void CMap_Cloud::Scale( vector3df vScale )
{
    vector3df edges[8];

    dimension2d<f32> dim;
    for ( int i = 0; i < nodes.size(); i++ )
    {
      dim = nodes[i]->getSize();
      dim.Width *= abs( vScale.X );
      dim.Height *= abs( vScale.Y );
      nodes[i]->setSize( dim );
      nodes[i]->setPosition( nodes[i]->getPosition() * vScale );
    }

    cloudbox.MinEdge *= vScale;
    cloudbox.MaxEdge *= vScale;
    cloudbox.repair();

    cloudbox.getEdges( edges );

    vSize.Z = ( edges[3] - edges[1] ).getLength();
    vSize.Y = ( edges[0] - edges[1] ).getLength();
    vSize.X = ( edges[5] - edges[1] ).getLength();  

    vCenter = cloudbox.getCenter();
} 

////////////////////////////////////////////
// CMap_Zone 
////////////////////////////////////////////
CMap_Zone::CMap_Zone( aabbox3df box, int type )
{
    box.repair();
    zonebox = box;
    node = NULL;
    water = NULL;

    this->type = type;
    if ( type == ZONE_WATER )
    {
      fLWF = WATER_FACTOR;
    }

    vector3df edges[8];
    box.getEdges( edges );

    vSize.Z = ( edges[3] - edges[1] ).getLength();
    vSize.Y = ( edges[0] - edges[1] ).getLength();
    vSize.X = ( edges[5] - edges[1] ).getLength();


    // Add some reflective water 
    if ( GAME.bShaderWater )
    {
      int tilesize = GAME.shaderWaterDetail;
      water = new CReflectedWater( "ReflectiveWater", IRR.device, IRR.smgr, -1, tilesize, vSize.X / tilesize, core::dimension2d<s32>( 512, 512 ) ); 
      water->m_waternode->setPosition( box.getCenter() - vector3df( 0, vSize.Y / 2.0f, 0.0f ) );
    }
    else
    {
      node = IRR.smgr->addCubeSceneNode( 1.0f );

      node->setPosition( box.getCenter() );
      node->setScale( vSize );


      node->setMaterialTexture( 1, IRR.video->getTexture( APP.useFile( "Textures/water.jpg" ).c_str() ) );
      node->setMaterialTexture( 0, IRR.video->getTexture( APP.useFile( "Textures/natural_water.jpg" ).c_str() ) );

      node->setMaterialType( video::EMT_REFLECTION_2_LAYER );
      node->setMaterialFlag( EMF_NORMALIZE_NORMALS, true );
      node->setMaterialFlag( EMF_LIGHTING, ( bool )IRR.useLighting );
      node->setVisible( true );
      node->setMaterialFlag( EMF_FOG_ENABLE, true );
    }
}

CMap_Zone::~CMap_Zone()
{
    if ( IRR.smgr )
    {
      if ( node )
      {
        node->remove();
      }
    }
}

void CMap_Zone::Scale( vector3df vScale )
{
    //vector3df edges[8];

    //node->setScale( vSize * vScale );
    //node->setPosition( node->getPosition() * vScale );
    //
    //zonebox.MinEdge *= vScale;
    //zonebox.MaxEdge *= vScale;
    //zonebox.repair();

    //zonebox.getEdges( edges );

    //vSize.Z = (edges[3] - edges[1]).getLength();
    //vSize.Y = (edges[0] - edges[1]).getLength();
    //vSize.X = (edges[5] - edges[1]).getLength();

    //
} 

////////////////////////////////////////////
// CMap_Plane 
////////////////////////////////////////////
CMap_Plane::CMap_Plane( vector3df p1, vector3df p2, vector3df p3, vector3df p4, f32 newSize )
{
    setPlane( p1, p2, p3, p4 );
    size = newSize;
}

void CMap_Plane::setPlane( vector3df p1, vector3df p2, vector3df p3, vector3df p4 )
{
    // right to left triangle
    triangle[0].set( p3, p2, p1 );
    triangle[1].set( p4, p2, p3 );
}

bool CMap_Plane::getIntersectionWithLine( vector3df p1, vector3df p2, vector3df& outIntersection )
{
    vector3df n;
    n.set( ( p2 - p1 ).normalize() );

    return ( triangle[0].getIntersectionWithLine( p1, n, outIntersection ) | triangle[1].getIntersectionWithLine( p1, n, outIntersection ) );
}

bool CMap_Plane::getIntersectionWithLimitedLine( vector3df p1, vector3df p2, vector3df& outIntersection )
{
    line3d<f32> l;
    l.setLine( p1, p2 );

    return ( triangle[0].getIntersectionWithLimitedLine( l, outIntersection ) | triangle[1].getIntersectionWithLimitedLine( l, outIntersection ) );
}

////////////////////////////////////////////
// CMap 
////////////////////////////////////////////
CMap::CMap()
{
    sky = 0;

    respawn = new CRespawn();
    editor = 0;
	atmo = NULL;

    //TEMP
    worldWidth = WORLD_BOUND;
    worldHeight = WORLD_BOUND;
    worldDepth = WORLD_WIDTH;
    planeSize = PLANE_SIZE;

    //  if ( strlen(fileName) < 2 )
    //{
    // TEMP: Construct new default map

    // floor
    //Add2DPlane( vector3df(-10, worldHeight/2, 0.0f), vector3df(worldWidth, worldHeight/2, 0.0f) );

    //f32 segment = 300.0f;
    //for (int i = 0; i < worldWidth/segment; i++)
    //{
    //  if (i % 2)
    //      Add2DPlane( vector3df(i*segment, worldHeight/2 + -4.0f, 0.0f), vector3df((i+1)*segment, worldHeight/2 + 33.0f, 0.0f) );
    //  else
    //      Add2DPlane( vector3df(i*segment, worldHeight/2 + 33.0f , 0.0f), vector3df((i+1)*segment, worldHeight/2 + -4.0f , 0.0f) );
    //}

    //// left wall
    //  Add2DPlane( vector3df(-WORLD_BOUND+20, -WORLD_BOUND, 0.0f), vector3df(-WORLD_BOUND, 5.0f, 0.0f) );
    //// right climb
    //  Add2DPlane( vector3df(WORLD_BOUND, 5.0f, 0.0f), vector3df(WORLD_BOUND-30, -WORLD_BOUND, 0.0f) );
    //// right floor
    //  Add2DPlane( vector3df(1130.0f, worldHeight, 0.0f), vector3df(1130.0f, -worldHeight/2, 0.0f) );
    //// right climb 2
    //Add2DPlane( vector3df(50.0f, 37.0f, 0.0f), vector3df(50.0f, 31.0f, 0.0f) );
    //// right floor 3
    //Add2DPlane( vector3df(50.0f, 31.0f, 0.0f), vector3df(90.0f, 14.0f, 0.0f) );
    //
    //// right cieling
    //Add2DPlane( vector3df(45.0f, -55.0f, 0.0f), vector3df(-80.0f, -10.0f, 0.0f) );

    //IRR.video->setFog( video::SColor(0,79,84,90), true, 2100, 2400, 0.0f, true);

    //IRR.video->setFog( video::SColor(0,74,144,204), true, 2150, 2400, 0.5f, true);

    //setFog (SColor color=SColor(0, 255, 255, 255), bool linearFog=true, f32 start=50.0f,
    //f32 end=100.0f, f32 density=0.01f, bool pixelFog=false, bool rangeFog=false)=0

    //IRR.video->setAmbientLight(SColor(0,234,244,204));

    //NewSky( "Textures/sky3.jpg" );

    //      addMeshMap( "Maps/test.dmf", vector3df(0,0,0), vector3df(0.1f, -0.1f, -0.1f) );

    //ISceneNode *billnode = IRR.smgr->addBillboardSceneNode( 0, dimension2d<f32>(13.0f, 3.0f), vector3df(0,-90,3) );
    //billnode->setMaterialFlag(video::EMF_LIGHTING, false);
    //billnode->setMaterialType(video::EMT_TRANSPARENT_ADD_COLOR);
    //billnode->setMaterialTexture(0, IRR.video->getTexture("Textures/light.png") );

    //CAnimSpriteSceneNode* Sprite = new CAnimSpriteSceneNode(IRR.smgr->getRootSceneNode(), IRR.smgr, -1); 
    //Sprite->Load("Textures/sonwalk.bmp",0,0,40*8,40,40,40,true); 
    //Sprite->setSpeed(50);
    //      Sprite->setPosition(vector3df(-16.0,-14.1,0));
    //      Sprite->setRotation(vector3df(0,0,0));   
    //      Sprite->setScale(vector3df(0.03, -0.03, 0));  
    //Sprite->setMaterialType(video::EMT_TRANSPARENT_ADD_COLOR);
    //Sprites.push_back(Sprite);

    //Sprite = new CAnimSpriteSceneNode(IRR.smgr->getRootSceneNode(), IRR.smgr, -1); 
    //Sprite->Load("Textures/light.png", true); 
    //      Sprite->setPosition(vector3df(-36.0,4.1,0));
    //      Sprite->setRotation(vector3df(0,0,0));   
    //      Sprite->setScale(vector3df(0.5, -0.3, 0));
    //Sprite->setMaterialType(video::EMT_TRANSPARENT_ADD_COLOR);
    //Sprites.push_back(Sprite);

    //CMap_Cloud* cloud = new CMap_Cloud( aabbox3df(vector3df(-145, -170, -40), vector3df( -65, -110, 40)), CLOUD_1 );
    //Clouds.push_back( cloud );

    //cloud = new CMap_Cloud( aabbox3df(vector3df(5, -250, -70), vector3df( 165, -110, 70)), CLOUD_1 );
    //Clouds.push_back( cloud );

    //CMap_Cloud* cloud;
    //f32 cloudY = -130.0f;
    //for (int i = -4; i < 4; i++)
    //{
    //  cloud = new CMap_Cloud( aabbox3df(vector3df(i*93, cloudY, -100), vector3df( i*93 + 200, cloudY - 170, 100)), (i+4) % 4 );
    //  Clouds.push_back( cloud );
    //}

    //CLensFlareSceneNode* lf = new CLensFlareSceneNode( IRR.smgr->getRootSceneNode(), IRR.smgr, -1, core::vector3df( 80, cloudY*3.11f, -2000 ), false );
    //lf->drop();
    //lf->setMaterialTexture( 0, IRR.video->getTexture( "Textures/flares.jpg" ) );

    //sunLight = IRR.smgr->addLightSceneNode(0, vector3df(0, -500, 1500), SColorf(1.0f, 1.0f, 1.0f, 1.0f), 10000.0f);

    //respawn->AddPoint( vector3df(39, 0, 0), "hydroplane" );
    //respawn->AddPoint( vector3df(4, -16, 0), "soldier" );
    //respawn->AddPoint( vector3df(14, -26, 0), "soldier" );
    //respawn->AddPoint( vector3df(-4, -36, 0), "soldier" );

    //vector3df vBPos, vBScale;
    //vBPos.set(0.0f, -14.0f, 0.0f);
    //vBScale.set(1.0f, 1.0f, 4.0f);
    //CBoolblock *block = new CBoolblock();

    //int t = getPreciseTime();
    //block->createBlock( vBPos, vBScale );
    //CONSOLE.addx(" createBlock: %ims", getPreciseTime() - t);

    //Blocks.push_back( block );

    //t = getPreciseTime();
    //createCollisionFromBlock( block, vBPos );
    //CONSOLE.addx(" createCollisionFromBlock: %ims", getPreciseTime() - t);


    //  Save( "Maps/test.cmp" );
    //}
    //else
    //  Load( "Maps/test.cmp" );

    // init optimization grid
    //int i;
    //optCellWidth = OPTCELL_WIDTH;
    //optCellHeight = OPTCELL_HEIGHT;
    //optRows = worldHeight / optCellHeight;
    //optColumns = worldWidth / optCellWidth;
    //optGrid = new optCell*[ optRows ]; 
    //for( i = 0; i < optRows; ++i ) 
    //  optGrid[i] = new optCell[ optColumns ];

    //GenerateOptimizationGrid();

    //SOUND.playSound("Sounds/init.wav");

    //ISceneNode *billnode = new CBackSpriteSceneNode(IRR.smgr->getRootSceneNode(), IRR.smgr, -1, 
    //  vector3df(0,100.0f,-12000), dimension2d<f32>(2000.0f, 2000.0f), 4 );
    //billnode->setMaterialFlag(video::EMF_LIGHTING, false);
    //billnode->setMaterialType(video::EMT_TRANSPARENT_ALPHA_CHANNEL);
    //billnode->setMaterialTexture(0, IRR.video->getTexture("Sprites/skyclouds.png") );
}

CMap::~CMap()
{
    int i, j;

    for ( i = 0; i < Planes.size(); i++ )
    {
      delete Planes[i];
    }
    Planes.clear();

    for ( i = 0; i < Zones.size(); i++ )
    {
      delete Zones[i];
    }
    Zones.clear();

    for ( i = 0; i < Clouds.size(); i++ )
    {
      delete Clouds[i];
    }
    Clouds.clear();

    if ( IRR.smgr )
    {
      for ( i = 0; i < Sprites.size(); i++ )
      {
        Sprites[i]->remove();
      }
    }
    Sprites.clear();

    if ( sky )
    {
      sky->remove();
    }

	if ( atmo )
		delete atmo;

    //for( i = 0; i < optRows; ++i ) 
    //  for( j = 0; j < optColumns; ++j ) 
    //      optGrid[i][j].Planes.clear();

    if ( respawn )
    {
      delete respawn;
    }

    if ( editor )
    {
      delete editor;
    }

    //for( i = 0; i < optRows; ++i ) 
    //  delete[] optGrid[i];
    //delete[] optGrid;
}

void CMap::Add2DPlane( vector3df p1, vector3df p2 )
{
    CMap_Plane* newPlane;
    vector3df p3, p4;

    p1.Z = p2.Z = -worldDepth;

    // 1---2
    // 3---4
    p3 = p1 + vector3df( 0, 0, 2 * worldDepth );
    p4 = p2 + vector3df( 0, 0, 2 * worldDepth );


    newPlane = new CMap_Plane( p1, p2, p3, p4, planeSize );
    AddPlane( newPlane );
}

void CMap::Render()
{
    if ( APP.DebugMode )
    {
      int i, j;
      triangle3df t1, t2;
      vector3df norm;
      f32 size;

      IRR.video->setTransform( ETS_WORLD, matrix4() );

      SMaterial m; 
      m.Lighting = false; 
      m.BackfaceCulling = false;
      IRR.video->setMaterial( m );

      // draw optGrid
      if ( APP.DebugMode > 1 )
      {
        for ( i = 0; i < optRows; i++ )
        {
          for ( j = 0; j < optColumns; j++ )
          {
            IRR.video->draw3DLine( getOptGridRealPos( i, j ), getOptGridRealPos( i + 1, j ), SColor( 0, 255, 0, 150 ) );
            IRR.video->draw3DLine( getOptGridRealPos( i, j ), getOptGridRealPos( i, j + 1 ), SColor( 0, 255, 0, 150 ) );
            IRR.video->draw3DLine( getOptGridRealPos( i + 1, j ), getOptGridRealPos( i + 1, j + 1 ), SColor( 0, 255, 0, 150 ) );
          }
        }
      }


      for ( j = 0; j < Planes.size(); j++ )
      {
        size = Planes[j]->GetSize();

        t1 = Planes[j]->GetTriangle( 0 );
        t2 = Planes[j]->GetTriangle( 1 );
        IRR.video->draw3DTriangle( t1, SColor( 0, 20, 220, 100 ) );
        IRR.video->draw3DTriangle( t2, SColor( 0, 20, 220, 100 ) );
      }

      for ( j = 0; j < Zones.size(); j++ )
      {
        aabbox3df box;
        box.MinEdge = Zones[j]->getBox()->MinEdge;
        box.MaxEdge = Zones[j]->getBox()->MaxEdge;


        IRR.video->draw3DBox( box, SColor( 0, 210, 42, 24 ) );
      }

      for ( j = 0; j < Clouds.size(); j++ )
      {
        aabbox3df box;
        box.MinEdge = Clouds[j]->getBox()->MinEdge;
        box.MaxEdge = Clouds[j]->getBox()->MaxEdge;


        IRR.video->draw3DBox( box, SColor( 0, 210, 42, 24 ) );
      }
    }

    if ( ( editor ) || ( APP.DebugMode ) )
    {
      respawn->Render();
    }

    if ( editor )
    {
      editor->Render();
    }
}

void CMap::Load( const c8* filename )
{
    s32 i, j;
    MapHeader mh;
    MapPlane mp;

    FILE* pFile = fopen( filename, "rb" );
    if ( pFile == NULL )
    {
      return;
    }

    fread( &mh, sizeof( MapHeader ), 1, pFile );

    worldWidth = mh.worldWidth;
    worldHeight = mh.worldHeight;
    worldDepth = mh.worldDepth;
    skyFilename = mh.skyfile;
//    NewSky( skyFilename.c_str() );

    vector3df p1, p2, p3, p4;
    for ( i = 0; i < mh.planesNum; i++ )
    {
      fread( &mp, sizeof( MapPlane ), 1, pFile );

      //triangle[0].set(p3, p2, p1);
      //triangle[1].set(p4, p2, p3);
      p3 = mp.triangle[0].pointA;
      p2 = mp.triangle[0].pointB;
      p1 = mp.triangle[0].pointC;
      p4 = mp.triangle[1].pointA;

      CMap_Plane* plane = new CMap_Plane( p1, p2, p3, p4, mp.size );
      AddPlane( plane );
    }

    fclose( pFile );

    if ( APP.DebugMode )
    {
      CONSOLE.addx( "Map loaded from '%s'", filename );
    }
}

void CMap::Save( const c8* filename )
{
    s32 i, j;
    MapHeader mh;
    MapPlane mp;

    FILE* pFile = fopen( filename, "wb" );

    mh.version = MAPFILE_VERSION;
    mh.planesNum = Planes.size();
    mh.worldWidth = worldWidth;
    mh.worldHeight = worldHeight;
    mh.worldDepth = worldDepth;

    memset( mh.skyfile, 0, 255 );
    memcpy( mh.skyfile, skyFilename.c_str(), skyFilename.size() );

    fwrite( &mh, sizeof( MapHeader ), 1, pFile );

    for ( i = 0; i < mh.planesNum; i++ )
    {
      mp.size = Planes[i]->GetSize();
      mp.triangle[0] = Planes[i]->GetTriangle( 0 );
      mp.triangle[1] = Planes[i]->GetTriangle( 1 );

      fwrite( &mp, sizeof( MapPlane ), 1, pFile );
    }

    fclose( pFile );

    if ( APP.DebugMode )
    {
      CONSOLE.addx( "Map saved to '%s'", filename );
    }
}

void CMap::NewSky( const c8* filename, int hour, int minute)
{
    if ( sky )
    {
      sky->remove();
    }
	if ( atmo )
		delete atmo;

    sky = new CSkyBackSceneNode( IRR.video->getTexture( APP.useFile( filename ).c_str() ), IRR.smgr->getRootSceneNode(), IRR.smgr, -1 );

    skyFilename = filename;

	if ( (hour > -1)&&(minute > -1) )
	{
		atmo = new ATMOsphere(hour, minute);
		atmo->start(IRR.device, IRR.video, IRR.smgr->getRootSceneNode(), IRR.smgr, -1);
		atmo->setDaysPerDay((f64)WORLD.getDaySpeed());
	}
}

void CMap::NewFog( SColor color=SColor(0,255,255,255), bool linearFog=true, f32 start=50.0f, f32 end=100.0f,
			f32 density=0.01f, bool pixelFog=false, bool rangeFog=false )
{
	IRR.video->setFog( color, linearFog, start, end, density, pixelFog, rangeFog );
	this->linearFog = linearFog;
	this->startFog = start;
	this->endFog = end;
	this->densityFog = density;
	this->pixelFog = pixelFog;
	this->rangeFog = rangeFog;
	if ( (color.getRed() == 255)&&(color.getGreen() == 255)&&(color.getBlue() == 255) )
		dynamicFog = true;
	else 
		dynamicFog = false;
}

void CMap::SetFogColor( SColor color )
{
	if (dynamicFog)
		IRR.video->setFog( color, linearFog, startFog, endFog, densityFog, pixelFog, rangeFog );
}

void CMap::Think()
{
    respawn->Think();

    if ( editor )
    {
      editor->Think();
      if ( NET.rakClient->IsConnected() )
      {
        delete editor;
        editor = NULL;
      }
    }

    int i;
    for ( i = 0; i < Sprites.size(); i++ )
    {
      Sprites[i]->Update();
    }

    for ( i = 0; i < Zones.size(); i++ )
    {
      if ( Zones[i]->water )
      {
        Zones[i]->water->updateRendertarget( IRR.smgr );
      }
    }

	if (atmo)
		atmo->update( IRR.video );
}

vector3df CMap::getOptGridRealPos( int r, int c )
{
    return vector3df( c * optCellWidth, r * optCellHeight, 0 );
}

void CMap::CheckPlanesInCell( int i, int j, CMap_Plane* plane, vector3df vP1, vector3df vP2, int im )
{
    if ( PointInOptGrid( i, j ) )
    {
      for ( int k = 0; k < optGrid[i][j].Planes.size(); k++ )
      {
        if ( optGrid[i][j].Planes[k] == Planes[im] )
        {
          return;
        }
      }

      line3d<f32> line1, line2, line3, line4;
      vector3df vOut;

      if ( ( ( vP1.X >= getOptGridRealPos( i, j ).X ) && ( vP1.X < getOptGridRealPos( i, j + 1 ).X ) && ( vP1.Y >= getOptGridRealPos( i, j ).Y ) && ( vP1.Y < getOptGridRealPos( i + 1, j ).Y ) ) || ( ( vP2.X >= getOptGridRealPos( i, j ).X ) && ( vP2.X < getOptGridRealPos( i, j + 1 ).X ) && ( vP2.Y >= getOptGridRealPos( i, j ).Y ) && ( vP2.Y < getOptGridRealPos( i + 1, j ).Y ) ) )
      {
        //  CONSOLE.addx("point x%i y%i plane %i", j, i, im);
        optGrid[i][j].Planes.push_back( Planes[im] );
      }
      else
      {
        line1.setLine( getOptGridRealPos( i, j ), getOptGridRealPos( i, j + 1 ) );
        line2.setLine( getOptGridRealPos( i, j + 1 ), getOptGridRealPos( i + 1, j + 1 ) );
        line3.setLine( getOptGridRealPos( i + 1, j + 1 ), getOptGridRealPos( i + 1, j ) );
        line4.setLine( getOptGridRealPos( i + 1, j ), getOptGridRealPos( i, j ) );   

        if ( ( plane->getIntersectionWithLimitedLine( line1.start, line1.end, vOut ) ) || ( plane->getIntersectionWithLimitedLine( line2.start, line2.end, vOut ) ) || ( plane->getIntersectionWithLimitedLine( line3.start, line2.end, vOut ) ) || ( plane->getIntersectionWithLimitedLine( line4.start, line4.end, vOut ) ) )
        {
          //    CONSOLE.addx("line x%i y%i plane %i", j, i, im);
          optGrid[i][j].Planes.push_back( Planes[im] );
        }
      }
    }
}
void CMap::GenerateOptimizationGrid()
{
    int i, j, im;
    //

    CMap_Plane* plane = new CMap_Plane( vector3df( 0, 0, -100 ), vector3df( 0, 1000, -100 ), vector3df( 0, 0, 100 ), vector3df( 0, 1000, 100 ), 1.0f );

    vector3df vP1, vP2, vPoint, vStep;
    int px, py, tx, ty, stepsNum, step;

    for ( im = 0; im < Planes.size(); im++ )
    {
      vP1 = vector3df( Planes[im]->GetTriangle( 0 ).pointB.X, Planes[im]->GetTriangle( 0 ).pointB.Y, 0 );
      vP2 = vector3df( Planes[im]->GetTriangle( 0 ).pointC.X, Planes[im]->GetTriangle( 0 ).pointC.Y, 0 );

      plane->setPlane( vector3df( Planes[im]->GetTriangle( 0 ).pointB.X, Planes[im]->GetTriangle( 0 ).pointB.Y, -100 ), vector3df( Planes[im]->GetTriangle( 0 ).pointC.X, Planes[im]->GetTriangle( 0 ).pointC.Y, -100 ), vector3df( Planes[im]->GetTriangle( 0 ).pointB.X, Planes[im]->GetTriangle( 0 ).pointB.Y, 100 ), vector3df( Planes[im]->GetTriangle( 0 ).pointC.X, Planes[im]->GetTriangle( 0 ).pointC.Y, 100 ) );

      //for (i = 0; i < optRows-1; i++)
      //    for (j = 0; j < optColumns-1; j++)
      //    f32 lineLength = ( vP2 - vP1 ).getLength();
      tx = OptGridColumn( vP1.X );
      ty = OptGridRow( vP1.Y );
      vStep.X = ( vP1.X - vP2.X ) / optColumns;
      vStep.Y = ( vP1.Y - vP2.Y ) / optRows;

      stepsNum = ( int )( vP2 - vP1 ).getLength() / vStep.getLength();

      vPoint = vP2;
      px = OptGridColumn( vPoint.X );
      py = OptGridRow( vPoint.Y );

      //if ( im == 0 )
      //{
      //    CONSOLE.addx("a x%i y%i plane %i", px, py, im);
      //    CONSOLE.addx("b x%f y%f plane %i", stepX, stepY, im);
      //    CONSOLE.addx("t x%f y%f plane %i", vP1.X, vP1.Y, im);
      //    CONSOLE.addx("p x%f y%f plane %i", vP2.X, vP2.Y, im);
      //}

      step = i = j = -1;
      do
      {
        step++;
        if ( ( i != py ) || ( j != px ) )
        {
          i = py; j = px;
          CheckPlanesInCell( i, j, plane, vP1, vP2, im );
          CheckPlanesInCell( i + 1, j, plane, vP1, vP2, im );
          CheckPlanesInCell( i - 1, j, plane, vP1, vP2, im );
          CheckPlanesInCell( i, j + 1, plane, vP1, vP2, im );
          CheckPlanesInCell( i, j - 1, plane, vP1, vP2, im );
        }

        vPoint.X += vStep.X;
        vPoint.Y += vStep.Y;
        px = OptGridColumn( vPoint.X );
        py = OptGridRow( vPoint.Y );
      }
      while ( step < stepsNum );
    }

    delete plane;
}


//void CMap::createCollisionFromBlock( CBoolblock *block, vector3df vPos, vector3df vScale )
//{
//  NewtonCollision *treecollision;
//  treecollision = NewtonCreateTreeCollision(WORLD.GetPhysics()->nWorld, NULL);
//  NewtonTreeCollisionBeginBuild(treecollision);
//
//  int cMeshBuffer, j, i;
//  int v1i, v2i, v3i;
//  IMeshBuffer *mb;
//
//  vector3df vArray[3]; // vertex array (3*3 floats)
//
//  int tmpCount = 0;
//  
//
//  SMesh *mesh = block->mesh;
//  int buffCount = mesh->getMeshBufferCount();
//  for (i = 0; i < buffCount; i ++) 
//  {
//      for (cMeshBuffer=0; cMeshBuffer < mesh->getMeshBufferCount(); cMeshBuffer++) 
//      {   
//          mb = mesh->getMeshBuffer(cMeshBuffer);
//
//          video::S3DVertex* mb_vertices = (irr::video::S3DVertex*)mb->getVertices();
//
//          u16* mb_indices  = mb->getIndices();
//
//          // add each triangle from the mesh
//          for (j=0; j<mb->getIndexCount(); j+=3)
//          {
//              v1i = mb_indices[j];
//              v2i = mb_indices[j+1];
//              v3i = mb_indices[j+2];
//
//              // to make things easier, here we can use engine data type
//              vArray[0] = mb_vertices[v1i].Pos * 1;
//              vArray[1] = mb_vertices[v2i].Pos * 1;
//              vArray[2] = mb_vertices[v3i].Pos * 1;
//              vArray[0] += vPos;
//              vArray[1] += vPos;
//              vArray[2] += vPos;
//              vArray[0] *= vScale;
//              vArray[1] *= vScale;
//              vArray[2] *= vScale;
//
//              // make sure we do not add degenerated polygons to the tree
//              vector3df e0 (vArray[1] - vArray[0]);
//              vector3df e1 (vArray[2] - vArray[0]);
//
//              // the area of the face is the cross product
//              vector3df area (e0.crossProduct (e1));
//
//              // skip faces with very small area
//              float mag = area.dotProduct (area);
//              if (mag > 1.0e-6f) 
//              {
//                  NewtonTreeCollisionAddFace(treecollision, 3, &vArray[0].X, sizeof (vector3df), 1);
//              } 
//          }
//      }
//  }
// 
//  NewtonTreeCollisionEndBuild(treecollision, OPTIMIZE_COLLISION);
//
//  // create a rigid body
//  NewtonBody* treebody;
//  treebody = NewtonCreateBody(WORLD.GetPhysics()->nWorld, treecollision);
//
//  NewtonBodySetMaterialGroupID(treebody, CNewton::levelID);
//
//  // release the collsion tree 
//  NewtonReleaseCollision (WORLD.GetPhysics()->nWorld, treecollision);
//}

void CMap::addMeshMap( const c8* filename, vector3df vPos, vector3df vScale )
{
    //////////////////////////////////////////////////////////////////////////
    //
    // load map
    //
    //////////////////////////////////////////////////////////////////////////
    scene::IAnimatedMesh* g_map = 0;
    scene::ISceneNode* g_mapnode = 0;
    //g_map = IRR.smgr->getMesh(filename);

    CDMFLoader* loader = new CDMFLoader( IRR.video, IRR.smgr );
    IReadFile* file;
    file = IRR.device->getFileSystem()->createAndOpenFile( APP.useFile( filename ).c_str() );

    g_map = loader->createMesh( file, vScale );

    file->drop();
    delete loader;

    if ( !g_map )
    {
      //CONSOLE.addx( COLOR_ERROR, "Could not load mesh map: %s", filename );
      return;
    }
    g_mapnode = IRR.smgr->addOctTreeSceneNode( g_map->getMesh( 0 ) );
    g_mapnode->setPosition( vPos );
    g_mapnode->setScale( vScale );

    //for (int i=0; i < Sprites.size(); i++ )
    //{
    //  Sprites[i]->setPosition( Sprites[i]->getPosition() * vScale ); 
    //  //Sprites[i]->setScale( Sprites[i]->getScale() * vScale ); 
    //  Sprites[i]->setScale( vScale ); 
    //}

    //for (int i=0; i < respawn->points.size(); i++ )
    //{
    //  respawn->points[i]->vPosition *= vScale;
    //}

    //for (int i=0; i < Zones.size(); i++ )
    //{
    //  Zones[i]->Scale( vScale );
    //}

    //for (int i=0; i < Clouds.size(); i++ )
    //{
    //  Clouds[i]->Scale( vScale );
    //}

    //sunLight->setPosition( sunLight->getPosition() * vScale );
    //g_mapnode->setMaterialFlag( EMF_NORMALIZE_NORMALS, true);
    //g_mapnode->setMaterialFlag( EMF_LIGHTING, (bool)IRR.useLighting );

    g_mapnode->setMaterialFlag( EMF_FOG_ENABLE, true );
    g_mapnode->setMaterialFlag( EMF_LIGHTING, (bool)IRR.useLighting );



    //////////////////////////////////////////////////////////////////////////
    //
    // Create the newton collision tree from the map mesh
    //
    // Remember to use (video::S3DVertex) if you are loading a mesh without lightmaps
    // for your level. (Like a .x or .3ds level)
    //
    //////////////////////////////////////////////////////////////////////////
    NewtonCollision* newtonmap;
    newtonmap = NewtonCreateTreeCollision( WORLD.GetPhysics()->nWorld, NULL );
    NewtonTreeCollisionBeginBuild( newtonmap );
    int cMeshBuffer, j;
    int v1i, v2i, v3i;
    IMeshBuffer* mb;
    float mag, dif;
    dif = OPTIMIZE2D_COLLISION_DIF;

    vector3df vArray[3]; // vertex array (3*3 floats)

    int tmpCount = 0;

    for ( cMeshBuffer = 0; cMeshBuffer< g_map->getMesh( 0 )->getMeshBufferCount(); cMeshBuffer++ )
    {
      mb = g_map->getMesh( 0 )->getMeshBuffer( cMeshBuffer );

      video::S3DVertex2TCoords* mb_vertices = ( irr::video::S3DVertex2TCoords* )mb->getVertices();

      u16* mb_indices = mb->getIndices();

      // add each triangle from the mesh
      for ( j = 0; j< mb->getIndexCount(); j += 3 )
      {
        v1i = mb_indices[j];
        v2i = mb_indices[j + 1];
        v3i = mb_indices[j + 2];

        // to make thing easier, her we can use engine data type
        vArray[0] = mb_vertices[v1i].Pos * vScale + vPos;
        vArray[1] = mb_vertices[v2i].Pos * vScale + vPos;
        vArray[2] = mb_vertices[v3i].Pos * vScale + vPos; //* IrrToNewton

#ifdef OPTIMIZE2D_COLLISION
        // 2d optimize
     //   if ( ( ( abs( vArray[0].X - vArray[1].X ) < dif ) && ( abs( vArray[0].Y - vArray[1].Y ) < dif ) ) || ( ( abs( vArray[1].X - vArray[2].X ) < dif ) && ( abs( vArray[1].Y - vArray[2].Y ) < dif ) ) || ( ( abs( vArray[2].X - vArray[0].X ) < dif ) && ( abs( vArray[2].Y - vArray[0].Y ) < dif ) ) )
    	if ( ( ( vArray[0].Z >= 0.0f ) && ( vArray[1].Z <= 0.0f ) )
			||( ( vArray[0].Z <= 0.0f ) && ( vArray[1].Z >= 0.0f ) )
			||( ( vArray[1].Z >= 0.0f ) && ( vArray[2].Z <= 0.0f ) )
			||( ( vArray[1].Z <= 0.0f ) && ( vArray[2].Z >= 0.0f ) )
			||( ( vArray[2].Z >= 0.0f ) && ( vArray[0].Z <= 0.0f ) )
			||( ( vArray[2].Z <= 0.0f ) && ( vArray[0].Z >= 0.0f ) ) )
        #endif
        {
          // make sure we do not add degenerated polygons to the tree
          vector3df e0( vArray[1] - vArray[0] );
          vector3df e1( vArray[2] - vArray[0] );

          // the area of the face is the cross product
          vector3df area( e0.crossProduct( e1 ) );

          // skip faces with very small area
          mag = area.dotProduct( area );

          if ( ( mag > 1.0e-6f ) )
          {
            NewtonTreeCollisionAddFace( newtonmap, 3, &vArray[0].X, sizeof( vector3df ), 1 );
          }
        }
      }
    }

    // we definilte wan to optimize the mesh (improve performace and vehaviuor a lot)
    NewtonTreeCollisionEndBuild( newtonmap, 1 );


    // create a ridid body to represent the world
    NewtonBody* newtonmapbody;
    newtonmapbody = NewtonCreateBody( WORLD.GetPhysics()->nWorld, newtonmap );

    // release the collsion tree (this way the application does not have to do book keeping of Newton objects
    NewtonReleaseCollision( WORLD.GetPhysics()->nWorld, newtonmap );

    // set the newton world size based on the bsp size
    //vector3df boxP0; 
    //vector3df boxP1; 
    //matrix4 matrix; 
    //NewtonBodyGetMatrix (newtonmapbody, &matrix.M[0]); 
    //NewtonCollisionCalculateAABB (newtonmap, &matrix.M[0],  &boxP0.X, &boxP1.X); 
    //NewtonSetWorldSize (nWorld, &boxP0.X, &boxP1.X);


    // the world uses the ground material
    NewtonBodySetMaterialGroupID( newtonmapbody, CNewton::levelID );
}

void CMap::AddZone( vector3df vPos, vector3df vSize, int zonetype )
{
    CMap_Zone* zone = new CMap_Zone( aabbox3df( vector3df( vPos.X - vSize.X, vPos.Y - vSize.Y, vPos.Z - vSize.Z ), vector3df( vPos.X + vSize.X, vPos.Y + vSize.Y, vPos.Z + vSize.Z ) ), ( ZoneType )zonetype );
    Zones.push_back( zone );
}

void CMap::AddZone( aabbox3df box, int zonetype )
{
    CMap_Zone* zone = new CMap_Zone( box, ( ZoneType )zonetype );
    Zones.push_back( zone );
}

void CMap::addSprite( CAnimSpriteSceneNode* Sprite )
{
    Sprites.push_back( Sprite );
}

void CMap::addCloud( CMap_Cloud* cloud )
{
    Clouds.push_back( cloud );
}

CEditor* CMap::GetEditor()
{
    if ( editor == NULL )
    {
      editor = new CEditor();
    }

    return editor;
}
