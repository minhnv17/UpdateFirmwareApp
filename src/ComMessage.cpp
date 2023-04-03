#include "ComMessage.h"

#define COM_MSG_START_DETECT 0xAA
#define COM_MSG_END_DETECT 0xBB

uint16_t DoPackMsg(uint8_t *buffer, Com_Message_struct_t *data)
{
    uint16_t byteCount = 0U;
    uint16_t payloadLen;
    payloadLen = data->dataLen + data->adrLen;
    buffer[byteCount] = COM_MSG_START_DETECT;
    byteCount++;
    buffer[byteCount] = data->msgId;
    byteCount++;
    buffer[byteCount] = data->adrLen;
    byteCount++;
    buffer[byteCount] = data->dataLen;
    byteCount++;
    memcpy(&buffer[byteCount], data->payload, payloadLen);
    byteCount += payloadLen;
    memcpy(&buffer[byteCount], &data->checkSum, 2);
    byteCount += 2U;
    buffer[byteCount] = COM_MSG_END_DETECT;
    byteCount++;
    return byteCount;
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

