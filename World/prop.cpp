#include "prop.h"

////////////////////////////////////////////
// CProp 
////////////////////////////////////////////

//CProp::CProp( const c8* modelFilename, int timeToLive, BodyType bodyType, vector3df vScale, float fMass, f32 parentMass, bool breakable,
//           f32 splitFactor int maxSplitsNum )
//: CNewtonNode( modelFilename, bodyType, vScale, fMass)
//{
//  Reset();
//
//  vNodeScale = vScale;
//  bBreakable = breakable;
//  fParentMass = parentMass;
//  this->bodyType = bodyType;
//
//  setDebugText( modelFilename );
//}

CProp::CProp( IMesh* mesh, int timeToLive, BodyType bodyType, vector3df vScale, float fMass, bool breakable, f32 splitFactor, int maxSplitsNum ) : CNewtonNode( mesh, bodyType, vScale, fMass )
{
    Reset();

    vNodeScale = vScale;
    bBreakable = breakable;
    this->bodyType = bodyType;
    fSplitFactor = splitFactor;
    maxSplits = maxSplitsNum;
    alive = timeOut = timeToLive;
    fHealth = 100.0f;

    setDebugText( "Prop" );
}

CProp::~CProp()
{
}

void CProp::Reset()
{
    //  CNewtonNode::Reset();
    CEntity::Reset();

    NewtonBodySetAutoFreeze( this->body, 1 );

    bZombie = true;
    type = NODECLASS_PROP;
    bFix2DRot = false;
}

void CProp::Think()
{
    CEntity::Think();

    // update the debug position
    if ( APP.DebugMode )
    {
      setDebugPos( IRR.smgr->getSceneCollisionManager()->getScreenCoordinatesFrom3DPosition( getNodeCenter(), IRR.smgr->getActiveCamera() ) );
    }
}

void CProp::Render()
{
    CEntity::Render();
}

void CProp::PhysicsCollision( const NewtonMaterial* material, const NewtonContact* contact, NewtonBody* cbody )
{
    CNewtonNode::PhysicsCollision( material, contact, cbody );

    dFloat mass, Ixx, Iyy, Izz;
    NewtonBodyGetMassMatrix( cbody, &mass, &Ixx, &Iyy, &Izz );


    //vector3df vPointVel = WORLD.GetPhysics()->getPointVelocity( body, vPoint );
    //CONSOLE_FLOAT(fSpeed);
    if ( ( ( mass == 0.0f ) || ( mass > fMass ) ) && ( bBreakable ) )
    {
      vector3df vPoint, vNormal;
      NewtonMaterialGetContactPositionAndNormal( material, &vPoint.X, &vNormal.X );

      vector3df vPointVel = vVelocity;
      f32 fSpeed = vPointVel.getLength();
      if ( fSpeed > ( fMass / fSplitFactor ) )
      {
        vPointVel.rotateXYBy( 90, vector3df( 0, 0, 0 ) );
        BreakAtPoint( vPoint, ( vPointVel ), true );// + vPointVel.normalize()

        if ( mass != 0.0f )
        {
          CNewtonNode* n = ( CNewtonNode* )NewtonBodyGetUserData( cbody );
          if ( fMass >= mass )
          {
            if ( n->getType() == NODECLASS_PROP )
            {
              CProp* prop = static_cast<CProp*>( n );
              prop->BreakAtPoint( vPoint, ( -vPointVel ), true );
            }
          }
        }
      }
    }


    //HACK
    //vector3df vel, omg;
    //NewtonBodyGetVelocity(body, &vel.X);
    //vel *= 0.9;
    //NewtonBodySetVelocity(body, &vel.X);
    //NewtonBodyGetOmega(body, &omg.X);
    //omg *= 0.9;
    //NewtonBodySetOmega(body, &omg.X);
}


void CProp::BreakAtPoint( vector3df vCutPos, vector3df vCutNormal, bool breakableChildren )
{
    if ( !bBreakable )
    {
      return;
    }
    bBreakable = false;
    if ( maxSplits < 1 )
    {
      return;
    }
    bCanDie = true;

    plane3df cutplane;
    vector3df vN = vCutNormal;

    vN.rotateXYBy( -90, vector3df( 0, 0, 0 ) );
    vN.X *= -1;
    vN.normalize();
    vCutPos = vCutPos - getPosition();
    //vCutPos *= 0.85f;

    matrix4 rotMat;
    rotMat.setRotationDegrees( getRotation() );
    rotMat.transformVect( vCutPos );
    rotMat.transformVect( vN );

    vCutPos.X /= vNodeScale.X;
    vCutPos.Y /= vNodeScale.Y;
    vCutPos.Z /= vNodeScale.Z;

    cutplane.setPlane( vCutPos, vN );

    CProp* p1, * p2;

    IMesh* fMesh, * bMesh;
    fMesh = getSlicedMesh( iNodeMesh, bMesh, cutplane );

    vVelocity *= 0.85f;

    if ( fMesh )
    {
      p1 = new CProp( fMesh, timeOut / 2, ( BodyType )bodyType, vNodeScale, fMass / 2.0f, breakableChildren, fSplitFactor, maxSplits - 1 );
      p1->setPosition( getPosition() );
      p1->setRotation( getRotation() );
      NewtonBodySetVelocity( p1->body, &vVelocity.X );
      NewtonBodySetOmega( p1->body, &vOmega.X );
      p1->node->setMaterialTexture( 0, node->getMaterial( 0 ).Texture1 );
    }

    if ( bMesh )
    {
      p2 = new CProp( bMesh, timeOut / 2, ( BodyType )bodyType, vNodeScale, fMass / 2.0f, breakableChildren, fSplitFactor, maxSplits - 1 );
      p2->setPosition( getPosition() );
      p2->setRotation( getRotation() );
      NewtonBodySetVelocity( p2->body, &vVelocity.X );
      NewtonBodySetOmega( p2->body, &vOmega.X );
      p2->node->setMaterialTexture( 0, node->getMaterial( 0 ).Texture1 );
    }
}


///////////////////////////////////////////////////////

///////////////////////////////////////////////////////


IMesh* getCrushedMesh( IMesh* iMesh, plane3df plane )
{
    SMesh* smesh = new SMesh;
    S3DVertex vertex;
    s32 i, j;
    f32 bDotPlane, fDistSum;
    vector3df vN;

    //plane.Normal.normalize();

    // copy the mesh buffer to a smesh so we can manipulate it

    for ( j = 0; j < iMesh->getMeshBufferCount(); j++ )
    {
      SMeshBuffer* smeshbuf = new SMeshBuffer;

      irr::scene::IMeshBuffer* pMeshBuffer = iMesh->getMeshBuffer( j ); 

      s32 vcount = pMeshBuffer->getVertexCount(); 
      irr::video::S3DVertex* currentVertex = ( irr::video::S3DVertex* )pMeshBuffer->getVertices();
      for ( i = 0; i < vcount; i++ )
      {
        smeshbuf->Vertices.push_back( currentVertex[i] );
      }
      s32 icount = pMeshBuffer->getIndexCount();
      u16* currentIndex = ( u16* )pMeshBuffer->getIndices();
      for ( i = 0; i < icount; i++ )
      {
        smeshbuf->Indices.push_back( currentIndex[i] );
      }
      smeshbuf->Material = pMeshBuffer->getMaterial();

      // clip the vertices to the plane

      for ( i = 0; i < vcount; i++ )
      {
        if ( plane.classifyPointRelation( smeshbuf->Vertices[i].Pos ) != ISREL3D_BACK )
        {
          vN = plane.Normal * plane.getDistanceTo( smeshbuf->Vertices[i].Pos );
          smeshbuf->Vertices[i].Pos -= vN;
        }
      }

      // delete polygons on plane

      for ( i = 0; i < smeshbuf->Indices.size(); i += 3 )
      {
        //CONSOLE_FLOAT(plane.getDistanceTo( smeshbuf->Vertices[smeshbuf->Indices[i+0]].Pos ));
        //CONSOLE_FLOAT(plane.getDistanceTo( smeshbuf->Vertices[smeshbuf->Indices[i+1]].Pos ));
        //CONSOLE_FLOAT(plane.getDistanceTo( smeshbuf->Vertices[smeshbuf->Indices[i+2]].Pos ));

        fDistSum = plane.getDistanceTo( smeshbuf->Vertices[smeshbuf->Indices[i + 0]].Pos ) + plane.getDistanceTo( smeshbuf->Vertices[smeshbuf->Indices[i + 1]].Pos ) + plane.getDistanceTo( smeshbuf->Vertices[smeshbuf->Indices[i + 2]].Pos );

        //CONSOLE_FLOAT(fDistSum);

        if ( fDistSum < 0.00001f )
        {
          smeshbuf->Indices.erase( i, 3 );
          if ( i > 2 )
          {
            i -= 3;
          }
        }

        //smeshbuf->Vertices[smeshbuf->Indices[i+2]].Pos.X = smeshbuf->Vertices[smeshbuf->Indices[i+2]].Pos.X;
      }

      smesh->addMeshBuffer( smeshbuf );
      smeshbuf->drop();
    }

    return smesh;
}

///////////////////////////////////////////////////////

IMesh* getSlicedMesh( IMesh* sourceMesh, IMesh*& backMesh, plane3df plane )
{
    SMesh* smesh = new SMesh;
    SMesh* smesh2 = new SMesh;
    irr::scene::IMeshBuffer* pMeshBuffer;
    SMeshBuffer* smeshbuf;
    SMeshBuffer* smeshbuf2;
    S3DVertex vertex;
    s32 i, j;
    f32 bDotPlane, fDistSum, fControlSum, fControlSum2;
    vector3df vN, vMiddle;
    int count1, count2, onplane1, onplane2, allvertices;

    if ( APP.DebugMode > 1 )
    {
      CONSOLE.addx( "getSlicedMesh  %i", sourceMesh->getMeshBufferCount() );
    }

    smesh->BoundingBox.reset( vector3df( 0, 0, 0 ) );
    smesh2->BoundingBox.reset( vector3df( 0, 0, 0 ) );

    plane.Normal.normalize();

    allvertices = 0;

    if ( sourceMesh->getBoundingBox().classifyPlaneRelation( plane ) == ISREL3D_FRONT )
    {
      smesh = IRR.smgr->getMeshManipulator()->createMeshCopy( sourceMesh );
      backMesh = NULL;
      if ( APP.DebugMode > 1 )
      {
        CONSOLE.add( "getSlicedMesh ISREL3D_FRONT" );
      }
      return smesh;
    }
    if ( sourceMesh->getBoundingBox().classifyPlaneRelation( plane ) == ISREL3D_BACK )
    {
      smesh2 = IRR.smgr->getMeshManipulator()->createMeshCopy( sourceMesh );
      backMesh = smesh2;
      if ( APP.DebugMode > 1 )
      {
        CONSOLE.add( "getSlicedMesh ISREL3D_BACK" );
      }
      return NULL;
    }
    if ( APP.DebugMode > 1 )
    {
      CONSOLE.add( "getSlicedMesh  ISREL3D_CLIPP" );
    }

    // copy the mesh buffer to two smesh's so we can manipulate it
    for ( j = 0; j < sourceMesh->getMeshBufferCount(); j++ )
    {
      smeshbuf = new SMeshBuffer;
      smeshbuf2 = new SMeshBuffer;

      pMeshBuffer = sourceMesh->getMeshBuffer( j ); 

      // get vertices and clip them to the plane
      s32 vcount = pMeshBuffer->getVertexCount(); 
      irr::video::S3DVertex* currentVertex = ( irr::video::S3DVertex* )pMeshBuffer->getVertices();

      //vMiddle.set(0,0,0);
      //for ( i = 0; i < vcount; i++) 
      //{ 
      //    vMiddle += currentVertex[i].Pos;
      //}
      //vMiddle /= vcount;

      //count1 = count2 = 0;
      //for ( i = 0; i < vcount; i++) 
      //{ 
      //    if (plane.classifyPointRelation( currentVertex[i].Pos ) == ISREL3D_BACK )
      //        count2++;
      //    else
      //        count1++;
      //}

      onplane1 = onplane2 = 0;
      smeshbuf->Vertices.clear();
      smeshbuf2->Vertices.clear();
      for ( i = 0; i < vcount; i++ )
      {
        smeshbuf->Vertices.push_back( currentVertex[i] );
        smeshbuf2->Vertices.push_back( currentVertex[i] );
        vN = plane.Normal * plane.getDistanceTo( currentVertex[i].Pos );
        if ( plane.classifyPointRelation( currentVertex[i].Pos ) == ISREL3D_BACK )
        {
          smeshbuf->Vertices[i].Pos -= vN;
          onplane1++;
        }
        else
        {
          smeshbuf2->Vertices[i].Pos -= vN;
          onplane2++;
        }
        allvertices++;

        //smeshbuf->Vertices[i].Pos -= vMiddle;
        //smeshbuf2->Vertices[i].Pos -= vMiddle;
      }
      // get indices (send front polygons to front mesh and all to back mesh)
      s32 icount = pMeshBuffer->getIndexCount();
      u16* currentIndex = ( u16* )pMeshBuffer->getIndices();
      smeshbuf->Indices.clear();
      smeshbuf2->Indices.clear();
      for ( i = 0; i < icount; i += 3 )
      {
        if ( ( plane.classifyPointRelation( currentVertex[currentIndex[i + 0]].Pos ) == ISREL3D_BACK ) && ( plane.classifyPointRelation( currentVertex[currentIndex[i + 1]].Pos ) == ISREL3D_BACK ) && ( plane.classifyPointRelation( currentVertex[currentIndex[i + 2]].Pos ) == ISREL3D_BACK ) )
        {
          smeshbuf2->Indices.push_back( currentIndex[i + 0] );
          smeshbuf2->Indices.push_back( currentIndex[i + 1] );
          smeshbuf2->Indices.push_back( currentIndex[i + 2] );
        }
        else
        {
          smeshbuf->Indices.push_back( currentIndex[i + 0] );
          smeshbuf->Indices.push_back( currentIndex[i + 1] );
          smeshbuf->Indices.push_back( currentIndex[i + 2] );
          smeshbuf2->Indices.push_back( currentIndex[i + 0] );
          smeshbuf2->Indices.push_back( currentIndex[i + 1] );
          smeshbuf2->Indices.push_back( currentIndex[i + 2] );
        }
      }
      // get materials
      smeshbuf->Material = smeshbuf2->Material = pMeshBuffer->getMaterial();

      // delete junk polygons aligned on the plane on the back mesh
      for ( i = 0; i < smeshbuf2->Indices.size(); i += 3 )
      {
        fDistSum = abs( plane.getDistanceTo( smeshbuf2->Vertices[smeshbuf2->Indices[i + 0]].Pos ) ) + abs( plane.getDistanceTo( smeshbuf2->Vertices[smeshbuf2->Indices[i + 1]].Pos ) ) + abs( plane.getDistanceTo( smeshbuf2->Vertices[smeshbuf2->Indices[i + 2]].Pos ) );

        if ( fDistSum < 0.001f )
        {
          smeshbuf2->Indices.erase( i, 3 );
          if ( i > 2 )
          {
            i -= 3;
          }
        }
      }

      fControlSum = fControlSum2 = 0.0f;
      for ( i = 0; i < smeshbuf->Indices.size(); i++ )
      {
        fControlSum += abs( plane.getDistanceTo( smeshbuf->Vertices[smeshbuf->Indices[i]].Pos ) );
        //CONSOLE.addx("      smeshbuf %f   %f", fControlSum, plane.getDistanceTo( smeshbuf->Vertices[smeshbuf->Indices[i]].Pos ) );
      }
      for ( i = 0; i < smeshbuf2->Indices.size(); i++ )
      {
        fControlSum2 += abs( plane.getDistanceTo( smeshbuf2->Vertices[smeshbuf2->Indices[i]].Pos ) );
      }

      count1 = smeshbuf->Indices.size();
      count2 = smeshbuf2->Indices.size();


      if ( APP.DebugMode > 1 )
      {
        CONSOLE.addx( " count1 %i onplane1 %i fsidt %f", count1, onplane1, fControlSum );
      }
      if ( ( count1 > 8 ) && ( allvertices - onplane1 > 4 ) && ( fControlSum > 5.0f ) && ( allvertices > 4 ) )
      {
        smesh->addMeshBuffer( smeshbuf );
        for ( i = 0; i < smeshbuf->Indices.size(); i++ )
        {
          smeshbuf->BoundingBox.addInternalPoint( smeshbuf->Vertices[smeshbuf->Indices[i]].Pos );
        }
        smesh->BoundingBox.addInternalBox( smeshbuf->BoundingBox );
      }
      else if ( APP.DebugMode > 1 )
      {
        CONSOLE.add( "drop mesh1" );
      }

      if ( APP.DebugMode > 1 )
      {
        CONSOLE.addx( " count2 %i onplane2 %i fsidt2 %f", count2, onplane2, fControlSum2 );
      }
      if ( ( count2 > 8 ) && ( allvertices - onplane2 > 4 ) && ( fControlSum2 > 5.0f ) && ( allvertices > 4 ) )
      {
        smesh2->addMeshBuffer( smeshbuf2 );
        for ( i = 0; i < smeshbuf2->Indices.size(); i++ )
        {
          smeshbuf2->BoundingBox.addInternalPoint( smeshbuf2->Vertices[smeshbuf2->Indices[i]].Pos );
        }
        smesh2->BoundingBox.addInternalBox( smeshbuf2->BoundingBox );
      }
      else if ( APP.DebugMode > 1 )
      {
        CONSOLE.add( "drop mesh2" );
      }


      //CONSOLE.addx(" count1 %i count2 %i onplane1 %i onplane2 %i allvertices %i fsidt %f fsidt2 %f box1 %f box2 %f",
      //    count1, count2, onplane1, onplane2, allvertices, fControlSum, fControlSum2, smeshbuf->BoundingBox.getExtent(), smeshbuf2->BoundingBox.getExtent() );

      smeshbuf->drop();
      smeshbuf2->drop();
    }


    if ( ( smesh->getMeshBufferCount() == 0 ) )
    {
      smesh->drop();
      smesh = NULL;
      if ( APP.DebugMode > 1 )
      {
        CONSOLE.add( "droop1" );
      }
    }

    if ( ( smesh2->getMeshBufferCount() == 0 ) )
    {
      smesh2->drop();
      smesh2 = NULL;
      if ( APP.DebugMode > 1 )
      {
        CONSOLE.add( "droop2" );
      }
    }


    backMesh = smesh2;
    return smesh;
}