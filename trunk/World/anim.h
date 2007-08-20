#ifndef ANIM_H
#define ANIM_H

#include "skeleton.h"

#define VECTOR_ANIM_RIGHT vector3df(1.0f, 1.0f, 1.0f)
#define VECTOR_ANIM_LEFT vector3df(-1.0f, 1.0f, -1.0f)

#define ANIM_POWER 0.5f
#define ANIM_CHANGEPOWER 0.5f
#define ANIM_ANGLECHANGE 1.0f

#define ANIMSPEED 45
#define ANIMHITSPEED 20

#define HIT_RADIUS 9.0f

enum AnimPlay { PLAY_ALWAYS, PLAY_GROUND, PLAY_AIR };
enum AnimLoop { LOOP_NO, LOOP_YES, LOOP_AIR, LOOP_GROUND };


////////////////////////////////////////////
// CAnimation_Frame 
////////////////////////////////////////////
class CAnimation_Frame
{
public:
	CAnimation_Frame::~CAnimation_Frame();

	s32 GetPartsNum()					{ return Parts.size(); }
	vector3df GetPartPos(s32 i)			{ vector3df v; v.X = Parts[i]->X; v.Y = Parts[i]->Y; v.Z = Parts[i]->Z; return v; }
	vector3df* GetPart(s32 i)			{ return Parts[i]; }
	void AddPartPos(vector3df Pos)		{ vector3df* PPos = new vector3df(Pos); Parts.push_back(PPos); }

	s32 GetRelativePartNum()			{ return relativePart; }
	vector3df GetRelativePart()			{ return RelativePartPos; }
	void SetRelativePart(s32 part, vector3df partPos)		
										{ relativePart = part; RelativePartPos = partPos; }

	void SetActionForce(vector3df f)	{ ActionForce.set(f); }
	vector3df GetActionForce()			{ return ActionForce; }

private:
	array<vector3df*> Parts;
	s32 relativePart;
	vector3df RelativePartPos;

	vector3df ActionForce;
};



////////////////////////////////////////////
// CAnimation 
////////////////////////////////////////////
class CAnimation
{
public:
	CAnimation::CAnimation(s32 newSpeed, AnimPlay newPlay, AnimLoop newLoop)
	{
		speed = newSpeed;
		play = newPlay;
		loop = newLoop;
	}
	CAnimation::~CAnimation();

	s32 GetFramesNum()										{ return Frames.size(); }
	CAnimation_Frame* GetFrame(s32 i)						{ return Frames[i]; } //{ if ( FrameExists(i) )	return Frames[i]; else return NULL; }
	bool FrameExists(s32 i)									{ if ((i<0)||(i>(s32)Frames.size()-1)) return false; else return true;  }

	void AddFrame(CAnimation_Frame* Frame)					{ Frame->SetActionForce(vector3df(0,0,0)); Frames.push_back(Frame); }
	void InsertFrame(CAnimation_Frame* Frame, s32 place)	{ Frames.insert(Frame, place); }
	void DeleteFrame(s32 place)								{ delete Frames[place]; Frames.erase(place); }

	s32 GetSpeed()											{ return speed; }
	void SetSpeed(s32 i)									{ speed = i; }
	AnimPlay GetPlay()										{ return play; }
	AnimLoop GetLoop()										{ return loop; }

	void SetPartInCollision(s32 i);
	bool PartInCollision(s32 i);

	void Clear();

	bool Load(const c8* filename);
	bool Save(const c8* filename);

private:
	array<CAnimation_Frame*> Frames;

	s32 speed;
	AnimPlay play;
	AnimLoop loop;

	array<bool> inCollisionPart;
};



////////////////////////////////////////////
// CCharacter 
////////////////////////////////////////////
class CCharacter: public CPhys_Skeleton
{
public:
	CCharacter();
	CCharacter(vector3df CenterPos, f32 mass, f32 radius);

	~CCharacter();

	virtual void Think();
	virtual void Reset();

	// animations //

	void ApplyAnimation(CAnimation* newAnim);
	void NoAnimation()						{ currentAnim = NULL; animChanged = true; }
	
	void UpdateRelativeAnimation();
	void UpdateAnimation();
	void CalcNextFrame(s32 &index, bool makeLoop);
	void DoAnimation();

	s32 GetCurrentFrame()						{ return currentFrame; }
	void SetFrameIndex(s32 i)				{ currentFrame = i; UpdateAnimation(); }

	CPhys_Part* GetRelativePart()			{ return RelativePart; }
	s32 GetRelativePartNum()				{ return relativePart; }
	void SetRelativePart(s32 part)			{ relativePart = part; 	if (PartExists(relativePart)) RelativePart->OldPos = RelativePart->Pos = Parts[relativePart]->Pos; }

	vector3df GetOrientVector()				{ return orient; }
	void SetOrientVector(vector3df o)		{ orient.set(o); }
	f32 GetAnimPower()						{ return power; }
	void SetAnimPower(f32 p)				{ power = p; }
	f32 GetAnimAngle()						{ return angle; }
	void SetAnimAngle(f32 a)				{ targetAngle = a; }

	void ForcePosition(s32 partNum, s32 frames, vector3df Pos);
	s32 GetForcePositionPartNum()			{ if (forcePositionFrames > 0) return forcePositionPartNum; return -1;}
	vector3df GetForcePositionStart()		{ return forcePositionPrevPos;}

	CAnimation* currentAnim;

	// visuals //

	virtual void Render();

	void AddDrawListPart(s32 i)				{ DrawList.push_back(i); }
	//void AddModel(IrrBox* m)				{ Models.push_back(m); }

	//IrrBox* GetModel(s32 i)					{ if (ModelExists(i)) return Models[i]; else return NULL; }
	//s32 GetModelNum()						{ return Models.size(); }
	//bool ModelExists(s32 i)					{ if ((i<0)||(i>(s32)Models.size()-1)) return false; else return true;  }

	s32 GetDrawListNum()					{ return DrawList.size(); }
	s32 GetDrawNum(s32 i)					{ return DrawList[i]; }

	// file operations

	virtual void Load(const c8* filename);
	void Save(const c8* filename);

	// other

	// check if any of the inCollision parts has collided
	bool CheckInCollisionPartCollided();

	bool CheckRelativePartCollided()		{ if (RelativePart->collided) return true; return false;}

	// if loaded character is invalid don't do think
	bool ValidCharacter()					{ return !bInvalidCharacter; }

protected:
	bool bInvalidCharacter;

	array<CAnimation*> Animations;

private:
	// animations //

	void UpdateModels();
	void AnimateAngle();

	s32 currentFrame, nextFrame;
	s32 speedTick;

	CPhys_Part* RelativePart;
	s32 relativePart, oldRelativePart;

	vector3df orient;
	f32 power;
	f32 angle, targetAngle, angleChangeRate;

	// for UpdateRelativeAnimation(), to center the skeleton correctly after animchange
	bool animChanged;

	// force position of a part during given number of frames
    s32 forcePositionPartNum;
	s32 forcePositionFrames;
	vector3df forcePositionNextPos, forcePositionPrevPos;


	// visuals //

	array<s32> DrawList;
	//array<IrrBox*> Models;
};





// Character File Structures
#define FILE_VERSION 2

struct CharHeader
{
	s32 version;
    s32 partsNum, jointsNum, planesNum, drawNum, modelsNum;

	//s32 dummy1, dummy2, dummy3, dummy4, dummy5, dummy6, dummy7, dummy8;
};

struct CharPart
{
    f32 x, y, z;
    f32 radius, oneOverMass;

    //s32 dummy1, dummy2, dummy3, dummy4, dummy5, dummy6, dummy7, dummy8;
};

struct CharJoint
{
    f32 length, spring1, spring2, diff;
    s32 part1, part2;

   // s32 dummy1, dummy2, dummy3, dummy4, dummy5, dummy6, dummy7, dummy8;
};

struct CharPlane
{
    f32 spring1, spring2, orient;
    s32 part1, part2, part3, part4;

   // s32 dummy1, dummy2, dummy3, dummy4, dummy5, dummy6, dummy7, dummy8;
};

struct CharDraw
{
    s32 num;

   // s32 dummy1, dummy2, dummy3, dummy4, dummy5, dummy6, dummy7, dummy8;
};

struct CharModel
{
    s32 part1, part2;
	f32 sx, sy, sz;
	c8 mname[255];
	c8 tname[255];

  //  s32 dummy1, dummy2, dummy3, dummy4, dummy5, dummy6, dummy7, dummy8;
};


// Animation File Structures
#define ANIMFILE_VERSION 1

struct AnimHeader
{
	s32 version;
    s32 framesNum, partsNum;

	//s32 dummy1, dummy2, dummy3, dummy4, dummy5, dummy6, dummy7, dummy8;
};

struct AnimFrame
{
	s32 relativePart;
	vector3df RelativePos;

	//s32 dummy1, dummy2, dummy3, dummy4, dummy5, dummy6, dummy7, dummy8;
};
struct AnimFrame_Old
{
	s32 relativePart;
};

struct AnimPart
{
	vector3df Pos;

	//s32 dummy1, dummy2, dummy3, dummy4, dummy5, dummy6, dummy7, dummy8;
};


#endif