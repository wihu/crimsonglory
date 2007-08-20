//#ifndef AIRPLANE_H_INCLUDED
//#define AIRPLANE_H_INCLUDED
//
//#include "../World/controllable.h"
//
//class CAirplaneDebugRender;
//
////////////////////////////////
//// CAirplane
////////////////////////////////
//
//#define ANIM_AIRPLANE_ROLL 0
//
//class CAirplane : public CControllableCharacter
//{
//  public:
//    CAirplane();
//    virtual ~CAirplane();
//
//	virtual void Think();
//	virtual void Reset();
//	virtual void Render();
//
//	virtual void Load(const c8* filename);
//
//  protected:
//
//  private:
//	  vector3df vMainAxis, vPerpMainAxis, vEngine, 
//		  vLeftWing, vRightWing, vTailWing, 
//		  vLeftWingResist, vRightWingResist, vTailWingResist, vEngineResist;
//
//	  f32 fwingforcefiv, ftailwingforcediv, fliftforcediv, fwingresistancediv, 
//		  ftailwingresistancediv, fliftresistancediv, fengineforcediv, ftailsteerforcediv, fengineresistancediv;
//
//	  bool flipstate;
//
//	  CAirplaneDebugRender* debugRender;      
//};
//
////////////////////////////////
//// CAirplaneDebugRender
////////////////////////////////
//
//class CAirplaneDebugRender : public CRenderable
//{
//  public:
//    CAirplaneDebugRender() : CRenderable( RENDER_ABOVE_GUI )
//    {
//    }
//    virtual ~CAirplaneDebugRender()
//    {
//    }
//    AUTO_SIZE;
//
//    virtual void Render();
//
//	f32 planeVelocity;
//	vector3df planePos;
//};
//
//#endif