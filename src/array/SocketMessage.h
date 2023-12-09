#pragma once

union MessageSize {
    uint8_t data[8];
    uint64_t l = 0;
};

class SocketMessage {

private:
    uint8_t lengthBytes = 0;
    size_t size = 0;
    size_t filled = 0;
    uint8_t * data = NULL;
public:
    SocketMessage(uint8_t lengthBytes) {
        this->lengthBytes;
    }
    ~SocketMessage() {
        if (data != NULL) {
            free(data);
            data = NULL;
        }
        filled = 0;
        size = 0;
    }

    bool IsFull() {
        return size > 0 && filled == size;
    }

    size_t AddData(uint8_t * data, size_t length, size_t maxMessageSize) {
        if (IsFull()) return 0;
        
        // Fetch item size from buffer
        if (this->size == 0) {
            // We havent received enough data for length
            this->size = GetLength(this->data, this->filled, data, length);
            if (this->size == 0) {
                // still not enough bytes for length
                if (this->data == NULL) { // nothing been written
                    // Allocate memory for storing "data length size"
                    this->data = (uint8_t *)malloc(lengthBytes);
                    this->filled = length;
                    memcpy(this->data, data, length);
                } else {
                    // We already have allocated memory for length bytes
                    this->data = (uint8_t *)realloc(this->data, this->filled + length);
                    memcpy(this->data + this->filled, data, length);
                    this->filled += length;
                }
                return length;
            } else {
                if (this->size >= maxMessageSize) {
                    return 0;
                }
                if (this->data == NULL) {
                    // No data has been stored for length, but content has full of it
                    this->data = (uint8_t *)malloc(this->size);
                    this->filled = length - lengthBytes;
                    if (this->filled > this->size) {
                        this->filled = this->size;
                    }
                    if (this->filled > 0) {
                        memcpy(this->data, data + lengthBytes, this->filled);
                    }
                    return this->filled + lengthBytes;
                } else {
                    // Partly length bytes has been stored, so need to fill proper values
                    size_t sizeRemainPart = lengthBytes - this->filled;
                    size_t dataLen = length - sizeRemainPart;

                    free(this->data); // Flush old stored data
                    this->data = (uint8_t *)malloc(this->size); // Create new buffer with proper size
                    if (this->size >= dataLen) {
                        memcpy(this->data, data + sizeRemainPart, dataLen);
                        this->filled = dataLen;
                    } else {
                        memcpy(this->data, data + sizeRemainPart, this->size);
                        this->filled = this->size;
                    }
                    return this->filled + sizeRemainPart;
                }
            }
        }
        // Append unfilled data with known message size
        size_t append = this->size - this->filled;
        if (append > length) append = length;
        memcpy(this->data + this->filled, data, append);
        this->filled += append;
        return append;
    }

    uint64_t GetLength(uint8_t * data, size_t length, uint8_t * data2 = NULL, size_t length2 = 0) {

        if ((uint64_t)length + (uint64_t)length2 < (uint64_t)lengthBytes) {
            return 0;
        }
        
        MessageSize msz;
        for (uint8_t i = 0; i < lengthBytes; i++) {
            if (i < length) {
                msz.data[i] = data[i];
            } else {
                msz.data[i] = data2[i - length];
            }
        }

        //Serial.print("Message Length: ");
        //Serial.println((uint32_t)msz.l);

        return msz.l;
    }

    size_t GetFilled() {
        return filled;
    }
    uint8_t *GetData() {
        return data;
    }
    size_t GetSize() {
        return size;
    }
};