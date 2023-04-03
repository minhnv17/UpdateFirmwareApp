#include <iostream>
#include <windows.system.threading.h>
#include "interface/UartCom.h"
#include "SrecFormat.h"
#include "ComMessage.h"

void readCb(uint8_t *buff, uint16_t size)
{
    uint16_t ind;
    if(size > 0)
    {
        for(ind = 0U; ind < size; ind++)
        {
            printf("%d", buff[ind]);
        }
    }
    else
    {
        printf("No, can't get anything!");
    }
}

int main(int argc, char *argv[])
{
    UartCom serial1;
    Com_Message_struct_t comMsgLine;
    uint8_t comMsgLineBuff[4096];
    uint16_t comMsgLineLen;
    uint32_t lineCount = 0U;
    uint32_t lineIndex = 0U;
    FILE *fsrecord;
    char srecRecord[514];
    int result = 0;
    SRecord srec;

    fsrecord = fopen("srecord_example1", "r");
    if(fsrecord != NULL)
    {
        // Calculate line count
        while(fgets(srecRecord, 514, fsrecord) != NULL)
        {
            lineCount++;
        }
        fseek(fsrecord, 0, SEEK_SET);
        // First we verify srec record
        while(fgets(srecRecord, 514, fsrecord) != NULL && result == 0U)
        {
            result = Read_SRecord(&srec, srecRecord);
            if(result == 0U)
            {
                lineIndex++;
            }
            std::cout << "Verifing hex file by line ";
            std::cout << "[" << lineIndex << "/" << lineCount << "]\r";
            std::cout.flush();
        }
        std::cout << std::endl;
        if(lineIndex == lineCount)
        {
            std::cout << "Done! Hex file is good, ready to booting!" << std::endl;
            fseek(fsrecord, 0, SEEK_SET);
            printf("%d\n", serial1.open_port("COM2", COM_115200, 20, readCb, "rw"));
            while(fgets(srecRecord, 514, fsrecord) != NULL)
            {
                Read_SRecord(&srec, srecRecord);
                Print_SRecord(&srec);
                comMsgLine.msgId = COM_MSG_WRITE_DATA;
                comMsgLine.adrLen = 4U;
                comMsgLine.dataLen = srec.dataLen;
                memcpy(comMsgLine.payload, (void*)&srec.address, 4);
                memcpy(&comMsgLine.payload[4], srec.data, comMsgLine.dataLen);
                comMsgLine.checkSum = 3 + 4 + comMsgLine.dataLen + 2;
                comMsgLineLen = DoPackMsg(comMsgLineBuff, &comMsgLine);
                printf("bytes: %d\n", serial1.serial_write((void*)comMsgLineBuff, comMsgLineLen));
            }
        }
    }
    return 0;
}
