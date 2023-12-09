#pragma once
#include <Arduino.h>

class ByteArrayStream
{
private:
#ifdef ESP32
    SemaphoreHandle_t xSemaphore = xSemaphoreCreateMutex();
#endif

    size_t bufferSize;
    size_t length = 0;
    uint8_t * buffer = NULL;

public:
    ByteArrayStream(size_t bufferSize);
    virtual ~ByteArrayStream();

    size_t Read(uint8_t *buffer, size_t length);
    size_t Write(uint8_t *buffer, size_t length);

    size_t GetFilled();
    size_t GetRemainFree();

    void Clear();
};