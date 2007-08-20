#include "network.h"

#include "../App/app.h"
#include "../IrrConsole/console_vars.h"
#include "../Engine/misc.h"

CNetwork::CNetwork()
{
    rakServer = RakNetworkFactory::GetRakServerInterface();
#ifdef _CLIENT
    rakClient = RakNetworkFactory::GetRakClientInterface();
#endif

    fileVerifier = new CFileVerifier();

    stringCompressor->AddReference();
}

CNetwork::~CNetwork()
{
    RakNetworkFactory::DestroyRakServerInterface( rakServer );

#ifdef _CLIENT
    RakNetworkFactory::DestroyRakClientInterface( rakClient );
#endif

    delete fileVerifier;

    stringCompressor->RemoveReference();
}

bool CNetwork::Start()
{
#ifdef _CLIENT
#endif

    return true;
}

void CNetwork::Update()
{
    rakServerMultiplayer.ProcessPackets( rakServer );
#ifdef _CLIENT
    rakClientMultiplayer.ProcessPackets( rakClient );
#endif
}

void CNetwork::Stop()
{
#ifdef _CLIENT
    rakClient->Disconnect( disconnectTime );
#endif

    rakServer->Disconnect( disconnectTime );
}

void CNetwork::Connect( const char* host, int remotePort )
{
#ifdef _CLIENT
    char* temp = new char[255];
    strcpy( temp, host );

    if ( APP.DebugMode )
    {
      WideString wstr = "Connecting to ";
      wstr += IPplusPort( host, remotePort );
      APPLOG.Write( wide2string( wstr ).c_str() );
    }

    //virtual bool Connect( const char* host, unsigned short serverPort, unsigned short clientPort, unsigned int depreciated, int threadSleepTimer )=0;
    rakClient->Connect( temp, remotePort, clientPort, 0, threadSleepTime );

    delete temp;
#endif
}

void CNetwork::Disconnect()
{
#ifdef _CLIENT

    if ( APP.DebugMode )
    {
      APPLOG.Write( "Disconnecting." );
    }

    rakClient->Disconnect( disconnectTime );
#endif
}

bool CNetwork::StartServer()
{
    if ( rakServer->IsActive() )
    {
      rakServer->Disconnect( disconnectTime );
      if ( APP.DebugMode )
      {
        APPLOG.Write( "Server disconnected." );
      }
      return true;
    }
    else
    {
      //Start( unsigned short AllowedPlayers, unsigned int depreciated, int threadSleepTimer, unsigned short port, const char *forceHostAddress=0 )=0;
      rakServer->Start( maxPlayers, 0, threadSleepTime, serverPort );
      if ( APP.DebugMode )
      {
        APPLOG.Write( "Server started." );
      }
      return false;
    }
}
