#include "server.h"

#include "network.h"
#include "../App/app.h"

#include "CustomPackets.h"

ServerMultiplayer::ServerMultiplayer()
{
}

ServerMultiplayer::~ServerMultiplayer()
{
}

void ServerMultiplayer::ProcessPackets( RakServerInterface* interfaceType )
{
    Packet* p;
    unsigned char packetIdentifier;

    this->interfaceType = interfaceType;

    p = interfaceType->Receive();

    while ( p )
    {
      if ( ( unsigned char ) p->data[0] == ID_TIMESTAMP )
      {
        if ( p->length > sizeof( unsigned char ) + sizeof( unsigned int ) )
        {
          packetIdentifier = ( unsigned char ) p->data[sizeof( unsigned char ) + sizeof( unsigned int )];
        }
        else
        {
          break;
        }
      }
      else
      {
        packetIdentifier = ( unsigned char ) p->data[0];
      }


      // Check if this is a native packet
      switch ( packetIdentifier )
      {
        case ID_REMOTE_DISCONNECTION_NOTIFICATION:
          //    ReceiveRemoteDisconnectionNotification( p  );
          break;

        case ID_REMOTE_CONNECTION_LOST:
          //    ReceiveRemoteConnectionLost( p  );
          break;

        case ID_REMOTE_NEW_INCOMING_CONNECTION:
          //    ReceiveRemoteNewIncomingConnection( p  );
          break;

        case ID_REMOTE_EXISTING_CONNECTION:
          //    ReceiveRemoteExistingConnection( p  );
          break;

        case ID_REMOTE_STATIC_DATA:
          //    ReceiveRemoteStaticData( p  );
          break;

        case ID_CONNECTION_BANNED:
          //    ReceiveConnectionBanned( p  );
          break;

        case ID_CONNECTION_REQUEST_ACCEPTED:
          //    ReceiveConnectionRequestAccepted( p  );
          break;

        case ID_NEW_INCOMING_CONNECTION:
          ReceiveNewIncomingConnection( p );
          break;

        case ID_CONNECTION_ATTEMPT_FAILED:
          ReceiveConnectionAttemptFailed( p );
          break;

        case ID_NO_FREE_INCOMING_CONNECTIONS:
          ReceiveNoFreeIncomingConnections( p );
          break;

        case ID_DISCONNECTION_NOTIFICATION:
          ReceiveDisconnectionNotification( p );
          break;

        case ID_CONNECTION_LOST:
          ReceiveConnectionLost( p );
          break;

        case ID_RECEIVED_STATIC_DATA:
          ReceivedStaticData( p );
          break;

        case ID_INVALID_PASSWORD:
          //    ReceiveInvalidPassword( p  );
          break;

        case ID_MODIFIED_PACKET:
          //    ReceiveModifiedPacket( p  );
          break;

          //    case ID_REMOTE_PORT_REFUSED:
          //    ReceiveRemotePortRefused( p  );
          //    break;

          //    case ID_VOICE_PACKET:
          //    ReceiveVoicePacket( p  );
          //    break;

        case ID_PONG:
          //    ReceivePong( p  );
          break;

        case ID_ADVERTISE_SYSTEM:
          //    ReceiveAdvertisedSystem( p  );
          break;

          //    case ID_UPDATE_DISTRIBUTED_NETWORK_OBJECT:
          //    ReceiveDistributedNetworkObject( p  );
          //    break;

          //    case ID_DISTRIBUTED_NETWORK_OBJECT_CREATION_ACCEPTED:
          //    ReceiveDistributedNetworkObjectCreationAccepted( p  );
          //    break;

          //case ID_DISTRIBUTED_NETWORK_OBJECT_CREATION_REJECTED:
          //    ReceiveDistributedNetworkObjectCreationRejected( p  );
          //break;

        case ID_AUTOPATCHER_REQUEST_FILE_LIST:
          //    ReceiveAutopatcherRequestFileList( p  );
          break;

        case ID_AUTOPATCHER_FILE_LIST:
          //    ReceiveAutopatcherFileList( p  );
          break;

        case ID_AUTOPATCHER_REQUEST_FILES:
          //    ReceiveAutopatcherRequestFiles( p  );
          break;

        case ID_AUTOPATCHER_SET_DOWNLOAD_LIST:
          //    ReceiveAutopatcherSetDownloadList( p  );
          break;

        case ID_AUTOPATCHER_WRITE_FILE:
          //    ReceiveAutopatcherWriteFile( p  );
          break;

          ////

        case ID_CRIMSON_VERIFYFILES:
          ReceiveVerifyFiles( p );
          break;

        case ID_CRIMSON_CLIENTOK:
          ReceiveClientOK( p );
          break;

        default:
          // If not a native packet send it to ProcessUnhandledPacket which should have been written by the user
          ProcessUnhandledPacket( p, packetIdentifier );
          break;
      }

      interfaceType->DeallocatePacket( p );

      p = interfaceType->Receive();
    }
}

void ServerMultiplayer::ProcessUnhandledPacket( Packet* p, unsigned char packetIdentifier )
{
    if ( NET.serverGameProcess )
    {
      NET.serverGameProcess->importPacket.length = p->length;
      NET.serverGameProcess->importPacket.bitSize = p->bitSize;
      NET.serverGameProcess->importPacket.deleteData = p->deleteData;
      NET.serverGameProcess->importPacket.playerId = p->playerId;
      NET.serverGameProcess->importPacket.playerIndex = p->playerIndex;
      NET.serverGameProcess->importPacket.data = p->data;
      //printf( "server " );
      // for (int i=0; i < p->length; i++)
      // printf("%i ",p->data[i]);
      // printf("\n");
      NET.serverGameProcess->ProcessPacket( packetIdentifier );
    }
}

void ServerMultiplayer::ReceiveNewIncomingConnection( Packet* packet )
{
    // send verify files
    SendVerifyFiles();

    ProcessUnhandledPacket( packet, ID_NEW_INCOMING_CONNECTION );
}

void ServerMultiplayer::ReceiveConnectionAttemptFailed( Packet* packet )
{
    // ...
    ProcessUnhandledPacket( packet, ID_CONNECTION_ATTEMPT_FAILED );
}

void ServerMultiplayer::ReceiveNoFreeIncomingConnections( Packet* packet )
{
    // ...
    ProcessUnhandledPacket( packet, ID_NO_FREE_INCOMING_CONNECTIONS );
}

void ServerMultiplayer::ReceiveDisconnectionNotification( Packet* packet )
{
    // ...
    ProcessUnhandledPacket( packet, ID_DISCONNECTION_NOTIFICATION );
}

void ServerMultiplayer::ReceiveConnectionLost( Packet* packet )
{
    // ...
    ProcessUnhandledPacket( packet, ID_CONNECTION_LOST );
}

void ServerMultiplayer::ReceivedStaticData( Packet* packet )
{
    // ...
    ProcessUnhandledPacket( packet, ID_RECEIVED_STATIC_DATA );
}

void ServerMultiplayer::ReceiveVerifyFiles( Packet* packet )
{
    // ...

    //ProcessUnhandledPacket(packet, ID_CRIMSON_VERIFYFILES );
}

void ServerMultiplayer::ReceiveClientOK( Packet* packet )
{
    // ...

    ProcessUnhandledPacket( packet, ID_CRIMSON_CLIENTOK );
}

void ServerMultiplayer::SendVerifyFiles()
{
    RakNet::BitStream myBitStream;
    myBitStream.Write( ID_CRIMSON_VERIFYFILES );
    NET.fileVerifier->Serialize( &myBitStream );
    printf( " SERVER------------ %i\n", APP.filesUsed.size() );
    interfaceType->Send( &myBitStream, HIGH_PRIORITY, RELIABLE, 0, UNASSIGNED_PLAYER_ID, true );
}
