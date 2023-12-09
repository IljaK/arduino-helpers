#include "SocketMessageBuffer.h"


SocketMessageBuffer::SocketMessageBuffer(const uint8_t lengthBytes, const size_t maxArraySize, const size_t maxMessageSize):
    StackArray(maxArraySize),
    maxMessageSize(maxMessageSize),
    lengthBytes(lengthBytes) {
}

SocketMessageBuffer::~SocketMessageBuffer()
{

}

bool SocketMessageBuffer::Append(SocketMessage *item) {
    if (size == maxSize) return false;
    arr[size] = item;
    size++;
    return false;
}

bool SocketMessageBuffer::AppendItem() {
    SocketMessage *msg = new SocketMessage(lengthBytes);
    if (!Append(msg)) {
        delete msg;
        return false;
    }
    return true;
}

size_t SocketMessageBuffer::AppendInternal(uint8_t *data, size_t length) {
    if (length == 0) return 0;
    if (data == NULL) return 0;

    if (Size() == 0 && !AppendItem()) {
        return 0;
    }

    SocketMessage *lastItem = arr[Size() - 1];

    if (!lastItem->IsFull()) {
        return lastItem->AddData(data, length, maxMessageSize);
    }
    
    // Last element full -> chech if whole stack is filled
    if (Size() == MaxSize()) {
        return 0;
    }

    if (!AppendItem()) {
        return 0;
    }
    lastItem = arr[Size() - 1];
    
    return lastItem->AddData(data, length, maxMessageSize);
}

void SocketMessageBuffer::FreeItem(SocketMessage * item) {
    if (item == NULL) return;
    delete item;
}

size_t SocketMessageBuffer::Append(const uint8_t *item, size_t length) {
    size_t remain = length;
    while (remain > 0) {
        size_t added = AppendInternal((uint8_t *)item + (length - remain), remain);
        if (added == 0) {
            break;
        }
        remain -= added;
    }
    return (length - remain);
}

bool SocketMessageBuffer::HasMessage() {
    if (Size() == 1) {
        return arr[0]->IsFull();
    }
    return Size() > 0;
}

size_t SocketMessageBuffer::TotalLength() {
    size_t totalLength = 0;
    for (size_t i = 0; i < Size(); i++) {
        if (arr[i] != NULL) {
            totalLength += arr[i]->GetFilled();
        }
    }
    return totalLength;
}

SocketMessage * SocketMessageBuffer::Unshift(size_t index)
{
    if (size == 0) {
        return NULL;
    }
    if (index >= maxSize) {
        return NULL;
    }
    if (arr[index]->IsFull()) {
        return StackArray::Unshift(index);
    }
    return NULL;
}