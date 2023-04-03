#ifndef COM_MESSAGE__H
#define COM_MESSAGE__H
#include <stdio.h>
#include <stdint.h>
#include <string.h>

#define MAX_ADDRESS_LENGTH  4U
#define MAX_DATA_LENGTH     254U

typedef enum
{
    COM_MSG_ACK = 0U,
    COM_MSG_NACK,
    COM_MSG_START,
    COM_MSG_WRITE_DATA,
    COM_MSG_END
} Msg_Id_t;

typedef struct
{
    uint8_t msgId;
    uint8_t adrLen;
    uint8_t dataLen;
    uint8_t payload[MAX_ADDRESS_LENGTH + MAX_DATA_LENGTH];
    uint16_t checkSum;
} Com_Message_struct_t;

uint16_t DoPackMsg(uint8_t *buffer, Com_Message_struct_t *data);
uint16_t DoUnPackMsg(Com_Message_struct_t *data, uint8_t *buffer);

#endif
