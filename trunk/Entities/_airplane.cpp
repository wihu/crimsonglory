//#include "airplane.h"
//#include "../IrrConsole/console_vars.h"
//
//#define WING_FORCEDIV 5
//#define TAILWING_FORCEDIV 16
//#define LIFT_FORCEDIV 9
//#define TAILVERT_FORCEDIV 90
//#define TAIL_WING_PROPORTION TAILWING_FORCEDIV/WING_FORCEDIV
//#define WING_RESISTANCEDIV 90
//#define TAILWING_RESISTANCEDIV 288
//#define LIFT_RESISTANCEDIV 1
////#define PLANE_MAXSPEED 5.7f
//#define ENGINE_POWER 200
//
////////////////////////////////
//// CAirplane
////////////////////////////////
//
//CAirplane::CAirplane()
//{
//	Reset();
//	debugRender = new CAirplaneDebugRender();
//
//	CAnimation* newAnim;
//
//	newAnim = new CAnimation(16, PLAY_ALWAYS, LOOP_NO);
//	if ( !newAnim->Load("Chars/Anims/newanim.jan") )
//		CONSOLE.addx(COLOR_ERROR, "Could not load animation file: %s", "Chars/Anims/newanim.jin");
//	Animations.push_back(newAnim); // ANIM_AIRPLANE_ROLL
//
////	ApplyAnimation( Animations[ANIM_AIRPLANE_ROLL] );
//	flipstate = false;
//}
//
//CAirplane::~CAirplane()
//{
//	debugRender->Kill();
//}
//
//void CAirplane::Reset()
//{
//	CControllableCharacter::Reset();
//	FixZAxis( true );
//}
//
//void CAirplane::Think()
//{
//	// align to 2d
//	GetPart(2)->OldPos.Z = GetPart(2)->Pos.Z = 0.0f;
//	GetPart(3)->OldPos.Z = GetPart(3)->Pos.Z = 0.0f;
//
//	// calc main plane axis
//	vEngine = (GetPart(0)->Pos + GetPart(1)->Pos + GetPart(2)->Pos) / 3;
//	vMainAxis =  vEngine - GetPart(3)->Pos;
//	vPerpMainAxis.X = vMainAxis.Y;
//	vPerpMainAxis.Y = -vMainAxis.X;
//
//	vector3df vNMainAxis = vMainAxis;
//	vector3df VNPerpMainAxis = vPerpMainAxis;
//	vNMainAxis.normalize();
//	VNPerpMainAxis.normalize();
//
//	// wings lift
//	f32 fLeftWingSpeed = ( (GetPart(4)->GetVelocity() * vNMainAxis).getLength() );
//	f32 fRightWingSpeed = ( (GetPart(5)->GetVelocity() * vNMainAxis).getLength() );
//	f32 fTailWingSpeed = ( (GetPart(3)->GetVelocity() * vNMainAxis).getLength() );
//	vLeftWing = ( VNPerpMainAxis * fLeftWingSpeed / fwingforcefiv ) / fliftforcediv;
//	vRightWing = ( VNPerpMainAxis * fRightWingSpeed / fwingforcefiv ) / fliftforcediv;
//	vTailWing = ( VNPerpMainAxis * fTailWingSpeed / ftailwingforcediv ) / fliftforcediv;
//	GetPart(3)->ApplyForce( vTailWing );
//	GetPart(4)->ApplyForce( vLeftWing );
//	GetPart(5)->ApplyForce( vRightWing );
//
//
//	//wind resistance
//	f32 fVertLeftWingSpeed = ( (GetPart(4)->GetVelocity() * VNPerpMainAxis).getLength() );
//	f32 fVertRightWingSpeed = ( (GetPart(5)->GetVelocity() * VNPerpMainAxis).getLength() );
//	f32 fVertTailWingSpeed = ( (GetPart(3)->GetVelocity() * VNPerpMainAxis).getLength() );
//	vLeftWingResist = ( GetPart(4)->GetVelocity().normalize() * fVertLeftWingSpeed / -fwingresistancediv ) / fliftresistancediv;
//	vRightWingResist = ( GetPart(5)->GetVelocity().normalize() * fVertRightWingSpeed / -fwingresistancediv ) / fliftresistancediv;
//	vTailWingResist = ( GetPart(3)->GetVelocity().normalize() * fVertTailWingSpeed / -ftailwingresistancediv ) / fliftresistancediv;
//	GetPart(4)->ApplyForce( vLeftWingResist );
//	GetPart(5)->ApplyForce( vRightWingResist );
//	GetPart(3)->ApplyForce( vTailWingResist );
//
//	vEngineResist = ( GetPart(2)->GetVelocity().normalize() * GetPart(2)->GetVelocity().getLength() / -fengineresistancediv);
//	GetPart(0)->ApplyForce( vEngineResist );
//	GetPart(1)->ApplyForce( vEngineResist );
//	GetPart(2)->ApplyForce( vEngineResist );
//
//	//vPlaneResist = ( GetPart(2)->GetVelocity().normalize() * vTailWing * -10 );
//	//GetPart(3)->ApplyForce( vPlaneResist );
//	//GetPart(2)->ApplyForce( vPlaneResist );
//
//	// lift tail
////	vector3df vVertPos = GetPart(3)->Pos + (vN * 0.01);
////	GetPart(7)->OldPos = GetPart(7)->Pos =  vVertPos; 
//
//
//	//vector3df vTailStab = vNMain * -0.1;
//	//GetPart(7)->ApplyForce( vTailStab );
//
//	f32 fTailWingSpeed2 = ( (GetPart(3)->GetVelocity() ).getLength() );
//
//	if ( control )
//	{
//		if ( control->KeyPressed( irr::KEY_SHIFT ) ) //thrust
//		{
//			vector3df vPower = vMainAxis / fengineforcediv;
//			GetPart(0)->ApplyForce( vPower );
//			GetPart(1)->ApplyForce( vPower );
//			GetPart(2)->ApplyForce( vPower );
//		}
//		if ( control->KeyPressed( irr::KEY_KEY_W ) ) // fly down
//		{
//			vector3df vTail = VNPerpMainAxis * fTailWingSpeed2 / ftailsteerforcediv;
//			GetPart(3)->ApplyForce( vTail );
//		}
//		if ( control->KeyPressed( irr::KEY_KEY_S ) ) // fly up
//		{
//			vector3df vTail = - VNPerpMainAxis * fTailWingSpeed2 / ftailsteerforcediv;
//			GetPart(3)->ApplyForce( vTail );
//		}
//		if ( !flipstate )
//		if ( control->KeyPressed( irr::KEY_SPACE ) ) // roll
//		{
//			//ApplyAnimation( Animations[ANIM_AIRPLANE_ROLL] );
//			//FixZAxis( false );
//			//flipAngle - 180;
//
//			GetPart(1)->Exchange( GetPart(0) );
//			GetPart(5)->Exchange( GetPart(4) );
//			GetPart(2)->Move( GetPart(2)->Pos - VNPerpMainAxis*4.0f );
//			GetPart(7)->Move( GetPart(7)->Pos + VNPerpMainAxis*4.0f );
//			flipstate = true;
//			CONSOLE.addx( "flipState" );
//		}
//	}
//
//	//if ( currentAnim )
//	//	if ( GetCurrentFrame() == currentAnim->GetFramesNum()-1 )
//	//	{
//	//		//CONSOLE.add( "Null anim" );
//	//		NoAnimation();
//	//		FixZAxis( true );
//	//		SetAnimAngle( flipAngle );
//	//		CONSOLE.addx( "flipState %i", flipAngle );
//	//	}
//
//	CControllableCharacter::Think();
//
//	// throw stuff to debugRender
//	debugRender->planeVelocity = GetPart(2)->GetVelocity().getLength();
//	debugRender->planePos = vCenter;
//}
//
//void CAirplane::Load(const c8* filename)
//{
//	// partnum 0 - front left engine
//	// partnum 1 - front right engine
//	// partnum 2 - front wheel
//	// partnum 3 - tail
//	// partnum 4 - left wing
//	// partnum 5 - right wing
//	// partnum 6 - hor. tail wing
//	// partnum 7 - ver. tail wing
//
//	// load script file
//	SCRIPT.RunFile( filename );
//
//	String modelname;
//	GMVARTOSTRING( "airplane_modelname", modelname );
//	GMVARTOVALUE( "airplane_wing_forcediv", f32, fwingforcefiv );
//	GMVARTOVALUE( "airplane_tailwing_forcediv", f32, ftailwingforcediv );
//	GMVARTOVALUE( "airplane_wingslift_forcediv", f32, fliftforcediv );
//	GMVARTOVALUE( "airplane_tailsteer_forcediv", f32, ftailsteerforcediv );
//	GMVARTOVALUE( "airplane_wingresistance_forcediv", f32, fwingresistancediv );
//	GMVARTOVALUE( "airplane_tailwingresistance_forcediv", f32, ftailwingresistancediv );
//	GMVARTOVALUE( "airplane_wingsresistance_forcediv", f32, fliftresistancediv );
//	GMVARTOVALUE( "airplane_engineresistance_forcediv", f32, fengineresistancediv );
//	GMVARTOVALUE( "airplane_engine_forcediv", f32, fengineforcediv );
//
//	// start up stuff
//	CCharacter::Load( modelname.c_str() );
//
//	// check if the loaded character can be a plane
//	if ( GetPartsNum() < 8 )
//	{
//		bInvalidCharacter = true;
//		CONSOLE.addx( COLOR_WARNING, "Loaded character '%s' is an incorrect Airplane character", filename );
//		return;
//	}
//
//	GetPart(2)->colDamping = 0.0f;
//	GetPart(3)->colDamping = 0.0f;
//	GetPart(7)->colDamping = 0.0f;
//
//	SetRelativePart( 2 );
//}
//
//void CAirplane::Render()
//{
//	if (bInvalidCharacter)
//		return;
//
//	if ( APP.DebugMode )
//	{
//		IRR.video->draw3DLine(GetPart(0)->Pos, GetPart(1)->Pos, SColor(0,20,150,50));
//		IRR.video->draw3DLine(GetPart(1)->Pos, GetPart(2)->Pos, SColor(0,20,150,50));
//		IRR.video->draw3DLine(GetPart(2)->Pos, GetPart(0)->Pos, SColor(0,20,150,50));
//		IRR.video->draw3DLine(GetPart(2)->Pos, GetPart(3)->Pos, SColor(0,20,150,50));
//		IRR.video->draw3DLine(GetPart(3)->Pos, GetPart(0)->Pos, SColor(0,20,150,50));
//		IRR.video->draw3DLine(GetPart(3)->Pos, GetPart(1)->Pos, SColor(0,20,150,50));
//		IRR.video->draw3DLine(GetPart(4)->Pos, GetPart(5)->Pos, SColor(0,120,150,50));
//		IRR.video->draw3DLine(GetPart(3)->Pos, GetPart(6)->Pos, SColor(0,20,150,50));
//		IRR.video->draw3DLine(GetPart(7)->Pos, GetPart(3)->Pos, SColor(0,20,120,90));
//
//		vector3df Pos = vEngine;
//		IRR.video->draw3DLine(Pos-vector3df(0.9,0,0), Pos+vector3df(0.9,0,0), SColor(0,23,5,55));			
//		IRR.video->draw3DLine(Pos-vector3df(0,0.9,0), Pos+vector3df(0,0.9,0), SColor(0,23,5,55));			
//		IRR.video->draw3DLine(Pos-vector3df(0,0,0.9), Pos+vector3df(0,0,0.9), SColor(0,23,5,55));	
//
//		// forces
//		//Pos = GetPart(4)->Pos + vLeftWingResist*TAILWING_FORCEDIV*3;
//		//IRR.video->draw3DLine(GetPart(4)->Pos, Pos, SColor(0,255,100,200));
//		//Pos = GetPart(5)->Pos + vRightWingResist*TAILWING_FORCEDIV*3;
//		//IRR.video->draw3DLine(GetPart(5)->Pos, Pos, SColor(0,255,100,200));
//		//Pos = GetPart(3)->Pos + vTailWingResist*TAILWING_FORCEDIV*3;
//		//IRR.video->draw3DLine(GetPart(3)->Pos, Pos, SColor(0,255,100,200));
//
//		// speed
//		Pos = vEngine + (GetPart(2)->GetVelocity()* 2);
//		IRR.video->draw3DLine(vEngine, Pos, SColor(0,55,30,230));
//
//		//
//		IRR.video->draw3DLine(GetPart(4)->Pos, GetPart(0)->Pos, SColor(0,220,250,150));
//
//	}
//}
//
////////////////////////////////
//// CAirplaneDebugRender
////////////////////////////////
//
//void CAirplaneDebugRender::Render()
//{
//    if ( !APP.DebugMode )
//    {
//        return;
//    }
//
//	IRR.gui->getBuiltInFont()->draw( L"Airplane: ",
//                                          core::rect<s32>( 10, IRR.getScreenHeight() - 406, IRR.getScreenWidth(),
//                                                           IRR.getScreenHeight() ), irr::video::SColor( 255, 165, 15, 65 ), false, true );
//
//	WideString sVel = "Velocity: ";
//	sVel += f2wchar( planeVelocity );
//    IRR.gui->getBuiltInFont()->draw( sVel.c_str(),
//                                          core::rect<s32>( 10, IRR.getScreenHeight() - 392, IRR.getScreenWidth(),
//                                                           IRR.getScreenHeight() ), irr::video::SColor( 255, 165, 15, 65 ), false, true );
//	sVel = "Position: [ "; sVel += f2wchar( planePos.X );
//	sVel += "  ,  "; sVel += f2wchar( planePos.Y ); sVel += " ]";
//    IRR.gui->getBuiltInFont()->draw( sVel.c_str(),
//                                          core::rect<s32>( 10, IRR.getScreenHeight() - 378, IRR.getScreenWidth(),
//                                                           IRR.getScreenHeight() ), irr::video::SColor( 255, 165, 15, 65 ), false, true );
//
//}