#ifndef SOUNDENTITY_H_INCLUDED
#define SOUNDENTITY_H_INCLUDED

#include "../Engine/engine.h"
#include "../World/entity.h"
#include "../FreeSL/SoundTask.h"
#include "../Newton/newton_node.h"

////////////////////////////////////////////
// CSoundEntity 
////////////////////////////////////////////

class CSoundEntity : public CSoundObject, public CEntity
{
  public:
    CSoundEntity( const char* strFile, CNewtonNode* ownerNode );
    virtual ~CSoundEntity();

    virtual void Reset();
    virtual void Think();
    virtual void Render();

  private:
    CNewtonNode* owner;
};

#endif