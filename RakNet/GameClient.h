#ifndef __GAMECLIENT_H
#define __GAMECLIENT_H

#include "network.h"

class CGameClient : public IProcessPacket
{
  public:
    CGameClient();
    ~CGameClient();
    virtual void ProcessPacket( unsigned char packetIdentifier );
    virtual void ProcessError( const char* text, int error );

    static void SendChat( char* text, u16 addresser );

  protected:
    virtual void ReceiveConnectionRequestAccepted( Packet* packet );
    virtual void ReceivedStaticData( Packet* packet );
    virtual void ReceiveRemoteStaticData( Packet* packet );

    virtual void ReceiveNewActor( Packet* packet );
    virtual void ReceiveVerifyFiles( Packet* packet );
    virtual void ReceiveChat( Packet* packet );

    WideString client_name;
};


#endif
