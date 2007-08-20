#ifndef ENTITY_H_INCLUDED
#define ENTITY_H_INCLUDED

#include "../Engine/engine.h"

////////////////////////////////////////////
// CEntity 
// - entity is an object that is called once per frame by the world container
////////////////////////////////////////////

class CEntity// : public CRenderable

{
  public:
    // on construction it adds itself to the WORLD.Entitys list
    CEntity();
    // delete - should never be called on entity only .Die()
    virtual ~CEntity();
    //AUTO_SIZE;

    virtual void Reset();
    virtual void Think();
    virtual void Render();

    void setDebugText( String w )
    {
        debugText = w;
    }
    const c8* getDebugText()
    {
        return debugText.c_str();
    }
    void setDebugPos( core::position2d<s32> p )
    {
        debugScreenPos = p;
    }
    core::position2d<s32> getDebugPos()
    {
        return debugScreenPos;
    }
    // if entity should not think
    bool ValidEntity()
    {
        return ( !bInvalidEntity && !bCanDie );
    }

  protected:
    friend class CWorldTask;

    bool bInvalidEntity;
    int alive, timeOut;

    void Die();
    bool bCanDie;

  private:
    String debugText;
    core::position2d<s32> debugScreenPos;
};


#endif