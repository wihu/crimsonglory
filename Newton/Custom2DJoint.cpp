
#include "../Game/stdafx.h"
#include "Custom2DJoint.h"
#include "../Newton/newton_physics.h"
#include "../World/world.h"

//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////

CustomJoint2D::CustomJoint2D( NewtonBody* kpBody0, dVector kPlaneNormal )
{
    mpBody0 = kpBody0;
    mPlaneNormal = kPlaneNormal;

    dMatrix matrix0;
    NewtonBodyGetMatrix( mpBody0, &matrix0[0][0] );
    mPlaneOrigin = matrix0.m_posit;

    mpJoint = NewtonConstraintCreateUserJoint( WORLD.GetPhysics()->nWorld, 6, SubmitConstraints, mpBody0, 0 );

    NewtonJointSetUserData( mpJoint, ( void* ) this );

    NewtonJointSetDestructor( mpJoint, Destructor );
}

CustomJoint2D::~CustomJoint2D()
{
    //_ASSERTE (m_joint);

    //if the joint has user data it means the application is destroy the joint
    if ( NewtonJointGetUserData( mpJoint ) )
    {
      // set the joint call to NULL to prevent infinite recursion 
      NewtonJointSetDestructor( mpJoint, NULL );  

      // destroy this joint
      NewtonDestroyJoint( WORLD.GetPhysics()->nWorld, mpJoint );
    }
}

void CustomJoint2D::Destructor( const NewtonJoint* me )
{
    CustomJoint2D* joint;  

    // get the pointer to the joint class
    joint = ( CustomJoint2D * )NewtonJointGetUserData( me );  

    // set the joint call to NULL to prevent infinite recursion
    NewtonJointSetDestructor( me, NULL );  
    NewtonJointSetUserData( me, NULL );  

    // delete the joint class
    delete joint;
}

void CustomJoint2D::SubmitConstraints( const NewtonJoint* me )
{
    CustomJoint2D* joint;  

    // get the pointer to the joint class
    joint = ( CustomJoint2D * )NewtonJointGetUserData( me );  
    joint->LocalSubmitConstraints( me );
}

void CustomJoint2D::LocalSubmitConstraints( const NewtonJoint* kpJoint )
{
    dMatrix matrix0;
    NewtonBodyGetMatrix( mpBody0, &matrix0[0][0] );

    // this line clamps the origin to the plane
    NewtonUserJointAddLinearRow( mpJoint, &matrix0.m_posit[0], &mPlaneOrigin[0], &mPlaneNormal[0] );

    // we can prevent rotation that takes any points out of the plane by clamping a point on the
    // object that is on the line through the origin of the object with the same vector as the
    // plane normal.  The clamp is to another plane that is parallel to the first, offset by the
    // plane normal vector.  Rotations around either of the axes orthogonal to the plane normal
    // will be prevented because they take the object point off that parallel plane.

    dVector object_point;
    dVector world_point;

    object_point = matrix0.TransformVector( mPlaneNormal );
    world_point = mPlaneOrigin + mPlaneNormal;
    //  NewtonUserJointAddLinearRow (mpJoint, &object_point[0], &world_point[0], &mPlaneNormal[0]);
}