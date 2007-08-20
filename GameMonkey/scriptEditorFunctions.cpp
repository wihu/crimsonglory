#include "scriptEditorFunctions.h"
#include "../Game/GameDLL.h"
#include "../Game/SingletonIncludes.h"
#include "../World/map.h"
#include "../Irrlicht/CLensFlaresSceneNode.h"

// SCRIPTBIND( gmStartServer, "startServer");
//int GM_CDECL gmStartServer( gmThread* a_thread )
//{
//  NET.StartServer();
//
//    return GM_OK;
//}
//GM_CHECK_NUM_PARAMS( 2 ); 
//GM_CHECK_INT_PARAM( owner, 0 );
//GM_CHECK_STRING_PARAM( caption, 1 );

// SCRIPTBIND( gmSetFog, "mapSetFog");
int GM_CDECL gmSetFog( gmThread* a_thread )
{
    GM_CHECK_NUM_PARAMS( 9 ); 
    GM_CHECK_INT_PARAM( color_r, 0 );
    GM_CHECK_INT_PARAM( color_g, 1 );
    GM_CHECK_INT_PARAM( color_b, 2 );
    GM_CHECK_INT_PARAM( linear, 3 );
    GM_CHECK_FLOAT_PARAM( start, 4 );
    GM_CHECK_FLOAT_PARAM( end, 5 );
    GM_CHECK_FLOAT_PARAM( density, 6 );
    GM_CHECK_INT_PARAM( pixel, 7 );
    GM_CHECK_INT_PARAM( range, 8 );

    //\param color: Color of the fog
    //\param linearFog: Set this to true for linear fog, otherwise exponential fog is applied.
    //\param start: Only used in linear fog mode (linearFog=true). Specifies where fog starts.
    //\param end: Only used in linear fog mode (linearFog=true). Specifies where fog ends.
    //\param density: Only used in expotential fog mode (linearFog=false). Must be a value between 0 and 1.
    //\param pixelFog: Set this to false for vertex fog, and true if you want pixel fog.
    //\param rangeFog: Set this to true to enable range-based vertex fog. The distance
    //from the viewer is used to compute the fog, not the z-coordinate. This is
    //better, but slower. This is only available with D3D and vertex fog. 
    IRR.video->setFog( video::SColor( 0, color_r, color_g, color_b ), ( bool )linear, start, end, density, ( bool )pixel, ( bool )range );

    return GM_OK;
}

// SCRIPTBIND( gmSetAmbientLight, "mapSetAmbientLight");
int GM_CDECL gmSetAmbientLight( gmThread* a_thread )
{
    GM_CHECK_NUM_PARAMS( 3 ); 
    GM_CHECK_INT_PARAM( color_r, 0 );
    GM_CHECK_INT_PARAM( color_g, 1 );
    GM_CHECK_INT_PARAM( color_b, 2 );

    IRR.video->setAmbientLight( SColor( 0, color_r, color_g, color_b ) );

    return GM_OK;
}

// SCRIPTBIND( gmSetSky, "mapSetSky");
int GM_CDECL gmSetSky( gmThread* a_thread )
{
    GM_CHECK_NUM_PARAMS( 3 ); 
    GM_CHECK_STRING_PARAM( texture, 0 );
    GM_CHECK_INT_PARAM( h, 1 );
    GM_CHECK_INT_PARAM( m, 2 );

    WORLD.GetMap()->NewSky( texture, h, m );

    return GM_OK;
}

// SCRIPTBIND( gmAddZone, "mapAddZone");
int GM_CDECL gmAddZone( gmThread* a_thread )
{
    GM_CHECK_NUM_PARAMS( 7 ); 
    GM_CHECK_FLOAT_PARAM( px, 0 );
    GM_CHECK_FLOAT_PARAM( py, 1 );
    GM_CHECK_FLOAT_PARAM( pz, 2 );
    GM_CHECK_FLOAT_PARAM( sx, 3 );
    GM_CHECK_FLOAT_PARAM( sy, 4 );
    GM_CHECK_FLOAT_PARAM( sz, 5 );
    GM_CHECK_INT_PARAM( zonetype, 6 );

    WORLD.GetMap()->AddZone( vector3df( px, py, pz ), vector3df( sx / 2, sy / 2, sz / 2 ), zonetype );

    return GM_OK;
}

// SCRIPTBIND( gmAddMesh, "mapAddMesh");
int GM_CDECL gmAddMesh( gmThread* a_thread )
{
    GM_CHECK_NUM_PARAMS( 1 ); 
    GM_CHECK_STRING_PARAM( filename, 0 );
    GM_CHECK_FLOAT_PARAM( px, 1 );
    GM_CHECK_FLOAT_PARAM( py, 2 );
    GM_CHECK_FLOAT_PARAM( pz, 3 );
    GM_CHECK_FLOAT_PARAM( sx, 4 );
    GM_CHECK_FLOAT_PARAM( sy, 5 );
    GM_CHECK_FLOAT_PARAM( sz, 6 );

    WORLD.GetMap()->addMeshMap( filename, vector3df( px, py, pz ), vector3df( sx, sy, sz ) );

    return GM_OK;
}

// SCRIPTBIND( gmAddSprite, "mapAddSprite");
int GM_CDECL gmAddSprite( gmThread* a_thread )
{
    GM_CHECK_NUM_PARAMS( 12 ); 
    GM_CHECK_STRING_PARAM( filename, 0 );
    GM_CHECK_INT_PARAM( colorkey, 1 );
    GM_CHECK_FLOAT_PARAM( px, 2 );
    GM_CHECK_FLOAT_PARAM( py, 3 );
    GM_CHECK_FLOAT_PARAM( pz, 4 );
    GM_CHECK_FLOAT_PARAM( rx, 5 );
    GM_CHECK_FLOAT_PARAM( ry, 6 );
    GM_CHECK_FLOAT_PARAM( rz, 7 );
    GM_CHECK_FLOAT_PARAM( sx, 8 );
    GM_CHECK_FLOAT_PARAM( sy, 9 );
    GM_CHECK_FLOAT_PARAM( sz, 10 );
    GM_CHECK_INT_PARAM( material, 11 );

    CAnimSpriteSceneNode* Sprite = new CAnimSpriteSceneNode( IRR.smgr->getRootSceneNode(), IRR.smgr, -1 ); 
    Sprite->Load( APP.useFile( filename ).c_str(), colorkey, false ); 
    Sprite->setPosition( vector3df( px, py, pz ) );
    Sprite->setRotation( vector3df( rx, ry, rz ) );   
    Sprite->setScale( vector3df( sx, sy, sz ) );
    Sprite->setMaterialType( ( E_MATERIAL_TYPE )material );
    Sprite->setDebugDataVisible( true );
    Sprite->updateAbsolutePosition();

    WORLD.GetMap()->addSprite( Sprite );

    return GM_OK;
}

// SCRIPTBIND( gmAddAnimSprite, "mapAddAnimSprite");
int GM_CDECL gmAddAnimSprite( gmThread* a_thread )
{
    GM_CHECK_NUM_PARAMS( 19 ); 
    GM_CHECK_STRING_PARAM( filename, 0 );
    GM_CHECK_INT_PARAM( colorkey, 1 );
    GM_CHECK_FLOAT_PARAM( px, 2 );
    GM_CHECK_FLOAT_PARAM( py, 3 );
    GM_CHECK_FLOAT_PARAM( pz, 4 );
    GM_CHECK_FLOAT_PARAM( rx, 5 );
    GM_CHECK_FLOAT_PARAM( ry, 6 );
    GM_CHECK_FLOAT_PARAM( rz, 7 );
    GM_CHECK_FLOAT_PARAM( sx, 8 );
    GM_CHECK_FLOAT_PARAM( sy, 9 );
    GM_CHECK_FLOAT_PARAM( sz, 10 );
    GM_CHECK_INT_PARAM( material, 11 );
    GM_CHECK_INT_PARAM( Ax, 12 );
    GM_CHECK_INT_PARAM( Ay, 13 );
    GM_CHECK_INT_PARAM( Aw, 14 );
    GM_CHECK_INT_PARAM( Ah, 15 );
    GM_CHECK_INT_PARAM( frmWidth, 16 );
    GM_CHECK_INT_PARAM( frmHeight, 17 );
    GM_CHECK_INT_PARAM( speed, 18 );

    CAnimSpriteSceneNode* Sprite = new CAnimSpriteSceneNode( IRR.smgr->getRootSceneNode(), IRR.smgr, -1 ); 
    Sprite->Load( APP.useFile( filename ).c_str(), Ax, Ay, Aw, Ah, frmWidth, frmHeight, colorkey, false ); 
    Sprite->setPosition( vector3df( px, py, pz ) );
    Sprite->setRotation( vector3df( rx, ry, rz ) );   
    Sprite->setScale( vector3df( sx, sy, sz ) );
    Sprite->setMaterialType( ( E_MATERIAL_TYPE )material );
    Sprite->setSpeed( speed );
    Sprite->updateAbsolutePosition();

    WORLD.GetMap()->addSprite( Sprite );

    return GM_OK;
}

// SCRIPTBIND( gmAddCloud, "mapAddCloud");
int GM_CDECL gmAddCloud( gmThread* a_thread )
{
    GM_CHECK_NUM_PARAMS( 7 ); 
    GM_CHECK_FLOAT_PARAM( px, 0 );
    GM_CHECK_FLOAT_PARAM( py, 1 );
    GM_CHECK_FLOAT_PARAM( pz, 2 );
    GM_CHECK_FLOAT_PARAM( p2x, 3 );
    GM_CHECK_FLOAT_PARAM( p2y, 4 );
    GM_CHECK_FLOAT_PARAM( p2z, 5 );
    GM_CHECK_INT_PARAM( cloudtype, 6 );

    CMap_Cloud* cloud = new CMap_Cloud( aabbox3df( vector3df( px, py, pz ), vector3df( p2x, p2y, p2z ) ), cloudtype );
    WORLD.GetMap()->addCloud( cloud );

    return GM_OK;
}

// SCRIPTBIND( gmAddSun, "mapAddSun");
int GM_CDECL gmAddSun( gmThread* a_thread )
{
    GM_CHECK_NUM_PARAMS( 8 ); 
    GM_CHECK_STRING_PARAM( filename, 0 );
    GM_CHECK_FLOAT_PARAM( px, 1 );
    GM_CHECK_FLOAT_PARAM( py, 2 );
    GM_CHECK_FLOAT_PARAM( pz, 3 );
    GM_CHECK_INT_PARAM( lockx, 4 );
    GM_CHECK_FLOAT_PARAM( color_r, 5 );
    GM_CHECK_FLOAT_PARAM( color_g, 6 );
    GM_CHECK_FLOAT_PARAM( color_b, 7 );

    CLensFlareSceneNode* lf = new CLensFlareSceneNode( IRR.smgr->getRootSceneNode(), IRR.smgr, -1, core::vector3df( px, py, pz ), ( bool )lockx );
    lf->drop();
    lf->setMaterialTexture( 0, IRR.video->getTexture( APP.useFile( filename ).c_str() ) );

    WORLD.GetMap()->setSunLight( lf );

    //  WORLD.GetMap()->setSunLight( IRR.smgr->addLightSceneNode(0, vector3df(px, py, -pz/3), SColorf(1.0f, color_r, color_g, color_b), -pz*100.0f) );

    return GM_OK;
}

// SCRIPTBIND( gmAddRespawn, "mapAddRespawn");
int GM_CDECL gmAddRespawn( gmThread* a_thread )
{
    GM_CHECK_NUM_PARAMS( 5 ); 
    GM_CHECK_STRING_PARAM( actorname, 0 );
    GM_CHECK_FLOAT_PARAM( px, 1 );
    GM_CHECK_FLOAT_PARAM( py, 2 );
    GM_CHECK_FLOAT_PARAM( pz, 3 );
    GM_CHECK_INT_PARAM( team, 4 );

    if ( WORLD.GetMap() )
    {
      WORLD.GetMap()->GetRespawn()->AddPoint( vector3df( px, py, pz ), actorname, team );
    }

    return GM_OK;
}

// SCRIPTBIND( gmSetActorConfig, "mapSetActorConfig");
int GM_CDECL gmSetActorConfig( gmThread* a_thread )
{
    GM_CHECK_NUM_PARAMS( 1 ); 
    GM_CHECK_STRING_PARAM( filename, 0 );

    if ( WORLD.GetMap() )
    {
      WORLD.GetMap()->GetEditor()->actorConfigFile = filename;
    }

    return GM_OK;
}

// SCRIPTBIND( gmDeleteActor, "mapDeleteActor");
int GM_CDECL gmDeleteActor( gmThread* a_thread )
{
    GM_CHECK_NUM_PARAMS( 1 ); 
    GM_CHECK_FLOAT_PARAM( radius, 0 );

    if ( WORLD.GetMap() )
    {
      WORLD.GetMap()->GetEditor()->DeleteActorAtCursor( radius );
    }

    return GM_OK;
}

// SCRIPTBIND( gmDeleteRespawn, "mapDeleteRespawn");
int GM_CDECL gmDeleteRespawn( gmThread* a_thread )
{
    GM_CHECK_NUM_PARAMS( 1 ); 
    GM_CHECK_FLOAT_PARAM( radius, 0 );

    if ( WORLD.GetMap() )
    {
      WORLD.GetMap()->GetEditor()->DeleteRespawnAtCursor( radius );
    }

    return GM_OK;
}

// SCRIPTBIND( gmaddSkyDome, "addSkyDome");
int GM_CDECL gmaddSkyDome( gmThread* a_thread )
{
    GM_CHECK_NUM_PARAMS( 1 ); 
    GM_CHECK_STRING_PARAM( texture, 0 );

	//IRR.smgr->addSkyBoxSceneNode( IRR.video->getTexture( APP.useFile( texture ).c_str() ), 
	//	IRR.video->getTexture( APP.useFile( texture ).c_str() ),
	//	IRR.video->getTexture( APP.useFile( texture ).c_str() ),
	//	IRR.video->getTexture( APP.useFile( texture).c_str() ),
	//	IRR.video->getTexture( APP.useFile( texture ).c_str() ),
	//	IRR.video->getTexture( APP.useFile( texture ).c_str() ) );
	IRR.smgr->addSkyDomeSceneNode( IRR.video->getTexture( APP.useFile( texture ).c_str()), 150, 150, 1, 2 );

    return GM_OK;
}

//\\//\\//\\//\\//\\//\\//\\//\\//\\//\\//\\//\\//\\//\\//\\//\\//\\//\\

void BindScriptEditorFunctions()
{
    // bind GM script functions
    SCRIPTBIND( gmSetFog, "mapSetFog" );
    SCRIPTBIND( gmSetAmbientLight, "mapSetAmbientLight" );
    SCRIPTBIND( gmSetSky, "mapSetSky" );
    SCRIPTBIND( gmAddZone, "mapAddZone" );
    SCRIPTBIND( gmAddMesh, "mapAddMesh" );
    SCRIPTBIND( gmAddSprite, "mapAddSprite" );
    SCRIPTBIND( gmAddAnimSprite, "mapAddAnimSprite" );
    SCRIPTBIND( gmAddCloud, "mapAddCloud" );
    SCRIPTBIND( gmAddSun, "mapAddSun" );
    SCRIPTBIND( gmAddRespawn, "mapAddRespawn" );
    SCRIPTBIND( gmSetActorConfig, "mapSetActorConfig" );
    SCRIPTBIND( gmDeleteActor, "mapDeleteActor" );
    SCRIPTBIND( gmDeleteRespawn, "mapDeleteRespawn" );
	SCRIPTBIND( gmaddSkyDome, "addSkyDome");
}

