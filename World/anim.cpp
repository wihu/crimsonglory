#include "anim.h"

////////////////////////////////////////////
// CAnimation_Frame 
////////////////////////////////////////////
CAnimation_Frame::~CAnimation_Frame()
{
	u32 i;

	for (i=0; i < Parts.size(); i++)
		delete Parts[i];
	Parts.clear();
}


////////////////////////////////////////////
// CAnimation 
////////////////////////////////////////////
bool CAnimation::Load(const c8* filename)
{
	s32 i, j;
	AnimHeader ah;
	AnimFrame af;
	AnimPart ap;

	FILE *pFile = fopen(filename, "rb");
    if (pFile==NULL) return false;

	Clear();

	fread(&ah, sizeof(AnimHeader), 1, pFile);

	for (i = 0; i < ah.framesNum; i++)
	{	
		fread(&af, sizeof(AnimFrame), 1, pFile);

		CAnimation_Frame* newFrame = new CAnimation_Frame();

		newFrame->SetRelativePart(af.relativePart, af.RelativePos);

		for (j = 0; j < ah.partsNum; j++)
		{
			fread(&ap, sizeof(AnimPart), 1, pFile);

			newFrame->AddPartPos(ap.Pos);
		}

		AddFrame(newFrame);
	}
	
	fclose(pFile);

	return true;
}

bool CAnimation::Save(const c8* filename)
{
	if (GetFramesNum() < 1)
		return false;

	s32 i, j;
	AnimHeader ah;
	AnimFrame af;
	AnimPart ap;

	FILE *pFile = fopen(filename,"wb");

	ah.version = ANIMFILE_VERSION;
	ah.framesNum = GetFramesNum();
	ah.partsNum = Frames[0]->GetPartsNum();

	fwrite(&ah, sizeof(AnimHeader), 1, pFile);

	for (i = 0; i < ah.framesNum; i++)
	{	
		af.relativePart = Frames[i]->GetRelativePartNum();
		af.RelativePos = Frames[i]->GetRelativePart();

		fwrite(&af, sizeof(AnimFrame), 1, pFile);

		for (j = 0; j < ah.partsNum; j++)
		{
			ap.Pos.set(Frames[i]->GetPartPos(j));

			fwrite(&ap, sizeof(AnimPart), 1, pFile);
		}
	}
	
	fclose(pFile);

	return true;
}

void CAnimation::Clear()
{
	u32 i;

	for (i=0; i < Frames.size(); i++)
		delete Frames[i];
	Frames.clear();
}

CAnimation::~CAnimation()
{
	Clear();
}

void CAnimation::SetPartInCollision(s32 i)
{
	if (inCollisionPart.size() < 1)
	{
		// create the array
		for (s32 i = 0; i < GetFrame(0)->GetPartsNum(); i++)
		{
			inCollisionPart.push_back(false);
		}
    }

	inCollisionPart[i] = true;
}

bool CAnimation::PartInCollision(s32 i)
{
	if (inCollisionPart.size() < 1)
	{
		return false;
    }

	return inCollisionPart[i];
}


////////////////////////////////////////////
// CCharacter 
////////////////////////////////////////////
CCharacter::CCharacter()
{
	Reset();

	RelativePart = new CPhys_Part(vector3df(0,0,0), 1, 1);
}

CCharacter::CCharacter(vector3df CenterPos, f32 mass, f32 radius)
{
	RelativePart = new CPhys_Part(CenterPos, mass, radius);
}


CCharacter::~CCharacter()
{
	u32 i;

	//for (i=0; i < Models.size(); i++)
	//	delete Models[i];
	//Models.clear();

	DrawList.clear();

	//delete RelativePart;
	RelativePart->Kill();

	for (i=0; i < Animations.size(); i++)
		delete Animations[i];
	Animations.clear();
}


void CCharacter::Think()
{
	DoAnimation();

	// Apply frame action force
	//if (currentAnim)
	//{
	//	//if (RelativePart->collided)
	//	if (speedTick == 1)
	//	{
	//		RelativePart->ApplyForce(currentAnim->GetFrame(currentFrame)->GetActionForce()*orient);
	//	}
	//}

	//RelativePart->Think();

	CPhys_Skeleton::Think();

	UpdateModels();
}


void CCharacter::ApplyAnimation(CAnimation* newAnim)
{
	// if ground anim and starts in air don't change
	//if ( (!RelativePart->collided)&&(newAnim->GetPlay() == PLAY_GROUND) )
	//	return;

	if (newAnim == currentAnim) return;

	currentFrame = 0;
	currentAnim = newAnim;
	animChanged = true;
	power = ANIM_CHANGEPOWER;
}

void CCharacter::UpdateRelativeAnimation()
{
	u32 i;
	s32 s = Parts.size();

	if (!currentAnim)
	{
		CONSOLE.addx("ERROR: currentAnim = NULL");
		return;
	}

	if ( (currentFrame < 0)||(currentFrame > currentAnim->GetFramesNum()-1) )
	{
		CONSOLE.addx("ERROR: currentFrame out of bounds %i > %i", currentFrame, currentAnim->GetFramesNum());
		return;
	}
	if (s != currentAnim->GetFrame(currentFrame)->GetPartsNum())
	{
		CONSOLE.addx("ERROR: character animation mismatch Parts.size=%i GetFrame(currentFrame)->GetPartsNum()=%i ",
			s, currentAnim->GetFrame(currentFrame)->GetPartsNum());
		return;
	}

	//vector3df oldCenter = CalcCenter();
	vector3df oldCenter = CalcCenter();

	if ( !animChanged )
		oldRelativePart = relativePart;
	relativePart = currentAnim->GetFrame(currentFrame)->GetRelativePartNum();

	// change relative pos 
	if ( (oldRelativePart != relativePart) && (!animChanged) )
	//if ( (oldRelativePart != relativePart) )
	{
		vector3df relVector = 
			currentAnim->GetFrame(currentFrame)->GetPartPos(relativePart) -
			currentAnim->GetFrame(currentFrame)->GetRelativePart();

		RelativePart->Move(Parts[relativePart]->Pos + relVector);
		RelativePart->OldPos = Parts[relativePart]->OldPos + relVector;

		RelativePart->oneOverMass = Parts[relativePart]->oneOverMass/2;
		RelativePart->radius = Parts[relativePart]->radius;
	}

	// change anim angle
	AnimateAngle();

	vector3df t, p, n, t2;

	//printf("f: %i \n", forcePositionFrames);

    //Loop through Parts - animation with interpolation!
    for (i=0; i < s; i++)
	//if (!Parts[i]->collided)
		if ( (forcePositionFrames > 0)&&(i == forcePositionPartNum) )
		{
			t = ((forcePositionNextPos
				- forcePositionPrevPos)/(f32)ANIMHITSPEED)*(f32)(forcePositionFrames);

			p = forcePositionNextPos - t;

			Parts[i]->OldPos = Parts[i]->Pos;
			Parts[i]->Pos = p;

			power = ANIM_CHANGEPOWER;
		}
		else
		{
			if (currentAnim->GetFrame(currentFrame)->GetRelativePartNum() != 
				currentAnim->GetFrame(nextFrame)->GetRelativePartNum())
				t2 = currentAnim->GetFrame(nextFrame)->GetPartPos(relativePart);
			else
				t2 = currentAnim->GetFrame(nextFrame)->GetRelativePart();

			t = currentAnim->GetFrame(currentFrame)->GetPartPos(i) 
				+ ((currentAnim->GetFrame(nextFrame)->GetPartPos(i)
				- currentAnim->GetFrame(currentFrame)->GetPartPos(i))/(f32)currentAnim->GetSpeed())*(f32)speedTick
				- (currentAnim->GetFrame(currentFrame)->GetRelativePart()
				+ ((t2
				- currentAnim->GetFrame(currentFrame)->GetRelativePart())/(f32)currentAnim->GetSpeed())*(f32)speedTick
				);

			t *= orient;
			n = (RelativePart->Pos + t);
			n.rotateXYBy((f32)angle, RelativePart->Pos);

			p = n - Parts[i]->Pos;
			p *= power;

			Parts[i]->OldPos = Parts[i]->Pos;
			Parts[i]->Pos += p;

		}

	vector3df newCenter = CalcCenter();

	// center skeleton on Z = 0
	t = RelativePart->Pos - vector3df(0, 0, newCenter.Z);
	RelativePart->Move(t);

	// move relativepart on animchange according to center of mass change
	// FUCKED UP BEYOND RECOGNITION!!!
	if (animChanged)
	{
		//t = RelativePart->OldPos - (newCenter - oldCenter);
		//RelativePart->Move(t);

		//printf("f: %f \n", t.X);

		// HACK!
		//RelativePart->Think();
		//RelativePart->Stop();
	}

	animChanged = false;
}

void CCharacter::UpdateAnimation()
{
	u32 i;
	s32 s = Parts.size();

	if (!currentAnim)
	{
		CONSOLE.addx("ERROR: currentAnim = NULL");
		return;
	}

	if ( (currentFrame < 0)||(currentFrame > currentAnim->GetFramesNum()-1) )
	{
		CONSOLE.addx("ERROR: currentFrame out of bounds %i > %i", currentFrame, currentAnim->GetFramesNum());
		return;
	}
	if (s != currentAnim->GetFrame(currentFrame)->GetPartsNum())
	{
		CONSOLE.addx("ERROR: character animation mismatch Parts.size=%i GetFrame(currentFrame)->GetPartsNum()=%i ",
			s, currentAnim->GetFrame(currentFrame)->GetPartsNum());
		return;
	}

	relativePart = currentAnim->GetFrame(currentFrame)->GetRelativePartNum();

	if (PartExists(relativePart))
		RelativePart->OldPos = RelativePart->Pos = 
			currentAnim->GetFrame(currentFrame)->GetRelativePart();

    //Loop through Parts
    for (i=0; i < s; i++)
	{
		Parts[i]->Pos = currentAnim->GetFrame(currentFrame)->GetPartPos(i);
		Parts[i]->OldPos = Parts[i]->Pos;
	}
}

void CCharacter::UpdateModels()
{
	u32 i;

	//Loop through models
	//vector3df JointCenter;
	//for (i=0;i<Models.size();i++)
	//{
	//	JointCenter = GetPart(Models[i]->GetPart1())->Pos +
	//				(GetPart(Models[i]->GetPart2())->Pos - GetPart(Models[i]->GetPart1())->Pos)*0.5;
	//	Models[i]->getNode()->setPosition(JointCenter);
	//	Models[i]->getNode()->setRotation(getTargetAngle(GetPart(Models[i]->GetPart1())->Pos, GetPart(Models[i]->GetPart2())->Pos));
	//}
}


void CCharacter::CalcNextFrame(s32 &index, bool makeLoop)
{
	index++;

	// end frame
	if (index > currentAnim->GetFramesNum()-1)
	{
		if (makeLoop)
			index = 0;
		else
			index--;

//		power = ANIM_CHANGERELPOWER;
	}
}

void CCharacter::DoAnimation()
{
	// force position timer
	if (forcePositionFrames > 0)
	{
		forcePositionFrames--;
	}

	if (!currentAnim)
	{
		if (PartExists(relativePart))
			RelativePart->Move(Parts[relativePart]->Pos);
		return;
	}

	bool proceed = true;
	bool makeLoop = true;
	AnimPlay play = currentAnim->GetPlay();
	AnimLoop loop = currentAnim->GetLoop();
	
	// decide if not playing animation - and if looping
	if (loop == LOOP_NO)
		makeLoop = false;

	if (PartExists(relativePart))
	{
		if (speedTick == 0)
		{
			if ( (play == PLAY_GROUND) && (!RelativePart->collided) )
				proceed = false;
			if ( (play == PLAY_AIR) && (RelativePart->collided) )
				proceed = false;  
		}

		if ( (loop == LOOP_AIR) && (RelativePart->collided) )
			makeLoop = false;
		if ( (loop == LOOP_GROUND) && (!RelativePart->collided) )
			makeLoop = false;

	}

	//for (s32 i=0; i < Parts.size(); i++)
	//	if ( currentAnim->PartInCollision(i) && (GetPart(i)->collided) )
	//		proceed = false;

	if (proceed)
		speedTick++;

	if (speedTick > currentAnim->GetSpeed()-1)
	{
		power = ANIM_POWER;

		speedTick = 0;
		CalcNextFrame(currentFrame, makeLoop);
	}

	// calc nextframe
	nextFrame = currentFrame;
	CalcNextFrame(nextFrame, makeLoop);

	//if (PartExists(relativePart))
		UpdateRelativeAnimation();
	//else
	//	UpdateAnimation();
}

void CCharacter::Reset()
{
	CPhys_Skeleton::Reset();

	currentAnim = NULL;
	speedTick = 0;
	animChanged = false;
	orient.set(VECTOR_ANIM_RIGHT);
	power = ANIM_POWER;
	targetAngle = angle = 0;
	angleChangeRate = ANIM_ANGLECHANGE;
	bInvalidCharacter = false;
}

void CCharacter::AnimateAngle()
{
	//if ( (angle < 180)&&(targetAngle < 180) )
	//{
	//	if (targetAngle > angle)
	//		angle += angleChangeRate;
	//	if (targetAngle < angle)
	//		angle -= angleChangeRate;
	//}
	//if ( (angle < 180)&&(targetAngle > 180) )
	//{
	//	if (targetAngle > angle)
	//		angle -= angleChangeRate;
	//	if (targetAngle < angle)
	//		angle += angleChangeRate;
	//}
	//if ( (angle > 180)&&(targetAngle < 180) )
	//{
	//	if (targetAngle > angle)
	//		angle -= angleChangeRate;
	//	if (targetAngle < angle)
	//		angle += angleChangeRate;
	//}
	//if ( (angle > 180)&&(targetAngle > 180) )
	//{
	//	if (targetAngle > angle)
	//		angle += angleChangeRate;
	//	if (targetAngle < angle)
	//		angle -= angleChangeRate;
	//}

	//if (angle < 0.0f)
	//	angle = 360.0f;
	//if (angle > 360.0f)
	//	angle = 0.0f;

	angle = targetAngle;

	//printf("angle %f, target %f \n\n", angle, targetAngle);
}

void CCharacter::Render()
{
	u32 i, j;

	IRR.video->setTransform(ETS_WORLD, matrix4());

	SMaterial m; 
	m.Lighting = false; 
	m.BackfaceCulling = false;
	IRR.video->setMaterial(m);

    if ( PartExists(GetRelativePartNum()) )
	{
		vector3df Pos = GetPart(GetRelativePartNum())->Pos;
		IRR.video->draw3DLine(Pos-vector3df(0.9,0,0), Pos+vector3df(0.9,0,0), SColor(0,255,255,100));			
		IRR.video->draw3DLine(Pos-vector3df(0,0.9,0), Pos+vector3df(0,0.9,0), SColor(0,255,255,100));			
		IRR.video->draw3DLine(Pos-vector3df(0,0,0.9), Pos+vector3df(0,0,0.9), SColor(0,255,255,100));	
	}

	//triangle3df t;
	if ( (DrawList.size() > 2) )
	{
		const s32 s = 99; 
		S3DVertex Vertices[s];
		u16 indices[s];

		for (i=0;i<DrawList.size();i+=3)
			if ( (PartExists(DrawList[i]))&&(PartExists(DrawList[i+1]))&&(PartExists(DrawList[i+2])) )
			{
				Vertices[i] = S3DVertex(GetPart(DrawList[i])->Pos.X,GetPart(DrawList[i])->Pos.Y,GetPart(DrawList[i])->Pos.Z, 1,1,0,video::SColor(255,233,245,245),0,1);
				Vertices[i+1] = S3DVertex(GetPart(DrawList[i+1])->Pos.X,GetPart(DrawList[i+1])->Pos.Y,GetPart(DrawList[i+1])->Pos.Z, 1,1,0,video::SColor(255,233,245,245),0,1);
				Vertices[i+2] = S3DVertex(GetPart(DrawList[i+2])->Pos.X,GetPart(DrawList[i+2])->Pos.Y,GetPart(DrawList[i+2])->Pos.Z, 1,1,0,video::SColor(255,233,245,245),0,1);

				indices[i] = i;
				indices[i+1] = i+1;
				indices[i+2] = i+2;
			}

		IRR.video->drawIndexedTriangleList(&Vertices[0], s, &indices[0], s/3);
	}

}

void CCharacter::Load(const c8* filename)
{
	s32 i;
	CharHeader ch;
	CharPart cp;
	CharJoint cj;
	CharPlane cpl;
	CharDraw cd;
	//CharModel cm;

	FILE *pFile = fopen(filename, "rb");
    if (pFile==NULL) return;

	fread(&ch, sizeof(CharHeader), 1, pFile);

	for (i = 0; i < ch.partsNum; i++)
	{
		fread(&cp, sizeof(CharPart), 1, pFile);

		AddPart(vector3df(cp.x, cp.y, cp.z), 1/cp.oneOverMass, cp.radius);
	}

	for (i = 0; i < ch.jointsNum; i++)
	{
		fread(&cj, sizeof(CharJoint), 1, pFile);
		
		if ((cj.spring1 < 1.0f)&&(cj.spring1 > 0.0f) )
			cj.spring1 = JOINT_CLOTH;
		if ((cj.spring2 < 1.0f)&&(cj.spring2 > 0.0f) )
			cj.spring2 = JOINT_CLOTH;

		ConnectParts(GetPart(cj.part1), GetPart(cj.part2), cj.spring1, cj.spring2);
	}

	for (i = 0; i < ch.planesNum; i++)
	{
		fread(&cpl, sizeof(CharPlane), 1, pFile);

		CPhys_PlaneConstraint* newPlane = new CPhys_PlaneConstraint( cpl.spring1, cpl.spring2, cpl.part3, cpl.part4,
			GetPart(cpl.part1), GetPart(cpl.part2), GetPart(cpl.part3), GetPart(cpl.part4), cpl.orient );

		AddPlaneConstraint(newPlane);		
	}

	for (i = 0; i < ch.drawNum; i++)
	{
		fread(&cd, sizeof(CharDraw), 1, pFile);

		AddDrawListPart(cd.num);
	}

	//for (i = 0; i < ch.modelsNum; i++)
	//{
	//	fread(&cm, sizeof(CharModel), 1, pFile);

	//	AddModel(LoadModelJoint(cm.part1, cm.part2, vector3df(cm.sx, cm.sy, cm.sz), cm.mname, cm.tname));

	//	ModelNames.push_back(cm.mname);
	//	TextureNames.push_back(cm.tname);
	//}

	fclose(pFile);
}

void CCharacter::Save(const c8* filename)
{
	s32 i, j;
	CharHeader ch;
	CharPart cp;
	CharJoint cj;
	CharPlane cpl;
	CharDraw cd;
//	CharModel cm;

	FILE *pFile = fopen(filename,"wb");

	ch.version = FILE_VERSION;
	ch.partsNum = GetPartsNum();
	ch.jointsNum = GetJointsNum();
	ch.planesNum = GetPlanesNum();
	ch.drawNum = GetDrawListNum();
//	ch.modelsNum = GetModelNum();

	fwrite(&ch, sizeof(CharHeader), 1, pFile);

	for (i = 0; i < ch.partsNum; i++)
	{
		cp.x = GetPart(i)->Pos.X;
		cp.y = GetPart(i)->Pos.Y;
		cp.z = GetPart(i)->Pos.Z;
		cp.radius = GetPart(i)->radius;
		cp.oneOverMass = GetPart(i)->oneOverMass;		

		fwrite(&cp, sizeof(CharPart), 1, pFile);
	}
	
	for (i = 0; i < ch.jointsNum; i++)
	{
		cj.part1 = GetPartIndex(GetJoint(i)->GetPart(0));
		cj.part2 = GetPartIndex(GetJoint(i)->GetPart(1));	
		cj.spring1 = GetJoint(i)->GetSpring(0);	
		cj.spring2 = GetJoint(i)->GetSpring(1);
		cj.length = GetJoint(i)->GetLength();
		cj.diff = GetJoint(i)->GetDiff();

		fwrite(&cj, sizeof(CharJoint), 1, pFile);
	}

	for (i = 0; i < ch.planesNum; i++)
	{
		cpl.part1 = GetPartIndex(GetPlane(i)->GetPart(0));
		cpl.part2 = GetPartIndex(GetPlane(i)->GetPart(1));
		cpl.part3 = GetPartIndex(GetPlane(i)->GetPlanePart(0));
		cpl.part4 = GetPartIndex(GetPlane(i)->GetPlanePart(1));	
		cpl.spring1 = GetPlane(i)->GetSpring(0);	
		cpl.spring2 = GetPlane(i)->GetSpring(1);
		cpl.orient = GetPlane(i)->GetOrient();

		fwrite(&cpl, sizeof(CharPlane), 1, pFile);
	}
	
	for (i = 0; i < ch.drawNum; i++)
	{
		cd.num = GetDrawNum(i);

		fwrite(&cd, sizeof(CharDraw), 1, pFile);
	}
	
	//for (i = 0; i < ch.modelsNum; i++)
	//{
	//	cm.part1 = GetModel(i)->GetPart1();
	//	cm.part2 = GetModel(i)->GetPart2();
	//	cm.sx = GetModel(i)->GetWidth();
	//	cm.sy = GetModel(i)->GetHeight();
	//	cm.sz = GetModel(i)->GetDepth();

	//	for (j = 0; j < 255; j++)
	//		cm.mname[j] = 0;
	//	for (j = 0; j < 255; j++)
	//		cm.tname[j] = 0;

	//	for (j = 0; j < ModelNames[i].size(); j++)
	//		cm.mname[j] = ModelNames[i].c_str()[j];

	//	for (j = 0; j < TextureNames[i].size(); j++)
	//		cm.tname[j] = TextureNames[i].c_str()[j];

	//	fwrite(&cm, sizeof(CharModel), 1, pFile);
	//}
	//
	fclose(pFile);
}

bool CCharacter::CheckInCollisionPartCollided()
{
	if (!currentAnim)
		return false;

	s32 i;

    for (i=0; i < GetPartsNum(); i++)
	{
		if ( (currentAnim->PartInCollision(i))&&(Parts[i]->collided) )
			return true;
	}

	return false;
}

void CCharacter::ForcePosition(s32 partNum, s32 frames, vector3df Pos)
{
	//if (forcePositionFrames > 0) 
	//{
	//	forcePositionFrames = frames;
	//	forcePositionPrevPos = forcePositionNextPos;
	//	return
	//}

	forcePositionPartNum = partNum; 

	forcePositionPrevPos = Parts[partNum]->Pos;

	forcePositionFrames = frames; 

	vector3df n = (Pos - CalcCenter());

	if (n.getLength() > HIT_RADIUS)
		n = n.normalize()*HIT_RADIUS;

	forcePositionNextPos = CalcCenter() + n; 

	//forcePositionNextPos.Z = Parts[partNum]->Pos.Z;
	//forcePositionNextPos.Y = 28.0f;
}