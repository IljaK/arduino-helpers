#include "ByteArrayStream.h"

#if defined(ESP32)
    #define BUFFER_MUTEX_LOCK()    while (xSemaphoreTake( xSemaphore, portMAX_DELAY ) != pdPASS) {}
    #define BUFFER_MUTEX_UNLOCK()  xSemaphoreGive(xSemaphore)
#else
    #define BUFFER_MUTEX_LOCK()
    #define BUFFER_MUTEX_UNLOCK()
#endif

ByteArrayStream::ByteArrayStream(size_t bufferSize)
{
    this->bufferSize = bufferSize;
    buffer = (uint8_t *)malloc(bufferSize);
    length = 0;
}
ByteArrayStream::~ByteArrayStream() {
    length = 0;
    bufferSize = 0;
    free(buffer);
}

size_t ByteArrayStream::Read(uint8_t *buffer, size_t length)
{
    BUFFER_MUTEX_LOCK();
    size_t toRead = min(length, bufferSize - length);

    if (toRead > 0) {
        for(size_t i = 0; i < length; i++) {
            buffer[i] = this->buffer[i];
            if (i+toRead < length) {
                this->buffer[i] = this->buffer[i+toRead];
            }
        }
        length -= toRead;
    }

    BUFFER_MUTEX_UNLOCK();
    return toRead;
}
size_t ByteArrayStream::Write(uint8_t *buffer, size_t length)
{
    BUFFER_MUTEX_LOCK();
    size_t toWrite = min(length, bufferSize - this->length);
    if (toWrite > 0) {
        memcpy(this->buffer + length, buffer, toWrite);
        length += toWrite;
    }
    BUFFER_MUTEX_UNLOCK();
    return toWrite;
}

size_t ByteArrayStream::GetFilled()
{
    BUFFER_MUTEX_LOCK();
    size_t filled = this->length;
    BUFFER_MUTEX_UNLOCK();
    return filled;
}
size_t ByteArrayStream::GetRemainFree()
{
    BUFFER_MUTEX_LOCK();
    size_t remain = bufferSize - this->length;
    BUFFER_MUTEX_UNLOCK();
    return remain;
}

void ByteArrayStream::Clear()
{
    BUFFER_MUTEX_LOCK();
    length = 0;
    bufferSize = 0;
    BUFFER_MUTEX_UNLOCK();
}