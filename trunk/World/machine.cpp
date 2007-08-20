#include "machine.h"

#include "../World/prop.h"

////////////////////////////////////////////
// CMachine 
////////////////////////////////////////////

CMachine::CMachine( const c8* scriptFilename ) : CActor( scriptFilename )
{
    Reset();
}

CMachine::~CMachine()
{
}

void CMachine::Reset()
{
    CActor::Reset();

    type = NODECLASS_MACHINE;
    weapon = NULL;
    maxSplits = 5;
    fSplitFactor = 20.0f;
    propTimeout = 1000;
}

void CMachine::Think()
{
    CActor::Think();

    if ( control )
    {
      if ( weapon )
      {
        weapon->setControls( control );
      }
    }
    else // no control - out of control!
    {
      if ( weapon )
      {
        weapon->setControls( NULL );
      }
    }
}

void CMachine::Render()
{
    CActor::Render();
}

void CMachine::LoadZombie( matrix4 matrix )
{
    CProp* p1, * p2;

    SMesh* sMesh;
    sMesh = IRR.smgr->getMeshManipulator()->createMeshCopy( iNodeMesh );

    p1 = new CProp( sMesh, propTimeout, ( BodyType )bodyType, vNodeScale, fMass * 0.99, true, fSplitFactor, maxSplits );
    p1->setPosition( matrix.getTranslation() );
    p1->setRotation( matrix.getRotationDegrees() );
    NewtonBodySetVelocity( p1->body, &vVelocity.X );
    NewtonBodySetOmega( p1->body, &vOmega.X );
    p1->node->setMaterialTexture( 0, node->getMaterial( 0 ).Texture1 );
}


void CMachine::PhysicsCollision( const NewtonMaterial* material, const NewtonContact* contact, NewtonBody* cbody )
{
    CNewtonNode::PhysicsCollision( material, contact, cbody );

    f32 fSpeed = fVelocity;
    // TEMP: 
    if ( fSpeed > ( fMass / fSplitFactor ) )
    {
      vector3df vPoint, vNormal;
      NewtonMaterialGetContactPositionAndNormal( material, &vPoint.X, &vNormal.X );

      dFloat mass, Ixx, Iyy, Izz;
      NewtonBodyGetMassMatrix( cbody, &mass, &Ixx, &Iyy, &Izz );

      if ( mass != 0.0f ) // hits object
      {
        CNewtonNode* n = ( CNewtonNode* )NewtonBodyGetUserData( cbody );

        if ( fMass >= mass )
        {
          if ( n->getType() == NODECLASS_MACHINE )
          {
            CMachine* machine = static_cast<CMachine*>( n );
            // Die when health drops
            //machine->Zombify();
            //bZombie = true;
          }

          this->takeDamage( mass / fMass, UNASSIGNED_PLAYER_ID );
        }
      }
      else // hits map
      {
        bZombie = true;
      }
    }
}

bool CMachine::onChildAttached( CNewtonNode* what, void* data )
{
    if ( CActor::onChildAttached( what, data ) )
    {
      if ( what->getType() == NODECLASS_WEAPON )
      {
        weapon = static_cast<CWeapon*>( what );
      }

      return true;
    }
    return false;
}

void CMachine::onChildUnAttached( CNewtonNode* what )
{
    CActor::onChildUnAttached( what );

    if ( what->getType() == NODECLASS_WEAPON )
    {
      weapon = NULL;
    }
}

void CMachine::setDirection( f32 angle )
{
    //vAxisRotation.X = 180;
    //vAxisRotation.Y = 0;//angle+180;
    //vAxisRotation.Z = 180;
    vAxisRotation.X = angle + 180;
    vAxisRotation.Y = 0;
    vAxisRotation.Z = angle + 180;
    vOldAxisRotation = vAxisRotation;
    setRotation( vAxisRotation );
}