#ifndef __SERVER_H
#define __SERVER_H

#include "PacketEnumerations.h"
#include "RakNetworkFactory.h"
#include "RakServerInterface.h"

class ServerMultiplayer
{
  public:
    ServerMultiplayer();
    ~ServerMultiplayer();

    void ProcessPackets( RakServerInterface* interfaceType );

  protected:
    void ProcessUnhandledPacket( Packet* p, unsigned char packetIdentifier );

    void ReceiveNewIncomingConnection( Packet* packet );
    void ReceiveConnectionAttemptFailed( Packet* packet );
    void ReceiveNoFreeIncomingConnections( Packet* packet );
    void ReceiveDisconnectionNotification( Packet* packet );
    void ReceiveConnectionLost( Packet* packet );
    void ReceivedStaticData( Packet* packet );

    void ReceiveVerifyFiles( Packet* packet );
    void ReceiveClientOK( Packet* packet );

    void SendVerifyFiles();

    RakServerInterface* interfaceType;
};


#endif
