#include "client.h"

#include "network.h"
#include "../App/app.h"

#include "CustomPackets.h"

ClientMultiplayer::ClientMultiplayer()
{
}

ClientMultiplayer::~ClientMultiplayer()
{
}

void ClientMultiplayer::ProcessPackets( RakClientInterface* interfaceType )
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
          //        ReceiveRemoteDisconnectionNotification( p );
          break;

        case ID_REMOTE_CONNECTION_LOST:
          //        ReceiveRemoteConnectionLost( p );
          break;

        case ID_REMOTE_NEW_INCOMING_CONNECTION:
          //        ReceiveRemoteNewIncomingConnection( p );
          break;

        case ID_REMOTE_EXISTING_CONNECTION:
          //        ReceiveRemoteExistingConnection( p );
          break;

        case ID_REMOTE_STATIC_DATA:
          ReceiveRemoteStaticData( p );
          break;

        case ID_CONNECTION_BANNED:
          //        ReceiveConnectionBanned( p );
          break;

        case ID_CONNECTION_REQUEST_ACCEPTED:
          ReceiveConnectionRequestAccepted( p );
          break;

        case ID_NEW_INCOMING_CONNECTION:
          //        ReceiveNewIncomingConnection( p );
          break;

        case ID_CONNECTION_ATTEMPT_FAILED:
          //        ReceiveConnectionAttemptFailed( p );
          break;

        case ID_NO_FREE_INCOMING_CONNECTIONS:
          //        ReceiveNoFreeIncomingConnections( p );
          break;

        case ID_DISCONNECTION_NOTIFICATION:
          //        ReceiveDisconnectionNotification( p );
          break;

        case ID_CONNECTION_LOST:
          //        ReceiveConnectionLost( p );
          break;

        case ID_RECEIVED_STATIC_DATA:
          ReceivedStaticData( p );
          break;

        case ID_INVALID_PASSWORD:
          //        ReceiveInvalidPassword( p );
          break;

        case ID_MODIFIED_PACKET:
          //        ReceiveModifiedPacket( p );
          break;

          //        case ID_REMOTE_PORT_REFUSED:
          //        ReceiveRemotePortRefused( p );
          //        break;

          //        case ID_VOICE_PACKET:
          //        ReceiveVoicePacket( p );
          //        break;

        case ID_PONG:
          //        ReceivePong( p );
          break;

        case ID_ADVERTISE_SYSTEM:
          //        ReceiveAdvertisedSystem( p );
          break;

          //    case ID_UPDATE_DISTRIBUTED_NETWORK_OBJECT:
          //    ReceiveDistributedNetworkObject( p );
          //    break;

          //    case ID_DISTRIBUTED_NETWORK_OBJECT_CREATION_ACCEPTED:
          //    ReceiveDistributedNetworkObjectCreationAccepted( p );
          //    break;

          //    case ID_DISTRIBUTED_NETWORK_OBJECT_CREATION_REJECTED:
          //    ReceiveDistributedNetworkObjectCreationRejected( p );
          //    break;

        case ID_AUTOPATCHER_REQUEST_FILE_LIST:
          //    ReceiveAutopatcherRequestFileList( p );
          break;

        case ID_AUTOPATCHER_FILE_LIST:
          //    ReceiveAutopatcherFileList( p );
          break;

        case ID_AUTOPATCHER_REQUEST_FILES:
          //    ReceiveAutopatcherRequestFiles( p );
          break;

        case ID_AUTOPATCHER_SET_DOWNLOAD_LIST:
          //    ReceiveAutopatcherSetDownloadList( p );
          break;

        case ID_AUTOPATCHER_WRITE_FILE:
          //    ReceiveAutopatcherWriteFile( p );
          break;

          ///

        case ID_CRIMSON_VERIFYFILES:
          ReceiveVerifyFiles( p );
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

void ClientMultiplayer::ProcessUnhandledPacket( Packet* p, unsigned char packetIdentifier )
{
    if ( NET.clientGameProcess )
    {
      NET.clientGameProcess->importPacket.length = p->length;
      NET.clientGameProcess->importPacket.bitSize = p->bitSize;
      NET.clientGameProcess->importPacket.deleteData = p->deleteData;
      NET.clientGameProcess->importPacket.playerId = p->playerId;
      NET.clientGameProcess->importPacket.playerIndex = p->playerIndex;
      NET.clientGameProcess->importPacket.data = p->data;

      NET.clientGameProcess->ProcessPacket( packetIdentifier );
    }
}

void ClientMultiplayer::ReceiveConnectionRequestAccepted( Packet* packet )
{
    // ...
    ProcessUnhandledPacket( packet, ID_CONNECTION_REQUEST_ACCEPTED );
}

void ClientMultiplayer::ReceivedStaticData( Packet* packet )
{
    // ...
    ProcessUnhandledPacket( packet, ID_RECEIVED_STATIC_DATA );
}

void ClientMultiplayer::ReceiveRemoteStaticData( Packet* packet )
{
    // ...
    ProcessUnhandledPacket( packet, ID_REMOTE_STATIC_DATA );
}

void ClientMultiplayer::ReceiveVerifyFiles( Packet* packet )
{
    int i;
    RakNet::BitStream inBitStream( ( unsigned char* )packet->data, packet->length, false );
    inBitStream.Read( i ); // ID_CRIMSON_VERIFYFILES

    char* output = 0;
    output = NET.fileVerifier->DeserializeAndValidate( &inBitStream );

    printf( " CLIENT------------ %i\n", APP.filesUsed.size() );
    //  APPLOG.Write( "---------------++++++++++++++++++" );
    //  APPLOG.Write( output );

    if ( output )
    {
      if ( NET.clientGameProcess )
      {
        NET.clientGameProcess->ProcessError( output, ERROR_FILE_VERIFICATION_FAILED );
        NET.rakServer->Kick( packet->playerId );
      }
    }

    if ( !output )
    {
      RakNet::BitStream myBitStream;
      myBitStream.Write( ID_CRIMSON_CLIENTOK );
      myBitStream.Write( random( 5000 ) );
      interfaceType->Send( &myBitStream, HIGH_PRIORITY, RELIABLE, 0 );
    }

    ProcessUnhandledPacket( packet, ID_CRIMSON_VERIFYFILES );
}
