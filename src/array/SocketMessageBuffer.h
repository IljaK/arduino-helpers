#pragma once
#include "array/StackArray.h"
#include "SocketMessage.h"

class SocketMessageBuffer: public StackArray<SocketMessage *> {

private:
    const size_t maxMessageSize;
    const uint8_t lengthBytes;

    bool Append(SocketMessage *item) override;

    bool AppendItem();

    size_t AppendInternal(uint8_t *data, size_t length);
public:
    void FreeItem(SocketMessage * item) override;

    SocketMessageBuffer(const uint8_t lengthBytes, const size_t maxArraySize, const size_t maxMessageSize = 256u);
    virtual ~SocketMessageBuffer();

    size_t Append(const uint8_t *item, size_t length);

    bool HasMessage();

    size_t TotalLength();

    SocketMessage * Unshift(size_t index) override;
};