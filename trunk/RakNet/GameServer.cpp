#include "GameServer.h"

#include "../App/app.h"
#include "../Game/SingletonIncludes.h"

#include "../World/world.h"
#include "../World/player.h"
#include "../World/actor.h"
#include "../World/map.h"
#include "../World/rules.h"

#include "CustomPackets.h"


CGameServer::CGameServer()
{
    stringCompressor->AddReference();
    NET.rakServer->StartOccasionalPing();
}

CGameServer::~CGameServer()
{
    stringCompressor->RemoveReference();
}

void CGameServer::ProcessPacket( unsigned char packetIdentifier )
{
    Packet* p = &importPacket;

    switch ( packetIdentifier )
    {
        //  case ID_REMOTE_DISCONNECTION_NOTIFICATION:
        ////    ReceiveRemoteDisconnectionNotification( p  );
        //  break;
        //  
        //  case ID_REMOTE_CONNECTION_LOST:
        ////    ReceiveRemoteConnectionLost( p  );
        //  break;
        //  
        //  case ID_REMOTE_NEW_INCOMING_CONNECTION:
        ////    ReceiveRemoteNewIncomingConnection( p  );
        //  break;
        //  
        //  case ID_REMOTE_EXISTING_CONNECTION:
        ////    ReceiveRemoteExistingConnection( p  );
        //  break;
        //  
        //case ID_REMOTE_STATIC_DATA:
        //ReceiveRemoteStaticData( p  );
        //break;
        //
        //  case ID_CONNECTION_BANNED:
        ////    ReceiveConnectionBanned( p  );
        //  break;
        //  
        //  case ID_CONNECTION_REQUEST_ACCEPTED:
        ////    ReceiveConnectionRequestAccepted( p  );
        //  break;

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

        //  case ID_INVALID_PASSWORD:
        ////    ReceiveInvalidPassword( p  );
        //  break;
        //  
        //  case ID_MODIFIED_PACKET:
        ////    ReceiveModifiedPacket( p  );
        //  break;
        //  
        //  case ID_REMOTE_PORT_REFUSED:
        ////    ReceiveRemotePortRefused( p  );
        //  break;
        //  
        //  case ID_VOICE_PACKET:
        ////    ReceiveVoicePacket( p  );
        //  break;
        //  
        //  case ID_PONG:
        ////    ReceivePong( p  );
        //  break;
        //  
        //  case ID_ADVERTISE_SYSTEM:
        ////    ReceiveAdvertisedSystem( p  );
        //  break;
        //  
        //  case ID_UPDATE_DISTRIBUTED_NETWORK_OBJECT:
        ////    ReceiveDistributedNetworkObject( p  );
        //  break;
        //  
        //  case ID_DISTRIBUTED_NETWORK_OBJECT_CREATION_ACCEPTED:
        ////    ReceiveDistributedNetworkObjectCreationAccepted( p  );
        //  break;
        //  
        //  case ID_DISTRIBUTED_NETWORK_OBJECT_CREATION_REJECTED:
        ////    ReceiveDistributedNetworkObjectCreationRejected( p  );
        //  break;
        //  
        //  case ID_AUTOPATCHER_REQUEST_FILE_LIST:
        ////    ReceiveAutopatcherRequestFileList( p  );
        //  break;
        //  
        //  case ID_AUTOPATCHER_FILE_LIST:
        ////    ReceiveAutopatcherFileList( p  );
        //  break;
        //  
        //  case ID_AUTOPATCHER_REQUEST_FILES:
        ////    ReceiveAutopatcherRequestFiles( p  );
        //  break;
        //  
        //  case ID_AUTOPATCHER_SET_DOWNLOAD_LIST:
        ////    ReceiveAutopatcherSetDownloadList( p  );
        //  break;
        //  
        //  case ID_AUTOPATCHER_WRITE_FILE:
        ////    ReceiveAutopatcherWriteFile( p  );
        //  break;

      case ID_CRIMSON_CHAT:
        ReceiveChat( p );
        break;

      case ID_CRIMSON_CLIENTOK:
        ReceiveClientOK( p );
        break;

      default:
        // If not a native packet send it to ProcessUnhandledPacket which should have been written by the user
        //
        CONSOLE.addx( "Server: UNKNOWN PACKET ID %i from %u:%u", packetIdentifier, p->playerId.binaryAddress, p->playerId.port );
        break;
    }
}

void CGameServer::ProcessError( const char* text, int error )
{
}

void CGameServer::ReceiveNewIncomingConnection( Packet* packet )
{
    if ( APP.DebugMode )
    {
      CONSOLE.addx( "Server: ID_NEW_INCOMING_CONNECTION from PlayerID:%u:%u on %p.", packet->playerId.binaryAddress, packet->playerId.port );
    }
}

void CGameServer::ReceiveClientOK( Packet* packet )
{
    if ( APP.DebugMode )
    {
      CONSOLE.addx( "Server: ID_CRIMSON_CLIENTOK from PlayerID:%u:%u on %p.", packet->playerId.binaryAddress, packet->playerId.port );
    }

    CPlayer* player = WORLD.GetPlayers()->AddPlayer( packet->playerId );

    // send all active actors / loop through actors with no parents
    for ( int i = 0; i < CActor::actorsList.size(); i++ )
    {
      if ( !CActor::actorsList[i]->isAttached() )
      {
        CActor::actorsList[i]->Broadcast();
      }
    }

    // BUG:? duplicating when reconnecting because of queue
    //WORLD.GetMap()->GetRespawn()->AddToQueue( "soldier", "Scripts/soldier_dod.gm", 10, player );
    WORLD.GetRules()->OnNewPlayerJoin( player );
}


void CGameServer::ReceiveConnectionAttemptFailed( Packet* packet )
{
    if ( APP.DebugMode )
    {
      CONSOLE.addx( "Server: ID_CONNECTION_ATTEMPT_FAILED from PlayerID:%u:%u on %p.", packet->playerId.binaryAddress, packet->playerId.port );
    }
}

void CGameServer::ReceiveNoFreeIncomingConnections( Packet* packet )
{
    if ( APP.DebugMode )
    {
      CONSOLE.addx( "Server: ID_NO_FREE_INCOMING_CONNECTIONS from PlayerID:%u:%u on %p.", packet->playerId.binaryAddress, packet->playerId.port );
    }
}

void CGameServer::ReceiveDisconnectionNotification( Packet* packet )
{
    if ( APP.DebugMode )
    {
      CONSOLE.addx( "Server: ID_DISCONNECTION_NOTIFICATION from PlayerID:%u:%u on %p.", packet->playerId.binaryAddress, packet->playerId.port );
    }

    WORLD.GetPlayers()->RemovePlayer( packet->playerId );

    if ( APP.DebugMode )
    {
      CONSOLE.addx( "Player destroyed : %u:%u", packet->playerId.binaryAddress, packet->playerId.port );
    }
}

void CGameServer::ReceiveConnectionLost( Packet* packet )
{
    if ( APP.DebugMode )
    {
      CONSOLE.addx( "Server: ID_CONNECTION_LOST from PlayerID:%u:%u on %p.", packet->playerId.binaryAddress, packet->playerId.port );
    }

    ReceiveDisconnectionNotification( packet );
}

void CGameServer::ReceivedStaticData( Packet* packet )
{
    if ( APP.DebugMode )
    {
      CONSOLE.addx( "Server: ID_RECEIVED_STATIC_DATA from PlayerID:%u:%u on %p.", packet->playerId.binaryAddress, packet->playerId.port );
    }

    // Got static data
    if ( NET.rakServer->GetStaticClientData( packet->playerId ) )
    {
      //    CONSOLE.addx("ID_RECEIVED_STATIC_DATA");
      //    CONSOLE.addx("%s has connected", ((StaticClientDataStruct*)(NET.rakServer->GetStaticClientData(packet->playerId))->GetData())->name);
    }
}

void CGameServer::BroadcastActor( CActor* actor )
{
    RakNet::BitStream bs;

    bs.Write( ID_CRIMSON_NEWACTOR );
    bs.WriteVector( actor->getPosition().X, actor->getPosition().Y, actor->getPosition().Z );
    bs.Write( actor->GetNetworkID() );
    bs.Write( actor->getOwnersPlayerID() );
    if ( actor->isAttached() )
    {
      bs.Write( actor->getParentAttachment()->GetNetworkID() );
    }
    else
    {
      bs.Write( UNASSIGNED_NETWORK_ID );
    }
    stringCompressor->EncodeString( ( char* )actor->getFactoryName().c_str(), 32, &bs );

    // add the actor config data to the bitstream
    actor->Serialize( bs );

    NET.rakServer->Send( &bs, MEDIUM_PRIORITY, RELIABLE_ORDERED, 0, UNASSIGNED_PLAYER_ID, true );
}

void CGameServer::ReceiveChat( Packet* packet )
{
    if ( APP.DebugMode )
    {
      CONSOLE.addx( "Server: ID_CRIMSON_CHAT from PlayerID:%u:%u on %p.", packet->playerId.binaryAddress, packet->playerId.port );
    }

    RakNet::BitStream inBitStream( ( unsigned char* )packet->data, packet->length, false );
    int i;
    u16 addresser;
    inBitStream.Read( i ); // ID_CRIMSON_NEWACTOR
    inBitStream.Read( addresser );
    char* inString = NULL;
    inString = new char[MAX_CHAT_TEXT];
    stringCompressor->DecodeString( inString, MAX_CHAT_TEXT, &inBitStream );

    String name = "???";
    if ( WORLD.GetPlayers() )
    {
      name = WORLD.GetPlayers()->getPlayerName( packet->playerId );
    }
    String str = "<";
    str += name;
    str += "> ";
    str += inString;
    if ( IRR.getChatConsole() )
    {
      IRR.getChatConsole()->add( str.c_str() );
    }

    // send to all
    RakNet::BitStream bs;
    bs.Write( ID_CRIMSON_CHAT );
    bs.Write( addresser );
    bs.Write( packet->playerId );
    stringCompressor->EncodeString( inString, MAX_CHAT_TEXT, &bs );
    NET.rakServer->Send( &bs, LOW_PRIORITY, RELIABLE, 0, UNASSIGNED_PLAYER_ID, true );

    delete inString;
}
