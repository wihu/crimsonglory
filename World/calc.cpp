#include "calc.h"

f32 DistancePointPlane( vector3df Plane, vector3df PlaneNormal, vector3df Point )
{
    f32 PlaneD = Plane.X* PlaneNormal.X + Plane.Y* PlaneNormal.Y + Plane.Z* PlaneNormal.Z; 
    return ( Plane.dotProduct( Point ) - PlaneD );
}

// http://www.sccs.swarthmore.edu/users/00/ceg/computer/3d.html
f32 Distance_Line_Point( vector3df P1, vector3df P2, vector3df Center, vector3df& O )
{
    vector3df P;
    f32 u, d;

    u = ( ( Center.X - P1.X ) * ( P2.X - P1.X ) + ( Center.Y - P1.Y ) * ( P2.Y - P1.Y ) + ( Center.Z - P1.Z ) * ( P2.Z - P1.Z ) ) / ( ( P2.X - P1.X ) * ( P2.X - P1.X ) + ( P2.Y - P1.Y ) * ( P2.Y - P1.Y ) + ( P2.Z - P1.Z ) * ( P2.Z - P1.Z ) );
    if ( u < 0 )
    {
      P = P1;
    }
    else if ( u > 1 )
    {
      P = P2;
    }
    else
    {
      P = P1 + ( ( P2 - P1 ) * u );
    }

    d = ( f32 )( P - Center ).getLength();
    O = P;
    return d;
}

vector3df getTargetAngle( vector3df v, vector3df r )
{
    //v -current node position 
    //r -target node position 

    vector3df angle; 
    float x, y, z; 
    x = r.X - v.X; 
    y = r.Y - v.Y; 
    z = r.Z - v.Z; 

    //angle in X-Z plane 
    angle.Y = atan2( x, z ); 
    angle.Y *= ( 180 / PI ); //converting from rad to degrees 

    //just making sure angle is somewhere between 0-360 degrees 
    if ( angle.Y < -180 )
    {
      angle.Y += 360;
    } 
    if ( angle.Y >= 180 )
    {
      angle.Y -= 360;
    } 

    //angle in Y-Z plane while Z and X axes are already rotated around Y 
    float z1 = sqrt( x* x + z* z ); 

    angle.X = atan2( z1, y ); 
    angle.X *= ( 180 / PI ); //converting from rad to degrees 
    angle.X -= 90; 

    //just making sure angle is somewhere between 0-360 degrees 
    if ( angle.X < -180 )
    {
      angle.X += 360;
    } 
    if ( angle.X >= 180 )
    {
      angle.X -= 360;
    } 

    return angle;
}

/*

   angle.Y = atan2 (x, z); 
   angle.Y *= (180 / PI); //converting from rad to degrees 
    
   //just making sure angle is somewhere between 0-360 degrees 
   if(angle.Y < 0) angle.Y += 360; 
   if(angle.Y >= 360) angle.Y -= 360; 
    
   //angle in Y-Z plane while Z and X axes are already rotated around Y 
   float z1 = sqrt(x*x + z*z); 
    
   angle.X = atan2 (z1, y); 
   angle.X *= (180 / PI); //converting from rad to degrees 
   angle.X -= 90; 
    
   //just making sure angle is somewhere between 0-360 degrees 
   if(angle.X < 0) angle.X += 360; 
   if(angle.X >= 360) angle.X -= 360; 

   return angle; 
}

float v3_angle_between2 (vector3 v1,
                         vector3 v2) {
    float value;
    v1 = v3_normalize(v1);
    v2 = v3_normalize(v2);
    if (v3_dot_product(v1, v2) < 0) {
       value = v3_length(v3_add(v1, v2)) / 2.0;
       if (value > 1) value = 1;
       return M_PI - 2.0*asin(value);
    }
    value = v3_length(v3_subtract(v1, v2)) / 2.0;
    if (value > 1) value = 1;
    return 2.0*asin(value);
}
*/