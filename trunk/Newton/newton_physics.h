#ifndef NEWTON_PHYSICS_H
#define NEWTON_PHYSICS_H

#include "../Game/stdafx.h"
#include "../Engine/engine.h"
#include "../App/app.h"

#include "Newton.h"
#include "dVector.h"
#include "dMatrix.h"

//GAME
#include "../Game/GameDLL.h"
// IRR.
#include "../irrlicht/IrrlichtTask.h"
// CONSOLE.
#include "../IrrConsole/console.h"
#include "../IrrConsole/console_vars.h"


// scale factor between Newton and IRR
const float NewtonToIrr = 0.1f;
const float IrrToNewton = ( 1.0f / NewtonToIrr );

// structure use to hold game play especial effects
struct SpecialEffectStruct
{
    void* soundHandle;
    void* particleHandle;

    // create the resources for this materials effect here
    void Init( char* effectName )
    {
    }

    // destroy all resouces for this material interaction here
    void Detroy()
    {
    }

    // play the impact sound
    void PlayImpactSound( dFloat volume )
    {
    }

    // play scratch sound
    void PlayScratchSound( dFloat volume )
    {
    }

    NewtonBody* m_body0;
    NewtonBody* m_body1;
    dVector m_position;
    dFloat m_contactMaxNormalSpeed;
    dFloat m_contactMaxTangentSpeed;
};


////////////////////////////////////////////
// CNewton 
////////////////////////////////////////////
class CNewton : public ITask
{
  public:
    CNewton();
    virtual ~CNewton();
    AUTO_SIZE;

    virtual bool Start();
    virtual void Update();
    virtual void Stop();

    static void PhysicsApplyForceAndTorque( const NewtonBody* body );
    static void PhysicsSetTransform( const NewtonBody* body, const dFloat* matrix );
    static void PhysicsBodyDestructor( const NewtonBody* body );

    static void DebugShowGeometryCollision( const NewtonBody* body, int vertexCount, const dFloat* faceVertec, int id );
    static void DebugShowBodyCollision( const NewtonBody* body );
    virtual void Render();

    NewtonBody* addStaticBodyTree( ISceneNode* node, char* filename, vector3df vPos, vector3df vScale );
    NewtonBody* addRigidBodyBox( ISceneNode* node, vector3df vSize, vector3df vPos );
    NewtonBody* addStaticBodyBox( ISceneNode* node, vector3df vSize, vector3df vPos, vector3df vRot );

    // Helper functions: from OgreNewt by Walaber, translated by cheesyPoof - don't work
    void addForce( const NewtonBody* body, core::vector3df force );
    void addTorque( const NewtonBody* body, core::vector3df torque );
    void matrixToQuatPos( const float* matrix, core::quaternion& quat, core::vector3df& pos );
    void quatPosToMatrix( const core::quaternion quat, const core::vector3df pos, float* matrix );
    void getPositionOrientation( const NewtonBody* body, core::quaternion& orient, core::vector3df& pos );
    void addGlobalForce( const NewtonBody* body, core::vector3df force, core::vector3df pos );
    void addLocalForce( const NewtonBody* body, core::vector3df force, core::vector3df pos );

    vector3df getPointVelocity( const NewtonBody* body, vector3df vPos );

    vector3df getTickVelocity( vector3df vVel )
    {
        return vVel / minFrames;
    }

    NewtonWorld* nWorld;
    static dFloat dGravity;
    float timeStep;

    //materials
    static int woodID; 
    static int metalID; 
    static int levelID; 
    static int defaultnodeID; 
    static int nocollisionID; 

  private:
    void SetupMaterials();
    void CleanUpMaterials();

    int minFrames;
    double deltaTime, curTime, lastTime;
    double mAccumlativeLoopTime;

    //materials
    static int GenericContactBegin( const NewtonMaterial* material, const NewtonBody* body0, const NewtonBody* body1 );
    static int NodeContactProcess( const NewtonMaterial* material, const NewtonContact* contact );
    static void GenericContactEnd( const NewtonMaterial* material );
    static int GenericContactProcess( const NewtonMaterial* material, const NewtonContact* contact );
    static int NodeLevelContactProcess( const NewtonMaterial* material, const NewtonContact* contact );

    // strutures use to hold diffrents materials interation 
    static SpecialEffectStruct wood_wood;
    static SpecialEffectStruct wood_metal;
    static SpecialEffectStruct wood_level;
    static SpecialEffectStruct metal_metal;
    static SpecialEffectStruct metal_level;

    static SpecialEffectStruct* g_currectEffect;

    //{********** NUMBER27's TIMING ROUTINES ***********}
    void Number27Timing();

    int mainTickCounter;
    int TimeInMil, TimeInMilLast;
    int TimePassed;
    int Seconds, SecondsLast;
    int Ticks, TicksPerSecond;
    int Frames, FramesPerSecond, TickTime, TickTimeLast;
    int GoalTicks;
    int frc, fps, frp;

    int mainControl, loopTicks;
};


#endif