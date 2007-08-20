/// \file
/// \brief Voice compression and transmission interface
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

#ifndef __RAK_VOICE_H
#define __RAK_VOICE_H

class RakPeerInterface;
#include "NetworkTypes.h"
#include "PluginInterface.h"
#include "DS_OrderedList.h"

// How many frames large to make the circular buffers in the VoiceChannel structure
#define FRAME_OUTGOING_BUFFER_COUNT 100
#define FRAME_INCOMING_BUFFER_COUNT 100

struct VoiceChannel
{
	PlayerID playerId;
	void *enc_state;
	void *dec_state;
	bool remoteIsShortSampleType;
	unsigned short remoteSampleRate;
	
	// Circular buffer of unencoded sound data read from the user.
	char *outgoingBuffer;
	// Each frame sent to speex requires this many samples, of whatever size you are using.
	int speexOutgoingFrameSampleCount;
	// Index in is bytes.
	// Write index points to the next byte to write to, which must be free.
	unsigned outgoingReadIndex, outgoingWriteIndex;
	bool bufferOutput;
	bool copiedOutgoingBufferToBufferedOutput;
	unsigned short outgoingMessageNumber;

	// Circular buffer of unencoded sound data to be passed to the user.  Each element in the buffer is of size bufferSizeBytes bytes.
	char *incomingBuffer;
	int speexIncomingFrameSampleCount;
	unsigned incomingReadIndex, incomingWriteIndex;	// Index in bytes
	unsigned short incomingMessageNumber;  // The ID_VOICE message number we expect to get.  Used to drop out of order and detect how many missing packets in a sequence

	RakNetTime lastSend;
};
int VoiceChannelComp( const PlayerID &key, VoiceChannel * const &data );

// Voice compression and transmission interface
class RakVoice : public PluginInterface
{
public:
	RakVoice();
	~RakVoice();

	// --------------------------------------------------------------------------------------------
	// User functions
	// --------------------------------------------------------------------------------------------
	/// \param[in] sampleTypeIsShort true if your sample types are unsigned shorts - otherwise they must be floats
	void Init(bool sampleTypeIsShort, unsigned short speexSampleRate, unsigned bufferSizeBytes);
	void Deinit(void);
	
	// You will get ID_RAKVOICE_OPEN_CHANNEL_REPLY on success
	void RequestVoiceChannel(PlayerID recipient);

	// Other system will get ID_RAKVOICE_CLOSE_CHANNEL
	void CloseVoiceChannel(PlayerID recipient);

	// Other system will get ID_RAKVOICE_CLOSE_CHANNEL
	void CloseAllChannels(void);

	bool SendFrame(PlayerID recipient, void *inputBuffer);
	void ReceiveFrame(void *outputBuffer);
	
	// --------------------------------------------------------------------------------------------
	// Message handling functions
	// --------------------------------------------------------------------------------------------
	virtual void OnAttach(RakPeerInterface *peer);
	virtual void OnDisconnect(RakPeerInterface *peer);
	virtual void OnUpdate(RakPeerInterface *peer);
	virtual bool OnReceive(RakPeerInterface *peer, Packet *packet);
	virtual void OnCloseConnection(RakPeerInterface *peer, PlayerID playerId);
protected:
	void OnOpenChannelRequest(RakPeerInterface *peer, Packet *packet);
	void OnOpenChannelReply(RakPeerInterface *peer, Packet *packet);
	void OnVoiceData(RakPeerInterface *peer, Packet *packet);
	void OpenChannel(RakPeerInterface *peer, Packet *packet);
	void FreeChannelMemory(PlayerID recipient);
	void FreeChannelMemory(unsigned index);
	void WriteOutputToChannel(VoiceChannel *channel, char *dataToWrite);
	
	RakPeerInterface *rakPeer;
	DataStructures::OrderedList<PlayerID, VoiceChannel*, VoiceChannelComp> voiceChannels;
	bool shortSampleType;
	int sampleRate;
	int sampleSize; // Size of each sample - 2 if shortSampleType==true - 4 otherwise
	unsigned bufferSizeBytes;
	float *bufferedOutput;
	unsigned bufferedOutputCount;
	bool zeroBufferedOutput;
};

#endif
