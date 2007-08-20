#include "newton_node.h"

#include "../World/map.h"
#include "../World/calc.h"

#include "../FreeSL/SoundTask.h"

#define HULL_SIZE_MODIFER 0.95f;

////////////////////////////////////////////
// CNewton 
////////////////////////////////////////////

CNewtonNode::CNewtonNode()
{
    Reset();
}

CNewtonNode::CNewtonNode( const c8* modelFilename, BodyType bodyType, vector3df vScale, float fMass )
{
    Reset();
    assemblePhysics( modelFilename, bodyType, vScale, vector3df( 0, 0, 0 ), fMass );
}

CNewtonNode::CNewtonNode( IMesh* mesh, BodyType bodyType, vector3df vScale, float fMass )
{
    Reset();
    assemblePhysics( mesh, bodyType, vScale, fMass );
}

CNewtonNode::~CNewtonNode()
{
    int i;

    // get rid of attachment children and parents
    while ( childAttachments.size() > 0 )
    {
      childAttachments[0]->unAttachFromParent();
    }
    //CONSOLE.addx("childAttachments.size() %i", childAttachments.size());
    childAttachments.clear();

    if ( parentAttachment )
    {
      unAttachFromParent();
    }

    for ( i = 0; i < MountPlaces.size(); i++ )
    {
      delete MountPlaces[i];
    }
    MountPlaces.clear();

    if ( body )
    {
      NewtonDestroyBody( WORLD.GetPhysics()->nWorld, body );
    }
    if ( ( IRR.smgr ) && ( node ) )
    {
      node->remove();
    }
}

void CNewtonNode::Reset()
{
    bFix2DPos = bFix2DRot = true;
    parentAttachment = NULL;
    body = NULL;
    bZombie = false;
    type = NODECLASS_DEFAULT;
    bInWater = 0;
    fAngDamp = fLinDamp = 0.075f;
    fMass = 1.0f;
    iNodeMesh = NULL;
    watercheckcount = false;
    bModCol = false;
}

void CNewtonNode::assemblePhysics( const c8* modelFilename, BodyType bodyType, vector3df vScale, vector3df vColOffset, float fMass, bool modifiableCollision )
{
    vector3df vSize;
    //IMesh* loadedMesh;
    IAnimatedMesh* loadedMesh;
    node = NULL;
    node = loadNode_returnSize( modelFilename, vScale, NewtonToIrr, vSize, loadedMesh );

    iNodeMesh = loadedMesh->getMesh( 0 );
    vNodeScale = vScale;

    if ( !node )
    {
      CONSOLE.addx( "Could not load physics model: %s", modelFilename );
      return;
    }

    bModCol = modifiableCollision;
    makeBody( node, bodyType, vSize, vScale, vColOffset, fMass, loadedMesh, modifiableCollision );
}

void CNewtonNode::assemblePhysics( IMesh* mesh, BodyType bodyType, vector3df vScale, float fMass, bool modifiableCollision )
{
    vector3df vExchangeSize;

    node = NULL;
    node = IRR.smgr->addMeshSceneNode( mesh );
    node->setScale( vScale );

    iNodeMesh = mesh;
    vNodeScale = vScale;

    aabbox3d<f32> box = node->getBoundingBox();
    static vector3df edges[8];
    box.getEdges( edges );

    vExchangeSize.Z = ( edges[3] - edges[1] ).getLength();
    vExchangeSize.Y = ( edges[0] - edges[1] ).getLength();
    vExchangeSize.X = ( edges[5] - edges[1] ).getLength();
    vExchangeSize *= NewtonToIrr;

    if ( !node )
    {
      CONSOLE.addx( "Could not load physics model: %s", "IMesh" );
      return;
    }

    //TEMP:
    node->setMaterialFlag( EMF_LIGHTING, ( bool )IRR.useLighting );
    node->setMaterialFlag( EMF_BACK_FACE_CULLING, false );
    node->setMaterialFlag( EMF_NORMALIZE_NORMALS, true );

    if ( APP.DebugMode > 1 )
    {
      node->setDebugDataVisible( true );
    } 

    bModCol = modifiableCollision;
    makeBody( node, bodyType, vExchangeSize, vScale, fMass, mesh, modifiableCollision );
    mesh->drop();
    if ( APP.DebugMode > 1 )
    {
      CONSOLE.addx( "assemblePhysics" );
    }
}

void CNewtonNode::PhysicsApplyForceAndTorque( const NewtonBody* body )
{
    dFloat mass, Ixx, Iyy, Izz;
    int i;

    NewtonBodyGetMassMatrix( body, &mass, &Ixx, &Iyy, &Izz );

    matrix4 mat;
    NewtonBodyGetMatrix( body, &mat.M[0] ); 

    // add da gravity
    dVector gravforce( 0, mass* WORLD.GetPhysics()->dGravity* IrrToNewton, 0 );
    NewtonBodyAddForce( body, &gravforce.m_x );

    CNewtonNode* newtonNode = 0;
    newtonNode = ( CNewtonNode * )NewtonBodyGetUserData( body );
    if ( !newtonNode )
    {
      return;
    }   

    newtonNode->fMass = mass;

    // set parents speed and check if parent is zombie
    if ( newtonNode->isAttached() )
    {
      vector3df vTemp = newtonNode->getParentAttachment()->getVelocity();
      NewtonBodySetVelocity( body, &vTemp.X );
      vTemp = newtonNode->getParentAttachment()->getOmega();
      NewtonBodySetOmega( body, &vTemp.X );

      if ( newtonNode->getParentAttachment()->isZombie() )
      {
        newtonNode->unAttachFromParent();
      }

      NewtonBodyGetVelocity( body, &newtonNode->vVelocity.X );
      NewtonBodyGetOmega( body, &newtonNode->vOmega.X );
      newtonNode->fVelocity = newtonNode->vVelocity.getLength();
      return;
    }

    // add da buoyancy
    if ( newtonNode->watercheckcount )
    {
      bool nowater = true;
      for ( i = 0; i < WORLD.GetMap()->GetZonesNum(); i++ )
      {
        if ( WORLD.GetMap()->GetZone( i )->getBox()->intersectsWithBox( newtonNode->getBoundingBox() ) )
        {
          NewtonBodyAddBuoyancyForce( body, mass / newtonNode->weightFactor / WORLD.GetMap()->GetZone( i )->getLiquidWeightFactor(),
                                                                                                //mass / newtonNode->weightFactor / WORLD.GetMap()->GetZone(i)->getLiquidWeightFactor(),
                    0.09f, 0.09f, &gravforce.m_x, GetBuoyancy, WORLD.GetMap()->GetZone( i ) );

          if ( !newtonNode->bInWater )
          {
            newtonNode->OnEnterWater( WORLD.GetMap()->GetZone( i )->getBox() );
          }

          newtonNode->bInWater = i + 1;
          nowater = false;
          //CONSOLE_FLOAT( newtonNode->weightFactor );
        }
      }
      if ( nowater )
      {
        if ( newtonNode->bInWater )
        {
          newtonNode->OnExitWater( WORLD.GetMap()->GetZone( newtonNode->bInWater - 1 )->getBox() );
        }
        newtonNode->bInWater = 0;
      }

      if ( newtonNode->bInWater ) // in water
      {
        //  NewtonBodySetLinearDamping( body, (1.0f - newtonNode->fLinDamp / 1000.0f) );
        //  vector3df vAng( (1.0f - newtonNode->fAngDamp / 1000.0f), (1.0f - newtonNode->fAngDamp / 1000.0f), (1.0f - newtonNode->fAngDamp / 1000.0f) );
        NewtonBodySetLinearDamping( body, ( newtonNode->fLinDamp * 10.0f ) );
        vector3df vAng( ( newtonNode->fAngDamp * 100.0f ), ( newtonNode->fAngDamp * 100.0f ), ( newtonNode->fAngDamp * 100.0f ) );
        NewtonBodySetAngularDamping( body, &vAng.X );

		//water circle
		if (!newtonNode->isUnderWater())
			if ( KERNEL.GetTicks() % 40 == 0 )
			{
			if ( newtonNode->vVelocity.Y > 0.09f )
			{
				f32 radius = newtonNode->node->getBoundingBox().getExtent().getLength() / 2;
				radius *= newtonNode->vVelocity.Y / 5.0f;
				vector3df vWaterPos = newtonNode->vWaterEntryPos;
				vWaterPos.X = newtonNode->getPosition().X;
				FACTORY->Effects.Create( "watercircle", vWaterPos, vWaterPos, 0.0f, radius * 0.0003f, 180 );
			}
			}

      }
      else
      {
        NewtonBodySetLinearDamping( body, newtonNode->fLinDamp );
        vector3df vAng( newtonNode->fAngDamp, newtonNode->fAngDamp, newtonNode->fAngDamp );
        NewtonBodySetAngularDamping( body, &vAng.X );
      }
    }
    newtonNode->watercheckcount = true;

    if ( !newtonNode->isZombie() )
    {
      newtonNode->PhysicsControl();
    }

    // align 2d pos
    if ( newtonNode->bFix2DPos )
    {
      float vel[3];
      NewtonBodyGetVelocity( body, vel );
      vel[2] = 0.0f;
      NewtonBodySetVelocity( body, vel );

      newtonNode->setPosition( vector3df( 0, 0, 0 ), false, false, true );
    }
    // align 2d rot
    if ( newtonNode->bFix2DRot )
    {
      float omega[3];
      NewtonBodyGetOmega( body, omega );
      omega[0] = omega[1] = 0.0f;
      NewtonBodySetOmega( body, omega );
    }

    NewtonBodyGetVelocity( body, &newtonNode->vVelocity.X );
    NewtonBodyGetOmega( body, &newtonNode->vOmega.X );
    newtonNode->fVelocity = newtonNode->vVelocity.getLength();

    // enlarge collision according to velocity
    //if ( newtonNode->bModCol )
    //{
    //  f32 fS = newtonNode->fVelocity / 15.0f;
    //  newtonNode->setCollisionSize( vector3df(1.0f + fS, 
    //      1.0f + fS, 1.0f + fS) );
    //  //CONSOLE.addx("vel %f", newtonNode->fVelocity );
    //}
}

void CNewtonNode::PhysicsSetTransform( const NewtonBody* body, const dFloat* matrix )
{
    CNewtonNode* newtonNode = 0;
    newtonNode = ( CNewtonNode * )NewtonBodyGetUserData( body );
    if ( !newtonNode )
    {
      return;
    }   

    static matrix4 mat;
    memcpy( mat.M, matrix, sizeof( s32 ) * 16 );

    if ( !newtonNode->isAttached() )
    {
      newtonNode->transformMatrix = mat;
    }

    newtonNode->PhysicsTransform( newtonNode->transformMatrix );

    newtonNode->UpdateChildAttachments();
}


NewtonBody* CNewtonNode::makeBody( ISceneNode* node, BodyType bodyType, vector3df vSize, vector3df vScale, float fMass, IMesh* iMesh, bool modifiableCollision )
{
    NewtonCollision* collision;
    vSize *= IrrToNewton;
    vSize *= vScale;

    matrix4 offset;
    vector3df vOff;
    int numVerts = 0;
    int numInds = 0;
    float* vertArray;   
    int cMeshBuffer, j, i;
    IMeshBuffer* mb;

    // calculate the offset of the mesh
    if ( bodyType < 2 )
    {
      for ( cMeshBuffer = 0; cMeshBuffer < iMesh->getMeshBufferCount(); cMeshBuffer++ )
      {
        mb = iMesh->getMeshBuffer( cMeshBuffer );

        video::S3DVertex* mb_vertices = ( irr::video::S3DVertex* )mb->getVertices();
        for ( j = 0; j < mb->getVertexCount(); j += 1 )
        {
          vOff += mb_vertices[j].Pos;
        }

        numVerts += mb->getVertexCount();
      }
      vOff /= numVerts;
      vOff *= vScale;
      offset.setTranslation( vOff );
    }

    switch ( bodyType )
    {
      case BODY_BOX:
        collision = NewtonCreateBox( WORLD.GetPhysics()->nWorld, vSize.X, vSize.Y, vSize.Z, &offset.M[0] );
        break;
      case BODY_SPHERE:
        collision = NewtonCreateSphere( WORLD.GetPhysics()->nWorld, vSize.X / 2, vSize.Y / 2, vSize.Z / 2, &offset.M[0] );
        break;
      case BODY_HULL:
        vector3df vP; //
        array<f32> tempArray;
        tempArray.clear();

        for ( cMeshBuffer = 0; cMeshBuffer < iMesh->getMeshBufferCount(); cMeshBuffer++ )
        {
          mb = iMesh->getMeshBuffer( cMeshBuffer );

          video::S3DVertex* mb_vertices = ( irr::video::S3DVertex* )mb->getVertices();

          u16* mb_indices = mb->getIndices();
          numInds = mb->getIndexCount();
          //    if (APP.DebugMode > 1)
          //        CONSOLE.addx("makeBody getIndexCount %i", numInds );

          // add each triangle from the mesh
          numVerts += numInds;
          for ( j = 0; j < numInds; j += 1 )
          {
            // to make things easier, here we can use engine data type
            vP = mb_vertices[mb_indices[j]].Pos * vScale * HULL_SIZE_MODIFER;                       

            tempArray.push_back( vP.X );
            tempArray.push_back( vP.Y );
            tempArray.push_back( vP.Z );
          }
        }

        // HACK: anti-crash
        f32 size = 0.01f;
        vP = vector3df( size, size, size );
        tempArray.push_back( vP.X ); tempArray.push_back( vP.Y ); tempArray.push_back( vP.Z );
        vP = vector3df( -size, -size, -size );
        tempArray.push_back( vP.X ); tempArray.push_back( vP.Y ); tempArray.push_back( vP.Z );
        vP = vector3df( size, -size, -size );
        tempArray.push_back( vP.X ); tempArray.push_back( vP.Y ); tempArray.push_back( vP.Z );

        collision = NewtonCreateConvexHull( WORLD.GetPhysics()->nWorld, tempArray.size() / 3, &tempArray[0], 3 * sizeof( float ), NULL );
        //  if (APP.DebugMode > 1)
        //      CONSOLE.addx("makeBody NewtonCreateConvexHull: %i vertn %i collision %i box %f",
        //          iMesh->getMeshBufferCount(),  numVerts, collision, iMesh->getBoundingBox().getExtent() );
        break;
    }

    if ( modifiableCollision )
    {
      newtonCollision = NewtonCreateConvexHullModifier( WORLD.GetPhysics()->nWorld, collision );
      NewtonReleaseCollision( WORLD.GetPhysics()->nWorld, collision );
    }
    else
    {
      newtonCollision = collision;
    }

    body = NewtonCreateBody( WORLD.GetPhysics()->nWorld, newtonCollision );

    vector3df vInertia;
    vInertia.X = fMass * ( vSize.Y * vSize.Y + vSize.Z * vSize.Z ) / 12.0f;
    vInertia.Y = fMass * ( vSize.X * vSize.X + vSize.Z * vSize.Z ) / 12.0f;
    vInertia.Z = fMass * ( vSize.X * vSize.X + vSize.Y * vSize.Y ) / 12.0f;

    NewtonBodySetMassMatrix( body, fMass, vInertia.X, vInertia.Y, vInertia.Z );

    //if (!node)
    //{
    //  node = IRR.smgr->addCubeSceneNode();
    //  node->setScale(vSize);
    //}
    //node->setMaterialFlag( EMF_LIGHTING, IRR.useLighting );

    setUserData();

    // 2d joint
    //joint2d = new CustomJoint2D( body, dVector(0.0, 0.0, 1.0) );

    NewtonReleaseCollision( WORLD.GetPhysics()->nWorld, newtonCollision );

	node->updateAbsolutePosition();

	aabbox3d<f32> box = getBoundingBox();
	static vector3df edges[8];
	box.getEdges( edges );
	vBoxSize.Z = ( edges[3] - edges[1] ).getLength();
	vBoxSize.Y = ( edges[0] - edges[1] ).getLength();
	vBoxSize.X = ( edges[5] - edges[1] ).getLength();

    return body;
}


NewtonBody* CNewtonNode::makeBody( ISceneNode* node, BodyType bodyType, vector3df vSize, vector3df vScale, vector3df vColOffset, float fMass, IAnimatedMesh* iMesh, bool modifiableCollision )
{
    NewtonCollision* collision;
    vSize *= IrrToNewton;
    vSize *= vScale;

    matrix4 offset;
    vector3df vOff;
    int numVerts = 0;
    float* vertArray;   
    int cMeshBuffer, j, i;
    IMeshBuffer* mb;
    int buffCount = iMesh->getFrameCount();

    // calculate the offset of the mesh
    if ( bodyType < 2 )
    {
      for ( int i = 0; i < buffCount; i ++ )
      {
        for ( cMeshBuffer = 0; cMeshBuffer < iMesh->getMesh( i )->getMeshBufferCount(); cMeshBuffer++ )
        {
          mb = iMesh->getMesh( i )->getMeshBuffer( cMeshBuffer );

          video::S3DVertex* mb_vertices = ( irr::video::S3DVertex* )mb->getVertices();
          for ( j = 0; j < mb->getVertexCount(); j += 1 )
          {
            vOff += mb_vertices[j].Pos;
          }

          numVerts += mb->getVertexCount();
        }
      }
      vOff /= numVerts;
      vOff *= vScale;
      offset.setTranslation( vOff );
    }
    offset.setTranslation( vColOffset + vOff );

    switch ( bodyType )
    {
      case BODY_BOX:
        collision = NewtonCreateBox( WORLD.GetPhysics()->nWorld, vSize.X, vSize.Y, vSize.Z, &offset.M[0] );
        break;
      case BODY_SPHERE:
        collision = NewtonCreateSphere( WORLD.GetPhysics()->nWorld, vSize.X / 2, vSize.Y / 2, vSize.Z / 2, &offset.M[0] );
        break;
      case BODY_HULL:
        IAnimatedMesh* treemesh = iMesh;
        vector3df vP; //

        // calc all vertices count
        numVerts = 0;
        i = 0;
        //for (i = 0; i < buffCount; i ++) 
        for ( cMeshBuffer = 0; cMeshBuffer < treemesh->getMesh( i )->getMeshBufferCount(); cMeshBuffer++ )
        {
          numVerts += treemesh->getMesh( i )->getMeshBuffer( cMeshBuffer )->getVertexCount();
        }
        vertArray = new float[numVerts * 3];
        int vcount = 0;

        //for (i = 0; i < buffCount; i ++) 
        i = 0;
        {
            //CONSOLE.addx("BODY_HULL cMeshBuffer b %i mb %i", buffCount, treemesh->getMesh(i)->getMeshBufferCount() );

            for ( cMeshBuffer = 0; cMeshBuffer < treemesh->getMesh( i )->getMeshBufferCount(); cMeshBuffer++ )
            {
              mb = treemesh->getMesh( i )->getMeshBuffer( cMeshBuffer );

              video::S3DVertex* mb_vertices = ( irr::video::S3DVertex* )mb->getVertices();

              u16* mb_indices = mb->getIndices();

              // add each triangle from the mesh
              for ( j = 0; j < mb->getVertexCount(); j += 1 )
              {
                // to make things easier, here we can use engine data type
                vP = mb_vertices[j].Pos * vScale * HULL_SIZE_MODIFER;                       

                vertArray[vcount * 3] = vP.X;
                vertArray[vcount * 3 + 1] = vP.Y;
                vertArray[vcount * 3 + 2] = vP.Z;   
                vcount++;
              }
            }
        }

        collision = NewtonCreateConvexHull( WORLD.GetPhysics()->nWorld, numVerts, vertArray, 3 * sizeof( float ), &offset.M[0] );
        delete[] vertArray;
        break;
    }

    if ( modifiableCollision )
    {
      newtonCollision = NewtonCreateConvexHullModifier( WORLD.GetPhysics()->nWorld, collision );
      NewtonReleaseCollision( WORLD.GetPhysics()->nWorld, collision );
    }
    else
    {
      newtonCollision = collision;
    }

    body = NewtonCreateBody( WORLD.GetPhysics()->nWorld, newtonCollision );

    vector3df vInertia;
    vInertia.X = fMass * ( vSize.Y * vSize.Y + vSize.Z * vSize.Z ) / 12.0f;
    vInertia.Y = fMass * ( vSize.X * vSize.X + vSize.Z * vSize.Z ) / 12.0f;
    vInertia.Z = fMass * ( vSize.X * vSize.X + vSize.Y * vSize.Y ) / 12.0f;

    NewtonBodySetMassMatrix( body, fMass, vInertia.X, vInertia.Y, vInertia.Z );

    //if (!node)
    //{
    //  node = IRR.smgr->addCubeSceneNode();
    //  node->setScale(vSize);
    //}
    //node->setMaterialFlag( EMF_LIGHTING, IRR.useLighting );

    setUserData();

    // 2d joint
    //  joint2d = new CustomJoint2D( body, dVector(0.0, 0.0, 1.0) );

    NewtonReleaseCollision( WORLD.GetPhysics()->nWorld, newtonCollision );

	node->updateAbsolutePosition();

	aabbox3d<f32> box = getBoundingBox();
	static vector3df edges[8];
	box.getEdges( edges );
	vBoxSize.Z = ( edges[3] - edges[1] ).getLength();
	vBoxSize.Y = ( edges[0] - edges[1] ).getLength();
	vBoxSize.X = ( edges[5] - edges[1] ).getLength();

    return body;
}

vector3df CNewtonNode::getPosition()
{
    matrix4 mat;
    NewtonBodyGetMatrix( body, &mat.M[0] ); 

    return mat.getTranslation();
}

vector3df CNewtonNode::getRotation()
{
    matrix4 mat;
    NewtonBodyGetMatrix( body, &mat.M[0] ); 

    return mat.getRotationDegrees();
}

void CNewtonNode::setPosition( vector3df vPos, bool transformX, bool transformY, bool transformZ )
{
    matrix4 mat;
    NewtonBodyGetMatrix( body, &mat.M[0] ); 

    vector3df vTemp = mat.getTranslation();
    if ( transformX )
    {
      vTemp.X = vPos.X;
    }
    if ( transformY )
    {
      vTemp.Y = vPos.Y;
    }
    if ( transformZ )
    {
      vTemp.Z = vPos.Z;
    }
    mat.setTranslation( vTemp );

    NewtonBodySetMatrix( body, &mat.M[0] );
}

void CNewtonNode::setRotation( vector3df vRot, bool transformX, bool transformY, bool transformZ )
{
    matrix4 mat;
    NewtonBodyGetMatrix( body, &mat.M[0] ); 

    vector3df vTemp = mat.getRotationDegrees();
    if ( transformX )
    {
      vTemp.X = vRot.X;
    }
    if ( transformY )
    {
      vTemp.Y = vRot.Y;
    }
    if ( transformZ )
    {
      vTemp.Z = vRot.Z;
    }
    mat.setRotationDegrees( vTemp );

    NewtonBodySetMatrix( body, &mat.M[0] );
}

vector3df CNewtonNode::getNodeCenter()
{
    return node->getAbsolutePosition();
}

void CNewtonNode::setUserData()
{
    NewtonBodySetMaterialGroupID( body, CNewton::defaultnodeID );
    //TEMP::
    NewtonBodySetAutoFreeze( this->body, 0 );

    NewtonBodySetTransformCallback( body, PhysicsSetTransform );
    NewtonBodySetForceAndTorqueCallback( body, PhysicsApplyForceAndTorque );
    NewtonBodySetUserData( body, this );

    weightFactor = NewtonConvexCollisionCalculateVolume( NewtonBodyGetCollision( body ) );
}

void CNewtonNode::PhysicsTransform( matrix4 matrix )
{
    node->setRotation( matrix.getRotationDegrees() );
    node->setPosition( matrix.getTranslation() );
    node->updateAbsolutePosition();

    core::list<ISceneNode*>::Iterator it;
    for ( it = node->getChildren().begin(); it != node->getChildren().end(); it++ )
    {
		( *it )->updateAbsolutePosition();
    }
}

void CNewtonNode::childAttachmentPhysicsTransform( matrix4 matrix )
{
    transformMatrix = matrix;
    setPosition( matrix.getTranslation() );
    setRotation( matrix.getRotationDegrees() );
}

void CNewtonNode::attachToParentNode( CNewtonNode* parentnode, void* data )
{
    if ( parentnode->onChildAttached( this, data ) )
    {
      parentAttachment = parentnode;
      //NewtonWorldFreezeBody(WORLD.GetPhysics()->nWorld, body);
      NewtonBodySetMaterialGroupID( body, CNewton::nocollisionID );
    }
}

void CNewtonNode::unAttachFromParent()
{
    if ( !parentAttachment )
    {
      return;
    }

    parentAttachment->onChildUnAttached( this );
    parentAttachment = NULL;

    NewtonWorldUnfreezeBody( WORLD.GetPhysics()->nWorld, body );
    //TEMP:
    NewtonBodySetMaterialGroupID( body, CNewton::defaultnodeID );
}

bool CNewtonNode::onChildAttached( CNewtonNode* what, void* data )
{
    //int size = childAttachments.size();
    int i, j;
    bool occupied;

    //if ( size <= MountPlaces.size() )
    //TODO: check if the place is not occupied
    for ( i = 0; i < MountPlaces.size(); i++ )
    {
      occupied = false;
      if ( MountPlaces[i]->placetype == what->getType() )
      {
        for ( j = 0; j < childAttachments.size(); j++ )
        {
          if ( childAttachments[j]->getParentAttachmentPos() == MountPlaces[i]->vMountPos )
          {
            occupied = true;
          }
        }

        if ( !occupied )
        {
          childAttachments.push_back( what );
          what->setParentAttachmentPos( MountPlaces[i]->vMountPos );
          what->setParentAttachmentRot( MountPlaces[i]->vMountRot );
          return true;
        }
      }
    }

    return false;
    //what->unAttachFromParent();
    //vector3df vDefault(0,0,0);
    //what->setParentAttachmentPos( vDefault );
    //what->setParentAttachmentRot( vDefault );
}

void CNewtonNode::onChildUnAttached( CNewtonNode* what )
{
    childAttachments.erase( childAttachments.binary_search( what ) );
}

int CNewtonNode::GetBuoyancy( const int collisionID, void* context, const dFloat* globalSpaceMatrix, dFloat* globalSpacePlane )
{
    // the normal for the plane is just a unit vector.
    globalSpacePlane[0] = 0.0f;
    globalSpacePlane[1] = -1.0f;
    globalSpacePlane[2] = 0.0f;

    // the distance along this normal, to the origin.
    static matrix4 mat;
    memcpy( mat.M, globalSpaceMatrix, sizeof( s32 ) * 16 );

    CMap_Zone* zone = ( CMap_Zone* )context;
    //globalSpacePlane[3] = ( (zone->getBox()->getCenter().Y - zone->getSize()->Y/2) - (mat.getTranslation().Y)  ) * IrrToNewton;
    globalSpacePlane[3] = ( zone->getBox()->getCenter().Y - zone->getSize()->Y / 2 ) - 0.9f;

    //CONSOLE_FLOAT(globalSpacePlane[3]);
    return 1;
}

irr::core::aabbox3df CNewtonNode::getBoundingBox()
{
    //CONSOLE.addx("CNewtonNode::getBoundingBox()  -- %i %i", bZombie, node);
    if ( node )
    {
      //aabbox3df a = node->getTransformedBoundingBox(); ;
      //a.MinEdge *= 0.7f;
      //a.MaxEdge *= 0.7f;
      //return a;
      return node->getTransformedBoundingBox();
    }
    else
    {
      aabbox3df a;
      return a;
    }
} 

void CNewtonNode::OnEnterWater( aabbox3df* zonebox )
{
    if ( parentAttachment )
    {
      return;
    }

    // create water splash effect
    vector3df vPos = getPosition();

    static vector3df edges[8];
    zonebox->getEdges( edges );
    vPos.Y = edges[0].Y;
    vWaterEntryPos = vPos;

    f32 radius = getBoundingBox().getExtent().getLength() / 2;
    radius *= vVelocity.getLength() / 15.0f;
    FACTORY->Effects.Create( "splash", vPos, vPos, 0.0003f, radius, 80 );
    FACTORY->Effects.Create( "surfacesplash", vPos, vPos, 0.0f, radius * 0.05f, 130 );
    SOUND.playSound( "Sounds/water_fall.wav", vPos, vPos, radius, 1.0f - radius / 140.0f );
}

void CNewtonNode::OnExitWater( aabbox3df* zonebox )
{
    OnEnterWater( zonebox );
}

void CNewtonNode::UpdateChildAttachments()
{
    vector3df vP, vR;
    matrix4 rotmat, childmat; 
    for ( int i = 0; i < childAttachments.size(); i++ )
    {
      vP = childAttachments[i]->getParentAttachmentPos();
      vR = childAttachments[i]->getParentAttachmentRot();
      rotmat.setRotationDegrees( getRotation() );
      rotmat.transformVect( vP ); 
      childmat.setTranslation( transformMatrix.getTranslation() + vP );
      childmat.setRotationDegrees( transformMatrix.getRotationDegrees() + vR );

      childAttachments[i]->childAttachmentPhysicsTransform( childmat );
    }
}

bool CNewtonNode::isChildAttached( int nodetype )
{
    for ( int i = 0; i < childAttachments.size(); i++ )
    {
      if ( childAttachments[i]->getType() == nodetype )
      {
        return true;
      }
    }

    return false;
}

CNewtonNode* CNewtonNode::getAttachedChild( int nodetype )
{
    if ( nodetype < 0 )
    {
      if ( childAttachments.size() )
      {
        return childAttachments[0];
      }
      return NULL;
    }

    for ( int i = 0; i < childAttachments.size(); i++ )
    {
      if ( childAttachments[i]->getType() == nodetype )
      {
        return childAttachments[i];
      }
    }
    return NULL;
}

void CNewtonNode::addMountPlace( vector3df vPos, vector3df vRot, int nodeclasstype )
{
    MountPlace* mp = new MountPlace;
    mp->vMountPos = vPos;
    mp->vMountRot = vRot;
    mp->placetype = nodeclasstype;
    MountPlaces.push_back( mp );
}

CNewtonNode* CNewtonNode::DropChild( int nodetype )
{
    CNewtonNode* childnode = getAttachedChild( nodetype );

    if ( childnode )
    {
      childnode->unAttachFromParent();
      return childnode;
    }
    return NULL;
}

CNewtonNode* CNewtonNode::GetGreatestParent()
{
    CNewtonNode* n = parentAttachment;

    if ( n )
    {
      while ( n->parentAttachment )
      {
        n = n->parentAttachment;
      }
    }

    return n;
}

void CNewtonNode::setCollisionSize( vector3df vScale )
{
    matrix4 crouchmat; 
    crouchmat.setScale( vScale );

    //f32 bone1x = model->getCurrentModel()->getSkeleton()->getBone(33)->getTranslationAbsolute().x;
    //f32 bone2x = model->getCurrentModel()->getSkeleton()->getBone(bone2MouseBoneID)->getTranslationAbsolute().x;

    //f32 fSX = facing * 0.5f * (bone1x - bone2x);
    //crouchmat.setTranslation( vector3df( fSX, 0, 0 ) );
    //if ( abs(bone1x - bone2x) >  0.0f )
    //  fSX = fSX;
    //else
    //  fSX = 1.0f;
    ////CONSOLE_FLOAT( fSX );

    //setPosition( vector3df( getPosition().X + facing*( (vScale.X+fSX) - vHullScale.X), 
    //  getPosition().Y - (vScale.Z - vHullScale.Z), 0), true, true, false );

    setPosition( vector3df( 0, getPosition().Y - ( vScale.Z - vHullScale.Z ), 0 ), false, true, false );

    NewtonConvexHullModifierSetMatrix( newtonCollision, crouchmat.M );
    vHullScale = vScale;
}

void CNewtonNode::PhysicsCollision( const NewtonMaterial* material, const NewtonContact* contact, NewtonBody* cbody )
{
    dFloat mass, Ixx, Iyy, Izz;
    NewtonBodyGetMassMatrix( cbody, &mass, &Ixx, &Iyy, &Izz );

    f32 fSpeed = abs( NewtonMaterialGetContactNormalSpeed( material, contact ) );

    if ( ( mass == 0.0f ) && ( fSpeed > 0.5f ) ) //map
    {
      f32 radius = getBoundingBox().getExtent().getLength() / 2;
      radius *= fSpeed / 3500.0f;

      vector3df vPos, vNorm;

      NewtonMaterialGetContactPositionAndNormal( material, &vPos.X, &vNorm.X );

      FACTORY->Effects.Create( "dust", vPos, vPos, 0.5f, radius * 200.0f, 120 );
    }
}

bool CNewtonNode::isUnderWater()
{
	if (bInWater)
	{
//		CONSOLE.addx("p %f; w %f", getPosition().Y - V.Y/2, vBoxSize.Y/2);
		if (getPosition().Y - vBoxSize.Y/2 > vWaterEntryPos.Y)
			return true;
	}

	return false;
}

///////////////////////////////////////////////////////


ISceneNode* loadNode_returnSize( const c8* modelFilename, vector3df vScale, f32 exchange, vector3df& vExchangeSize, IMesh*& iMesh )
{
    //  CInterpolatedSceneNode* cnode = new CInterpolatedSceneNode( IRR.smgr->getRootSceneNode(), IRR.smgr, -1);

    ISceneNode* node = 0;
    IAnimatedMesh* animMesh;
    iMesh = 0;
    animMesh = IRR.smgr->getMesh( APP.useFile( modelFilename ).c_str() );
    iMesh = animMesh->getMesh( 0 );
    if ( !iMesh )
    {
      return NULL;
    }

    node = IRR.smgr->addMeshSceneNode( iMesh );
    node->setScale( vScale );
    aabbox3d<f32> box = node->getBoundingBox();
    static vector3df edges[8];
    box.getEdges( edges );

    vExchangeSize.Z = ( edges[3] - edges[1] ).getLength();
    vExchangeSize.Y = ( edges[0] - edges[1] ).getLength();
    vExchangeSize.X = ( edges[5] - edges[1] ).getLength();
    vExchangeSize *= exchange;

    //TEMP:
    node->setMaterialFlag( EMF_LIGHTING, ( bool )IRR.useLighting );
    node->setMaterialFlag( EMF_BACK_FACE_CULLING, true );
    node->setMaterialFlag( EMF_NORMALIZE_NORMALS, true );

    if ( APP.DebugMode > 1 )
    {
      node->setDebugDataVisible( true );
    } 

    //CInterpolatedSceneNode(ISceneNode* parent, ISceneManager* mgr, s32 id,
    //  const core::vector3df& position = core::vector3df(0,0,0),
    //  const core::vector3df& rotation = core::vector3df(0,0,0),
    //  const core::vector3df& scale = core::vector3df(1.0f, 1.0f, 1.0f))
    //  : IAnimatedMeshSceneNode(parent, mgr, id, position, rotation, scal

    //static_cast<CInterpolatedSceneNode*>(node);

    return node;
}

IAnimatedMeshSceneNode* loadNode_returnSize( const c8* modelFilename, vector3df vScale, f32 exchange, vector3df& vExchangeSize, IAnimatedMesh*& iMesh )
{
    IAnimatedMeshSceneNode* node = 0;
    iMesh = 0;
    iMesh = IRR.smgr->getMesh( APP.useFile( modelFilename ).c_str() );
    if ( !iMesh )
    {
      return NULL;
    }

    node = IRR.smgr->addAnimatedMeshSceneNode( iMesh );
    node->setScale( vScale );

    //node->addShadowVolumeSceneNode();

    aabbox3d<f32> box = node->getBoundingBox();
    static vector3df edges[8];
    box.getEdges( edges );

    vExchangeSize.Z = ( edges[3] - edges[1] ).getLength();
    vExchangeSize.Y = ( edges[0] - edges[1] ).getLength();
    vExchangeSize.X = ( edges[5] - edges[1] ).getLength();
    vExchangeSize *= exchange;

    //TEMP:
    node->setMaterialFlag( EMF_LIGHTING, ( bool )IRR.useLighting );
    node->setMaterialFlag( EMF_BACK_FACE_CULLING, true );
    node->setMaterialFlag( EMF_NORMALIZE_NORMALS, true );

	// add shadow
	node->addShadowVolumeSceneNode();	

    if ( APP.DebugMode > 1 )
    {
      node->setDebugDataVisible( true );
    } 

    //iMesh->drop();

    return node;
}
