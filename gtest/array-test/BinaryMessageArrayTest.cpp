#include <gtest/gtest.h>
#include "array/BinaryMessageStack.h"

TEST(BinaryMessageStackTest, BufferOverflowTest)
{
    BinaryMessageStack messageStack(3u);

    EXPECT_FALSE(messageStack.IsFull());
    EXPECT_EQ(messageStack.MaxSize(), 3);
    EXPECT_EQ(messageStack.Size(), 0);

    BinaryMessage * msg = messageStack.Peek();
    EXPECT_TRUE(msg == NULL);

    msg = messageStack.PeekLast();
    EXPECT_TRUE(msg == NULL);

    msg = new BinaryMessage();

    messageStack.Append(msg);
    EXPECT_EQ(messageStack.Size(), 1);

    EXPECT_TRUE(msg == messageStack.Peek());

    EXPECT_EQ(messageStack.Size(), 1);

    BinaryMessage *msg1 = messageStack.UnshiftFirst();
    EXPECT_TRUE(msg == msg1);

    EXPECT_EQ(messageStack.Size(), 0);

    messageStack.FreeItem(msg);
}