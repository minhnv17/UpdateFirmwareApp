#ifndef UART_COM_H
#define UART_COM_H
/*==============================================================================
*                               INCLUDE FILES
==============================================================================*/
#include <windows.h>
#include <mutex>
#include <iostream>
#include <stdint.h>
/*==============================================================================
*                               DEFINITIONS
==============================================================================*/
typedef void (*serial_read_cb)(uint8_t *buff, uint16_t size);

#define MAX_BUFFER_SIZE 4096

typedef enum {
    COM_9600 = 0,
    COM_19200,
    COM_38400,
    COM_57600,
    COM_115200,

    COM_UNKNOW = -1
} Baud_rate_t;

class UartCom
{
    private:
        char *comPort;
        Baud_rate_t baudrate;
        HANDLE ioHandler;
        DWORD errors;
        COMSTAT status;
        serial_read_cb readCb;
        uint16_t readByteSize;
        bool isReading;
        bool isConnected;
        HANDLE readCbMutex;
        DWORD WINAPI read_thread(LPVOID lpThreadParameter);
        uint8_t bufferRead[MAX_BUFFER_SIZE];
        uint16_t readCount;
    public:
        UartCom();
        ~UartCom();
        uint8_t open_port(char *_comPort, Baud_rate_t _baudrate, uint16_t readByes, serial_read_cb readCb, char *_rw);
        uint32_t serial_write(void *_data, uint32_t _size);
        uint8_t start_read();
        bool close_serial();
};

#endif /* UART_COM_H */
