#ifndef CUSTOMPACKETS_H_INCLUDED
#define CUSTOMPACKETS_H_INCLUDED

#include "network.h"

enum { ID_CRIMSON_DEFAULT = ID_RESERVED9 + 1, ID_CRIMSON_NEWACTOR, ID_CRIMSON_CHAT, ID_CRIMSON_VERIFYFILES, ID_CRIMSON_CLIENTOK };

//#pragma pack(1)
//struct structName
//{
//  unsigned char useTimeStamp; // Assign this to ID_TIMESTAMP
//  unsigned long timeStamp; // Put the system time in here returned by getTime()
//  unsigned char typeId; // This will be assigned to a type I add to PacketEnumerations.h , lets say ID_SET_TIMED_MINE
//  float x,y,z; // Mine position
//  NetworkID NetworkID; // NetworkID of the mine, used as a common method to refer to the mine on different computers
//  PlayerID playerId; // The PlayerID of the player that owns the mine
//};

#define MAX_CHAT_TEXT 128
#define MAX_NAME_TEXT 5

#define CHAT_FORMAT "<%s> %s"

struct StaticClientDataStruct
{
    unsigned char typeId;  // ID_SET_CLIENT_DATA
    char name[MAX_NAME_TEXT];
};

#pragma pack(1)
struct packetNewBody
{
    unsigned char typeId;
    NetworkID NetworkID;
    PlayerID playerId;
    float pos_x, pos_y, pos_z;
    unsigned char factoryObjectType;
    u16 loadedDataSize;
    char* loadedData;
};

#pragma pack(1)
struct packetChat
{
    unsigned char typeId;
    u16 loadedDataSize;
    char* text;
};


#endif