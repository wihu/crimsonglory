#include "character.h"

#include "../IrrConsole/console_vars.h"

#include "world.h"
//#include <typeinfo>
//#include <exception>

////////////////////////////////////////////
// CCharacter 
////////////////////////////////////////////

//bool bRegistered_CCharacter = FACTORY->Actors.Register<CCharacter>( "character" );

CCharacter::CCharacter()
{
    Reset();
}

CCharacter::CCharacter( const c8* configFilename ) : CActor( configFilename )
{
    Reset();
}

CCharacter::~CCharacter()
{
}

void CCharacter::Reset()
{
    CActor::Reset();

    weapon = NULL;
    vAxisRotation.X = 90;
    vAxisRotation.Y = ANGLE_FACE_RIGHT;
    vAxisRotation.Z = 0;

    fRotationRate = 5.0f;
    bOnGround = bStopped = false;
    bone2MouseBoneID = 13;
    fAnimSpeedFactor = 0.000075f;

    //vMountPos.X = 25;
    //vMountPos.Y = 21;

    bonesNum = 0;

    type = NODECLASS_CHARACTER;
}

void CCharacter::Think()
{
    CActor::Think();

    //model->UpdateAnimation( 1/WORLD.GetPhysics()->timeStep * fAnimSpeedFactor );
    model->UpdateAnimation( WORLD.GetPhysics()->timeStep * fAnimSpeedFactor );

    if ( ( control ) && ( !parentAttachment ) )
    {
      RotateBone2Mouse( bone2MouseBoneID );
    }

    //NetworkID NetworkID = this->GetNetworkID();
    //CONSOLE.addx( "id %s: %i", configFilename, NetworkID);

    //CNewtonNode *myObject = (CNewtonNode *) GET_OBJECT_FROM_ID(NetworkID);
    //if (myObject==0)
    //  CONSOLE.addx("NetworkID: could not find object %i", NetworkID);
    //CONSOLE_VECTOR(myObject->getPosition());

    //TEMP:
    //if (getParentAttachment())
    //  model->setVisible( false );
    //else
    //  model->setVisible( true );
}

void CCharacter::Render()
{
    CActor::Render();

    if ( APP.DebugMode > 1 )
    {
      f32 radius = 0.05f;
      CalVector cV;
      vector3df vP;

      irr::video::SMaterial mat; 
      mat.Lighting = false; 
      IRR.video->setMaterial( mat ); 

      for ( int i = 0; i < bonesNum; i++ )
      {
        cV = model->getCurrentModel()->getSkeleton()->getBone( i )->getTranslationAbsolute();
        vP.set( cV.x, cV.y, cV.z );
        IRR.video->draw3DBox( aabbox3df( vector3df( vP.X - radius, vP.Y - radius, vP.Z - radius ), vector3df( vP.X + radius, vP.Y + radius, vP.Z + radius ) ), SColor( 0, 210, 225, 49 ) );
      }

      model->setDebugDataVisible( ( bool )APP.DebugMode );
    }
}

void CCharacter::changeAnimation( int animNum, float delayOut, float weightTarget )
{
    model->executeAction( animNum, delayOut, weightTarget, true );
}

void CCharacter::morphAnimation( int animNum, float delayOut, float weightTarget )
{
    model->executeAction( animNum, delayOut, weightTarget, false );
}

void CCharacter::clearAnimation( int animNum, float delayOut )
{
    model->clearAnim( animNum, delayOut );
}


void CCharacter::setLOD( float lod )
{
    model->setLodLevel( lod );
}


void CCharacter::makeCharacterBody( vector3df vSize, f32 fMass, f32 volumeFactor )
{
    NewtonCollision* sphere;
    sphere = NewtonCreateSphere( WORLD.GetPhysics()->nWorld, vSize.X / 2, vSize.Y / 2, vSize.Z / 2, NULL );

    newtonCollision = NewtonCreateConvexHullModifier( WORLD.GetPhysics()->nWorld, sphere );
    NewtonReleaseCollision( WORLD.GetPhysics()->nWorld, sphere );

    body = NewtonCreateBody( WORLD.GetPhysics()->nWorld, newtonCollision );

    //matrix4 mat;
    //vector3df vTemp(0, 4, 0);
    //mat.setTranslation( vTemp );
    //   NewtonBodySetMatrix( body, &mat.M[0] );

    vector3df vInertia;
    vInertia.X = fMass * ( vSize.Y * vSize.Y + vSize.Z * vSize.Z ) / 12.0f;
    vInertia.Y = fMass * ( vSize.X * vSize.X + vSize.Z * vSize.Z ) / 12.0f;
    vInertia.Z = fMass * ( vSize.X * vSize.X + vSize.Y * vSize.Y ) / 12.0f;

    NewtonBodySetMassMatrix( body, fMass, vInertia.X, vInertia.Y, vInertia.Z );

    setUserData();

    NewtonBodySetAutoFreeze( body, 0 );
    NewtonWorldUnfreezeBody( WORLD.GetPhysics()->nWorld, body );

    // upvector joint
    vector3df upDirection( -1.0f, 0.0f, 0.0f );
    m_upVector = NewtonConstraintCreateUpVector( WORLD.GetPhysics()->nWorld, &upDirection.X, body );

    NewtonReleaseCollision( WORLD.GetPhysics()->nWorld, newtonCollision );

    setRotation( vAxisRotation );

    // set custom wieght factor
    weightFactor = NewtonConvexCollisionCalculateVolume( sphere ) * volumeFactor;

    vHullSize = vSize;
}

void CCharacter::PhysicsControl()
{
}

void CCharacter::PhysicsTransform( matrix4 matrix )
{
    transformMatrix = matrix;
    UpdateSkeleton();
}
//  static matrix4 mat;
//  static vector3df vR;
//  memcpy(mat.M, matrix, sizeof(s32) * 16);
//
//
//  if (!attachment)
//  {
//      //node->setRotation(mat.getRotationDegrees());
//      //node->setPosition(mat.getTranslation());
//  }
//  else
//  {
//      vR = vAttachmentPos;
//  //  if (attachment->getRotation().X < 90)
//          vR.rotateXYBy( attachment->getRotation().Z, vector3df(0,0,0) );
//      //else
//      //  vR.rotateXYBy( -attachment->getRotation().Z, vector3df(0,0,0) );
//      vR.rotateXZBy( -attachment->getRotation().X, vector3df(0,0,0) );
//  //  CONSOLE_FLOAT(attachment->getRotation().X);
//      vR.rotateYZBy( attachment->getRotation().X, vector3df(0,0,0) );
//      transformMatrix.setTranslation( attachment->getPosition() - vR );
//      transformMatrix.setRotationDegrees( attachment->getRotation() + vector3df(90,180,0) );
//
//      vAxisRotation = transformMatrix.getRotationDegrees();
//      setPosition( transformMatrix.getTranslation() );
//  }
//}

void CCharacter::PhysicsCollision( const NewtonMaterial* material, const NewtonContact* contact, NewtonBody* cbody )
{
    dVector point;
    dVector normal;
    dVector velocity;

    // Get the collision and normal
    NewtonMaterialGetContactPositionAndNormal( material, &point.m_x, &normal.m_x );

    if ( point.m_y > getPosition().Y )
    {
      vGroundNormal.set( normal.m_x, normal.m_y, normal.m_z );
    }

    // get the mass, check if static object
    dFloat mass, Ixx, Iyy, Izz;
    NewtonBodyGetMassMatrix( cbody, &mass, &Ixx, &Iyy, &Izz );

    // if the playe want to move set disable friction else set high ground friction so it can stop on slopes
    if ( bStopped )
    {
      NewtonMaterialSetContactStaticFrictionCoef( material, 2.0f, 0 );
      NewtonMaterialSetContactKineticFrictionCoef( material, 2.0f, 0 );
      NewtonMaterialSetContactStaticFrictionCoef( material, 2.0f, 1 );
      NewtonMaterialSetContactKineticFrictionCoef( material, 2.0f, 1 );
      //CONSOLE.add("bStopped");
    }
    else
    {
      NewtonMaterialSetContactFrictionState( material, 0.0f, 0 );
      NewtonMaterialSetContactFrictionState( material, 0.0f, 1 );
      //CONSOLE.add("NOT");
    }

    bOnGround = true;

    NewtonBodyGetVelocity( body, &velocity.m_x );

    // calculate ball velocity perpendicular to the contact normal
    dVector tangentVelocity( velocity - normal.Scale( normal % velocity ) );

    // align the tangent at the contact point with the tangent velocity vector of the ball
    NewtonMaterialContactRotateTangentDirections( material, &tangentVelocity.m_x );

    // we do do want bound back we hitting the floor
    //NewtonMaterialSetContactElasticity (material, 0.3f);

    // mount stuff
    if ( ( control ) && ( mass != 0.0f ) )
    {
      if ( getControls()->ActionKeyPressed( AK_MOUNT ) )
      {
        if ( !parentAttachment )
        {
          CNewtonNode* n = ( CNewtonNode* )NewtonBodyGetUserData( cbody );
          //if (APP.DebugMode)
          //    CONSOLE.addx("Collided type %i", n->getType());

          if ( n->getType() == NODECLASS_MACHINE )
          {
            attachToParentNode( n, control );
          }
        }
      }

      //try {
      //    cout << "a is: " << typeid(NewtonBodyGetUserData(cbody)).name() << '\n';
      //} 
      //catch (exception& e) 
      //{ cout << "Exception: " << e.what() << endl; }
    }
    // pick up stuff
    if ( ( control ) && ( mass != 0.0f ) )
            //if ( getControls()->ActionKeyPressed( AK_MOUNT ) )
    {
      CNewtonNode* n = ( CNewtonNode* )NewtonBodyGetUserData( cbody );
      //if (APP.DebugMode)
      //    CONSOLE.addx("Collided type %i", n->getType());

      if ( ( lastDropped != n ) || ( getControls()->ActionKeyPressed( AK_MOUNT ) ) )
      {
        if ( !weapon )
        {
          if ( n->getType() == NODECLASS_WEAPON )
          {
            n->attachToParentNode( ( CNewtonNode * )NewtonBodyGetUserData( body ), control );
            //weapon = static_cast<CWeapon*>(n);
            //weapon->HoldWeapon( this );
          }
        }

        if ( n->getType() == NODECLASS_ITEM )
        {
          n->attachToParentNode( ( CNewtonNode * )NewtonBodyGetUserData( body ), control );
        }
      }
    }

    //dVector localPoint (m_matrix.UntransformVector (point));

    //// if a contact is below the max nead consider the character is on the ground
    //if (localPoint.m_y < m_maxStepHigh)
    //{
    //  bOnGround = true;
    //      
    //  NewtonBodyGetVelocity(m_myBody, &velocity.m_x);

    //  // calculate ball velocity perpendicular to the contact normal
    //  dVector tangentVelocity (velocity - normal.Scale (normal % velocity));

    //  // align the tangent at the contact point with the tangent velocity vector of the ball
    //  NewtonMaterialContactRotateTangentDirections (material, &tangentVelocity.m_x);

    //  // we do do want bound back we hitting the floor
    //  NewtonMaterialSetContactElasticity (material, 0.3f);
    //
    //  // save the elevation of the highest step to take
    //  if (localPoint.m_y > m_stepContact.m_y) 
    //  {
    //      if (fabsf (normal.m_y) < 0.8f)
    //      {
    //          m_stepContact = localPoint;   
    //      }
    //  }

    //  // if the playe want to move set disable friction else set high ground friction so it can stop on slopes
    //  if (bStopped) 
    //  {
    //      NewtonMaterialSetContactStaticFrictionCoef (material, 2.0f, 0);
    //      NewtonMaterialSetContactKineticFrictionCoef (material, 2.0f, 0);
    //      NewtonMaterialSetContactStaticFrictionCoef (material, 2.0f, 1);
    //      NewtonMaterialSetContactKineticFrictionCoef (material, 2.0f, 1);

    //  } else
    //  {
    //      NewtonMaterialSetContactFrictionState (material, 0, 0);
    //      NewtonMaterialSetContactFrictionState (material, 0, 1);
    //  }

    //} else 
    //{
    //  //set contact above the max step to be friction less
    //  // disable fiction calculation for sphere collision
    //  NewtonMaterialSetContactFrictionState (material, 0, 0);
    //  NewtonMaterialSetContactFrictionState (material, 0, 1);
    //}
}

void CCharacter::RotateBone2Mouse( int boneID )
{
    if ( boneID < 0 )
    {
      return;
    }
    f32 angle;
    quaternion iq;
    vector3df vP( 0, 0, 0 );
    matrix4 mat, mat2;

    //calc angle from mouse
    CalQuaternion q;

    vector3df vMC = ( getPosition() ) - control->mousePosWorld;
    vMC.normalize();

    angle = vMC.getHorizontalAngle().X; 

    q = model->getCurrentModel()->getSkeleton()->getBone( boneID )->getRotation();
    iq.set( q.x, q.y, q.z, q.w );
    mat2 = iq.getMatrix();

    mat.setRotationDegrees( vector3df( mat2.getRotationDegrees().X, mat2.getRotationDegrees().Y, mat2.getRotationDegrees().Z - angle ) );

    iq = core::quaternion( mat ); 

    q.set( iq.X, iq.Y, iq.Z, iq.W );
    model->getCurrentModel()->getSkeleton()->getBone( boneID )->setRotation( q );
    //  model->getCurrentModel()->getSkeleton()->getBone(boneID)->calculateState();

    if ( control->mousePosWorld.X > getPosition().X )
    {
      facing = FACE_RIGHT;
    }
    else
    {
      facing = FACE_LEFT;
    }
}

irr::core::aabbox3df CCharacter::getBoundingBox()
{
    // get transformed calmodel bounding box
    return aabbox3df( model->getBoundingBox().MinEdge + getPosition(), model->getBoundingBox().MaxEdge + getPosition() );
} 


void CCharacter::RagDollTransform( const NewtonRagDollBone* bone )
{
    //matrix4 matrix;
    //IAnimatedMeshSceneNode* model;

    //// get the model from the rag doll bone 
    //model = (IAnimatedMeshSceneNode*) NewtonRagDollBoneGetUserData (bone);

    //// get the local matrix for this bone
    //NewtonRagDollBoneGetLocalMatrix (bone, &matrix.M[0]);

    //// Position the node
    //model->setPosition(matrix.getTranslation() * NewtonToIrr);        // set position
    //
    //vector3df euler;
    //// set the angle for the node
    //NewtonGetEulerAngle (&matrix.M[0], &euler.X);
    //model->setRotation(euler * (180.0f / 3.1416f));
}

void CCharacter::RagDollApplyForceAndTorque( const NewtonBody* body )
{
    dFloat mass, Ixx, Iyy, Izz;
    NewtonBodyGetMassMatrix( body, &mass, &Ixx, &Iyy, &Izz );

    matrix4 mat;
    NewtonBodyGetMatrix( body, &mat.M[0] ); 

    // add da gravity
    dVector gravforce( 0, mass* WORLD.GetPhysics()->dGravity* IrrToNewton, 0 );
    NewtonBodyAddForce( body, &gravforce.m_x );
}

void CCharacter::BuildRagDollBone( NewtonRagDoll* ragDoll, NewtonRagDollBone* parentDollBone, CalCoreBone* calBone, int boneID )
{
    NewtonBody* bonebody;
    NewtonRagDollBone* doll_bone;
    f32 fMass = 10.0f;

    // get the box of the bone
    //calBone->getBoundingBox()
    CalVector pb[8]; 
    model->getCoreModel()->getCoreSkeleton()->getCoreBone( boneID )->getBoundingBox().computePoints( pb ); 

    aabbox3d<f32> box;//( vector3df(-0.1,-0.1,-0.1), vector3df(0.1,0.1,0.1));
    box.reset( pb[0].x, pb[0].y, pb[0].z ); 
    for ( int i = 0; i < 8; ++i )
    {
      box.addInternalPoint( pb[i].x, pb[i].y, pb[i].z );
    }

    vector3df size( box.getExtent() ); 
    vector3df createboxsize = size / 2;

    vector3df bonePos( calBone->getTranslationAbsolute().x, calBone->getTranslationAbsolute().y, calBone->getTranslationAbsolute().z );
    //vector3df boneRelPos(calBone->getTranslation().x, calBone->getTranslation().y, calBone->getTranslation().z);
    vector3df boneRelPos( model->getCurrentModel()->getSkeleton()->getBone( boneID )->getTranslation().x, model->getCurrentModel()->getSkeleton()->getBone( boneID )->getTranslation().y, model->getCurrentModel()->getSkeleton()->getBone( boneID )->getTranslation().z );

    // get the bone local matrix and comver to newton units
    matrix4 matrix;
    matrix.setTranslation( boneRelPos );

    // create a rag doll bone at the origin of this bone
    NewtonCollision* collision;
    collision = NewtonCreateBox( WORLD.GetPhysics()->nWorld, createboxsize.X, createboxsize.Y, createboxsize.Z, NULL );

    doll_bone = NewtonRagDollAddBone( ragDoll, parentDollBone, calBone, fMass, &matrix.M[0], collision, &size.X );
    NewtonRagDollBoneSetID( doll_bone, boneID );
    CONSOLE.addx( "Bone: %i", boneID );

    // set the rad doll bone limits
    // the x axis is usually the box main axis
    vector3df coneDir( 1.0f, 0.0f, 0.0f );
    // the z axis is usually the box plane 
    vector3df lateralDir( 0.0f, 0.0f, 1.0f );

    // the axis limits are specifiied in global space, rotate by the joint matrix
    matrix4 boneMatrix;
    boneMatrix.setTranslation( bonePos );
    boneMatrix.rotateVect( coneDir );
    boneMatrix.rotateVect( lateralDir );


    // set the limis
    //NewtonRagDollBoneSetLimits (doll_bone, &coneDir.X, -PI * 0.25f, PI * 0.25f, -PI * 0.5f, &lateralDir.X, 0.0f, 0.0f);


    // get the bone rigid body and freeze it
    bonebody = NewtonRagDollBoneGetBody( doll_bone );   
    //  NewtonWorldFreezeBody (WORLD.GetPhysics()->nWorld, bonebody);
    NewtonBodySetMaterialGroupID( bonebody, CNewton::nocollisionID );

    vector3df vInertia;
    vInertia.X = fMass * ( size.Y * size.Y + size.Z * size.Z ) / 12.0f;
    vInertia.Y = fMass * ( size.X * size.X + size.Z * size.Z ) / 12.0f;
    vInertia.Z = fMass * ( size.X * size.X + size.Y * size.Y ) / 12.0f;
    NewtonBodySetMassMatrix( bonebody, fMass, vInertia.X, vInertia.Y, vInertia.Z );


    // recusivelly add old bones to the rag doll
    //const list<ISceneNode*>& children = bone->getChildren();
    //for (list<ISceneNode*>::Iterator it = children.begin(); it != children.end(); ++it) {
    //  child = (IrrBox*)*it;
    //  BuildRadDoll (ragDoll, doll_bone, child, veloc);
    //}

    std::list<int>::iterator it, ite = calBone->getListChildId().end(), itT;
    if ( boneID < 8 )
    {
      for ( it = calBone->getListChildId().begin(); it != ite; it++ )
      {
        //      CONSOLE.addx("Bone: %i", ( *it ));
        BuildRagDollBone( ragDoll, doll_bone, model->getCoreModel()->getCoreSkeleton()->getCoreBone( ( *it ) ), ( *it ) );
      }
    }
}

void CCharacter::ZombieDie()
{
    CActor::ZombieDie();
    //if (bZombie)
    //  return;

    //NewtonRagDoll* ragDoll;

    ////create a ragdoll container
    //ragDoll = NewtonCreateRagDoll (WORLD.GetPhysics()->nWorld);

    //// begin adding bones to the rad doll
    //NewtonRagDollBegin (ragDoll);

    //// set the force function
    ////NewtonRagDollSetForceAndTorqueCallback (ragDoll, RagDollApplyForceAndTorque);

    //// Set the trnasform function
    //NewtonRagDollSetTransformCallback (ragDoll, RagDollTransform);

    //BuildRagDollBone(ragDoll, NULL, model->getCoreModel()->getCoreSkeleton()->getCoreBone(1), 1 );

    //// finalize the rag doll
    //NewtonRagDollEnd (ragDoll);

    //bZombie = true;
}

void CCharacter::UpdateChildAttachments()
{
    vector3df vR, vA;
    matrix4 childmat; 
    int attachBone;
    for ( int i = 0; i < childAttachments.size(); i++ )
    {
      attachBone = ( int )childAttachments[i]->getParentAttachmentPos().X;
      vA.X = model->getCurrentModel()->getSkeleton()->getBone( attachBone )->getTranslationAbsolute().x;
      vA.Y = model->getCurrentModel()->getSkeleton()->getBone( attachBone )->getTranslationAbsolute().y;
      vA.Z = model->getCurrentModel()->getSkeleton()->getBone( attachBone )->getTranslationAbsolute().z;

      quaternion q;
      q.X = model->getCurrentModel()->getSkeleton()->getBone( attachBone )->getRotationAbsolute().x;
      q.Y = model->getCurrentModel()->getSkeleton()->getBone( attachBone )->getRotationAbsolute().y;
      q.Z = model->getCurrentModel()->getSkeleton()->getBone( attachBone )->getRotationAbsolute().z;
      q.W = model->getCurrentModel()->getSkeleton()->getBone( attachBone )->getRotationAbsolute().w;

      childmat.setTranslation( vA );
      childmat.setRotationDegrees( q.getMatrix().getRotationDegrees() );

      childAttachments[i]->childAttachmentPhysicsTransform( childmat );
    }
}

bool CCharacter::isHoldingWeapon()
{
    return isChildAttached( NODECLASS_WEAPON );
}

bool CCharacter::onChildAttached( CNewtonNode* what, void* data )
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

void CCharacter::onChildUnAttached( CNewtonNode* what )
{
    CActor::onChildUnAttached( what );

    if ( what->getType() == NODECLASS_WEAPON )
    {
      weapon = NULL;
    }
}

void CCharacter::UpdateSkeleton()
{
    //calc angle from mouse
    CalQuaternion q;
    quaternion iq;
    vector3df vP;
    CalVector cV;

    if ( parentAttachment )
    {
      //vAxisRotation = matrix.getRotationDegrees();
      vector3df vTRot = transformMatrix.getRotationDegrees();
      vTRot += vAxisRotation;
      vTRot.Y = ANGLE_FACE_LEFT;
      transformMatrix.setRotationDegrees( vTRot );
    }

    //vector3df vTRot = transformMatrix.getRotationDegrees();
    //  transformMatrix.setRotationDegrees( vTRot );

    iq = core::quaternion( transformMatrix );
    vP = transformMatrix.getTranslation();  

    //vP = vector3df(150,0,0);
    q.set( iq.X, iq.Y, iq.Z, iq.W );
    cV.set( vP.X, vP.Y, vP.Z );

    model->getCurrentModel()->getSkeleton()->getBone( 0 )->setTranslation( cV );
    model->getCurrentModel()->getSkeleton()->getBone( 0 )->setRotation( q );
    model->getCurrentModel()->getSkeleton()->getBone( 0 )->calculateState();

    //if ( vOldAxisRotation != vAxisRotation )
    setRotation( vAxisRotation );

    vOldAxisRotation = vAxisRotation;
    vAxisRotation.X = 90;
    //vAxisRotation.Y = 90;
    vAxisRotation.Z = 0;

    // set the collision hull size according to current height of character

    f32 footY;
    if ( model->getCurrentModel()->getSkeleton()->getBone( LFootBoneID )->getTranslationAbsolute().y > model->getCurrentModel()->getSkeleton()->getBone( RFootBoneID )->getTranslationAbsolute().y )
    {
      footY = model->getCurrentModel()->getSkeleton()->getBone( LFootBoneID )->getTranslationAbsolute().y;
    }
    else
    {
      footY = model->getCurrentModel()->getSkeleton()->getBone( RFootBoneID )->getTranslationAbsolute().y;
    }

    f32 fS = 0.0f + 0.5f * abs( model->getCurrentModel()->getSkeleton()->getBone( headBoneID )->getTranslationAbsolute().y - footY );
    if ( fS > 0.0f )
    {
      fS = fS;
    }
    else
    {
      fS = 1.0f;
    }


    //f32 fSX = 0.5f + 1.6f * abs(model->getCurrentModel()->getSkeleton()->getBone(10)->getTranslationAbsolute().x
    //  - model->getCurrentModel()->getSkeleton()->getBone(23)->getTranslationAbsolute().x);
    //if ( fSX > 0.0f )
    //  fSX = fSX;
    //else
    //  fSX = 1.0f;

    //vector3df vRot(fSX, 1.0f, fS );
    vector3df vRot( 1.0f, 1.0f, fS );
    //CONSOLE_FLOAT( fS );

    setCollisionSize( vRot );
}