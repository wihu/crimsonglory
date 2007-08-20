#include "GameClient.h"

#include "../App/app.h"
#include "../Game/SingletonIncludes.h"
#include "../IrrConsole/console_vars.h"

#include "../World/world.h"
#include "../World/player.h"
#include "../World/actor.h"
#include "../World/screentext.h"

#include "../Newton/newton_node.h"
#include "CustomPackets.h"

#include "BitStream.h"

CGameClient::CGameClient()
{
    stringCompressor->AddReference();

    CONSOLE_VAR_S( "cl_name", client_name, "Machiavelli", L"cl_name [name]. Ex. cl_name Machiavelli", L"Sets the players name." );
}

CGameClient::~CGameClient()
{
    stringCompressor->RemoveReference();

    //  StaticClientDataStruct staticClientData;
    //strcpy( staticClientData.name, wide2string( client_name ).c_str() );
    //strcpy( staticClientData.name, "gowno" );
    //NET.rakClient->SetStaticClientData(UNASSIGNED_PLAYER_ID, (char*)&staticClientData, sizeof(StaticClientDataStruct));
}

void CGameClient::ProcessPacket( unsigned char packetIdentifier )
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
      case ID_REMOTE_STATIC_DATA:
        ReceiveRemoteStaticData( p );
        break;
        //  
        //  case ID_CONNECTION_BANNED:
        ////    ReceiveConnectionBanned( p  );
        //  break;

      case ID_CONNECTION_REQUEST_ACCEPTED:
        ReceiveConnectionRequestAccepted( p );
        break;

        //  case ID_NEW_INCOMING_CONNECTION:
        ////    ReceiveNewIncomingConnection( p );
        //  break;

        //  case ID_CONNECTION_ATTEMPT_FAILED:
        ////    ReceiveConnectionAttemptFailed( p );
        //  break;
        //  
        //  case ID_NO_FREE_INCOMING_CONNECTIONS:
        ////    ReceiveNoFreeIncomingConnections( p );
        //  break;
        //  
        //  case ID_DISCONNECTION_NOTIFICATION:
        ////    ReceiveDisconnectionNotification( p );
        //  break;
        //  
        //  case ID_CONNECTION_LOST:
        ////    ReceiveConnectionLost( p );
        //  break;

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

      case ID_CRIMSON_NEWACTOR:
        ReceiveNewActor( p );
        break;

      case ID_CRIMSON_VERIFYFILES:
        ReceiveVerifyFiles( p );
        break;

      case ID_CRIMSON_CHAT:
        ReceiveChat( p );
        break;

      default:
        // If not a native packet send it to ProcessUnhandledPacket which should have been written by the user
        //
        CONSOLE.addx( "Client: UNKNOWN PACKET ID %i from %u:%u", packetIdentifier, p->playerId.binaryAddress, p->playerId.port );
        break;
    }
}

void CGameClient::ProcessError( const char* text, int error )
{
    switch ( error )
    {
      case ERROR_FILE_VERIFICATION_FAILED:
        CONSOLE.addx( COLOR_ERROR, "File verification failed: %s", text );
        WideString wstr = text;
        IGUIWindow* wnd = IRR.gui->addMessageBox( L"File verification failed", wstr.c_str(), true, EMBF_OK, 0, -1 );
        break;
    }
}

void CGameClient::ReceiveConnectionRequestAccepted( Packet* packet )
{
    if ( APP.DebugMode )
    {
      CONSOLE.addx( "Client: ID_CONNECTION_REQUEST_ACCEPTED from PlayerID:%u:%u on %p.", packet->playerId.binaryAddress, packet->playerId.port );
    }

    unsigned int binaryAddress;
    unsigned short port;
    PlayerID newId;

    RakNet::BitStream inBitStream( ( unsigned char* )packet->data, packet->length, false );
    inBitStream.IgnoreBits( 8 ); // ID_CONNECTION_REQUEST_ACCEPTED
    inBitStream.IgnoreBits( 8 * sizeof( unsigned short ) ); //inBitStream.Read(remotePort);
    //  inBitStream.IgnoreBits(8 * sizeof(unsigned int)); //inBitStream.Read(externalID.binaryAddress);
    //  inBitStream.IgnoreBits(8 * sizeof(unsigned short)); //inBitStream.Read(externalID.port);            
    inBitStream.Read( binaryAddress );
    inBitStream.Read( port );

    newId.binaryAddress = binaryAddress;
    newId.port = port;

    // CLIENT: CREATE MYPLAYER - if player with this playerid not created already
    WORLD.myPlayer = WORLD.GetPlayers()->AddPlayer( newId );
    if ( WORLD.myPlayer )
    {
      WORLD.myPlayer->setControls( &myControls );
      if ( APP.DebugMode )
      {
        CONSOLE.addx( "Created myPlayer: %u:%u on %p.", WORLD.myPlayer->playerID.binaryAddress, WORLD.myPlayer->playerID.port );
      }
    }
}

void CGameClient::ReceivedStaticData( Packet* packet )
{
    if ( APP.DebugMode )
    {
      CONSOLE.addx( "Client: ID_RECEIVED_STATIC_DATA from PlayerID:%u:%u on %p.", packet->playerId.binaryAddress, packet->playerId.port );
    }
}

void CGameClient::ReceiveRemoteStaticData( Packet* packet )
{
    if ( APP.DebugMode )
    {
      CONSOLE.addx( "Client: ID_REMOTE_STATIC_DATA from PlayerID:%u:%u on %p.", packet->playerId.binaryAddress, packet->playerId.port );
    }
}

void CGameClient::ReceiveNewActor( Packet* packet )
{
    if ( APP.DebugMode )
    {
      CONSOLE.addx( "Client: ID_CRIMSON_NEWACTOR from PlayerID:%u:%u on %p.", packet->playerId.binaryAddress, packet->playerId.port );
    }

    //packetNewBody *p = (packetNewBody *) packet->data;
    RakNet::BitStream inBitStream( ( unsigned char* )packet->data, packet->length, false );

    //inBitStream.IgnoreBits(8); // ID_CRIMSON_NEWACTOR

    int i;
    vector3df newPos;
    NetworkID newNetworkID, parentNetworkID;
    PlayerID newPlayerID;
    int factoryIndex;
    inBitStream.Read( i ); // ID_CRIMSON_NEWACTOR
    inBitStream.ReadVector( newPos.X, newPos.Y, newPos.Z );
    inBitStream.Read( newNetworkID );
    inBitStream.Read( newPlayerID );
    inBitStream.Read( parentNetworkID );

    char* inString = NULL;
    inString = new char[32];
    stringCompressor->DecodeString( inString, 32, &inBitStream );
    //inBitStream.Read( factoryIndex );
    //CONSOLE.addx( "C f %s", inString );
    //CONSOLE.addx( "Client: PlayerID:%u:%u on %p.", newPlayerID.binaryAddress, newPlayerID.port );

    CNewtonNode* object = ( CNewtonNode* )NetworkIDGenerator::GET_OBJECT_FROM_ID( newNetworkID );
    CNewtonNode* parentObject = ( CNewtonNode* )NetworkIDGenerator::GET_OBJECT_FROM_ID( parentNetworkID );
    CActor* actor = 0;

    //for (i = 0; i < FACTORY->Actors.indexArray.size(); i++ )
    //  CONSOLE.addx( "C factory %s", FACTORY->Actors.indexArray[i].c_str() );
    //CONSOLE.addx( "C factoryIndex %s", FACTORY->Actors.indexArray[factoryIndex].c_str() );


    CONSOLE.addx( "Recieved actor: %s ", inString );
    if ( object == 0 ) // needs creating - this is client not localhost
    {
      //  
      actor = FACTORY->Actors.Create( inString, "" );
      if ( actor )
      {
        actor->Unserialize( inBitStream );
        actor->setPosition( newPos );
        String text = "(client) ";
        text += inString;
        actor->setDebugText( text );
        actor->SetNetworkID( newNetworkID ); // set network ID
        actor->setOwnersPlayerID( newPlayerID );
      }
    }
    else
    {
      CONSOLE.add( "Actor already created" );
      if ( actor == 0 )
      {
        actor = static_cast<CActor*>( object );
      }
    }

    // this is my player's actor
	if ( WORLD.myPlayer )
		if ( WORLD.myPlayer->playerID == newPlayerID )
		{
		//CONSOLE.add( "MyPlayer!" );     
		WORLD.GetCamera()->setTarget( actor );
		// object doesn't work with setTarget, object not good for usage?
		}

    CPlayer* p = WORLD.GetPlayers()->GetPlayer( newPlayerID );
    if ( p )
    {
      actor->setControls( p->getControls() );
    }

    // attachment
    if ( !actor->isAttached() )
    {
      if ( parentObject )
      {
        actor->attachToParentNode( parentObject, actor->getControls() );
      }
    }


    delete[] inString;

    //CONSOLE.addx("Y: %f", p->pos_y );

    //assert(p->length == sizeof(MyStruct));
    //if (p->length != sizeof(MyStruct)) return;
}

void CGameClient::SendChat( char* text, u16 addresser )
{
    if ( strlen( text ) < 1 )
    {
      return;
    }
    RakNet::BitStream bs;
    bs.Write( ID_CRIMSON_CHAT );
    bs.Write( addresser );
    stringCompressor->EncodeString( text, MAX_CHAT_TEXT, &bs );
    NET.rakClient->Send( &bs, LOW_PRIORITY, RELIABLE, 0 );
}

void CGameClient::ReceiveVerifyFiles( Packet* packet )
{
    //int i;
    //RakNet::BitStream inBitStream( (char*)packet->data, packet->length, false );
    //inBitStream.Read( i ); // ID_CRIMSON_VERIFYFILES

    //char *output;
    //output = NET.fileVerifier->DeserializeAndValidate(&inBitStream);

    //if (output == 0)
    //  CONSOLE.add("Client file verification succeeded");
    //else
    //  CONSOLE.addx( COLOR_ERROR, "Client file verification failed on %s", output);
}

void CGameClient::ReceiveChat( Packet* packet )
{
    if ( APP.DebugMode )
    {
      CONSOLE.addx( "Client: ID_CRIMSON_CHAT from PlayerID:%u:%u on %p.", packet->playerId.binaryAddress, packet->playerId.port );
    }

    RakNet::BitStream inBitStream( ( unsigned char* )packet->data, packet->length, false );
    int i;
    u16 adresser;
    PlayerID pid;
    inBitStream.Read( i ); // ID_CRIMSON_NEWACTOR
    inBitStream.Read( adresser );
    inBitStream.Read( pid );
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

    if ( !NET.rakServer->IsActive() ) // server already has this text
    {
      if ( IRR.getChatConsole() )
      {
        IRR.getChatConsole()->add( str.c_str() );
      }
    }

    CActor* actor = CActor::getActorWithPlayerID( pid );
    if ( actor )
    {
      String str = inString;
      if ( actor->aboveText )
      {
        actor->aboveText->Kill();
      }
      actor->aboveText = new CScreenText( inString, actor, str.size() * 9, position2d<s32>( 0, -33 ) );
    }

    delete inString;
}