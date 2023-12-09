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
    this->buffer = (uint8_t *)malloc(bufferSize);
    this->length = 0;
}
ByteArrayStream::~ByteArrayStream() {
    this->length = 0;
    this->bufferSize = 0;
    free(buffer);
}

size_t ByteArrayStream::Read(uint8_t *buffer, size_t length)
{
    BUFFER_MUTEX_LOCK();
    size_t toRead = min(length, this->length);

    if (toRead > 0) {
        for(size_t i = 0; i < this->length; i++) {
            buffer[i] = this->buffer[i];
            if (i+toRead < this->length) {
                this->buffer[i] = this->buffer[i+toRead];
            }
        }
        this->length -= toRead;
    }

    BUFFER_MUTEX_UNLOCK();
    return toRead;
}
size_t ByteArrayStream::Write(uint8_t *buffer, size_t length)
{
    BUFFER_MUTEX_LOCK();
    size_t toWrite = min(length, bufferSize - this->length);
    if (toWrite > 0) {
        memcpy(this->buffer + this->length, buffer, toWrite);
        this->length += toWrite;
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