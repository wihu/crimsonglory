#include "worldpart.h"

#include "../App/app.h"
// IRR.
#include "../irrlicht/IrrlichtTask.h"
// CONSOLE.
#include "../IrrConsole/console.h"

#include "../FreeSL/SoundTask.h"

#include "world.h"
#include "map.h"

////////////////////////////////////////////
// CWorldPart 
////////////////////////////////////////////

// TODO:
// types of parts:
//  - rotate node according to velocity
//  - node scaled to velocity

CWorldPart::CWorldPart( vector3df NewPos, f32 oneOverMassConst, f32 radiusConst, f32 elasticity, bool collidable, int aliveTime ) : CPhys_Part( NewPos, oneOverMassConst, radiusConst )
{
    Reset();

    setDebugText( "worldpart" );

    bCollidable = collidable;
    alive = timeOut = aliveTime;
    fGravity = WORLD.GetPhysics()->dGravity;
    fTimeStep = WORLD.GetPhysics()->timeStep * 1.0f;
    fElasticity = elasticity;

    node = NULL;
	
    //node = IRR.smgr->addBillboardSceneNode( 0, dsize, NewPos );

    //node->setMaterialFlag(video::EMF_LIGHTING, false);
    //node->setMaterialType(video::EMT_TRANSPARENT_ADD_COLOR);
    //node->setMaterialTexture(0, IRR.video->getTexture(textureFilename) );

    //node = new CBeamNode( IRR.smgr->getRootSceneNode( ), IRR.smgr, -1, textureFilename ); 
    //beam = static_cast<CBeamNode*>(node);
    //beam->SetBeamScale( 0.1f );
    //beam->SetBeamColorStart( video::SColor( 255, 255, 255, 255 ) ); 
    //beam->SetBeamColorEnd( video::SColor( 255, 255, 255, 255 ) ); 
}

CWorldPart::~CWorldPart()
{
    if ( ( IRR.smgr ) && ( node ) )
    {
      node->remove();
    }
}

void CWorldPart::Reset()
{
    CPhys_Part::Reset();
    alive = timeOut;
}

void CWorldPart::Think()
{
    // do the physics
    CPhys_Part::Think();

    // manage collisions
    if ( bCollidable )
    {
      vector3df vColStart;
      //vColStart = OldPos - (Pos - OldPos);
      vColStart = OldPos;// - (Pos - OldPos).normalize() * radius;
      raycastbody = NULL;
      fIParam = 999999.0f;
      NewtonWorldRayCast( WORLD.GetPhysics()->nWorld, &vColStart.X, &Pos.X, CWorldPart::RayCastFilter, this );
      if ( raycastbody )
      {
        ResolveRayCastCollision();
      }

      // map zones (water)
      vector3df vIntersection;
      plane3d<f32> plane;
      for ( int i = 0; i < WORLD.GetMap()->GetZonesNum(); i++ )
      {
        static vector3df edges[8];
        WORLD.GetMap()->GetZone( i )->getBox()->getEdges( edges );
        plane.setPlane( edges[0], edges[2], edges[4] );
        if ( plane.getIntersectionWithLimitedLine( AltOldPos, Pos, vIntersection ) )
        {
          // create water splash effect
          //                CSplashEffect *se = new CSplashEffect( vIntersection, 0.0005f, 1.33f*radius, 25 );
          FACTORY->Effects.Create( "gunsplash", vIntersection, vIntersection, 0.00001f, 5.0f * radius, 30 );
          FACTORY->Effects.Create( "surfacesplash", vIntersection, vIntersection, 0.0f, radius * 0.3f, 60 );
          FACTORY->Effects.Create( "watercircle", vIntersection, vIntersection, 0.0f, radius * 0.02f, 100 );
          MakeBubbles( vIntersection ); 
          SOUND.playSound( "Sounds/bullet_water4.wav", vIntersection, vIntersection, 1.0f, 1.0f );
        }
      }
    }

    // set the rendering
    if ( node )
    {
      node->setPosition( Pos );
    }
}

dFloat CWorldPart::RayCastFilter( const NewtonBody* body, const dFloat* normal, int collisionID, void* userData, dFloat intersetParam )
{
    //dFloat mass;
    //dFloat Ixx;
    //dFloat Iyy;
    //dFloat Izz;
    //NewtonBodyGetMassMatrix (body, &mass, &Ixx, &Iyy, &Izz);

    CWorldPart* part = ( CWorldPart* )userData;
    if ( intersetParam < part->fIParam )
    {
      part->fIParam = intersetParam;
      part->raycastbody = ( NewtonBody * )body;
      //get point of collision
      part->vColPoint = part->OldPos + ( part->Pos - part->OldPos ) * intersetParam;
      // get bounce velocity
      memcpy( &part->vColNormal.X, normal, sizeof( f32 ) * 3 );
    }

    return intersetParam;
}

void CWorldPart::ResolveRayCastCollision()
{
    vector3df vPartVelocity = GetVelocity();
    vector3df vN = vPartVelocity;
    vN.normalize();
    vColNormal = vColNormal * ( -vN.dotProduct( vColNormal ) ) * 2 + vN;
    //R= 2*(-I dot N)*N + I

    AltOldPos = OldPos;
    Pos = vColPoint + vColNormal * 0.0001f;
    OldPos = ( Pos - vColNormal * vPartVelocity.getLength() * fElasticity );

    //part->Pos += WORLD.GetPhysics()->getPointVelocity( body, vColPoint ) /  103.0f;
    OnHit( raycastbody, vPartVelocity, 1 / oneOverMass );
}

void CWorldPart::MakeBubbles( vector3df vIntersect )
{
    int amount = 2 + random( 4 );
    vector3df vel = ( Pos - vIntersect ) / amount;
    vector3df currPos = vIntersect;
    vector3df randPos;
    int rad = 2;

    for ( int i = 0; i < amount; i++ )
    {
      currPos += vel;
      randPos = vector3df( -rad + random( 2 * rad ), -rad + random( 2 * rad ), -rad + random( 2 * rad ) ) / 10.0f;
      FACTORY->Effects.Create( "bubble", currPos + randPos, currPos + randPos, -0.00004f, 2.0f * radius, 140 - i * 10 );
    }
}