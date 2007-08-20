#ifndef SKELETON_H
#define SKELETON_H

#include "calc.h"
#include "physics.h"

#define JOINT_BONE 1.0f
#define JOINT_BONE_TO_CLOTH 0.0f
#define JOINT_CLOTH 0.005f

#define PHYS_ITERATIONS 5


////////////////////////////////////////////
// CPhys_Skeleton 
////////////////////////////////////////////
class CPhys_Skeleton : public CPhys_Container
{
public:
	CPhys_Skeleton();

	virtual void Think();
	virtual void Reset();

	virtual void OnMapCollision()			{}
	virtual void OnSkeletonCollision()		{}

	void UpdatePhysics(s32 count);

	vector3df CalcCenter();
	void setSkeletonPosition( vector3df v );

	void FixZAxis( bool b )					{ bFixZ = b; } 

	vector3df vCenter;
	f32 fRadius;

private:
	void ResolveInnerCollisions(s32 jointNum);

	bool bFixZ;
};

#endif