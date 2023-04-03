#include <iostream>
#include <windows.system.threading.h>
#include "interface/UartCom.h"
#include "SrecFormat.h"

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
    // UartCom serial1;
    // printf("%d\n", serial1.open_port("COM2", COM_115200, 20, readCb, "rw"));
    // char *data = "YOU ARE NOOB@!!!!";
    // printf("bytes: %d\n", serial1.serial_write((void*)data, strlen(data)));
    // serial1.start_read();
    // while(1)
    // {
    //     Sleep(200);
    // }
    // FILE *fsrecord;
    // char srecRecord[514];
    // int result = 0;
    // SRecord srec;
    // fsrecord = fopen("srecord_example", "r");
    // if(fsrecord != NULL)
    // {
    //     // First we verify srec record
    //     while(fgets(srecRecord, 514, fsrecord) != NULL && result == 0U)
    //     {
    //         result = Read_SRecord(&srec, srecRecord);
    //         Print_SRecord(&srec);
    //     }
    // }
    float progress = 0.0;
    while (progress < 1.0) {
        int barWidth = 70;

        std::cout << "[";
        int pos = barWidth * progress;
        for (int i = 0; i < barWidth; ++i) {
            if (i < pos) std::cout << "=";
            else if (i == pos) std::cout << ">";
            else std::cout << " ";
        }
        std::cout << "] " << int(progress * 100.0) << " %\r";
        std::cout.flush();

        progress += 0.16; // for demonstration only
        Sleep(100);
    }
    std::cout << std::endl;
    return 0;
}
