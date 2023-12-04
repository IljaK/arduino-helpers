#pragma once
#include "../common/Util.h"

struct ByteArray {
private:
    size_t length = 0;
    uint8_t * array = NULL;
public:
    virtual ~ByteArray() {
        Clear();
    }
    bool SetArray(uint8_t * array, size_t length) {
        if (this->array != NULL) {
            uint8_t *newArr = (uint8_t *)realloc(this->array, length);
            if (newArr == NULL) {
                return false;
            }
            this->array = newArr;
        } else {
            this->array = (uint8_t *)malloc(length);
            if (this->array == NULL) {
                this->length = 0;
                return false;
            }
        }
        memcpy(this->array, array, length);
        this->length = length;
        return true;
    }
    bool AppendArray(uint8_t * array, size_t length) {
        if (this->array == NULL) {
            return SetArray(array, length);
        }
        uint8_t *newArr = (uint8_t *)realloc(this->array, this->length + length);
        if (newArr == NULL) {
            return false;
        }
        this->array = newArr;
        memcpy(this->array + this->length, array, length);
        this->length += length;
        return true;
    }
    
    void Clear() {
        if (this->array != NULL) {
            free(this->array);
        }
        this->length = 0;
    }

    size_t GetLength() {
        return length;
    }
    uint8_t *GetArray() {
        return array;
    }
    uint8_t *GetArrayAt(size_t index) {
        return array + index;
    }
};