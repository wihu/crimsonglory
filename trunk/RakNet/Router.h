/// \file
/// \brief Router plugin.  Allows you to send to systems you are not directly connected to, and to route those messages
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

#ifndef __ROUTER_PLUGIN_H
#define __ROUTER_PLUGIN_H

class RakPeerInterface;
#include "NetworkTypes.h"
#include "PluginInterface.h"
#include "DS_OrderedList.h"
#include "DS_WeightedGraph.h"
#include "PacketPriority.h"
#include "SystemAddressList.h"

/// A connection graph.  Each peer will know about all other peers.
class Router : public PluginInterface
{
public:
	Router();
	~Router();

	// --------------------------------------------------------------------------------------------
	// User functions
	// --------------------------------------------------------------------------------------------
	/// We can restrict what kind of messages are routed by this plugin.
	/// This is useful for security, since you usually want to restrict what kind of messages you have to worry about from (as an example) other
	/// clients in a client / server system
	/// \param[in] restrict True to restrict what messages will be routed.  False to not do so (default).
	void SetRestrictRoutingByType(bool restrict);

	/// If types are restricted, this adds an allowed message type to be routed
	/// \param[in] messageId The type to not allow routing of.
	void AddAllowedType(unsigned char messageId);

	/// Removes a restricted type previously added with AddRestrictedType
	/// \param[in] messageId The type to no longer restrict routing of.
	void RemoveAllowedType(unsigned char messageId);

	/// Set the connection graph, which is a weighted graph of the topology of the network.  You can easily get this from the
	/// ConnectionGraph plugin.  See the 'router' sample for usage.
	/// This is necessary if you want to send (not necessary just to route).
	/// \param[in] connectionGraph A weighted graph representing the topology of the network.
	void SetConnectionGraph(DataStructures::WeightedGraph<PlayerID, unsigned short, false> *connectionGraph);

	/// Sends a bitstream to one or more systems.  If multiple systems are specified, the message will be multicasted using a minimum spanning tree
	/// \pre You just have called SetConnectionGraph with a valid graph representing the network topology
	/// \param[in] bitStream The bitstream to send
	/// \param[in] priority What priority level to send on.
	/// \param[in] reliability How reliability to send this data
	/// \param[in] orderingChannel When using ordered or sequenced packets, what channel to order these on.- Packets are only ordered relative to other packets on the same stream
	/// \param[in] recipients A list of recipients to send to.  To send to one recipient, just pass a PlayerID
	/// \return True on success, false mostly if the connection graph cannot find the destination.
	bool Send( RakNet::BitStream * bitStream, PacketPriority priority, PacketReliability reliability, char orderingChannel, SystemAddressList *recipients );

	// --------------------------------------------------------------------------------------------
	// Packet handling functions
	// --------------------------------------------------------------------------------------------
	virtual void OnAttach(RakPeerInterface *peer);
	virtual void OnDisconnect(RakPeerInterface *peer);
	virtual void OnUpdate(RakPeerInterface *peer);
	virtual bool OnReceive(RakPeerInterface *peer, Packet *packet);
	virtual void OnCloseConnection(RakPeerInterface *peer, PlayerID playerId);
protected:
	void SendTree(PacketPriority priority, PacketReliability reliability, char orderingChannel, DataStructures::Tree<PlayerID> *tree, RakNet::BitStream *payload, RakNet::BitStream *out, SystemAddressList *recipients);
	void SerializePreorder(DataStructures::Tree<PlayerID> *tree, RakNet::BitStream *out, SystemAddressList *recipients) const;
	DataStructures::WeightedGraph<PlayerID, unsigned short, false> *graph;
	bool restrictByType;
	DataStructures::OrderedList<unsigned char,unsigned char> allowedTypes;
	RakPeerInterface *rakPeer;
};

#endif
