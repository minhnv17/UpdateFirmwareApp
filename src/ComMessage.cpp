#include "ComMessage.h"

#define COM_MSG_START_DETECT 0xAA
#define COM_MSG_END_DETECT 0xBB

void DoPackMsg(uint8_t *buffer, Com_Message_struct_t *data)
{
    uint16_t payloadLen;
    payloadLen = data->dataLen + data->adrLen;
    buffer[0] = COM_MSG_START_DETECT;
    buffer[1] = data->msgId;
    buffer[2] = data->adrLen;
    buffer[3] = data->dataLen;
    memcpy(&buffer[4], data->payload, payloadLen);
    memcpy(&buffer[payloadLen + 4], &data->checkSum, 2);
    buffer[payloadLen + 6] = COM_MSG_END_DETECT;
}

uint16_t DoUnPackMsg(Com_Message_struct_t *data, uint8_t *buffer)
{
    uint16_t byteCount = 0U;
    uint16_t payloadLen;
    data->msgId = buffer[1];
    data->adrLen = buffer[2];
    data->dataLen = buffer[3];
    byteCount += 3U;
    payloadLen = data->dataLen + data->adrLen;
    memcpy(data->payload, &buffer[4], payloadLen);
    byteCount += payloadLen;
    memcpy(&data->checkSum, &buffer[payloadLen + 4], 2);
    byteCount += 2;
    if(data->checkSum != byteCount)
    {
        byteCount = 0U;
    }
    return byteCount;
}

