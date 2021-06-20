#include <gtest/gtest.h>
#include <Arduino.h>
#include "common/Timer.h"
#include "../mock/BaseSerialHandlerMock.h"
#include <Stream.h>
#include "../mock/TimerMock.h"


TEST(BaseSerialHandlerTest, BufferOverflowTest)
{
	timeOffset = 0;
	TimerMock::Reset();
	char data1[] = "some arg"; // length = 8

	Stream stream;
	BaseSerialHandlerMock serialHandler(&stream);

	serialHandler.Loop();
    TimerMock::Loop();

	serialHandler.StartTimeoutTimer(1000000ul);
	//wchar_t message[128];

	int maxIterations = SERIAL_RX_BUFFER_SIZE / strlen(data1) + 1;

	for (int i = 0; i < maxIterations; i++) {
		stream.AddRXBuffer(data1);
        TimerMock::Loop();
		serialHandler.Loop();


		if (serialHandler.isResponseReceived) {

			EXPECT_TRUE(serialHandler.isResponseOverFlow);
			EXPECT_EQ(i, maxIterations - 2);

			//if (!serialHandler.isResponseOverFlow) {
				//swprintf(message, 128, L"Command detected before owerflow %d available: %d", i, serialHandler.SerialStream()->available());
				//Assert::Fail(message);
			//}
			//else if (i != maxIterations - 2) {
				//swprintf(message, 128, L"Command owerflow happened on wrong index %d available: %d", (i + 1), serialHandler.SerialStream()->available());
				//Assert::Fail(message);
			//}
			return;
		}
	}

	FAIL();

	//swprintf(message, 128, L"Command owerflow has not been detected available: %d", serialHandler.SerialStream()->available());
	//Assert::Fail(message);
}

TEST(BaseSerialHandlerTest, SerialHandlerTimeoutTest)
{
	timeOffset = 0;
	TimerMock::Reset();

	Stream stream;
	BaseSerialHandlerMock serialHandler(&stream);
	serialHandler.Loop();

	uint8_t aray[] = { 5,5,5 };
	serialHandler.StartTimeoutTimer(1000000ul);

	Timer::Loop();
	serialHandler.Loop();
	EXPECT_FALSE(serialHandler.isResponseReceived);

	timeOffset = 1000000ul;

	Timer::Loop();
	serialHandler.Loop();

	EXPECT_TRUE(serialHandler.isResponseReceived);

	Timer::StopAll(&serialHandler);
}