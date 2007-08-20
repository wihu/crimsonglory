/// \file
/// \brief All the packet identifiers used by RakNet.  Packet identifiers comprise the first byte of any message.
///
/// This file is part of RakNet Copyright 2003 Kevin Jenkins.
///
/// Usage of RakNet is subject to the appropriate license agreement.
/// Creative Commons Licensees are subject to the
/// license found at
/// http://creativecommons.org/licenses/by-nc/2.5/
/// Single application licensees are subject to the license found at
/// http://www.rakkarsoft.com/SingleApplicationLicense.html
/// Custom license users are subject to the terms therein.
/// GPL license users are subject to the GNU General Public
/// License as published by the Free
/// Software Foundation; either version 2 of the License, or (at your
/// option) any later version.

#ifndef __PACKET_ENUMERATIONS_H
#define __PACKET_ENUMERATIONS_H 

/// You should not edit the file PacketEnumerations.h as it is a part of RakNet static library
/// To define your own message id, define an enum following the code example that follows. 
///
/// \code
/// enum {
///   ID_MYPROJECT_MSG_1 = ID_USER_PACKET_ENUM
///   ID_MYPROJECT_MSG_2, 
///    ... 
/// };
/// \endcode 
///
/// \note All these enumerations should be casted to (unsigned char) before writing them to RakNet::BitStream
enum {
	//
	// RESERVED TYPES - DO NOT CHANGE THESE
	//
	// Ignore these:
	/// 0: Ping from a connected system.  Update timestamps (internal use only)
	ID_INTERNAL_PING,  
	/// 1: Ping from an unconnected system.  Reply but do not update timestamps. (internal use only)
	ID_PING,
	/// 2: Ping from an unconnected system.  Only reply if we have open connections. Do not update timestamps. (internal use only)
	ID_PING_OPEN_CONNECTIONS,
	/// 3: Pong from a connected system.  Update timestamps (internal use only)
	ID_CONNECTED_PONG,
	/// 4: Someone asked for our static data (internal use only)
	ID_REQUEST_STATIC_DATA,
	/// 5: Asking for a new connection (internal use only)
	ID_CONNECTION_REQUEST,
	/// 6: Connecting to a secured server/peer
	ID_SECURED_CONNECTION_RESPONSE,
	/// 7: Connecting to a secured server/peer
	ID_SECURED_CONNECTION_CONFIRMATION,
	/// 8: Remote procedure call (internal use only)
	ID_RPC,
	/// 9: Remote procedure call reply, for RPCs that return data (internal use only)
	ID_RPC_REPLY,
	/// 10: Server / Client only - The server is broadcasting the pings of all players in the game (internal use only)
	ID_BROADCAST_PINGS,
	/// 11: Server / Client only - The server is broadcasting a random number seed (internal use only)
	ID_SET_RANDOM_NUMBER_SEED,
	/// 12: Packet that tells us the packet contains an integer ID to name mapping for the remote system
	ID_RPC_MAPPING,
	/// 13: A reliable packet to detect lost connections
	ID_DETECT_LOST_CONNECTIONS,
	/// 14: Offline message so we know when to reset and start a new connection
	ID_OPEN_CONNECTION_REQUEST,
	/// 15: Offline message response so we know when to reset and start a new connection
	ID_OPEN_CONNECTION_REPLY,

	//
	// USER TYPES - DO NOT CHANGE THESE
	//
	// Ordered from most useful to least useful

	/// [PEER|CLIENT] 16: In a client/server environment, our connection request to the server has been accepted.
	ID_CONNECTION_REQUEST_ACCEPTED,

	/// [PEER|SERVER|CLIENT] 17: Sent to the player when a connection request cannot be completed due to inability to connect
	ID_CONNECTION_ATTEMPT_FAILED,

	/// [PEER|SERVER] 18: A remote system has successfully connected.
	ID_NEW_INCOMING_CONNECTION,

	/// [PEER|CLIENT] 19: The system we attempted to connect to is not accepting new connections.
	ID_NO_FREE_INCOMING_CONNECTIONS,

	/// [PEER|SERVER|CLIENT] 20: The system specified in Packet::playerID has disconnected from us.  For the client, this would mean the server has shutdown.
	ID_DISCONNECTION_NOTIFICATION,

	/// [PEER|SERVER|CLIENT] 21: Reliable packets cannot be delivered to the system specifed in Packet::playerID.  The connection to that system has been closed.
	ID_CONNECTION_LOST,

	/// [CLIENT|PEER] 22: We preset an RSA public key which does not match what the system we connected to is using.
	ID_RSA_PUBLIC_KEY_MISMATCH,

	/// [PEER|CLIENT] 23: We are banned from the system we attempted to connect to.
	ID_CONNECTION_BANNED,

	/// [PEER|CLIENT] 24: The remote system is using a password and has refused our connection because we did not set the correct password.
	ID_INVALID_PASSWORD,

	/// [PEER|SERVER|CLIENT] 25: A packet has been tampered with in transit.  The sender is contained in Packet::playerID.
	ID_MODIFIED_PACKET,

	/// [PEER|SERVER|CLIENT] 26: The four bytes following this byte represent an unsigned int which is automatically modified by the difference in system times between the sender and the recipient. Requires that you call StartOccasionalPing.
	ID_TIMESTAMP,

    /// [CLIENT|PEER] 27: Pong from an unconnected system.  First byte is ID_PONG, second sizeof(RakNetTime) bytes is the ping, following bytes is system specific enumeration data.
	ID_PONG,

	/// [PEER|SERVER|CLIENT] 28: We got a bitstream containing static data.  You can now read this data. This packet is transmitted automatically on connections, and can also be manually sent.
	ID_RECEIVED_STATIC_DATA,

	/// [CLIENT] 29: In a client/server environment, a client other than ourselves has disconnected gracefully.  Packet::playerID is modified to reflect the playerID of this client.
	ID_REMOTE_DISCONNECTION_NOTIFICATION,

	/// [CLIENT] 30: In a client/server environment, a client other than ourselves has been forcefully dropped. Packet::playerID is modified to reflect the playerID of this client.
	ID_REMOTE_CONNECTION_LOST,

	/// [CLIENT] 31: In a client/server environment, a client other than ourselves has connected.  Packet::playerID is modified to reflect the playerID of this client.
	ID_REMOTE_NEW_INCOMING_CONNECTION,

	/// [CLIENT] 32: On our initial connection to the server, we are told of every other client in the game.  Packet::playerID is modified to reflect the playerID of this client.
	ID_REMOTE_EXISTING_CONNECTION,

	/// [CLIENT] - 33: Got the data for another client
	ID_REMOTE_STATIC_DATA,

	/// [PEER|SERVER|CLIENT] 34: Request for a list of downloadable files. Pass to Autopatcher::SendDownloadableFileList
	ID_AUTOPATCHER_REQUEST_FILE_LIST,
	/// [PEER|SERVER|CLIENT] 35: Got a list of downloadable files. Pass to Autopatcher::OnAutopatcherFileList
	ID_AUTOPATCHER_FILE_LIST,
	/// [PEER|SERVER|CLIENT] 36: Request for a particular set of downloadable files. Pass to Autopatcher::OnAutopatcherRequestFiles
	ID_AUTOPATCHER_REQUEST_FILES,
	/// [PEER|SERVER|CLIENT] 37: Set the list of files that were approved for download and are incoming. Pass to Autopatcher::OnAutopatcherSetDownloadList
	ID_AUTOPATCHER_SET_DOWNLOAD_LIST,
	/// [PEER|SERVER|CLIENT] 38: Got a file that we requested for download.  Pass to Autopatcher::OnAutopatcherWriteFile
	ID_AUTOPATCHER_WRITE_FILE,

	/// [MASTERSERVER] 39: Request to the master server for the list of servers that contain at least one of the specified keys
	ID_QUERY_MASTER_SERVER,
	/// [MASTERSERVER] 40: Remove a game server from the master server.
	ID_MASTER_SERVER_DELIST_SERVER,
	/// [MASTERSERVER|MASTERCLIENT] 41: Add or update the information for a server.
	ID_MASTER_SERVER_UPDATE_SERVER,
	/// [MASTERSERVER|MASTERCLIENT] 42: Add or set the information for a server.
	ID_MASTER_SERVER_SET_SERVER,
	/// [MASTERSERVER|MASTERCLIENT] 43: This message indicates a game client is connecting to a game server, and is relayed through the master server.
	ID_RELAYED_CONNECTION_NOTIFICATION,

	/// [PEER|SERVER|CLIENT] 44: Inform a remote system of our IP/Port.
	ID_ADVERTISE_SYSTEM,

	/// - Depreciated
	ID_FULLY_CONNECTED_MESH_JOIN_RESPONSE,
	/// - Depreciated
	ID_FULLY_CONNECTED_MESH_JOIN_REQUEST,

	// 47 - Depreciated
	ID_REPLICATOR_DATA_PUSH_OBJECT,
	// 48 - Depreciated
	ID_REPLICATOR_DATA_SEND_MEMORY,
	// 49 - Depreciated
	ID_REPLICATOR_DATA_SEND_OBJECT_SCOPE,
	// 50 - Depreciated
	ID_REPLICATOR_DATA_STOP,
	// 51 - Depreciated
	ID_REPLICATOR_OBJECT_CREATION_REQUEST,
	// 52 - Depreciated
	ID_REPLICATOR_OBJECT_CREATION_REQUEST_RESPONSE,
	// 53 - Depreciated
	ID_REPLICATOR_STR_MAP_INDEX,
	/// 54 - Depreciated
	ID_REPLICATOR_PUSH_COMPLETE,

	/// [RakNetTransport] 55
	ID_TRANSPORT_STRING,

	/// [ReplicaManager] 56
	ID_REPLICA_MANAGER_CONSTRUCTION,
	/// [ReplicaManager] 57
	ID_REPLICA_MANAGER_DESTRUCTION,
	/// [ReplicaManager] 58
	ID_REPLICA_MANAGER_SCOPE_CHANGE,
	/// [ReplicaManager] 59
	ID_REPLICA_MANAGER_SERIALIZE,
	/// [ReplicaManager] 60
	ID_REPLICA_MANAGER_DOWNLOAD_COMPLETE,

	/// [ConnectionGraph] 61
	ID_CONNECTION_GRAPH_REQUEST,
	/// [ConnectionGraph] 62
	ID_CONNECTION_GRAPH_REPLY,
	/// [ConnectionGraph] 63
	ID_CONNECTION_GRAPH_UPDATE,
	/// [ConnectionGraph] 64
	ID_CONNECTION_GRAPH_NEW_CONNECTION,
	/// [ConnectionGraph] 65
	ID_CONNECTION_GRAPH_CONNECTION_LOST,
	/// [ConnectionGraph] 66
	ID_CONNECTION_GRAPH_DISCONNECTION_NOTIFICATION,

	/// [Router] 67
	ID_ROUTE_AND_MULTICAST,

	/// [RakVoice] 68
	ID_RAKVOICE_OPEN_CHANNEL_REQUEST,
	/// [RakVoice] 69
	ID_RAKVOICE_OPEN_CHANNEL_REPLY,
	/// [RakVoice] 70
	ID_RAKVOICE_CLOSE_CHANNEL,
	/// [RakVoice] 71
	ID_RAKVOICE_DATA,
	
	/// Depreciated
	ID_RESERVED9,
	// For the user to use.  Start your first enumeration at this value.
	ID_USER_PACKET_ENUM,
	//-------------------------------------------------------------------------------------------------------------
 
};

#endif
