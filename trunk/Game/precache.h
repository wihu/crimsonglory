#ifndef PRECACHE_H_INCLUDED
#define PRECACHE_H_INCLUDED

#include "../Engine/engine.h"

////////////////////////////////////////////
// CPrecache 
////////////////////////////////////////////

class CPrecache
{
  public:
    CPrecache();
    virtual ~CPrecache();

    void LoadFromFile( const char* filename );
    void SaveToFile( const char* filename );

  private:
};

#endif