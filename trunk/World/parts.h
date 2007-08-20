#ifndef PARTS_H
#define PARTS_H

#include "entity.h"

////////////////////////////////////////////
// CPhys_Part 
////////////////////////////////////////////
class CPhys_Part : public CEntity
{
  public:
    CPhys_Part();
    CPhys_Part( vector3df NewPos, f32 oneOverMassConst, f32 radiusConst );
    ~CPhys_Part();

    void ApplyForce( vector3df AddForce )
    {
        Force += AddForce;
    }
    void NewPos( vector3df Pos2 )
    {
        OldPos = Pos2; Pos = Pos2;
    }
    vector3df GetVelocity()
    {
        return Pos - OldPos;
    }
    void SetVelocity( vector3df newVel );

    // move preserving previous velocity
    void Move( vector3df newPos );
    // change position with correct oldpos
    void ChangePos( vector3df newPos );

    void Exchange( CPhys_Part* part );

    void Stop()
    {
        OldPos = Pos;
    }

    virtual void Think();           
    virtual void Reset();
    virtual void Render();

    vector3df Pos, OldPos, Force, OldForce, AltOldPos;

    f32 oneOverMass;
    f32 radius;
    f32 colDamping;

    bool collided;

    f32 fGravity, fDamping, fTimeStep;
};


////////////////////////////////////////////
// CPhys_Constraint 
////////////////////////////////////////////
class CPhys_Constraint
{
  public:
    CPhys_Constraint()
    {
    }
    ~CPhys_Constraint()
    {
    }

    virtual void Think()
    {
    }
    virtual void Reset()
    {
    }
    virtual void Render()
    {
    }

    //! part 0 or 1
    CPhys_Part* GetPart( s32 i )
    {
        return Part[i];
    }
    f32 GetSpring( s32 i )
    {
        return spring[i];
    }

  protected:
    CPhys_Part* Part[2];
    f32 spring[2];
};

////////////////////////////////////////////
// CPhys_Joint 
////////////////////////////////////////////
class CPhys_Joint : public CPhys_Constraint
{
  public:
    CPhys_Joint( CPhys_Part* P1, CPhys_Part* P2, f32 spring1, f32 spring2 );
    ~CPhys_Joint();

    virtual void Think();           
    virtual void Reset()
    {
        CPhys_Constraint::Reset();
    }
    virtual void Render();

    //! part 0 or 1
    f32 GetLength()
    {
        return length;
    }
    f32 GetDiff()
    {
        return diff;
    }
    vector3df GetVector()
    {
        return Part[0]->Pos - Part[1]->Pos;
    }

  private:
    f32 length;
    f32 diff;
};



////////////////////////////////////////////
// CPhys_PlaneConstraint 
////////////////////////////////////////////
class CPhys_PlaneConstraint : public CPhys_Constraint
{
  public:
    CPhys_PlaneConstraint( f32 spring1, f32 spring2, s32 plane1, s32 plane2, CPhys_Part* P1, CPhys_Part* P2, CPhys_Part* PP1, CPhys_Part* PP2, f32 orient );
    ~CPhys_PlaneConstraint();

    virtual void Think();           
    virtual void Reset()
    {
        CPhys_Constraint::Reset();
    }
    virtual void Render()
    {
    }

    //! part 0 or 1
    CPhys_Part* GetPlanePart( s32 i )
    {
        return PlanePart[i];
    }
    f32 GetOrient()
    {
        return direction;
    }

    // 
    vector3df GetPointVector()
    {
        return point;
    }
    vector3df GetNormalVector()
    {
        return normal;
    }

  private:
    CPhys_Part* PlanePart[2];
    vector3df point, normal;
    f32 direction;
};


////////////////////////////////////////////
// CPhys_Container
////////////////////////////////////////////
class CPhys_Container
{
  public:
    CPhys_Container()
    {
        Reset();
    }
    ~CPhys_Container();

    virtual void Think();
    virtual void Reset()
    {
    }
    virtual void Render()
    {
    }

    CPhys_Part* AddPart( vector3df Pos, f32 mass, f32 radius );
    CPhys_Part* AddPart( CPhys_Part* Part )
    {
        Parts.push_back( Part ); return Part;
    }

    CPhys_Joint* ConnectParts( CPhys_Part* P1, CPhys_Part* P2, f32 spring1, f32 spring2 );
    CPhys_Joint* AddJoint( CPhys_Joint* Joint )
    {
        Joints.push_back( Joint ); return Joint;
    }

    void AddPlaneConstraint( CPhys_PlaneConstraint* Plane )
    {
        Planes.push_back( Plane );
    }

    void RemovePart( CPhys_Part* Part );
    void RemoveJoint( CPhys_Joint* Joint );

    CPhys_Part* GetPart( s32 i )
    {
        if ( PartExists( i ) )
        {
          return Parts[i];
        }
        else
        {
          return NULL;
        }
    }
    s32 GetPartsNum()
    {
        return Parts.size();
    }
    bool PartExists( s32 i )
    {
        if ( ( i < 0 ) || ( i > ( s32 )Parts.size() - 1 ) )
        {
          return false;
        }
        else
        {
          return true;
        }
    }
    s32 GetPartIndex( CPhys_Part* Part )
    {
        return Parts.binary_search( Part );
    }

    CPhys_Joint* GetJoint( s32 i )
    {
        if ( JointExists( i ) )
        {
          return Joints[i];
        }
        else
        {
          return NULL;
        }
    }
    s32 GetJointsNum()
    {
        return Joints.size();
    }
    bool JointExists( s32 i )
    {
        if ( ( i < 0 ) || ( i > ( s32 )Joints.size() - 1 ) )
        {
          return false;
        }
        else
        {
          return true;
        }
    }
    s32 GetJointIndex( CPhys_Joint* Joint )
    {
        return Joints.binary_search( Joint );
    }

    CPhys_PlaneConstraint* GetPlane( s32 i )
    {
        if ( PlaneExists( i ) )
        {
          return Planes[i];
        }
        else
        {
          return NULL;
        }
    }
    s32 GetPlanesNum()
    {
        return Planes.size();
    }
    bool PlaneExists( s32 i )
    {
        if ( ( i < 0 ) || ( i > ( s32 )Planes.size() - 1 ) )
        {
          return false;
        }
        else
        {
          return true;
        }
    }
    s32 GetPlaneIndex( CPhys_PlaneConstraint* Plane )
    {
        return Planes.binary_search( Plane );
    }

  protected:
    array<CPhys_Part*> Parts;
    array<CPhys_Joint*> Joints;
    array<CPhys_PlaneConstraint*> Planes;
};


#endif