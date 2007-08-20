#ifndef CALC_H
#define CALC_H

#include "../Game/stdafx.h"
#include "../Engine/engine.h"

extern f32 DistancePointPlane( vector3df Plane, vector3df PlaneNormal, vector3df Point );

extern f32 Distance_Line_Point( vector3df P1, vector3df P2, vector3df Center, vector3df& O );

extern vector3df getTargetAngle( vector3df v, vector3df r );

#endif