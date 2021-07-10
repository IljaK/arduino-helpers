#pragma once
#include "StackArray.h"

struct BinaryMessage {
    uint8_t length;
    uint8_t filled;
    uint8_t * data;

    bool isFilled() {
        return length == filled;
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
            free(arr[i]->data);
            free(arr[i]);
            arr[i] = NULL;
        }
    }

    void FreeItem(BinaryMessage * item) override {
        if (item->data != NULL) {
            free(item->data);
        }
        free(item);
    }

    BinaryMessage * UnshiftLast() {
        if (size > 0) {
            size--;
            return arr[size];
        }
        return NULL;
    }
};