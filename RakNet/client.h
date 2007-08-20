#ifndef __CLIENT_H
#define __CLIENT_H

#include "PacketEnumerations.h"
#include "RakNetworkFactory.h"
#include "RakClientInterface.h"

class ClientMultiplayer
{
  public:
    ClientMultiplayer();
    ~ClientMultiplayer();
    void ProcessPackets( RakClientInterface* interfaceType );

  protected:
    void ProcessUnhandledPacket( Packet* packet, unsigned char packetIdentifier );

    void ReceiveConnectionRequestAccepted( Packet* packet );
    void ReceivedStaticData( Packet* packete );
    void ReceiveRemoteStaticData( Packet* packete );

    void ReceiveVerifyFiles( Packet* packet );

    RakClientInterface* interfaceType;
};

#endif
