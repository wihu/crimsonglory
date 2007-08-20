/// \file
/// \ingroup RAKNET_RAKVOICE
/// \brief A pure virtual interface for RakVoice
///
/// This file is part of RakNet Copyright 2003 Kevin Jenkins.
/// Usage of Raknet is subject to the appropriate licence agreement.
/// "Shareware" Licensees with Rakkarsoft LLC are subject to the
/// shareware license found at
/// http://www.rakkarsoft.com/shareWareLicense.html
/// "Commercial" Licensees are subject to the commercial license found at
/// http://www.rakkarsoft.com/sourceCodeLicense.html
/// Custom license users are subject to the terms therein.
/// All other users are subject to the GNU General Public
/// License as published by the Free
/// Software Foundation; either version 2 of the License, or (at your
/// option) any later version.

#ifndef __RAK_VOICE_INTERFACE_H
#define __RAK_VOICE_INTERFACE_H 

/// \defgroup RAKNET_RAKVOICE RakVoice Subsystem
/// The RakVoice subsytem provide voice communications.
/// It uses RakNet to transmit the data and speex to encode and decode the voice PCM data.
/// Bandwidth usage is incredibly low, at about 500 bytes per second for 8 bit audio.
/// Used with RakNet's secure communication capabilities, you can use the internet for secure peer-to-peer voice communications.

class RakPeerInterface;
#include "NetworkTypes.h" 

/// \ingroup RAKNET_RAKVOICE
/// \brief Define the user interface of the RakVoice module. 
/// 
/// Users should only use the function provided by this interface while
/// using the RakVoice Subsystem.

// Define to test sending to yourself
//#define TEST_LOOPBACK

class RakVoiceInterface
{
public:

	/// Destructor 
	virtual ~RakVoiceInterface() {};
	
	/// Call this first.	
	///
	/// \param[in] samplingRate Self-explanatory
	/// \param[in] bitsPerSample Self-explanatory
	/// \param[in] _peer The instance of RakPeer to use for sends
	virtual void Init( int samplingRate, int bitsPerSample, RakPeerInterface *_peer )=0;
	
	/// \pre You must have called Init first
	/// Gets the frame size.
	/// \note It is best to send input to EncodeSoundData that matches this frame size
	/// \return The frame size used by the encoder, in bytes
	virtual int GetFrameSize( void ) const=0;
	
	/// Whenever a player disconnects RakVoice needs to know about it.
	/// Otherwise we will be using old values for our encoding.
	///
	/// \param[in] id PlayerID of the player who has disconnected.
	virtual void Disconnect( PlayerID id )=0;
	
	/// Sets the block size that EncodeSoundPacket will read and
	/// GetSoundPacket will write.  If you don't call this, it will
	/// default to GetFrameSize().  It is reset every call to Init.
	/// \pre Must have called Init first
	/// \param[in] _blockSize The size of each block that you want to process at a time.  Each network packet will have this size before compression.  It must be a multiple of frame_size.  The best _blockSize is what would be compressed to slightly under your MTU.  You can safely assume the compression rate is 50% (actually it's closer to 75%).
	virtual void SetBlockSize( int _blockSize )=0;
	
	/// Frees all memory used.  Call this before shutting down.
	virtual void Deinit( void )=0;
	
	/// Pass your raw sound data to this function.
	/// This function will encode and send in another thread the data as a packet Because of the way encoding works, you cannot broadcast voice data.
	///
	/// \param[in] input A byte array of block size containing the raw voice data.  this must be equal in size to the current blockSize, whatever you passed to SetBlockSize()
	/// \param[in] recipient The targeted peer or UNASSIGNED_PLAYER_ID if you want to send to the server as a client.
	virtual void EncodeSoundPacket( char *input, PlayerID recipient )=0;
	
	/// When you get a packet with the type ID_VOICE_PACKET, pass the packet data and length to this function.
	/// This function will decode the data and put it in the internal queue, or simply relay the data if this is the server and the target is not the server
	///
	/// \param[in] data Packet::data
	/// \param[in] length Packet::length
	/// \sa NetworkTypes.h
	virtual void DecodeAndQueueSoundPacket( char* data, int length )=0;
	
	/// This will get the next sound packet and copy it to output.
	/// By sound packet, I mean decompressed sound data, not network datagram
	/// \param[out] output The resulting raw sound voice data
	/// \param[in] sender Who sent this voice data
	/// \retval true Data was copied to output, of the same length you passed to SetBlockSize
	/// \retval false No sound packets are waiting.
	virtual bool GetSoundPacket( char *output, PlayerID *sender )=0;

	/// Returns the size, in bytes, of the next sound packet, or 0 for nothing left
	/// \return The size, in bytes, of the next sound packet, or 0 for nothing left
	virtual int GetNextSoundPacketSize( void )=0;

	/// This will tell you the total number of bytes waiting to be copied out via the function DecodeAndQueueSoundPacket()
	/// @return Returns the number of remaining byte in the buffer. 
	virtual int GetOutputBufferSize( void )=0;
};

#endif
