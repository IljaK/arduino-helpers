#pragma once
#include "../common/Util.h"
#include "StackArray.h"
#include "ByteArray.h"

class ByteStackArray: public StackArray<ByteArray *>
{
private:
    size_t AppendToItem(ByteArray *item , uint8_t * data, size_t length) {

        if (item == NULL) return 0;

        size_t append = maxItemLength - item->GetLength();
        if (append > length) append = length;

        if (item->AppendArray(data, append)) {
            return append;
        }
        return 0;

    }

    ByteArray * InsertNewItem(size_t index) {
        arr[index] = new ByteArray();
        size++;
        return arr[index];
    }

    size_t AppendInternal(uint8_t *data, size_t length) {
        if (length == 0) return 0;
        if (data == NULL) return 0;

        if (Size() == 0) {
            return AppendToItem(InsertNewItem(0), data, length);
        }

        ByteArray *lastItem = arr[Size() - 1];

        if (lastItem->GetLength() < maxItemLength) {
            return AppendToItem(lastItem, data, length);
        } 
        
        if (Size() == MaxSize()) {
            // Last element full
            return 0;
        }
        
        return AppendToItem(InsertNewItem(Size()), data, length);
    }
public:
    void FreeItem(ByteArray * item) override {
        if (item != NULL) {
            delete item;
        }
    }
    const size_t maxItemLength;

    ByteStackArray(const size_t maxSize, const size_t maxItemLength):
        StackArray(maxSize),
        maxItemLength(maxItemLength) {
    }

    bool IsElementEqual(ByteArray * item1, ByteArray * item2) override {
        return false;
    }

    bool Append(ByteArray *item) override {
        // TODO:
        return false;
    }

    size_t Append(const uint8_t *item, size_t length) {
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

    bool HasFilledPacket() {
        if (Size() == 1) {
            return arr[0]->GetLength() == maxItemLength;
        }
        return Size() > 0;
    }
};