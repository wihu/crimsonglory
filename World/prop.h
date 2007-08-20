#ifndef PROP_H_INCLUDED
#define PROP_H_INCLUDED

#include "../Engine/engine.h"

#include "../World/entity.h"
#include "../Newton/newton_physics.h"
#include "../Newton/newton_node.h"

////////////////////////////////////////////
// CProp
////////////////////////////////////////////

class CProp : public CNewtonNode, public CEntity
{
  public:
    //  CProp( const c8* modelFilename, int timeToLive, BodyType bodyType = BODY_BOX, vector3df vScale = vector3df(1.0f, 1.0f, 1.0f),
    //float fMass = 0.5f, f32 parentMass = 1.0f, bool breakable = true, f32 splitFactor = 25.0f, int maxSplitsNum = 5 );
    CProp( IMesh* mesh, int timeToLive, BodyType bodyType = BODY_BOX, vector3df vScale = vector3df(  1.0f, 1.0f, 1.0f), float fMass = 0.5f, bool breakable = true, f32 splitFactor = 25.0f, int maxSplitsNum = 5 );
    virtual ~CProp();

    virtual void Reset();
    virtual void Think();
    virtual void Render();

    void BreakAtPoint( vector3df vCutPos, vector3df vCutNormal, bool breakableChildren = true );

    vector3df getNodeScale()
    {
        return vNodeScale;
    }

    f32 fHealth;

  private:
    virtual void PhysicsCollision( const NewtonMaterial* material, const NewtonContact* contact, NewtonBody* cbody );

    bool bBreakable;
    vector3df vNodeScale;
    int maxSplits;
    f32 fSplitFactor;
};

extern IMesh* getCrushedMesh( IMesh* iMesh, plane3df plane );
extern IMesh* getSlicedMesh( IMesh* sourceMesh, IMesh*& backMesh, plane3df plane );


#endif