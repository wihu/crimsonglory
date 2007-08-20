#ifndef EDITOR_H_INCLUDED
#define EDITOR_H_INCLUDED

#include "../Engine/engine.h"
#include "entity.h"

////////////////////////////////////////////
// CEditor 
////////////////////////////////////////////

class CEditor
{
  public:
    CEditor();
    virtual ~CEditor();

    virtual void Reset();
    virtual void Think();
    virtual void Render();

    void DeleteActorAtCursor( f32 pickradius );
    void DeleteRespawnAtCursor( f32 pickradius );
    void SelectAtCursor( f32 pickradius );

    String actorConfigFile;

  private:
    array<String> cmenu;
    vector3df menuStartMousePosWorld;
    f32 pickRadius;

    s32 actorPick, respawnPick;
};

#endif