#ifndef __GAMESERVER_H
#define __GAMESERVER_H

#include "network.h"

class CActor;

class CGameServer : public IProcessPacket
{
  public:
    CGameServer();
    ~CGameServer();

    virtual void ProcessPacket( unsigned char packetIdentifier );
    virtual void ProcessError( const char* text, int error );

    static void BroadcastActor( CActor* actor );

  protected:
    virtual void ReceiveNewIncomingConnection( Packet* packet );
    virtual void ReceiveConnectionAttemptFailed( Packet* packet );
    virtual void ReceiveNoFreeIncomingConnections( Packet* packet );
    virtual void ReceiveDisconnectionNotification( Packet* packet );
    virtual void ReceiveConnectionLost( Packet* packet );
    virtual void ReceivedStaticData( Packet* packet );

    virtual void ReceiveChat( Packet* packet );
    virtual void ReceiveClientOK( Packet* packet );
};

#endif
