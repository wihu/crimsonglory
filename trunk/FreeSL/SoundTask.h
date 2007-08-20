
// By: Lukas Heise (axodoss@hotmail.com)
// Date: 2005-05-01

#ifndef __SOUND_ENGINE_H
#define __SOUND_ENGINE_H

// if you want to use the static library then uncomment 
// the next two lines and remove the third.
//#define FREESL_STATIC
//#pragma comment(lib,"freesl_static.lib")
//#pragma comment(lib,"freesl.lib")

#include "../Engine/engine.h"
#include "freesl.h"

const float IrrToSL = 0.05f;

class CSoundEntity;

class CSoundListenerEnvironment
{
  private:
    //  FREESLEAXLISTENERPROPERTIES env;

  public:
    CSoundListenerEnvironment();
    ~CSoundListenerEnvironment();

    void bindEnvironment();
    void loadEnvironment( const char* strFile );
    void loadEnvironmentFromData( const char* strData, unsigned int size );
};


class CSoundObject
{
  public:
    CSoundObject( const char* strFile );
    CSoundObject( FSLsound p );
    ~CSoundObject();

    void play();
    void rewind();
    void stop();
    void pause();
    bool isPlaying();
    bool isPaused();
    void setVolume( float vol );

    void setLooping( bool loop_sound );
    void setPosition( float* position );
    void setPosition( float x, float y, float z );
    void setVelocity( float* velocity );
    void setVelocity( float x, float y, float z );
    void setPitch( float pitch );
    void setGain( float gain );
    void setMaxDistance( float max_distance );
    void setConeOrientation( float* direction );
    void setConeOrientation( float x, float y, float z );
    void setConeInsideAngle( unsigned int val );
    void setConeOutsideAngle( unsigned int val );
    void setConeOutsideVolume( int val );

    bool isLooping();
    float* getPosition();
    float* getVelocity();
    float getPitch();
    float getGain();
    float getMaxDistance();
    float* getConeOrientation();
    unsigned int getConeInsideAngle();
    unsigned int getConeOutsideAngle();
    int getConeOutsideVolume();
    long getBufferLength();
    long getBufferPos();
    void setBufferPos( long m );

    int recheckDie;

  protected:
    friend class CSoundEngine;
    FSLsound obj;
};

#define SOUND CSoundEngine::GetSingleton()

class CSoundEngine : public ITask, public Singleton<CSoundEngine>
{
  public:
    CSoundEngine();
    virtual ~CSoundEngine();
    AUTO_SIZE;

    virtual bool Start();
    virtual void Update();
    virtual void Stop();

    CSoundObject* playSound( const char* strFile );
    CSoundObject* playSound( const char* strFile, vector3df vPosition, vector3df vVelocity = vector3df(  0.0f,0.0f,0.0f), float fGain = 1.0f, float fPitch = 1.0f );

    void precacheSound( const char* strFile );

    // listener functions
    void setListenerPosition( float* position );
    void setListenerVelocity( float* velocity );
    void setListenerOrientation( float* at, float* up );
    void setListenerPosition( float x, float y, float z );
    void setListenerVelocity( float x, float y, float z );
    void setListenerOrientation( float atx, float aty, float atz, float upx, float upy, float upz );
    void setListenerRolloff( float val );
    void setListenerDistanceModel( bool is_clamped );

    // EAX functions
    void setDefaultListenerEnvironment();

    // other sound functions
    void setDopplerParameters( float factor, float velocity );
    void setVolume( float val );
    void setSpeedOfSound( float val );
    void setMetersPerUnit( float val );

    // sound engine on or not
    int on;
    bool initialised;

    void NewMusic( const char* fileName, bool loop = false, int looppoint_begin = 0, int looppoint_end = 0, int fadetime = 400, float mv = 0.73f );

  private:
    // init/destroy functions
    bool loadCSoundEngine();
    bool unloadCSoundEngine();

    // sound loading
    CSoundObject* getDummySound(); // returns an empty sound object
    CSoundObject* loadSound( const char* strFile );
    void killSound( CSoundObject* p );

    array<CSoundObject*> soundObjects;
    array<CSoundObject*> garbageSoundObjects;

    float fGain;
    int fSoundSystem;

    //music
    CSoundObject* currentMusic, * prevMusic;
    int cm_looppoint_begin, cm_looppoint_end, pm_looppoint_begin, pm_looppoint_end;
    float cm_volume, pm_volume;
    ITimebasedInterpolator* music_interpolator;
};

#endif

