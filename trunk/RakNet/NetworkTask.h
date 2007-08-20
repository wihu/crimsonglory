#ifndef __NETWORKTASK_H
#define __NETWORKTASK_H

#include "../Engine/engine.h"
#include "../RakNet/network.h"

class CNetworkTask : public ITask
{
  public:
    CNetworkTask()
    {
    }
    virtual ~CNetworkTask()
    {
    }
    AUTO_SIZE;

  protected:
    virtual bool Start()
    {
        return NET.Start();
    }

    virtual void Update()
    {
        NET.Update();
    }

    virtual void Stop()
    {
        NET.Stop();
    }
};

#endif
