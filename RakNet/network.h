#ifndef __NETWORK_H
#define __NETWORK_H

#include "../Engine/engine.h"

#include "server.h"
#include "RakServerInterface.h"

#ifdef _CLIENT
#include "client.h"
#include "RakClientInterface.h"
#endif

#include "PacketEnumerations.h"
#include "RakNetworkFactory.h"
#include "Multiplayer.h"
#include "NetworkTypes.h"
#include "StringCompressor.h"
#include "FileVerifier.h"

enum NET_ERRORS { ERROR_FILE_VERIFICATION_FAILED };

class IProcessPacket;
class CFileVerifier;

using namespace RakNet;

#define NET CNetwork::GetSingleton()

class CNetwork : public Singleton<CNetwork>
{
  public:
    CNetwork();
    virtual ~CNetwork();

    virtual bool Start();
    virtual void Update();
    virtual void Stop();

    virtual void Connect( const char* host, int remotePort );
    virtual void Disconnect();

    int getClientPort()
    {
        return clientPort;
    }

    virtual bool StartServer(); // return true if shutdown

    RakServerInterface* rakServer;
#ifdef _CLIENT
    RakClientInterface* rakClient;
#endif

    IProcessPacket* serverGameProcess;
#ifdef _CLIENT
    IProcessPacket* clientGameProcess;
#endif

    int disconnectTime;
    int maxPlayers;
    int serverPort, clientPort;
    int threadSleepTime;

    CFileVerifier* fileVerifier;

  protected:
    ServerMultiplayer rakServerMultiplayer;
#ifdef _CLIENT
    ClientMultiplayer rakClientMultiplayer;
#endif


  private:
};


class IProcessPacket
{
  public:
    virtual void ProcessPacket( unsigned char packetIdentifier ) = 0;
    virtual void ProcessError( const char* text, int error ) = 0;

    // don't touch this, it is for packet exchange between EXE and DLL
    Packet importPacket;
};

// Derive a new class to handle unknown files
class CFileVerifier : public FileVerifier
{
    bool OnUnknownFile( char* filename )
    {
        //printf("Client has the file %s which we don't know about.\nPress 'y' to allow the file, 'n' to fail.\n",filename);
        //char buff[256];
        //if(gets(buff)&&(buff[0]=='y'))
        //  return true;
        //else
        //  return false;
        return true;
    }
};

#endif
