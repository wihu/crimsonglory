#ifndef CHARACTER_H_INCLUDED
#define CHARACTER_H_INCLUDED

#include "../Engine/engine.h"
#include "actor.h"
#include "weapon.h"
#include "../Cal3D/CCal3DSceneNode.h"

#define FACE_LEFT -1
#define FACE_RIGHT 1

////////////////////////////////////////////
// CCharacter 
////////////////////////////////////////////

class CCharacter : public CActor
{
  public:
    CCharacter();
    CCharacter( const c8* configFilename );
    virtual ~CCharacter();

    virtual void Reset();
    virtual void Think();
    virtual void Render();

    virtual irr::core::aabbox3df getBoundingBox(); 

    virtual void changeAnimation( int animNum, float delayOut = 1.0f, float weightTarget = 1.0f );
    virtual void morphAnimation( int animNum, float delayOut = 1.0f, float weightTarget = 1.0f );
    virtual void clearAnimation( int animNum, float delayOut = 1.0f );

    void setLOD( float lod );

    CCal3DSceneNode* getModel()
    {
        return model;
    }

  protected:
    virtual void PhysicsControl();
    virtual void PhysicsTransform( matrix4 matrix );
    virtual void PhysicsCollision( const NewtonMaterial* material, const NewtonContact* contact, NewtonBody* cbody );

    virtual bool onChildAttached( CNewtonNode* what, void* data );
    virtual void onChildUnAttached( CNewtonNode* what );

    // attach to bone instead to position like NewtonNode, vMountPos[i].X is the bone number (a bit of a hack)
    void UpdateChildAttachments();

    static void RagDollTransform( const NewtonRagDollBone* bone );
    static void RagDollApplyForceAndTorque( const NewtonBody* body );

    void makeCharacterBody( vector3df vSize, f32 fMass, f32 volumeFactor );

    void UpdateSkeleton();
    void RotateBone2Mouse( int boneID );

    virtual void ZombieDie();
    void BuildRagDollBone( NewtonRagDoll* ragDoll, NewtonRagDollBone* parentDollBone, CalCoreBone* calBone, int boneID );

    bool isHoldingWeapon();

    CCal3DSceneNode* model;

    NewtonJoint* m_upVector;
    vector3df vHullSize;

    int bonesNum;
    u16 bone2MouseBoneID;
    u16 headBoneID, RFootBoneID, LFootBoneID;

    f32 fAnimSpeedFactor;

    CWeapon* weapon;

    vector3df vGroundNormal;

    int facing;

    CNewtonNode* lastDropped;
};

#endif