#ifndef NEWTON_NODE_H_INCLUDED
#define NEWTON_NODE_H_INCLUDED

#include "../Engine/engine.h"

#include "newton_physics.h"
#include "../World/world.h"
#include "../RakNet/NetworkIDGenerator.h"
#include "Custom2DJoint.h"

enum BodyType { BODY_BOX, BODY_SPHERE, BODY_HULL };

enum DerivedClassType { NODECLASS_DEFAULT, NODECLASS_ACTOR, NODECLASS_CHARACTER, NODECLASS_ITEM, NODECLASS_WEAPON, NODECLASS_PROP, NODECLASS_MACHINE };

struct MountPlace
{
    vector3df vMountPos, vMountRot;
    int placetype;
};

#define ANGLE_FACE_RIGHT 0
#define ANGLE_FACE_LEFT -180

////////////////////////////////////////////
// CNewtonNode 
////////////////////////////////////////////

class CNewtonNode : public NetworkIDGenerator
{
  public:
    //! Constructor
    CNewtonNode();
    CNewtonNode( const c8* modelFilename, BodyType bodyType = BODY_BOX, vector3df vScale = vector3df(  1.0f, 1.0f, 1.0f), float fMass = 1.0f );
    CNewtonNode( IMesh* mesh, BodyType bodyType = BODY_BOX, vector3df vScale = vector3df(  1.0f, 1.0f, 1.0f), float fMass = 1.0f );
    //! Destructor
    virtual ~CNewtonNode();

    virtual vector3df getPosition();
    virtual vector3df getRotation();
    virtual void setPosition( vector3df vPos, bool transformX = true, bool transformY = true, bool transformZ = true );
    virtual void setRotation( vector3df vRot, bool transformX = true, bool transformY = true, bool transformZ = true );

    vector3df getNodeCenter();
    vector3df getVelocity()
    {
        return vVelocity;
    }   
    vector3df getTickVelocity()
    {
        return WORLD.GetPhysics()->getTickVelocity( vVelocity );
    }
    vector3df getOmega()
    {
        return vOmega;
    }   
    matrix4 getTransformMatrix()
    {
        return transformMatrix;
    }

    virtual irr::core::aabbox3df getBoundingBox(); 

    int getType()
    {
        return type;
    }

    // if is zombie the entity will die in one tick, so free the pointer to it if you detect a zombie
    bool isZombie()
    {
        return bZombie;
    }

    // Attachment system
    virtual void attachToParentNode( CNewtonNode* node, void* data = NULL );
    virtual void childAttachmentPhysicsTransform( matrix4 matrix );
    virtual void unAttachFromParent();
    void setParentAttachmentPos( vector3df vPos )
    {
        vParentAttachmentPos = vPos;
    }
    void setParentAttachmentRot( vector3df vRot )
    {
        vParentAttachmentRot = vRot;
    }
    vector3df getParentAttachmentPos()
    {
        return vParentAttachmentPos;
    }
    vector3df getParentAttachmentRot()
    {
        return vParentAttachmentRot;
    }
    bool isAttached()
    {
        return parentAttachment;
    }
    CNewtonNode* getParentAttachment()
    {
        return parentAttachment;
    }
    bool isChildAttached( int nodetype );
    CNewtonNode* getAttachedChild( int nodetype = -1 );
    CNewtonNode* DropChild( int nodetype = -1 );
    CNewtonNode* GetGreatestParent();

    bool isInWater()
    {
        return bInWater;
    }

    bool isUnderWater();

    bool IsNetworkIDAuthority( void ) const
    {
        return NET.rakServer->IsActive();
    }

    NewtonBody* body;
    ISceneNode* node;

    CustomJoint2D* joint2d;
    bool bFix2DPos, bFix2DRot;

  protected:
    friend class CNewton;
    virtual void PhysicsControl()
    {
    };
    virtual void PhysicsTransform( matrix4 matrix );
    virtual void PhysicsCollision( const NewtonMaterial* material, const NewtonContact* contact, NewtonBody* cbody );
    virtual void OnEnterWater( aabbox3df* zonebox );
    virtual void OnExitWater( aabbox3df* zonebox );

    void Reset();

    static void PhysicsApplyForceAndTorque( const NewtonBody* body );
    static void PhysicsSetTransform( const NewtonBody* body, const dFloat* matrix );
    static int GetBuoyancy( const int collisionID, void* context, const dFloat* globalSpaceMatrix, dFloat* globalSpacePlane );

    void assemblePhysics( const c8* modelFilename, BodyType bodyType, vector3df vScale, vector3df vColOffset, float fMass, bool modifiableCollision = false );
    void assemblePhysics( IMesh* mesh, BodyType bodyType, vector3df vScale, float fMass, bool modifiableCollision = false );

    NewtonBody* makeBody( ISceneNode* node, BodyType bodyType, vector3df vSize, vector3df vScale, float fMass, IMesh* iMesh, bool modifiableCollision = false );
    NewtonBody* makeBody( ISceneNode* node, BodyType bodyType, vector3df vSize, vector3df vScale, vector3df vColOffset, float fMass, IAnimatedMesh* iMesh, bool modifiableCollision = false );

    // must call this on creation
    void setUserData();

    // Attachment system
    virtual void UpdateChildAttachments();
    virtual void addMountPlace( vector3df vPos, vector3df vRot, int nodeclasstype );
    // call these when overriding
    virtual bool onChildAttached( CNewtonNode* what, void* data ); // returns of child was attached
    virtual void onChildUnAttached( CNewtonNode* what );

    CNewtonNode* parentAttachment;
    vector3df vParentAttachmentPos, vParentAttachmentRot;
    array<CNewtonNode*> childAttachments;
    array<MountPlace*> MountPlaces;

    virtual void setCollisionSize( vector3df vScale );

    NewtonCollision* newtonCollision;
    matrix4 transformMatrix;

    int type;

    f32 fAngDamp, fLinDamp, fMass;
    vector3df vVelocity, vOmega;
    f32 fVelocity;
    f32 weightFactor;
	vector3df vBoxSize; // size of the bounding box

    bool bZombie;
    // number of zone which node is in +1
    int bInWater;
    bool watercheckcount;
    vector3df vWaterEntryPos;

    // loadednode stuff
    IMesh* iNodeMesh;
    int bodyType;
    vector3df vNodeScale;
    // modifiable hull stuff
    bool bModCol;
    vector3df vHullScale;
};


extern ISceneNode* loadNode_returnSize( const c8* modelFilename, vector3df vScale, f32 exchange, vector3df& vExchangeSize, IMesh*& iMesh );
extern IAnimatedMeshSceneNode* loadNode_returnSize( const c8* modelFilename, vector3df vScale, f32 exchange, vector3df& vExchangeSize, IAnimatedMesh*& iMesh );

#endif