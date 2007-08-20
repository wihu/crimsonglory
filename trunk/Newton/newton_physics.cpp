#include "newton_physics.h"
#include "newton_node.h"

////////////////////////////////////////////
// CNewton 
////////////////////////////////////////////

dFloat CNewton::dGravity = 0;
SpecialEffectStruct* CNewton::g_currectEffect;
SpecialEffectStruct CNewton::wood_wood;
SpecialEffectStruct CNewton::wood_metal;
SpecialEffectStruct CNewton::wood_level;
SpecialEffectStruct CNewton::metal_metal;
SpecialEffectStruct CNewton::metal_level;
int CNewton::woodID; 
int CNewton::metalID; 
int CNewton::levelID; 
int CNewton::defaultnodeID; 
int CNewton::nocollisionID; 

CNewton::CNewton()
{
    // console vars
    CONSOLE_VAR( "w_gravity", dFloat, dGravity, 9.81, L"w_gravity [real]. Ex. w_gravity 9.81", L"The world physics gravity force value." );
    CONSOLE_VAR( "w_physics_timestep", float, timeStep, 1.0f, L"w_physics_timestep [sec]. Ex. w_physics_timestep 0.5", L"The amount of time processed by the physics engine each frame." );
    CONSOLE_VAR( "w_physics_frames", int, minFrames, 240, L"w_physics_frames [20-1000]. Ex. w_physics_frames 240", L"The world physics minimum frames per second. The higher the more stable the physics are." );
}

CNewton::~CNewton()
{
    //CONSOLE.add("Destroyed Newton Physics");
}

bool CNewton::Start()
{
    // initialise Newton
    nWorld = NewtonCreate( NULL, NULL );
    NewtonSetSolverModel( nWorld, 8 );
    NewtonSetFrictionModel( nWorld, 1 );
    NewtonSetMinimumFrameRate( nWorld, minFrames ); // 2*GAME.goalTicks
    vector3df vMin, vMax;
    vMin = vector3df( -10000, -10000, -100 );
    vMax = vector3df( 10000, 10000, -100 );
    NewtonSetWorldSize( nWorld, &vMin.X, &vMax.X );

    SetupMaterials();

    mAccumlativeLoopTime = 0;
    lastTime = getPreciseTime();


    // Timing variables
    GoalTicks = 60;
    mainTickCounter = 0;
    TimeInMil = TimeInMilLast = TimePassed = 0;
    Seconds = SecondsLast = 0;
    Ticks = TicksPerSecond = 0;
    Frames = FramesPerSecond = 0;
    TickTime = TickTimeLast = 0;
    frc = fps = frp = 0;

    return true;
}

void CNewton::Number27Timing()
{
    TimeInMilLast = TimeInMil;
    TimeInMil = getPreciseTime();
    if ( TimeInMil - TimeInMilLast > 2000 )
    {
      TimeInMilLast = TimeInMil;
    }
    TimePassed = TimePassed + ( TimeInMil - TimeInMilLast );

    SecondsLast = Seconds;
    Seconds = TimePassed / 1000;
    if ( Seconds != SecondsLast )
    {
      TicksPerSecond = Ticks;
      Ticks = 0;
      FramesPerSecond = Frames;
      Frames = 0;
    }


    Frames += 1;
    TickTimeLast = TickTime;
    TickTime = TimePassed / ( 1000 / GoalTicks );
}

void CNewton::Update()
{
    PROFILE( "Physics task" );

    // calculate time since last update
    curTime = getPreciseTime();
    deltaTime = curTime - lastTime;
    lastTime = curTime;

    //  NewtonUpdate(nWorld, timeStep );

    //CONSOLE_FLOAT(mAccumlativeLoopTime);
    //mAccumlativeLoopTime = mAccumlativeLoopTime + (deltaTime/1000);            // Accumulate the LoopTime (time slice) 
    //while (mAccumlativeLoopTime > timeStep) 
    //{ 
    //  NewtonUpdate(nWorld, timeStep); 
    //  mAccumlativeLoopTime -= timeStep; 

    //} // while loop

    //   float alpha = mAccumlativeLoopTime / timeStep;
    //CONSOLE_FLOAT(alpha);
    // update world camera, here for smoothness
    WORLD.GetCamera()->Think();

    NewtonUpdate( nWorld, timeStep );

    //State state = currentState*alpha + previousState*(1.0f-alpha);

    //IRR.UpdateNow();

    //Number27Timing();
    //   // frame-rate independent tasks
    //loopTicks = TickTime - TickTimeLast;
    //   for ( mainControl = 0; mainControl < loopTicks; mainControl++ )
    //   {
    //       Ticks += 1;
    //       mainTickCounter += 1;

    //  NewtonUpdate(nWorld, timeStep); 

    //  // update world camera, here for smoothness
    //  WORLD.GetCamera()->Think();
    //  //IRR.UpdateNow();
    //}

    //  NewtonUpdate(nWorld, timeStep); 

    // update world camera, here for smoothness

    //IRR.UpdateNow();
}

void CNewton::Stop()
{
    CleanUpMaterials();
    if ( !canKill ) //it was already done in WorldTask->Stop()
    {
      NewtonDestroy( nWorld );
    }
}

void CNewton::PhysicsApplyForceAndTorque( const NewtonBody* body )
{
    dFloat mass, Ixx, Iyy, Izz;
    NewtonBodyGetMassMatrix( body, &mass, &Ixx, &Iyy, &Izz );
    dVector force( 0, mass* dGravity* IrrToNewton, 0 );
    NewtonBodySetForce( body, &force.m_x );

    // align 2d
    //float vel[3];
    //NewtonBodyGetVelocity(body, vel);
    //vel[2] = 0.0f;
    //NewtonBodySetVelocity(body, vel);

    //float omega[3];
    //NewtonBodyGetOmega(body, omega);
    //omega[0] = omega[1] =  0.0f;
    //NewtonBodySetOmega(body, omega);
}

void CNewton::PhysicsSetTransform( const NewtonBody* body, const dFloat* matrix )
{
    ISceneNode* node = 0;
    node = ( ISceneNode * )NewtonBodyGetUserData( body );
    if ( !node )
    {
      return;
    }   

    static matrix4 mat;
    memcpy( mat.M, matrix, sizeof( s32 ) * 16 );
    node->setRotation( mat.getRotationDegrees() );
    node->setPosition( mat.getTranslation() ); //*IrrToNewton
}

// rigid body destructor
void CNewton::PhysicsBodyDestructor( const NewtonBody* body )
{
    //   ISceneNode *node = 0;
    //   node = (ISceneNode *) NewtonBodyGetUserData(body);
    //if (node)
    //  delete node;
    //node = 0;
}

// show simple collision geometry in debug mode
void CNewton::DebugShowGeometryCollision( const NewtonBody* body, int vertexCount, const dFloat* faceVertec, int id )
{
    int i;
    triangle3df t1, t2;

    i = vertexCount - 1;
    vector3df v0( faceVertec[i * 3 + 0], faceVertec[i * 3 + 1], faceVertec[i * 3 + 2] );
    i = vertexCount - 2;
    vector3df v1( faceVertec[i * 3 + 0], faceVertec[i * 3 + 1], faceVertec[i * 3 + 2] );  
    i = vertexCount - 3;
    vector3df v2( faceVertec[i * 3 + 0], faceVertec[i * 3 + 1], faceVertec[i * 3 + 2] );  
    t1.set( v0, v1, v2 );
    IRR.video->draw3DTriangle( t1, SColor( 0, 120, 250, 160 ) );
}

// show rigid body collision geometry
void CNewton::DebugShowBodyCollision( const NewtonBody* body )
{
    dFloat Ixx;
    dFloat Iyy;
    dFloat Izz;
    dFloat mass;

    // only show collision for dynamics objects
    NewtonBodyGetMassMatrix( body, &mass, &Ixx, &Iyy, &Izz );
    //if (mass > 0.0f)
    {
        NewtonBodyForEachPolygonDo( body, DebugShowGeometryCollision );
    }

    //if (mass > 0.0f)
    //{
    //   matrix4 mat;
    //NewtonBodyGetMatrix( body, &mat.M[0] ); 

    //CNewtonNode* newtonNode = 0;
    //newtonNode = (CNewtonNode*) NewtonBodyGetUserData(body);
    //   if (!newtonNode)
    //  return; 

    //newtonNode->PhysicsTransform(&newtonNode->transformMatrix.M[0]);
    //}
}


// show all collision geometry is debug mode
void CNewton::Render()
{
    if ( !APP.DebugMode )
    {
      return;
    }

    IRR.video->setTransform( ETS_WORLD, matrix4() );

    SMaterial m; 
    m.Lighting = false; 
    m.BackfaceCulling = false;
    IRR.video->setMaterial( m );

    //triangle3df t1;
    //vector3df v0(0, 0, 0);
    //vector3df v1(30, 3, 0);  
    //vector3df v2(3, -30,0 );  
    //t1.set( v0, v1, v2 );
    //IRR.video->draw3DTriangle(t1, SColor(0,220,250,160));  

    NewtonWorldForEachBodyDo( nWorld, DebugShowBodyCollision );
}

// this is how to make a static physics hull from any mesh you've loaded - ie, your level or terrain etc. this is the flat thing
// with blocks around the edge that you're driving the car about on.
// this was taken from the mercior tutorial, and therefore is mercior's (very impressive) work, not mine!
NewtonBody* CNewton::addStaticBodyTree( ISceneNode* node, char* filename, vector3df vPos, vector3df vScale )
{
    IAnimatedMesh* treemesh = IRR.smgr->getMesh( APP.useFile( filename ).c_str() );

    NewtonCollision* treecollision;
    treecollision = NewtonCreateTreeCollision( nWorld, NULL );
    NewtonTreeCollisionBeginBuild( treecollision );

    int cMeshBuffer, j, i;
    int v1i, v2i, v3i;
    IMeshBuffer* mb;

    vector3df vArray[3]; // vertex array (3*3 floats)

    int tmpCount = 0;


    IMesh* mesh = treemesh->getMesh( 0 );
    int buffCount = mesh->getMeshBufferCount();
    for ( i = 0; i < buffCount; i ++ )
    {
      for ( cMeshBuffer = 0; cMeshBuffer < treemesh->getMesh( i )->getMeshBufferCount(); cMeshBuffer++ )
      {
        mb = treemesh->getMesh( 0 )->getMeshBuffer( cMeshBuffer );

        video::S3DVertex* mb_vertices = ( irr::video::S3DVertex* )mb->getVertices();

        u16* mb_indices = mb->getIndices();

        // add each triangle from the mesh
        for ( j = 0; j< mb->getIndexCount(); j += 3 )
        {
          v1i = mb_indices[j];
          v2i = mb_indices[j + 1];
          v3i = mb_indices[j + 2];

          // to make things easier, here we can use engine data type
          vArray[0] = mb_vertices[v1i].Pos * IrrToNewton;
          vArray[1] = mb_vertices[v2i].Pos * IrrToNewton;
          vArray[2] = mb_vertices[v3i].Pos * IrrToNewton;
          vArray[0].Y = -vArray[0].Y;
          vArray[1].Y = -vArray[1].Y;
          vArray[2].Y = -vArray[2].Y;
          vArray[0] += vPos;
          vArray[1] += vPos;
          vArray[2] += vPos;
          vArray[0] *= vScale;
          vArray[1] *= vScale;
          vArray[2] *= vScale;

          // make sure we do not add degenerated polygons to the tree
          vector3df e0( vArray[1] - vArray[0] );
          vector3df e1( vArray[2] - vArray[0] );

          // the area of the face is the cross product
          vector3df area( e0.crossProduct( e1 ) );

          // skip faces with very small area
          float mag = area.dotProduct( area );
          if ( mag > 1.0e-6f )
          {
            NewtonTreeCollisionAddFace( treecollision, 3, &vArray[0].X, sizeof( vector3df ), 1 );
          }
        }
      }
    }

    NewtonTreeCollisionEndBuild( treecollision, 1 );

    // create a rigid body
    NewtonBody* treebody;
    treebody = NewtonCreateBody( nWorld, treecollision );

    NewtonBodySetMaterialGroupID( treebody, levelID );

    // release the collsion tree 
    NewtonReleaseCollision( nWorld, treecollision );

    return treebody;
}


NewtonBody* CNewton::addRigidBodyBox( ISceneNode* node, vector3df vSize, vector3df vPos )
{
    NewtonCollision* collision = NewtonCreateBox( nWorld, ( vSize.X* IrrToNewton ), ( vSize.Y* IrrToNewton ), ( vSize.Z* IrrToNewton ), NULL );
    NewtonBody* body = NewtonCreateBody( nWorld, collision );
    NewtonReleaseCollision( nWorld, collision );

    NewtonBodySetMassMatrix( body, 1.0f, 1.0f, 1.0f, 1.0f );

    if ( !node )
    {
      node = IRR.smgr->addCubeSceneNode();
      node->setPosition( vPos );
      node->setScale( vSize );
      node->setMaterialFlag( EMF_LIGHTING, IRR.useLighting );
      node->setMaterialTexture( 0, IRR.video->getTexture( APP.useFile( "Textures/default.jpg" ).c_str() ) );
    }

    NewtonBodySetUserData( body, node );

    NewtonBodySetDestructorCallback( body, PhysicsBodyDestructor );

    NewtonBodySetTransformCallback( body, PhysicsSetTransform );

    NewtonBodySetForceAndTorqueCallback( body, PhysicsApplyForceAndTorque );

    matrix4 mat;
    mat.setTranslation( vPos );
    NewtonBodySetMatrix( body, &mat.M[0] );


    //PhysicsSetTransform( body, &mat.M[0]);

    //NewtonReleaseCollision (nWorld, collision);

    return body;
}

NewtonBody* CNewton::addStaticBodyBox( ISceneNode* node, vector3df vSize, vector3df vPos, vector3df vRot )
{
    NewtonCollision* collision = NewtonCreateBox( nWorld, ( vSize.X* IrrToNewton ), ( vSize.Y* IrrToNewton ), ( vSize.Z* IrrToNewton ), NULL );
    NewtonBody* body = NewtonCreateBody( nWorld, collision );

    if ( !node )
    {
      node = IRR.smgr->addCubeSceneNode();
      node->setPosition( vPos );
      node->setScale( vSize );
      node->setMaterialFlag( EMF_LIGHTING, IRR.useLighting );
      node->setMaterialTexture( 0, IRR.video->getTexture( APP.useFile( "Textures/default.jpg" ).c_str() ) );
      node->setMaterialFlag( EMF_NORMALIZE_NORMALS, true );
    }

    NewtonBodySetUserData( body, node );

    NewtonBodySetDestructorCallback( body, PhysicsBodyDestructor );

    NewtonBodySetTransformCallback( body, PhysicsSetTransform );

    NewtonBodySetForceAndTorqueCallback( body, PhysicsApplyForceAndTorque );

    NewtonBodySetMaterialGroupID( body, levelID );

    matrix4 mat;
    mat.setTranslation( vPos );
    mat.setRotationDegrees( vRot );
    NewtonBodySetMatrix( body, &mat.M[0] );

    PhysicsSetTransform( body, &mat.M[0] );
    NewtonReleaseCollision( nWorld, collision );

    return body;
}


// Helper functions:

void CNewton::addForce( const NewtonBody* body, core::vector3df force )
{
    NewtonBodyAddForce( body, &force.X );
} 

void CNewton::addTorque( const NewtonBody* body, core::vector3df torque )
{
    NewtonBodyAddTorque( body, &torque.X );
} 

// Take a Newton matrix and create a Quaternion + Position_vector 
void CNewton::matrixToQuatPos( const float* matrix, core::quaternion& quat, core::vector3df& pos )
{
    // this takes a matrix returned by Newton, and creates a Quaternion 
    // and position Vector3, which is more meaningful for Ogre. 

    core::matrix4 _mat; 

    _mat.M[0] = matrix[0]; 
    _mat.M[1] = matrix[4]; 
    _mat.M[2] = matrix[8]; 
    _mat.M[3] = matrix[1]; 
    _mat.M[4] = matrix[5]; 
    _mat.M[5] = matrix[9]; 
    _mat.M[6] = matrix[2]; 
    _mat.M[7] = matrix[6]; 
    _mat.M[8] = matrix[10]; 

    quat = core::quaternion( _mat ); 

    pos = core::vector3df( matrix[12], matrix[13], matrix[14] );
} 

// Take a Quaternion and Position Matrix and create a Newton-happy float matrix! 
void CNewton::quatPosToMatrix( const core::quaternion quat, const core::vector3df pos, float* matrix )
{
    // this takes a Quaternion and a Vector3 and creates a float array 
    // which is more meaningful to Newton. 
    core::matrix4 rot; 
    core::vector3df xcol, ycol, zcol; 

    // creates a 3x3 rotation matrix from the Quaternion. 
    rot = quat.getMatrix(); 

    xcol = core::vector3df( rot( 0, 0 ), rot( 1, 0 ), rot( 2, 0 ) ); 

    ycol = core::vector3df( rot( 0, 1 ), rot( 1, 1 ), rot( 2, 1 ) ); 

    zcol = core::vector3df( rot( 0, 2 ), rot( 1, 2 ), rot( 2, 2 ) ); 

    // now fill the final matrix with the appropriate data: 
    matrix[0] = xcol.X; 
    matrix[1] = xcol.Y; 
    matrix[2] = xcol.Z; 
    matrix[3] = 0.0f; 

    matrix[4] = ycol.X; 
    matrix[5] = ycol.Y; 
    matrix[6] = ycol.Z; 
    matrix[7] = 0.0f; 

    matrix[8] = zcol.X; 
    matrix[9] = zcol.Y; 
    matrix[10] = zcol.Z; 
    matrix[11] = 0.0f; 

    matrix[12] = pos.X; 
    matrix[13] = pos.Y; 
    matrix[14] = pos.Z; 
    matrix[15] = 1.0;
} 

void CNewton::getPositionOrientation( const NewtonBody* body, core::quaternion& orient, core::vector3df& pos )
{
    float matrix[16]; 

    NewtonBodyGetMatrix( body, matrix ); 

    matrixToQuatPos( matrix, orient, pos );
} 

void CNewton::addGlobalForce( const NewtonBody* body, core::vector3df force, core::vector3df pos )
{
    core::vector3df bodypos; 
    core::quaternion bodyorient; 

    getPositionOrientation( body, bodyorient, bodypos ); 

    core::vector3df topoint = pos - bodypos; 
    core::vector3df torque = topoint.crossProduct( force ); 

    addForce( body, force ); 
    addTorque( body, torque );
} 

void CNewton::addLocalForce( const NewtonBody* body, core::vector3df force, core::vector3df pos )
{
    core::vector3df bodypos; 
    core::quaternion bodyorient; 

    getPositionOrientation( body, bodyorient, bodypos ); 

    core::vector3df globalforce = bodyorient* force; 
    core::vector3df globalpoint = ( bodyorient* pos ) + bodypos; 

    addGlobalForce( body, globalforce, globalpoint );
}


vector3df CNewton::getPointVelocity( const NewtonBody* body, vector3df vPos )
{
    matrix4 m;
    vector3df vel, omega;

    NewtonBodyGetMatrix( body, &m.M[0] );
    NewtonBodyGetVelocity( body, &vel.X );
    NewtonBodyGetOmega( body, &omega.X );

    vector3df tmp( vPos );
    vector3df r = tmp;// - m.getTranslation();

    //vector3df res = vel + r * omega;
    vector3df res = vel + omega.crossProduct( r );

    return res;
}


// initialize all material and material interations
void CNewton::SetupMaterials()
{
    int defaultID;

    // initialize the material intaration here
    wood_wood.Init( "wood_wood" );
    wood_metal.Init( "wood_metal" );
    wood_level.Init( "wood_level" );
    metal_metal.Init( "metal_metal" );
    metal_level.Init( "metal_level" );


    // create all materials ID
    woodID = NewtonMaterialCreateGroupID( nWorld );
    metalID = NewtonMaterialCreateGroupID( nWorld );
    levelID = NewtonMaterialCreateGroupID( nWorld );
    defaultnodeID = NewtonMaterialCreateGroupID( nWorld );
    nocollisionID = NewtonMaterialCreateGroupID( nWorld );

    // get the defuult material ID
    defaultID = NewtonMaterialGetDefaultGroupID( nWorld );

    // set default material properties
    NewtonMaterialSetDefaultSoftness( nWorld, defaultID, defaultID, 0.05f );
    NewtonMaterialSetDefaultElasticity( nWorld, defaultID, defaultID, 0.4f );
    NewtonMaterialSetDefaultCollidable( nWorld, defaultID, defaultID, 1 );
    NewtonMaterialSetDefaultFriction( nWorld, defaultID, defaultID, 1.0f, 0.5f );
    NewtonMaterialSetCollisionCallback( nWorld, woodID, woodID, &wood_wood, GenericContactBegin, GenericContactProcess, GenericContactEnd ); 


    // set the material properties for wood on wood
    NewtonMaterialSetDefaultElasticity( nWorld, woodID, woodID, 0.3f );
    NewtonMaterialSetDefaultFriction( nWorld, woodID, woodID, 1.1f, 0.7f );
    NewtonMaterialSetCollisionCallback( nWorld, woodID, woodID, &wood_wood, GenericContactBegin, GenericContactProcess, GenericContactEnd ); 

    // set the material properties for wood on metal
    NewtonMaterialSetDefaultElasticity( nWorld, woodID, metalID, 0.5f );
    NewtonMaterialSetDefaultFriction( nWorld, woodID, metalID, 0.8f, 0.6f );
    NewtonMaterialSetCollisionCallback( nWorld, woodID, metalID, &wood_metal, GenericContactBegin, GenericContactProcess, GenericContactEnd ); 

    // set the material properties for wood on levell
    NewtonMaterialSetDefaultElasticity( nWorld, woodID, levelID, 0.3f );
    NewtonMaterialSetDefaultFriction( nWorld, woodID, levelID, 0.9f, 0.3f );
    NewtonMaterialSetCollisionCallback( nWorld, woodID, levelID, &wood_level, GenericContactBegin, GenericContactProcess, GenericContactEnd ); 

    // set the material properties for metal on metal
    NewtonMaterialSetDefaultElasticity( nWorld, metalID, metalID, 0.7f );
    NewtonMaterialSetDefaultFriction( nWorld, metalID, metalID, 0.5f, 0.2f );
    NewtonMaterialSetCollisionCallback( nWorld, metalID, metalID, &metal_metal, GenericContactBegin, GenericContactProcess, GenericContactEnd ); 

    // set the material properties for metal on level
    NewtonMaterialSetDefaultElasticity( nWorld, metalID, levelID, 0.4f );
    NewtonMaterialSetDefaultFriction( nWorld, metalID, levelID, 0.6f, 0.4f );
    NewtonMaterialSetCollisionCallback( nWorld, metalID, levelID, &metal_level, GenericContactBegin, GenericContactProcess, GenericContactEnd ); 

    // level on level is will never hapens


    // crete the charater material interactions

    NewtonMaterialSetDefaultElasticity( nWorld, woodID, defaultnodeID, 0.3f );
    NewtonMaterialSetDefaultFriction( nWorld, woodID, defaultnodeID, 1.1f, 0.7f );
    NewtonMaterialSetCollisionCallback( nWorld, woodID, defaultnodeID, &wood_wood, GenericContactBegin, NodeLevelContactProcess, GenericContactEnd ); 

    // set the material properties for character on metal
    NewtonMaterialSetDefaultElasticity( nWorld, metalID, defaultnodeID, 0.7f );
    NewtonMaterialSetDefaultFriction( nWorld, metalID, defaultnodeID, 0.5f, 0.2f );
    NewtonMaterialSetCollisionCallback( nWorld, metalID, defaultnodeID, &metal_metal, GenericContactBegin, NodeLevelContactProcess, GenericContactEnd ); 

    // set the material properties for character on level 
    NewtonMaterialSetDefaultElasticity( nWorld, levelID, defaultnodeID, 0.4f );
    NewtonMaterialSetDefaultFriction( nWorld, levelID, defaultnodeID, 0.4f, 0.4f );
    NewtonMaterialSetCollisionCallback( nWorld, levelID, defaultnodeID, &metal_level, GenericContactBegin, NodeLevelContactProcess, GenericContactEnd ); 

    // set the material properties for character on level 
    NewtonMaterialSetDefaultElasticity( nWorld, defaultnodeID, defaultnodeID, 0.1f );
    NewtonMaterialSetDefaultFriction( nWorld, defaultnodeID, defaultnodeID, 0.9f, 0.9f );
    NewtonMaterialSetCollisionCallback( nWorld, defaultnodeID, defaultnodeID, &metal_level, GenericContactBegin, NodeContactProcess, GenericContactEnd ); 

    //nocollison
    NewtonMaterialSetDefaultCollidable( nWorld, nocollisionID, defaultID, 0 );
    NewtonMaterialSetDefaultCollidable( nWorld, nocollisionID, defaultnodeID, 0 );
    NewtonMaterialSetDefaultCollidable( nWorld, nocollisionID, levelID, 0 );
}


// destroy all material resources
void CNewton::CleanUpMaterials()
{
    wood_wood.Detroy();
    wood_metal.Detroy();
    wood_level.Detroy();
    metal_metal.Detroy();
    metal_level.Detroy();
}

// this callback is called when the two aabb boxes of the collisiong object overlap
int CNewton::GenericContactBegin( const NewtonMaterial* material, const NewtonBody* body0, const NewtonBody* body1 )
{
    // get the pointer to the special effect struture
    g_currectEffect = ( SpecialEffectStruct * )NewtonMaterialGetMaterialPairUserData( material );

    // save the collisiong bodies
    g_currectEffect->m_body0 = ( NewtonBody * )body0;
    g_currectEffect->m_body1 = ( NewtonBody * )body1;

    // clear the contact normal speed 
    g_currectEffect->m_contactMaxNormalSpeed = 0.0f;

    // clear the contact sliding speed 
    g_currectEffect->m_contactMaxTangentSpeed = 0.0f;

    // return one the tell Newton the application wants to proccess this contact
    return 1;
}


// this callback is called for every contact between the two bodies
int CNewton::GenericContactProcess( const NewtonMaterial* material, const NewtonContact* contact )
{
    dFloat speed0;
    dFloat speed1;
    dVector normal;

    // Get the maximun normal speed of this impact. this can be used for particels of playing collision sound
    speed0 = NewtonMaterialGetContactNormalSpeed( material, contact );
    if ( speed0 > g_currectEffect->m_contactMaxNormalSpeed )
    {
      // save the position of the contact (for 3d sound of particles effects)
      g_currectEffect->m_contactMaxNormalSpeed = speed0;
      NewtonMaterialGetContactPositionAndNormal( material, &g_currectEffect->m_position.m_x, &normal.m_x );
    }

    // get the maximun of the two sliding contact speed
    speed0 = NewtonMaterialGetContactTangentSpeed( material, contact, 0 );
    speed1 = NewtonMaterialGetContactTangentSpeed( material, contact, 1 );
    if ( speed1 > speed0 )
    {
      speed0 = speed1;
    }

    // Get the maximun tangent speed of this contact. this can be used for particles(sparks) of playing scratch sounds 
    if ( speed0 > g_currectEffect->m_contactMaxTangentSpeed )
    {
      // save the position of the contact (for 3d sound of particles effects)
      g_currectEffect->m_contactMaxTangentSpeed = speed0;
      NewtonMaterialGetContactPositionAndNormal( material, &g_currectEffect->m_position.m_x, &normal.m_x );
    }

    //if (debugLinesMode) {
    //  if (debugCount < 1000) {
    //      dVector point;
    //      dVector normal; 
    //      NewtonMaterialGetContactPositionAndNormal (material, &point.m_x, &normal.m_x);
    //      debugFace[debugCount][0] = point;
    //      debugFace[debugCount][1] = point + normal.Scale (3.0f);
    //      debugCount ++;
    //  }
    //}


    // return one to tell Newton we want to accept this contact
    return 1;
}

// this function is call affter all contacts for this pairs is proccesed
void CNewton::GenericContactEnd( const NewtonMaterial* material )
{
#define MIN_CONTACT_SPEED 15
#define MIN_SCRATCH_SPEED 5

    // if the max contact speed is larger than some minumum value. play a sound
    if ( g_currectEffect->m_contactMaxNormalSpeed > MIN_CONTACT_SPEED )
    {
      g_currectEffect->PlayImpactSound( g_currectEffect->m_contactMaxNormalSpeed - MIN_CONTACT_SPEED );
    }

    // if the max contact speed is larger than some minumum value. play a sound
    if ( g_currectEffect->m_contactMaxNormalSpeed > MIN_SCRATCH_SPEED )
    {
      g_currectEffect->PlayScratchSound( g_currectEffect->m_contactMaxNormalSpeed - MIN_SCRATCH_SPEED );
    }

    // implement here any other effects
}



// this is use to constomized the chatarer contril contact vectors
int CNewton::NodeLevelContactProcess( const NewtonMaterial* material, const NewtonContact* contact )
{
    dFloat mass;
    dFloat Ixx;
    dFloat Iyy;
    dFloat Izz;
    NewtonBody* body;
    NewtonBody* body2;
    CNewtonNode* newtonNode;
    //  unsigned collisionID;

    // apply the default behaviuor
    GenericContactProcess( material, contact );

    // Get the spherical body, it is the body with non zero mass
    // This is a quit and dirty way to determine the player body, but it no safe, 
    // it only work in this case because one of the two bodies is the terrain which we now have //infinite mass.
    // a better way to do this is by getting the user data and finding some object identifier stored with the user data.
    body = g_currectEffect->m_body0;
    body2 = g_currectEffect->m_body1;
    NewtonBodyGetMassMatrix( g_currectEffect->m_body0, &mass, &Ixx, &Iyy, &Izz );
    if ( mass == 0.0f )
    {
      body = g_currectEffect->m_body1;
      body2 = g_currectEffect->m_body0;
      NewtonBodyGetMassMatrix( g_currectEffect->m_body1, &mass, &Ixx, &Iyy, &Izz );
    }

    // Get the pointer to the character

    newtonNode = ( CNewtonNode * )NewtonBodyGetUserData( body );
    newtonNode->PhysicsCollision( material, contact, body2 );


    /*
        // get the collision ID
        collisionID = NewtonMaterialGetBodyCollisionID(material, body);
        // if this collision is a sphere, the this constact shod be frictio less 
        if (collisionID == SPHERE_COLLISION) {
            // disable fiction calculation for sphere collision
            NewtonMaterialSetContactFrictionState (material, 0, 0);
            NewtonMaterialSetContactFrictionState (material, 0, 1);
        } else {
            // align the tangent contact direction with the velocity vector of the ball
            dVector posit;
            dVector normal;
            
            dVector velocity;
            NewtonBodyGetVelocity(body, &velocity.m_x);
            NewtonMaterialGetContactPositionAndNormal (material, &posit.m_x, &normal.m_x);
            // calculate ball velocity perpendicular to the contact normal
            dVector tangentVelocity (velocity - normal.Scale (normal % velocity));
            // align the tangent at the contact point with the tangent velocity vector of the ball
            NewtonMaterialContactRotateTangentDirections (material, &tangentVelocity.m_x);
        }
    */  


    // return one to tell Newton we want to accept this contact
    return 1;
}

// this is use to constomized the chatarer contril contact vectors
int CNewton::NodeContactProcess( const NewtonMaterial* material, const NewtonContact* contact )
{
    dFloat mass;
    dFloat Ixx;
    dFloat Iyy;
    dFloat Izz;
    NewtonBody* body;
    CNewtonNode* newtonNode;
    //  unsigned collisionID;

    // apply the default behaviuor
    GenericContactProcess( material, contact );

    // Get the pointer to the character

    newtonNode = ( CNewtonNode * )NewtonBodyGetUserData( g_currectEffect->m_body0 );
    newtonNode->PhysicsCollision( material, contact, g_currectEffect->m_body1 );

    newtonNode = ( CNewtonNode * )NewtonBodyGetUserData( g_currectEffect->m_body1 );
    newtonNode->PhysicsCollision( material, contact, g_currectEffect->m_body0 );


    // return one to tell Newton we want to accept this contact
    return 1;
}
