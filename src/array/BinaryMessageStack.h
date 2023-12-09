#pragma once
#include "StackArray.h"

struct BinaryMessage {

private:
    size_t length = 0;
    size_t filled = 0;
    uint8_t * data = NULL;
public:
    BinaryMessage() {}
    virtual ~BinaryMessage() {
        FreeData();
    }

    void FreeData() {
        if (data != NULL) {
            free(data);
            data = NULL;
        }
    }

    bool isFilled() {
        return filled >= length;
    }
    uint8_t *GetData() {
        return data;
    }
    size_t GetLength() {
        return length;
    }
    size_t GetFilled() {
        return filled;
    }
};

typedef void (*BinaryMessageCallback)(BinaryMessage *);

class BinaryMessageStack: public StackArray<BinaryMessage *>
{
public:
    BinaryMessageStack(const size_t maxSize):StackArray(maxSize) {}

    virtual ~BinaryMessageStack() {
        for(size_t i = 0; i < size; i++) {
            if (arr[i] == NULL) break;
            FreeItem(arr[i]);
            arr[i] = NULL;
        }
    }

    void FreeItem(BinaryMessage * item) override {
        delete item;
    }

    BinaryMessage * UnshiftLast() {
        if (size > 0) {
            size--;
            return arr[size];
        }
        return NULL;
    }
};