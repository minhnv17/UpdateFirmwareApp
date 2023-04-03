/*==============================================================================
*                               INCLUDE FILES
==============================================================================*/
#include <windows.system.threading.h>
#include <string.h>
#include "UartCom.h"
/*==============================================================================
*                               DEFINITIONS
==============================================================================*/

/*==============================================================================
*                               PROTOTYPES
==============================================================================*/
static inline int parse_baud(int _baudrate);

/*==============================================================================
*                               INPLEMENTATIONS
==============================================================================*/

int parse_baud(Baud_rate_t _baudrate)
{
    switch (_baudrate)
    {
        case COM_9600:
            return 9600;
        case COM_19200:
            return 19200;
        case COM_38400:
            return 38400;
        case COM_57600:
            return 57600;
        case COM_115200:
            return 115200;
        default:
            return 0;
    }
}

UartCom::UartCom()
{
    comPort = "";
    baudrate = COM_UNKNOW;
    readCb = nullptr;
    isReading = false;
    isConnected = false;
    readByteSize = 0U;
}

UartCom::~UartCom()
{
    this->close_serial();
}

uint8_t UartCom::open_port(char *_comPort, Baud_rate_t _baudrate, uint16_t readByes, serial_read_cb readCb, char *_rw)
{
    uint8_t result = 0U; // Todo: return fail code
    uint32_t readWrite = 0;
    DCB dcbSerialParams = { 0 };
    this->comPort = _comPort;
    this->baudrate = _baudrate;
    this->readByteSize = readByes;
    this->readCb = readCb;

    if(!this->isConnected && result == 0U)
    {
        if(strcmp(_rw, "r") == 0)
        {
            readWrite = GENERIC_READ;
        }
        else if(strcmp(_rw, "w") == 0)
        {
            readWrite = GENERIC_WRITE;
        }
        else if(strcmp(_rw, "rw") == 0)
        {
            readWrite = GENERIC_READ | GENERIC_WRITE;
        }
        else
        {
            result = 1U;
        }

        if(result == 0U)
        {
            ioHandler = CreateFileA(static_cast<LPCSTR>(this->comPort), readWrite,
                                0,
                                NULL,
                                OPEN_EXISTING,
                                FILE_ATTRIBUTE_NORMAL,
                                NULL);
            if(ioHandler == INVALID_HANDLE_VALUE)
            {
                result = 1U;
            }
        }

        if(result == 0U)
        {
            if (!GetCommState(ioHandler, &dcbSerialParams))
            {
                result = 1U;
                printf("Warning: Failed to get current serial params");
            }
        }

        if(result == 0U)
        {
            dcbSerialParams.BaudRate = parse_baud(this->baudrate);
            dcbSerialParams.ByteSize = 8;
            dcbSerialParams.StopBits = ONESTOPBIT;
            dcbSerialParams.Parity = NOPARITY;
            dcbSerialParams.fDtrControl = DTR_CONTROL_ENABLE;

            if (!SetCommState(ioHandler, &dcbSerialParams))
            {
                result = 1U;
                printf("Warning: could not set serial port params\n");
            }
            else
            {
                isConnected = true;
                PurgeComm(ioHandler, PURGE_RXCLEAR | PURGE_TXCLEAR);
            }
        }
    }
    else
    {
        result = 1U;
    }
    return result;
}

uint32_t UartCom::serial_write(void *_data, uint32_t _size)
{
    DWORD byteSent;
    if(isConnected)
    {
        if (!WriteFile(this->ioHandler, (void*)_data, _size, &byteSent, NULL))
        {
            ClearCommError(this->ioHandler, &this->errors, &this->status);
        }
    }
    return (uint32_t)byteSent;
}

uint8_t UartCom::start_read()
{
    uint8_t res = 0U;
    if(isConnected)
    {
        this->isReading = true;
        DWORD myThreadID;
        HANDLE myHandle = CreateThread(NULL, 0, (LPTHREAD_START_ROUTINE)&this->read_thread, reinterpret_cast<void*>(this), NULL, &myThreadID);
    }
    return res;
}

DWORD WINAPI UartCom::read_thread(LPVOID lpThreadParameter)
{
    uint8_t res = 0U;
    DWORD Err;
    COMMTIMEOUTS ct;

    if(this->isConnected)
    {
        if(!GetCommTimeouts(this->ioHandler, &ct))
        {
            printf("GetCommTimeouts failed!\n");
            res = 1U;
        }

        ct.ReadIntervalTimeout = MAXDWORD;
        ct.ReadTotalTimeoutMultiplier = MAXDWORD;
        ct.ReadTotalTimeoutConstant = 0;
        ct.ReadIntervalTimeout = 2000; // Todo: set timeout

        if(res == 0U)
        {
            if(!SetCommTimeouts(this->ioHandler, &ct))
            {
                printf("SetCommTimeouts failed!\n");
                res = 1U;
            }
        }

        while(this->isReading && res == 0U)
        {
            if(!ReadFile(this->ioHandler, this->bufferRead, this->readByteSize, (DWORD *)&this->readCount, NULL))
            {
                ClearCommError(this->ioHandler, &Err, NULL);
            }
            if(this->readCb != nullptr)
            {
                this->readCb(this->bufferRead, this->readCount);
            }
        }
    }
    return res;
}

bool UartCom::close_serial()
{
    bool res = true;
    if (this->isConnected) {
        this->isReading = false;
        this->isConnected = false;
        CloseHandle(this->ioHandler);
    }
    else
    {
        res = false;
    }
    return res;
}