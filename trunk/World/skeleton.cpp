#include "skeleton.h"	

CPhys_Skeleton::CPhys_Skeleton()
{
	Reset();
}

void CPhys_Skeleton::Reset()					
{ 
	CPhys_Container::Reset();
	bFixZ = false;
}

void CPhys_Skeleton::Think()					
{ 
	CPhys_Container::Think(); 

	// align to 2d
	if ( bFixZ )
		for ( int i = 0; i < GetPartsNum(); i++ )
			GetPart(i)->Pos.Z = GetPart(i)->OldPos.Z;

	vCenter = CalcCenter();
}


void CPhys_Skeleton::ResolveInnerCollisions(int jointNum)
{
	u32 i, k;
	f32 D;
	vector3df ColPart;

	// do jointNum parts collide with other joints?
	for (i=0;i<Joints.size();i++)
		for (k=0;k<2;k++)
			if ( (i != jointNum) && 
				(Joints[jointNum]->GetSpring(0) == JOINT_BONE) && (Joints[i]->GetSpring(1) == JOINT_BONE) &&
			//	( ( (Joints[i]->GetSpring() == JOINT_STIFF)&&(Joints[jointNum]->GetSpring() != JOINT_STIFF) ) ||
			//	  ( (Joints[i]->GetSpring() != JOINT_STIFF)&&(Joints[jointNum]->GetSpring() == JOINT_STIFF) ) )&&
				( (Joints[i]->GetPart(0) != Joints[jointNum]->GetPart(k)) 
				&& (Joints[i]->GetPart(1) != Joints[jointNum]->GetPart(k)) ) 
				)
			{
				D = Distance_Line_Point(Joints[i]->GetPart(0)->Pos, 
					Joints[i]->GetPart(1)->Pos, Joints[jointNum]->GetPart(k)->Pos, ColPart);

				// collision - respond
				f32 r = Joints[jointNum]->GetPart(1)->radius*0.5f;
				if ( D <  r)
				{
					//Joints[jointNum]->GetPart(k)->OldPos = Joints[jointNum]->GetPart(k)->Pos;
					//Joints[i]->GetPart(0)->OldPos = Joints[i]->GetPart(0)->Pos;
					//Joints[i]->GetPart(1)->OldPos = Joints[i]->GetPart(1)->Pos;

					vector3df Perp = (Joints[jointNum]->GetPart(k)->Pos - ColPart);
					Perp.normalize();
					Joints[jointNum]->GetPart(k)->Pos += Perp*  (r - D) / 3;

					Perp = (Joints[i]->GetPart(0)->Pos - Joints[jointNum]->GetPart(k)->Pos);
					Perp.normalize();
					Joints[i]->GetPart(0)->Pos += Perp*  (r - D) / 3;

					Perp = (Joints[i]->GetPart(1)->Pos - Joints[jointNum]->GetPart(k)->Pos);
					Perp.normalize();
					Joints[i]->GetPart(1)->Pos += Perp*  (r - D) / 3;
				}
			}
	
}

vector3df CPhys_Skeleton::CalcCenter()
{
	s32 i;
	s32 s = Parts.size();
	vector3df o;

	if (s < 1) return o;

    for (i=0; i < s; i++)
	{
		o += Parts[i]->Pos;
	}

	o.X /= s;
	o.Y /= s;
	o.Z /= s;

	return o;
}

void CPhys_Skeleton::setSkeletonPosition( vector3df v )
{
	s32 i;
	s32 s = Parts.size();
	vector3df a;

	

	a = v - vCenter;

    for (i=0; i < s; i++)
	{
		Parts[i]->Move( Parts[i]->Pos + a);
	}

}