/// \file
/// \brief Connection graph plugin.  This maintains a graph of connections for the entire network, so every peer knows about every other peer.
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

#ifndef __CONNECTION_GRAPH_H
#define __CONNECTION_GRAPH_H

class RakPeerInterface;
#include "NetworkTypes.h"
#include "PluginInterface.h"
#include "DS_List.h"
#include "DS_WeightedGraph.h"
#include "GetTime.h"

/// A connection graph.  Each peer will know about all other peers.
class ConnectionGraph : public PluginInterface
{
public:
	ConnectionGraph();
	~ConnectionGraph();

	// --------------------------------------------------------------------------------------------
	// User functions
	// --------------------------------------------------------------------------------------------
	/// Plaintext encoding of the password, or 0 for none.  If you use a password, use secure connections
	void SetPassword(const char *password);

	/// Returns the connection graph
	/// \return The connection graph, stored as map of adjacency lists
	DataStructures::WeightedGraph<PlayerID, unsigned short, false> *GetGraph(void);

	// --------------------------------------------------------------------------------------------
	// Packet handling functions
	// --------------------------------------------------------------------------------------------
	virtual void OnDisconnect(RakPeerInterface *peer);
	virtual void OnUpdate(RakPeerInterface *peer);
	virtual bool OnReceive(RakPeerInterface *peer, Packet *packet);
	virtual void OnCloseConnection(RakPeerInterface *peer, PlayerID playerId);
protected:
	void HandleDroppedConnection(RakPeerInterface *peer, PlayerID playerId, unsigned char packetId);
	void DeleteFromPeerList(PlayerID playerId);

	void OnNewIncomingConnection(RakPeerInterface *peer, Packet *packet);
	void OnConnectionRequestAccepted(RakPeerInterface *peer, Packet *packet);
	void OnConnectionGraphRequest(RakPeerInterface *peer, Packet *packet);
	void OnConnectionGraphReply(RakPeerInterface *peer, Packet *packet);
	void OnConnectionGraphUpdate(RakPeerInterface *peer, Packet *packet);
	void OnNewConnection(RakPeerInterface *peer, Packet *packet);
	bool OnConnectionLost(RakPeerInterface *peer, Packet *packet, unsigned char packetId);
	void OnConnectionAddition(RakPeerInterface *peer, Packet *packet);
	void OnConnectionRemoval(RakPeerInterface *peer, Packet *packet);
	void SendConnectionGraph(PlayerID target, unsigned char packetId, RakPeerInterface *peer);
	void SerializeWeightedGraph(RakNet::BitStream *out, const DataStructures::WeightedGraph<PlayerID, unsigned short, false> &g) const;
	bool DeserializeWeightedGraph(RakNet::BitStream *inBitstream, RakPeerInterface *peer);
	void AddAndRelayConnection(DataStructures::OrderedList<PlayerID,PlayerID> &ignoreList, PlayerID node1, PlayerID node2, unsigned short ping, RakPeerInterface *peer);
	bool RemoveAndRelayConnection(DataStructures::OrderedList<PlayerID,PlayerID> &ignoreList, unsigned char packetId, PlayerID node1, PlayerID node2, RakPeerInterface *peer);
	void RemoveParticipant(PlayerID playerId);
	void AddParticipant(PlayerID playerId);
	void BroadcastGraphUpdate(DataStructures::OrderedList<PlayerID,PlayerID> &ignoreList, RakPeerInterface *peer);
	void NotifyUserOfRemoteConnection(PlayerID node1,PlayerID node2,unsigned short ping, RakPeerInterface *peer);
	bool IsNewRemoteConnection(PlayerID node1,PlayerID node2,RakPeerInterface *peer);
	bool DeserializeIgnoreList(DataStructures::OrderedList<PlayerID,PlayerID> &ignoreList, RakNet::BitStream *inBitstream );
	void SerializeIgnoreListAndBroadcast(RakNet::BitStream *outBitstream, DataStructures::OrderedList<PlayerID,PlayerID> &ignoreList, RakPeerInterface *peer);

	RakNetTime nextWeightUpdate;
	char *pw;
	DataStructures::OrderedList<PlayerID, PlayerID> participantList;
	DataStructures::WeightedGraph<PlayerID, unsigned short, false> graph;

	// Used to broadcast new connections after some time so the pings are correct
	RakNetTime forceBroadcastTime;

};

#endif
