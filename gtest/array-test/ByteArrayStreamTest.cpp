#include <gtest/gtest.h>
#include "array/ByteArrayStream.h"


TEST(ByteArrayStreamTest, ByteArrayStreamTestReadWrite)
{
    uint8_t array[3] = {0,1,2};

    uint8_t size = 8;
    ByteArrayStream * stream = new ByteArrayStream(size);

	EXPECT_EQ(stream->GetFilled(), 0);
	EXPECT_EQ(stream->GetRemainFree(), size);
    
    stream->Write(array, 3);

	EXPECT_EQ(stream->GetFilled(), 3);
	EXPECT_EQ(stream->GetRemainFree(), size - 3);

    stream->Write(array, 3);

	EXPECT_EQ(stream->GetFilled(), 6);
	EXPECT_EQ(stream->GetRemainFree(), size - 6);

    stream->Write(array, 3);

	EXPECT_EQ(stream->GetFilled(), 8);
	EXPECT_EQ(stream->GetRemainFree(), 0);

    stream->Read(array, 3);
    for (uint8_t i = 0; i < 3; i++) {
        EXPECT_EQ(array[i], i);
    }
	EXPECT_EQ(stream->GetFilled(), 5);
	EXPECT_EQ(stream->GetRemainFree(), 3);

    stream->Read(array, 3);
    for (uint8_t i = 0; i < 3; i++) {
        EXPECT_EQ(array[i], i);
    }
	EXPECT_EQ(stream->GetFilled(), 2);
	EXPECT_EQ(stream->GetRemainFree(), 6);


    stream->Read(array, 3);
    for (uint8_t i = 0; i < 2; i++) {
        EXPECT_EQ(array[i], i);
    }
	EXPECT_EQ(stream->GetFilled(), 0);
	EXPECT_EQ(stream->GetRemainFree(), size);

    delete stream;
}