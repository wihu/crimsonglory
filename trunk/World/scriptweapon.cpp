#include "scriptweapon.h"

////////////////////////////////////////////
// CScriptWeapon 
////////////////////////////////////////////

bool bRegistered_CScriptWeapon = FACTORY->Actors.Register<CScriptWeapon>( "scriptweapon" );

CScriptWeapon::CScriptWeapon( const c8* scriptFilename )
{
    Reset();

    factoryName = "scriptweapon";

    if ( scriptFilename != "" )
    {
      Load( scriptFilename );
      Unserialize( loadedBitStream );
    }
}

CScriptWeapon::~CScriptWeapon()
{
}

void CScriptWeapon::Render()
{
    CWeapon::Render();
}

void CScriptWeapon::Reset()
{
    CWeapon::Reset();
}


void CScriptWeapon::Think()
{
    CWeapon::Think();
}

void CScriptWeapon::Load( const c8* filename )
{
    // load script file
    SCRIPT.RunFile( filename );

    LOAD_STRING( "scriptweapon_modelname" );
    LOAD_STRING( "scriptweapon_texturename" );
    LOAD_STRING( "scriptweapon_firesoundname" );
    LOAD_VECTOR( "scriptweapon_modelscale_x", "scriptweapon_modelscale_y", "scriptweapon_modelscale_z" );
    LOAD_VALUE( "scriptweapon_mass", f32 );
    LOAD_VALUE( "scriptweapon_bodytype", int );
    LOAD_VALUE( "scriptweapon_power", f32 );
    LOAD_VALUE( "scriptweapon_fireinterval", int );
    LOAD_VALUE( "scriptweapon_autofire", bool );
    LOAD_VALUE( "scriptweapon_boltaction", bool );
    LOAD_VECTOR( "scriptweapon_muzzlepos_x", "scriptweapon_muzzlepos_y", "scriptweapon_muzzlepos_z" );
    LOAD_VALUE( "scriptweapon_bullet_alivetime", int );
    LOAD_VALUE( "scriptweapon_bullet_mass", f32 );
    LOAD_VALUE( "scriptweapon_bullet_radius", f32 );
    LOAD_VALUE( "scriptweapon_bullet_velocity", f32 );
    LOAD_STRING( "scriptweapon_bullet_texturename" );
    LOAD_VALUE( "scriptweapon_bullet_colorstart_a", int );
    LOAD_VALUE( "scriptweapon_bullet_colorstart_r", int );
    LOAD_VALUE( "scriptweapon_bullet_colorstart_g", int );
    LOAD_VALUE( "scriptweapon_bullet_colorstart_b", int );
    LOAD_VALUE( "scriptweapon_bullet_colorend_a", int );
    LOAD_VALUE( "scriptweapon_bullet_colorend_r", int );
    LOAD_VALUE( "scriptweapon_bullet_colorend_g", int );
    LOAD_VALUE( "scriptweapon_bullet_colorend_b", int );
}

void CScriptWeapon::Unserialize( RakNet::BitStream& bt )
{
    int stringLength = 0;
    char* inString = NULL;
    vector3df vScale;
    f32 fMass, fPower;
    int iBody;
    int cstart[4];
    int cend[4];

    UNLOAD_STRING
    String modelname = inString;    delete[] inString; inString = NULL;

    UNLOAD_STRING
    String texname = inString;  delete[] inString; inString = NULL;

    UNLOAD_STRING fireSoundName = inString;
    delete[] inString; inString = NULL;

    bt.ReadVector( vScale.X, vScale.Y, vScale.Z );
    bt.Read( fMass );
    bt.Read( iBody );
    bt.Read( bulletData.fHitPower );
    bt.Read( fireInterval );
    bt.Read( bAutoFire );
    bt.Read( bBoltAction );
    bt.ReadVector( vMuzzlePos.X, vMuzzlePos.Y, vMuzzlePos.Z );
    bt.Read( bulletData.aliveTime );
    bt.Read( bulletData.fMass );
    bt.Read( bulletData.fRadius );
    bt.Read( bulletData.fVelocity );

    UNLOAD_STRING
    bulletData.textureName = inString;  delete[] inString; inString = NULL;

    bt.Read( cstart[0] );
    bt.Read( cstart[1] );
    bt.Read( cstart[2] );
    bt.Read( cstart[3] );
    bt.Read( cend[0] );
    bt.Read( cend[1] );
    bt.Read( cend[2] );
    bt.Read( cend[3] );

    bulletData.beamColorStart = SColor( cstart[0], cstart[1], cstart[2], cstart[3] );
    bulletData.beamColorEnd = SColor( cend[0], cend[1], cend[2], cend[3] );

    // little hack
    vMuzzlePos *= vScale;

    assemblePhysics( modelname.c_str(), ( BodyType )iBody, vScale, vector3df( 0, 0, 0 ), fMass, true );
    if ( node )
    {
      node->setMaterialTexture( 0, IRR.video->getTexture( texname.c_str() ) );
    }

    if ( ( !node ) || ( !body ) )
    {
      bInvalidEntity = true;
      return;
    }

    // continuous collision
    NewtonBodySetContinuousCollisionMode( body, 1 );
}
