#include <gtest/gtest.h>
#include "array/SocketMessageBuffer.h"
#include "array/SocketMessage.h"

void verifySocketMessage(SocketMessage *msg, uint8_t *arr, size_t arrLen, uint8_t sizeBytes) {
    ASSERT_TRUE(msg != NULL);

    ASSERT_TRUE(msg->IsFull());

    ASSERT_EQ(4, msg->GetSize());

    for (size_t i = 0; i < msg->GetSize(); i++)
    {
        ASSERT_EQ(msg->GetData()[i], arr[i + sizeBytes]);
    }
}

TEST(SocketMessageBufferTest, SocketMessageBufferAppendTest)
{
    const uint8_t sizeBytes = 3;

    SocketMessageBuffer *messageBuffer = new SocketMessageBuffer(sizeBytes, 12, 2048);

    MessageSize ms;
    ms.l = 4;
    uint8_t arr[] = { ms.data[0], ms.data[1], ms.data[2], 0, 1, 2, 3};
    messageBuffer->Append(arr, 7);

    SocketMessage *msg = messageBuffer->UnshiftFirst();

    verifySocketMessage(msg, arr, 7, 3);
    delete msg;

    ASSERT_TRUE(messageBuffer->Append(arr, 2) == 2);
    msg = messageBuffer->Peek();
    ASSERT_TRUE(msg != NULL);

    ASSERT_FALSE(msg->IsFull());

    ASSERT_TRUE(messageBuffer->Append(arr + 2, 2) == 2);
    ASSERT_FALSE(msg->IsFull());
    ASSERT_TRUE(messageBuffer->Append(arr + 4, 3) == 3);


    verifySocketMessage(msg, arr, 7, 3);
    delete msg;
    
}