
// By: Lukas Heise (axodoss@hotmail.com)
// Date: 2005-05-01

#include "SoundTask.h"

#include "../Game/GameDLL.h"
#include "../IrrConsole/console_vars.h"
#include "../World/world.h"
#include "../Newton/newton_node.h"

#define MAX_SND_DIST 600.0f*IrrToSL

inline String fileVariation( const char* strFile )
{
    String file;
    int rnum = 10;

    file = strFile;
    for ( int i = 0; i < file.size(); i++ )
    {
      if ( file[i] == '?' )
      {
        while ( 1 )
        {
          file = strFile;
          file[i] = 48 + random( rnum );
          //CONSOLE.add( file.c_str() );
          if ( fileExists2( file.c_str() ) )
          {
            return APP.useFile( file.c_str() );
          }
          rnum = file[i] - 48;
        }
      }
    }

    return APP.useFile( strFile );
}

// ##############################
// ## CSoundListenerEnvironment ##
// ##############################

CSoundListenerEnvironment::CSoundListenerEnvironment()
{
}

CSoundListenerEnvironment::~CSoundListenerEnvironment()
{
}

void CSoundListenerEnvironment::bindEnvironment()
{
    //  fslSetListenerEnvironmentA(&env);
}

void CSoundListenerEnvironment::loadEnvironment( const char* strFile )
{
    //  env = fslLoadListenerEnvironment(strFile);
}

void CSoundListenerEnvironment::loadEnvironmentFromData( const char* strData, unsigned int size )
{
    //  env = fslCreateListenerEnvironmentA(strData,size);
}


// #################
// ## CSoundObject ##
// #################

CSoundObject::CSoundObject( const char* strFile )
{
    obj = fslLoadSound( fileVariation( strFile ).c_str() );
    recheckDie = 0;
}

CSoundObject::CSoundObject( FSLsound p )
{
    obj = p;
    recheckDie = 0;
}

CSoundObject::~CSoundObject()
{
    if ( obj )
    {
      if ( SOUND.initialised )
      {
        fslFreeSound( obj, true );
      }
    }
    obj = 0;
}

void CSoundObject::play()
{
    if ( obj )
    {
      fslSoundPlay( obj );
    }
}

void CSoundObject::rewind()
{
    if ( obj )
    {
      fslSoundRewind( obj );
    }
}

void CSoundObject::stop()
{
    if ( obj )
    {
      fslSoundStop( obj );
    }
}

void CSoundObject::pause()
{
    if ( obj )
    {
      fslSoundPause( obj );
    }
}

bool CSoundObject::isPlaying()
{
    if ( obj )
    {
      return fslSoundIsPlaying( obj );
    }
    else
    {
      return false;
    }
}

bool CSoundObject::isPaused()
{
    if ( obj )
    {
      return fslSoundIsPaused( obj );
    }
    else
    {
      return false;
    }
}

void CSoundObject::setVolume( float vol )
{
    if ( obj )
    {
      fslSoundSetGain( obj, vol );
    }
}

void CSoundObject::setLooping( bool loop_sound )
{
    if ( obj )
    {
      fslSoundSetLooping( obj, loop_sound );
    }
}

void CSoundObject::setPosition( float* position )
{
    if ( obj )
    {
      fslSoundSetPositionVec( obj, position );
    }
}

void CSoundObject::setPosition( float x, float y, float z )
{
    if ( obj )
    {
      fslSoundSetPosition( obj, x, y, z );
    }
}

void CSoundObject::setVelocity( float* velocity )
{
    if ( obj )
    {
      fslSoundSetVelocityVec( obj, velocity );
    }
}

void CSoundObject::setVelocity( float x, float y, float z )
{
    if ( obj )
    {
      fslSoundSetVelocity( obj, x, y, z );
    }
}

void CSoundObject::setPitch( float pitch )
{
    if ( obj )
    {
      fslSoundSetPitch( obj, pitch );
    }
}

void CSoundObject::setGain( float gain )
{
    if ( obj )
    {
      fslSoundSetGain( obj, gain );
    }
}

void CSoundObject::setMaxDistance( float max_distance )
{
    if ( obj )
    {
      fslSoundSetMaxDistance( obj, max_distance );
    }
}

void CSoundObject::setConeOrientation( float* direction )
{
    if ( obj )
    {
      fslSoundSetConeOrientationVec( obj, direction );
    }
}

void CSoundObject::setConeOrientation( float x, float y, float z )
{
    if ( obj )
    {
      fslSoundSetConeOrientation( obj, x, y, z );
    }
}

void CSoundObject::setConeInsideAngle( unsigned int val )
{
    if ( obj )
    {
      fslSoundSetConeInsideAngle( obj, val );
    }
}

void CSoundObject::setConeOutsideAngle( unsigned int val )
{
    if ( obj )
    {
      fslSoundSetConeOutsideAngle( obj, val );
    }
}

void CSoundObject::setConeOutsideVolume( int val )
{
    if ( obj )
    {
      fslSoundSetConeOutsideVolume( obj, val );
    }
}

bool CSoundObject::isLooping()
{
    if ( obj )
    {
      return fslSoundIsLooping( obj );
    }
    else
    {
      return false;
    }
}


float* CSoundObject::getPosition()
{
    if ( obj )
    {
      float x[3];
      fslSoundGetPosition( obj, x[0], x[1], x[2] );
      return &x[0];
    }
    else
    {
      return 0;
    }
}

float* CSoundObject::getVelocity()
{
    if ( obj )
    {
      float x[3];
      fslSoundGetVelocity( obj, x[0], x[1], x[2] );
      return &x[0];
    }
    else
    {
      return 0;
    }
}

float CSoundObject::getPitch()
{
    if ( obj )
    {
      return fslSoundGetPitch( obj );
    }
    else
    {
      return 0;
    }
}

float CSoundObject::getGain()
{
    if ( obj )
    {
      return fslSoundGetGain( obj );
    }
    else
    {
      return 0;
    }
}

float CSoundObject::getMaxDistance()
{
    if ( obj )
    {
      return fslSoundGetMaxDistance( obj );
    }
    else
    {
      return 0;
    }
}

float* CSoundObject::getConeOrientation()
{
    if ( obj )
    {
      float x[3];
      fslSoundGetConeOrientation( obj, x[0], x[1], x[2] );
      return &x[0];
    }
    else
    {
      return 0;
    }
}

unsigned int CSoundObject::getConeInsideAngle()
{
    if ( obj )
    {
      return fslSoundGetConeInsideAngle( obj );
    }
    else
    {
      return 0;
    }
}

unsigned int CSoundObject::getConeOutsideAngle()
{
    if ( obj )
    {
      return fslSoundGetConeOutsideAngle( obj );
    }
    else
    {
      return 0;
    }
}

int CSoundObject::getConeOutsideVolume()
{
    if ( obj )
    {
      return fslSoundGetConeOutsideVolume( obj );
    }
    else
    {
      return 0;
    }
}

long CSoundObject::getBufferLength()
{
    if ( obj )
    {
      return fslSoundGetBufferLength( obj );
    }
    else
    {
      return 0;
    }
}

long CSoundObject::getBufferPos()
{
    if ( obj )
    {
      return fslSoundGetBufferPosition( obj );
    }
    else
    {
      return 0;
    }
}

void CSoundObject::setBufferPos( long m )
{
    if ( obj )
    {
      fslSoundSetBufferPosition( obj, m );
    }
}

// #################
// ## CSoundEngine ##
// #################

CSoundEngine::CSoundEngine()
{
    CONSOLE_VAR( "s_soundon", int, on, 1, L"s_soundon [0/1]. Ex. s_soundon 1", L"Determines if the sound engine starts or not. Requires restart to turn sound on/off." );
    CONSOLE_VAR( "s_volume", float, fGain, 1.0f, L"s_volume [0.0-1.0]. Ex. s_volume 1.0", L"Sets the overall sound volume." );
    CONSOLE_VAR( "s_system", int, fSoundSystem, 2, L"s_system [0-7]. Ex. s_system 1", L"Sets the sound system (check config script for more info)." );

#ifndef _CLIENT
    on = 0;
#endif
    initialised = false;
}

CSoundEngine::~CSoundEngine()
{
}

bool CSoundEngine::Start()
{
    if ( !on )
    {
      return false;
    }

    initialised = loadCSoundEngine();

    setVolume( fGain );
    setMetersPerUnit( 1.0f );

    currentMusic = prevMusic = NULL;
    music_interpolator = NULL;

    //currentMusic = loadSound( "Music/crimson_march.ogg" );
    //currentMusic->play();
    //currentMusic->setLooping( true );
    //cm_looppoint_begin = 224016;
    //cm_looppoint_end = 5216000;
    //cm_volume = 0.0f;
    //currentMusic->setVolume( cm_volume );

    //nextMusic = loadSound( "Music/skyward_v2.ogg" );
    //nextMusic->play();
    //nextMusic->setLooping( true );
    //nm_looppoint_begin = 635136;
    //nm_looppoint_end = 8224560;
    //nm_volume = 1.0f;
    //nextMusic->setVolume( nm_volume );

    //music_interpolator = new CQuadraticTimeInterpolator( cm_volume, 400, 0.0f, 0.73f, 1.0f );

    //NewMusic( "Music/crimson_march.ogg", true, 224016, 5216000 );
    //NewMusic( "Music/skyward_v2.ogg", true, 635136, 8224560 );

    return initialised;
}

void CSoundEngine::NewMusic( const char* fileName, bool loop, int looppoint_begin, int looppoint_end, int fadetime, float mv )
{
    prevMusic = currentMusic;
    pm_looppoint_begin = cm_looppoint_begin;
    pm_looppoint_end = cm_looppoint_end;
    pm_volume = cm_volume;

    currentMusic = loadSound( fileName );
    currentMusic->play();
    currentMusic->setLooping( loop );
    cm_looppoint_begin = looppoint_begin;
    cm_looppoint_end = looppoint_end;
    cm_volume = 0.0f;
    currentMusic->setVolume( cm_volume );

    if ( fadetime > 0 )
    {
      music_interpolator = new CQuadraticTimeInterpolator( cm_volume, fadetime, 0.0f, mv, 1.0f );
    }
    else
    {
      cm_volume = mv;
      currentMusic->setVolume( cm_volume );
    }
}

void CSoundEngine::Stop()
{
    for ( int i = 0; i < soundObjects.size(); i++ )
    {
      killSound( soundObjects[i] );
    }
    soundObjects.clear();

    unloadCSoundEngine();

    initialised = false;
}

void CSoundEngine::Update()
{
    vector3df vCamPos, vCamVel;

    if ( GAME.worldLoaded )
    {
      if ( WORLD.GetCamera()->getTarget() )
      {
        vCamPos = WORLD.GetCamera()->getTarget()->getPosition() * IrrToSL;
        //vCamPos = WORLD.GetCamera()->getIrrCamera()->getPosition() * IrrToSL;
        //vCamPos.Z = 0.0f;
        vCamVel = ( WORLD.GetCamera()->getTarget()->getVelocity() ) * IrrToSL;
        vector3df vCamTarget( 0.0f, 0.0f, 1.0f );
        vector3df vCamUp( 0.0f, -1.0f, 0.0f );
        setListenerPosition( &vCamPos.X );
        setListenerVelocity( &vCamVel.X );
        setListenerOrientation( &vCamTarget.X, &vCamUp.X );
      }
    }
    else
    {
      vCamPos = vector3df( 0.0f, 0.0f, 0.0f );
      vCamVel = vector3df( 0.0f, 0.0f, 0.0f );
      vector3df vCamTarget( 0.0f, 0.0f, 1.0f );
      vector3df vCamUp( 0.0f, -1.0f, 0.0f );
      setListenerPosition( &vCamPos.X );
      setListenerVelocity( &vCamVel.X );
      setListenerOrientation( &vCamTarget.X, &vCamUp.X );
    }

    //setListenerRolloff( 1011.0f );
    //setListenerDistanceModel(true);

    //CONSOLE.addx( "Sound error: %i", fslGetError() );

    while ( garbageSoundObjects.size() )
    {
      killSound( garbageSoundObjects[0] );
      garbageSoundObjects.erase( 0 );
      //if (APP.DebugMode > 1)
      //    CONSOLE.addx("Sound object dropped, garb#%i sound#%i", garbageSoundObjects.size(), soundObjects.size() );
    }

    //if (KERNEL.GetTicks() % 160)
    //  CONSOLE.addx("fslCountSoundsTotal#%i    fslGetSoundMemoryUsage#%i", fslCountSoundsTotal(), fslGetSoundMemoryUsage() );


    int i;
    // remove the sound if has stopped playing
    // TODO: Am I removing also paused sounds?
    // TODO: delete temporarily unused obj?
    // TODO: the sounds may be broken, only 1 sound of file plays at once now
    for ( i = 0; i < soundObjects.size(); i++ )
    {
      //CONSOLE.addx("Sound object#%i isPaused#%i isPlaying#%i", i, 
      //(int)soundObjects[i]->isPaused(),(int)soundObjects[i]->isPlaying());

      //CONSOLE.addx("Sound object buffer #%i", soundObjects[i]->getBufferPos() );
      //    if ( soundObjects[i]->recheckDie > 0 )
      if ( ( soundObjects[i]->isPlaying() == 0 ) )
      {
        // soundObjects[i]->stop();
        garbageSoundObjects.push_back( soundObjects[i] );
        soundObjects.erase( i );
        i = 0;
      }
    }

    //CONSOLE.addx( "currentMusic %i", currentMusic->getBufferPos() );

    // Music
    if ( currentMusic )
    {
      if ( currentMusic->isLooping() )
      {
        if ( ( cm_looppoint_end - currentMusic->getBufferPos() ) < 1000 )
        {
          currentMusic->setBufferPos( cm_looppoint_begin );
        }
      }

      currentMusic->setVolume( cm_volume );
    }
    if ( prevMusic )
    {
      if ( prevMusic->isLooping() )
      {
        if ( ( pm_looppoint_end - prevMusic->getBufferPos() ) < 1000 )
        {
          prevMusic->setBufferPos( pm_looppoint_begin );
        }
      }

      pm_volume = 1.0f - cm_volume;
      if ( pm_volume < 0.0f )
      {
        pm_volume = 0.0f;
      }
      prevMusic->setVolume( pm_volume );
    }

    setVolume( fGain );
    fslUpdate();
    fslSleep( 0.001f );
}

bool CSoundEngine::loadCSoundEngine()
{
    return fslInit( ( FSL_SOUND_SYSTEM )fSoundSystem );
}

bool CSoundEngine::unloadCSoundEngine()
{
    fslShutDown();
    return true;
}

CSoundObject* CSoundEngine::playSound( const char* strFile )
{
    if ( !on )
    {
      return 0;
    }
    CSoundObject* p = loadSound( fileVariation( strFile ).c_str() );
    p->play();
    return p;
}

CSoundObject* CSoundEngine::playSound( const char* strFile, vector3df vPosition, vector3df vVelocity, float fGain, float fPitch )
{
    CSoundObject* p = playSound( fileVariation( strFile ).c_str() );
    if ( p )
    {
      vPosition = vPosition * IrrToSL;
      vVelocity = vVelocity * IrrToSL;
      p->setPosition( &vPosition.X );
      p->setVelocity( &vVelocity.X );
      p->setGain( fGain );
      p->setPitch( fPitch );
    }
    return p;
}

CSoundObject* CSoundEngine::getDummySound()
{
    CSoundObject* p = new CSoundObject( fslCreateDummySound() );
    return p;
}

CSoundObject* CSoundEngine::loadSound( const char* strFile )
{
    //FSLsound* ob = fslFindSound( fileVariation( strFile ).c_str() );

    //if ( !ob)
    //{
    //  ob = fslLoadSound( fileVariation( strFile ).c_str() );
    //  //CONSOLE.addx( COLOR_ERROR, "Loaded sound: %s", strFile );
    //}

    FSLsound ob = fslLoadSound( fileVariation( strFile ).c_str() );
    //  CONSOLE.addx( COLOR_ERROR, "soundObjects: %s,   %i", strFile, ob );

    if ( ob == 0 )
    {
      if ( on )
      {
        CONSOLE.addx( COLOR_ERROR, "Could not load sound: %s", strFile );
      }
      return getDummySound(); // or return null
    }

    //for (int i=0; i < soundObjects.size(); i++)
    //{
    //  if ( soundObjects[i]->obj == ob )
    //  {
    //      return soundObjects[i];
    //  }
    //}

    CSoundObject* p = new CSoundObject( ob );
    p->setMaxDistance( MAX_SND_DIST );

    soundObjects.push_back( p );

    return p;
}

void CSoundEngine::precacheSound( const char* strFile )
{
    if ( on )
    {
      FSLsound ob = fslLoadSound( strFile );

      if ( ob == 0 )
      {
        CONSOLE.addx( COLOR_ERROR, "Could not load sound: %s", strFile );
      }
    }
}

void CSoundEngine::killSound( CSoundObject* p )
{
    if ( p )
    {
      delete p;
    }
}

void CSoundEngine::setListenerPosition( float* position )
{
    fslSetListenerPositionVec( position );
}

void CSoundEngine::setListenerVelocity( float* velocity )
{
    fslSetListenerVelocityVec( velocity );
}

void CSoundEngine::setListenerOrientation( float* at, float* up )
{
    fslSetListenerOrientationVec( at, up );
}

void CSoundEngine::setListenerPosition( float x, float y, float z )
{
    fslSetListenerPosition( x, y, z );
}

void CSoundEngine::setListenerVelocity( float x, float y, float z )
{
    fslSetListenerVelocity( x, y, z );
}

void CSoundEngine::setListenerOrientation( float atx, float aty, float atz, float upx, float upy, float upz )
{
    fslSetListenerOrientation( atx, aty, atz, upx, upy, upz );
}

void CSoundEngine::setListenerRolloff( float val )
{
    fslSetRolloff( val );
}

void CSoundEngine::setListenerDistanceModel( bool is_clamped )
{
    fslSetListenerDistanceModel( ( is_clamped ) ? FSL_INVERSE_DISTANCE_CLAMPED : FSL_INVERSE_DISTANCE );
}

void CSoundEngine::setDefaultListenerEnvironment()
{
    fslSetListenerDefaultEnvironment();
}

void CSoundEngine::setDopplerParameters( float factor, float velocity )
{
    fslSetDopplerParameters( factor, velocity );
}

void CSoundEngine::setVolume( float val )
{
    fslSetVolume( val );
}

void CSoundEngine::setSpeedOfSound( float val )
{
    fslSetSpeedOfSound( val );
}

void CSoundEngine::setMetersPerUnit( float val )
{
    fslSetMetersPerUnit( val );
}


