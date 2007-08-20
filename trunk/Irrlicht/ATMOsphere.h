// Copyright (C) 2002-2005 Nikolaus Gebhardt
// This file is part of the "Irrlicht Engine".
// For conditions of distribution and use, see copyright notice in irrlicht.h

#ifndef __ATMOSPHERE_H_INCLUDED__
#define __ATMOSPHERE_H_INCLUDED__

#include "../Engine/engine.h"
#include "../App/app.h"

class CATMOstarSceneNode;
class CATMOskySceneNode;
class CLensFlareSceneNode;

class ATMOsphere
{
      
struct PIXEL
{
   u8  B,G,R,A;
};


CATMOstarSceneNode* bill;
  u32 kin,ilgis;
  u32 i;
//u8 palete[256][3];
//u8 prspalva[3];
//u8 pbspalva[3];
//u8 vspalva[3];
float step[3];
float pspalva[3];
CATMOskySceneNode *Sky;
CATMOstarSceneNode *Sun;
video::IImage* image;
s32 skyid;
video::ITexture* dangus;
IrrlichtDevice* device;
IVideoDriver* driver;
scene::ISceneManager* smag;
double rad;//radian
u32 tex_size;//texture size
u32 half_tex;//half texture size
//--Timer----
ITimer *Atimer;//ATMOsphere timer
f32 currentTime, startTime, dTime;
//how many virtual day per real day. 1440 = day per 1 min.
//24*60=1440/dayspeed=1day-1min
f64 dayspeed; // how long goes day in minutes

u32 ptime,gtime;
f64 time_int_step;//sun place interpolation position (0-1)
f64 counter_time;//loop variable,count time from interpolation start in J
f64 J1minute;
//--Sun position---
double J;//julias date
double sun_angle[2];//sun pos in angles
f32 sun_interpolation_speed;//how minutes in virtual time takes interpolate sun from start to end position
double J1;
vector3df sun_pos_from,sun_pos_to;//interpolations points
f64 sun_angle_from,sun_angle_to;//sun height angle for interpolation
//--Sky texture----
//setSkyImage
video::IImage* skyimage;//sky texture
PIXEL *pixels;//palete
f32 uvX;
//---Date conversions
u16 Ndate[5];//stores date converted from Julian calendar
//-------------
CLensFlareSceneNode* lensflare;
ILightSceneNode* light;

public:
 ATMOsphere(int hour, int minute);
 ~ATMOsphere();
 vector3df getInterpolated3df(vector3df from,vector3df to, f32 d);
 void prep_interpolation(f64 Jdate, f64 time);
 void saule(f64 pl,f64 lw,f64 J);
 void setSkyImage(const char *filename);
 void CreateSkyPallete();
 void start(IrrlichtDevice* device2,video::IVideoDriver* Driver,scene::ISceneNode* parent, scene::ISceneManager* mgr, s32 id);
 void startTimer();
 void updateTimer();
 f64 getSunXAngle();
 f64 getSunYAngle();
 void setDaysPerDay(f64 days);
 f64 getDayspeed();
 void update(video::IVideoDriver* driver);
 wchar_t getTextDate();
 f64 DateToJulian(u16 y,u16 m,u16 d,u16 h,u16 min);
 void JulianToDate(f64 x);
 f32 round360(f32 angle);
 };

#endif

