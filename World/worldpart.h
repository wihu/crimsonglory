#ifndef WORLDPART_H_INCLUDED
#define WORLDPART_H_INCLUDED

#include "../Engine/engine.h"
#include "parts.h"

#include "../Newton/newton_physics.h"
#include "../Newton/newton_node.h"

////////////////////////////////////////////
// CWorldPart 
////////////////////////////////////////////

class CWorldPart : public CPhys_Part
{
  public:
    CWorldPart( vector3df NewPos = vector3df(  0.0f, 0.0f, 0.0f), f32 oneOverMassConst = 1.0f, f32 radiusConst = 1.0f, f32 elasticity = 0.66f, bool collidable = true, int aliveTime = -1 );
    virtual ~CWorldPart();

    virtual void Think();           
    virtual void Reset();

    virtual void OnHit( const NewtonBody* body, vector3df vImpactVel, f32 fImpactMass )
    {
    }

    // this is NULL on creation, it is up to the programmer to load the node
    IBillboardSceneNode* node;

  protected:
    static dFloat RayCastFilter( const NewtonBody* body, const dFloat* normal, int collisionID, void* userData, dFloat intersetParam );

    void ResolveRayCastCollision();

    void MakeBubbles( vector3df vIntersect );

    f32 fElasticity;
    bool bCollidable;

    // ray casting
    NewtonBody* raycastbody;
    dFloat fIParam;
    vector3df vColPoint, vColNormal;
};

#endif