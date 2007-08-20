#include "parts.h"

#include "../App/app.h"
// IRR.
#include "../irrlicht/IrrlichtTask.h"
// CONSOLE.
#include "../IrrConsole/console.h"


#define FORCE_DRAW_MULTIPLY 1
#define PHYSCONST_DAMPING 0.999f
#define PHYSCONST_GRAVITY 0.03f
#define PHYSCONST_WIND 0.0000f
#define POINT_DRAW 0.2f

////////////////////////////////////////////
// CPhys_Part 
////////////////////////////////////////////

CPhys_Part::CPhys_Part() : CEntity()
{
    Reset();
}

CPhys_Part::CPhys_Part( vector3df NewPos, f32 oneOverMassConst, f32 radiusConst ) : CEntity()
{
    Reset();

    Pos = OldPos = AltOldPos = NewPos;
    oneOverMass = oneOverMassConst;
    radius = radiusConst;
    colDamping = 1.0f;
    fGravity = PHYSCONST_GRAVITY;
    fDamping = PHYSCONST_DAMPING;
    fTimeStep = 1.0f;
}

CPhys_Part::~CPhys_Part()
{
}


void CPhys_Part::Reset()
{
    CEntity::Reset();
}

void CPhys_Part::Think()
{
    CEntity::Think();

    // update the debug position
    setDebugPos( IRR.smgr->getSceneCollisionManager()->getScreenCoordinatesFrom3DPosition( Pos, IRR.smgr->getActiveCamera() ) );

    if ( oneOverMass == 0.0f )
    {
      return;
    }

    //
    Force += vector3df( 0, fGravity, 0 );
    //Force += vector3df(fWind, 0, 0);
    //

    // Verlet integration
    vector3df NewPos, Dp, Do;
    Dp = Pos * ( 1.0f + fDamping );
    Do = OldPos * fDamping;
    NewPos = ( Dp - Do ) + ( Force * oneOverMass );// *  fTimeStep *  fTimeStep;
    OldPos = Pos; AltOldPos = Pos;
    Pos = NewPos;
    OldForce = Force;
    Force = Force * 0;

    OldPos.Z = Pos.Z = 0.0f;
}

void CPhys_Part::SetVelocity( vector3df newVel )
{
    OldPos = Pos - newVel;
    AltOldPos = OldPos;
}

void CPhys_Part::Move( vector3df newPos )
{
    vector3df oldVel = ( Pos - OldPos );

    // add previous velocity
    OldPos = Pos = newPos;
    Pos += oldVel;
}

void CPhys_Part::ChangePos( vector3df newPos )
{
    OldPos = Pos;
    Pos = newPos;
}

void CPhys_Part::Render()
{
    CEntity::Render();

    if ( APP.DebugMode > 1 )
    {
      IRR.video->draw3DLine( Pos - vector3df( POINT_DRAW, 0, 0 ), Pos + vector3df( POINT_DRAW, 0, 0 ), SColor( 0, 255, 0, 100 ) );          
      IRR.video->draw3DLine( Pos - vector3df( 0, POINT_DRAW, 0 ), Pos + vector3df( 0, POINT_DRAW, 0 ), SColor( 0, 255, 0, 100 ) );          
      IRR.video->draw3DLine( Pos - vector3df( 0, 0, POINT_DRAW ), Pos + vector3df( 0, 0, POINT_DRAW ), SColor( 0, 255, 0, 100 ) );

      // draw forces
      vector3df vP = ( Pos + OldForce* FORCE_DRAW_MULTIPLY );
      IRR.video->draw3DLine( Pos, vP, SColor( 0, 235, 60, 170 ) );
    }
}

void CPhys_Part::Exchange( CPhys_Part* part )
{
    vector3df vTemp, vTemp2;

    vTemp = part->Pos;
    vTemp2 = part->OldPos;
    part->Pos = Pos;
    part->OldPos = OldPos;
    Pos = vTemp;
    OldPos = vTemp2;
}

////////////////////////////////////////////
// CPhys_Joint 
////////////////////////////////////////////
CPhys_Joint::CPhys_Joint( CPhys_Part* P1, CPhys_Part* P2, f32 spring1, f32 spring2 )
{
    Reset();

    Part[0] = P1;
    Part[1] = P2;

    length = ( f32 )( P2->Pos - P1->Pos ).getLength();
    spring[0] = spring1;
    spring[1] = spring2;
}

CPhys_Joint::~CPhys_Joint()
{
}

void CPhys_Joint::Think()
{
    // satisfy constraints
    vector3df delta = Part[1]->Pos - Part[0]->Pos;
    f32 deltalength = ( f32 )delta.getLength();
    //f32 deltalength;
    //deltalength *= (f32) (length*length / (delta*delta + length*length))-0.5;
    diff = ( deltalength - length ) / deltalength;
    Part[0]->Pos += delta * 0.5 * diff * spring[0];
    Part[1]->Pos -= delta * 0.5 * diff * spring[1];
}

void CPhys_Joint::Render()
{
    if ( APP.DebugMode > 2 )
    {
      CPhys_Part* p1 = Part[0];
      CPhys_Part* p2 = Part[1];

      IRR.video->draw3DLine( p1->Pos, p2->Pos, SColor( 0, 0, 50, 100 ) );
    }
}

////////////////////////////////////////////
// CPhys_PlaneConstraint 
////////////////////////////////////////////
CPhys_PlaneConstraint::CPhys_PlaneConstraint( f32 spring1, f32 spring2, s32 plane1, s32 plane2, CPhys_Part* P1, CPhys_Part* P2, CPhys_Part* PP1, CPhys_Part* PP2, f32 orient )
{
    Reset();

    Part[0] = P1;           Part[1] = P2;
    PlanePart[0] = PP1;     PlanePart[1] = PP2;
    spring[0] = spring1;    spring[1] = spring2;
    direction = orient;
}

CPhys_PlaneConstraint::~CPhys_PlaneConstraint()
{
}

void CPhys_PlaneConstraint::Think()
{
    plane3df plane;
    f32 l;
    //      p1      p0 pp2    pp1   pp0
    // 4 \\\ 2 ----- 1 ------ 0 //// 3

    point = ( PlanePart[1]->Pos + Part[0]->Pos ) * 0.5f;
    normal = ( PlanePart[1]->Pos - PlanePart[0]->Pos ).crossProduct( Part[1]->Pos - Part[0]->Pos ).normalize();
    normal *= direction;
    plane.setPlane( point, normal );

    l = plane.getDistanceTo( Part[0]->Pos );
    if ( l < 0 )
    {
      Part[0]->OldPos = Part[0]->Pos;  
      Part[1]->OldPos = Part[1]->Pos;
      Part[0]->Pos -= plane.Normal * l * spring[0];  
      Part[1]->Pos += plane.Normal * l * spring[1];
    }
}


////////////////////////////////////////////
// CPhys_Container
////////////////////////////////////////////

CPhys_Container::~CPhys_Container()
{
    u32 i;

    for ( i = 0; i < Parts.size(); i++ )
    {
      delete Parts[i];
    }
    Parts.clear();

    for ( i = 0; i < Joints.size(); i++ )
    {
      delete Joints[i];
    }
    Joints.clear();

    for ( i = 0; i < Planes.size(); i++ )
    {
      delete Planes[i];
    }
    Planes.clear();
}

CPhys_Part* CPhys_Container::AddPart( vector3df Pos, f32 mass, f32 radius )
{
    //TODO: Check dynamic memory delete

    CPhys_Part* p = new CPhys_Part( Pos, 1 / mass, radius );

    Parts.push_back( p );

    return p;
}

CPhys_Joint* CPhys_Container::ConnectParts( CPhys_Part* P1, CPhys_Part* P2, f32 spring1, f32 spring2 )
{
    CPhys_Joint* j = new CPhys_Joint( P1, P2, spring1, spring2 );

    Joints.push_back( j );

    return j;
}

void CPhys_Container::RemovePart( CPhys_Part* Part )
{
    u32 j;

    // remove any joints connecting this part
    for ( j = 0; j < Joints.size(); j++ )
    {
      if ( ( Joints[j]->GetPart( 0 ) == Part ) || ( Joints[j]->GetPart( 1 ) == Part ) )
      {
        RemoveJoint( Joints[j] );
        j = 0;
      }
    }

    // TODO: remove polygons
    // TODO: remove models

    s32 i = Parts.binary_search( Part );
    if ( PartExists( i ) )
    {
      delete Parts[i];
      Parts.erase( i );
    }
}

void CPhys_Container::RemoveJoint( CPhys_Joint* Joint )
{
    s32 i = Joints.binary_search( Joint );
    if ( JointExists( i ) )
    {
      delete Joints[i];
      Joints.erase( i );
    }
}

void CPhys_Container::Think()
{
    u32 i;

    for ( i = 0; i < Parts.size(); i++ )
    {
      Parts[i]->Think();
    }

    for ( i = 0; i < Joints.size(); i++ )
    {
      Joints[i]->Think();
    }

    for ( i = 0; i < Planes.size(); i++ )
    {
      Planes[i]->Think();
    }
}