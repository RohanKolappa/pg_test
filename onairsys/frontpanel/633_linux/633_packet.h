//============================================================================
// 633 WinTest Code.
// 633_packet,h: Ultra-simple 633 command-line communications example
// Copyright 2001, Crystalfontz America, Inc. Written by Brent A. Crosby
// www.crystalfontz.com, brent@crystalfontz.com
//============================================================================
#define MAX_DATA_LENGTH 18
//============================================================================
#define MAX_COMMAND 35
//============================================================================
typedef struct
  {
  ubyte
    command;
  ubyte
    data_length; 
  ubyte
    data[MAX_DATA_LENGTH];
  WORD_UNION
    CRC;
  }COMMAND_PACKET;
//============================================================================
word get_crc(ubyte *bufptr,word len,word seed);
//============================================================================
extern COMMAND_PACKET
  incoming_response;
extern COMMAND_PACKET
  outgoing_command;
//============================================================================
ubyte check_for_packet(void);
void send_packet(void);
//============================================================================
