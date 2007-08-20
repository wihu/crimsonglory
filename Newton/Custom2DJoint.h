//********************************************************************
// Newton Game dynamics 
// copyright 2000-2004
// By Julio Jerez
// VC: 6.0
// simple demo list vector class with iterators
//********************************************************************

// CustomUniversal.h: interface for the CustomUniversal class.
//
//////////////////////////////////////////////////////////////////////

#if !defined(AFX_CUSTOMUNIVERSAL_H__B631F556_468B_4331_B7D7_F85ECF3E9ADE__INCLUDED_)
#define AFX_CUSTOMUNIVERSAL_H__B631F556_468B_4331_B7D7_F85ECF3E9ADE__INCLUDED_

#include "NewtonCustomJoint.h"

class CustomJoint2D
{
  public:
    CustomJoint2D( NewtonBody* kpBody0, dVector kPlaneNormal );
    virtual ~CustomJoint2D();

    void LocalSubmitConstraints( const NewtonJoint* kpJoint );

  private:
    NewtonBody* mpBody0;
    NewtonJoint* mpJoint;
    dVector mPlaneOrigin, mPlaneNormal;

    // this are the callback needed to have transparent c++ method interfaces 
    static void Destructor( const NewtonJoint* me );    
    static void SubmitConstraints( const NewtonJoint* me );
};

#endif // !defined(AFX_CUSTOMUNIVERSAL_H__B631F556_468B_4331_B7D7_F85ECF3E9ADE__INCLUDED_)
