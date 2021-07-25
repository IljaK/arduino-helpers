#include <gtest/gtest.h>
#include "common/Timer.h"
#include <Arduino.h>
#include "mock/TimerMock.h"

TEST(TimerTest, TimerTestRunStop)
{
	timeOffset = 0;
	TimerMock::Reset();

			
	TimerMock * timerMock = new TimerMock();

	Timer::Loop();
	timerMock->Start(1000000ul);
	Timer::Loop();

	//wchar_t message[128];
	//swprintf(message, 128, L"Timer IsRunning Failed! now: %lu, Remain: %lu", micros(), timerMock->Remain());
	//Assert::IsFalse(timerMock->IsCompleted(), message);
	EXPECT_FALSE(timerMock->IsCompleted());

	timeOffset = 1000000ul;
	Timer::Loop();

	EXPECT_TRUE(timerMock->IsCompleted());

	Timer::StopAll(timerMock);
	timeOffset = 0;
	TimerMock::Reset();

	//swprintf(message, 128, L"Timer Complete Failed! now: %lu, Remain: %lu", micros(), timerMock->Remain());
	//Assert::IsTrue(timerMock->IsCompleted(), message);

}

TEST(TimerTest, TimerTestOverflowMicros)
{
	timeOffset = 0;
    timeOffset -= 10;

	Timer::Loop();

	TimerMock* timerMock = new TimerMock();
	TimerMock* timerMock2 = new TimerMock();
	TimerMock* timerMock3 = new TimerMock();

	timerMock->Start(8ul);
	timerMock2->Start(10ul);
	timerMock3->Start(15ul);

	Timer::Loop();

	timeOffset += 8;
	Timer::Loop();
	EXPECT_TRUE(timerMock->IsCompleted());
	EXPECT_FALSE(timerMock2->IsCompleted());
	EXPECT_FALSE(timerMock3->IsCompleted());


	timeOffset += 4;
	Timer::Loop();
	EXPECT_TRUE(timerMock->IsCompleted());
	EXPECT_TRUE(timerMock2->IsCompleted());
	EXPECT_FALSE(timerMock3->IsCompleted());

	timeOffset += 4;
	Timer::Loop();
	EXPECT_TRUE(timerMock->IsCompleted());
	EXPECT_TRUE(timerMock2->IsCompleted());
	EXPECT_TRUE(timerMock3->IsCompleted());

	Timer::StopAll(timerMock);
	timeOffset = 0;
	TimerMock::Reset();
}

TEST(TimerTest, TimerFillTest)
{
	timeOffset = 0;
	TimerMock::Reset();

	//int maxTimers = 1 + (rand() % MAXBYTE);
	TimerMock timers[MAXBYTE];
	//wchar_t message[128];

	unsigned long maxVal = 0;

	// Fill timers
	for (int i = 0; i < MAXBYTE; i++) {
		unsigned long random = (unsigned long)rand() + 100ul;
		if (maxVal < random) maxVal = random;
		timers[i].Start(random);
	}

	// Iterate delta time
	unsigned long delta = maxVal / 20;
	for (unsigned long i = 0; i < 19; i++) {
		timeOffset += delta;
		Timer::Loop();
	}

	// Reset completed timers
	for (int i = 0; i < MAXBYTE; i++) {
		if (timers[i].IsCompleted()) {
			timers[i].Start(maxVal);
			EXPECT_EQ(i + 1, timers[i].TimerId());
		}
	}

	// Check proper refill
	for (int i = 0; i < MAXBYTE; i++) {
		for (int n = i+1; n < MAXBYTE; n++) {
			EXPECT_NE(timers[n].TimerId(), timers[i].TimerId());
		}
	}


	timeOffset += maxVal;
	Timer::Loop();

	// Check all completed
	for (int i = 0; i < MAXBYTE; i++) {
		EXPECT_TRUE(timers[i].IsCompleted());
	}

	for (int i = 0; i < MAXBYTE; i++) {
		Timer::StopAll(&timers[i]);
	}
	timeOffset = 0;
	TimerMock::Reset();
}

TEST(TimerTest, TimerAddOnCallbackTest)
{
	timeOffset = 0;
	TimerMock::Reset();

	const uint8_t length = 3;
	TimerMock timers[length];

	for (int i = 0; i < length; i++) {
		timers[i].Start(1);
		timers[i].resetOnComplete = true;
	}

	timeOffset += 1;
	Timer::Loop();

	for (int i = 0; i < length; i++) {
		EXPECT_FALSE(timers[i].IsCompleted());
		timers[i].resetOnComplete = false;
	}

	timeOffset += 1;
	Timer::Loop();

	for (int i = 0; i < length; i++) {
		EXPECT_TRUE(timers[i].IsCompleted());
	}
	timeOffset = 0;
	TimerMock::Reset();
}

void onTimerComplete(TimerID timer, uint8_t data) {
    TimerMock::StopAll();
}

TEST(TimerTest, TimerStopAllOnCompleteTest)
{
    timeOffset = 0;
	TimerMock::Reset();

    for (uint8_t i = 0; i < 10; i++) {
        Timer::Start(onTimerComplete, (i + 1));
    }

    for (uint8_t i = 0; i < 10; i++) {
        timeOffset += 1;
        TimerMock::Loop();
    }
}