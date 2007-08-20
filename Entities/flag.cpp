#include "flag.h"

////////////////////////////////////////////
// CFlag 
////////////////////////////////////////////

bool bRegistered_CFlag = FACTORY->Actors.Register<CFlag>( "flag" );

CFlag::CFlag( const c8* scriptFilename )
{
    Reset();

    factoryName = "flag";

    if ( scriptFilename != "" )
    {
      Load( scriptFilename );
      Unserialize( loadedBitStream );
    }
}

CFlag::~CFlag()
{
}

void CFlag::Render()
{
    CItem::Render();
}

void CFlag::Reset()
{
    CItem::Reset();
}


void CFlag::Think()
{
    CItem::Think();
}

void CFlag::Load( const c8* filename )
{
    // load script file
    SCRIPT.RunFile( filename );

    LOAD_STRING( "flag_modelname" );
    LOAD_STRING( "flag_texturename" );
    LOAD_VECTOR( "flag_modelscale_x", "flag_modelscale_y", "flag_modelscale_z" );
    LOAD_VALUE( "flag_mass", f32 );
    LOAD_VALUE( "flag_bodytype", int );
    LOAD_VALUE( "flag_team", int );
}

void CFlag::Unserialize( RakNet::BitStream& bt )
{
    int stringLength = 0;
    char* inString = NULL;
    vector3df vScale;
    f32 fMass;
    int iBody;

    UNLOAD_STRING
    String modelname = inString;    delete[] inString; inString = NULL;

    UNLOAD_STRING
    String texname = inString;  delete[] inString; inString = NULL;

    bt.ReadVector( vScale.X, vScale.Y, vScale.Z );
    bt.Read( fMass );
    bt.Read( iBody );
    bt.Read( team );

    assemblePhysics( modelname.c_str(), ( BodyType )iBody, vScale, vector3df( 0, 0, 0 ), fMass, false );

    if ( ( !node ) || ( !body ) )
    {
      bInvalidEntity = true;
      return;
    }

    //bFix2DPos = bFix2DRot = false;

    node->setMaterialTexture( 0, IRR.video->getTexture( texname.c_str() ) );

    // upvector joint
    vector3df upDirection( -1.0f, 0.0f, 0.0f );
    m_upVector = NewtonConstraintCreateUpVector( WORLD.GetPhysics()->nWorld, &upDirection.X, body );

    // continuous collision
    NewtonBodySetContinuousCollisionMode( body, 1 );

    //setRotation( vector3df(0, 0, 90 ) );
}