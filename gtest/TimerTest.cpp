#include <gtest/gtest.h>
#include "common/Timer.h"
#include <Arduino.h>
#include "mock/TimerMock.h"

TEST(TimerTest, TimerTestRunStop)
{
	timeOffset = 0;
	TimerMock::Reset();

			
	TimerMock timerMock;

	Timer::Loop();
	timerMock.Start(1000000ul);
	Timer::Loop();

	//wchar_t message[128];
	//swprintf(message, 128, L"Timer IsRunning Failed! now: %lu, Remain: %lu", micros(), timerMock.Remain());
	//Assert::IsFalse(timerMock.IsCompleted(), message);
	EXPECT_FALSE(timerMock.IsCompleted());

	timeOffset = 1000000ul;
	Timer::Loop();

	EXPECT_TRUE(timerMock.IsCompleted());

	Timer::StopAll(&timerMock);
	timeOffset = 0;
	TimerMock::Reset();

	//swprintf(message, 128, L"Timer Complete Failed! now: %lu, Remain: %lu", micros(), timerMock.Remain());
	//Assert::IsTrue(timerMock.IsCompleted(), message);

}

TEST(TimerTest, TimerTestOverflowMicros)
{
	timeOffset = 0;
    timeOffset -= 10;

	Timer::Loop();

	TimerMock timerMock = TimerMock();
	TimerMock timerMock2 = TimerMock();
	TimerMock timerMock3 = TimerMock();

	timerMock.Start(8ul);
	timerMock2.Start(10ul);
	timerMock3.Start(15ul);

	Timer::Loop();

	timeOffset += 8;
	Timer::Loop();
	EXPECT_TRUE(timerMock.IsCompleted());
	EXPECT_FALSE(timerMock2.IsCompleted());
	EXPECT_FALSE(timerMock3.IsCompleted());


	timeOffset += 4;
	Timer::Loop();
	EXPECT_TRUE(timerMock.IsCompleted());
	EXPECT_TRUE(timerMock2.IsCompleted());
	EXPECT_FALSE(timerMock3.IsCompleted());

	timeOffset += 4;
	Timer::Loop();
	EXPECT_TRUE(timerMock.IsCompleted());
	EXPECT_TRUE(timerMock2.IsCompleted());
	EXPECT_TRUE(timerMock3.IsCompleted());

	Timer::StopAll(&timerMock);
	Timer::StopAll(&timerMock2);
	Timer::StopAll(&timerMock3);
    
	timeOffset = 0;
	TimerMock::Reset();
}

TEST(TimerTest, TimerFillTest)
{
	timeOffset = 0;
	TimerMock::Reset();

    const int timerCount = MAXBYTE;
	TimerMock timers[timerCount];
	//wchar_t message[128];

	unsigned long maxVal = 0;
    int maxSteps = 20;

	// Fill timers
	for (int i = 0; i < timerCount; i++) {
		unsigned long random = (unsigned long)rand() + 100ul;
		if (maxVal < random) maxVal = random;
		timers[i].Start(random);
	}

    //TimerMock::PrintAll();

	// Iterate delta time
	unsigned long delta = maxVal / maxSteps;
    unsigned long reminder = (maxVal - (unsigned long)maxSteps * delta);

	for (int i = 0; i < maxSteps; i++) {
        timeOffset += delta;
		Timer::Loop();
	}

    if (reminder > 0) {
        timeOffset += reminder;
		Timer::Loop();
    }

    //TimerMock::PrintAll();

    Timer::Loop();
    EXPECT_EQ(TimerMock::GetCount(), 0);

	// Reset completed timers
	for (int i = 0; i < timerCount; i++) {
		if (timers[i].IsCompleted()) {
			timers[i].Start(maxVal);
			EXPECT_EQ(i + 1, timers[i].TimerId());
            EXPECT_FALSE(timers[i].IsCompleted());
		}
	}

	// Check proper refill
	for (int i = 0; i < timerCount; i++) {
		for (int n = i+1; n < timerCount; n++) {
			EXPECT_NE(timers[n].TimerId(), timers[i].TimerId());
		}
	}

	timeOffset += maxVal;
	Timer::Loop();

	// Check all completed
	for (int i = 0; i < timerCount; i++) {
		EXPECT_TRUE(timers[i].IsCompleted());
	}

	for (int i = 0; i < timerCount; i++) {
		Timer::StopAll(&timers[i]);
	}

	for (int i = 0; i < timerCount; i++) {
		EXPECT_TRUE(timers[i].IsCompleted());
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
		timers[i].Start(length - i);
		timers[i].resetOnComplete = true;
        timers[i].duration = length;
	}

    EXPECT_EQ(TimerMock::GetCount(), length);
    EXPECT_FALSE(TimerMock::HasDuplicateID());

    for (int i = 0; i < length; i++) {
        timeOffset += 1;
        Timer::Loop();
        EXPECT_EQ(TimerMock::GetCount(), length);
    }

    EXPECT_FALSE(TimerMock::HasDuplicateID());
    EXPECT_EQ(TimerMock::GetCount(), length);

	for (int i = 0; i < length; i++) {
		EXPECT_FALSE(timers[i].IsCompleted());
		timers[i].resetOnComplete = false;
	}

    EXPECT_FALSE(TimerMock::HasDuplicateID());
    EXPECT_EQ(TimerMock::GetCount(), length);

	timeOffset += 3;
	Timer::Loop();

	for (int i = 0; i < length; i++) {
		EXPECT_TRUE(timers[i].IsCompleted());
	}

    EXPECT_EQ(TimerMock::GetCount(), 0);

    for (int i = 0; i < length; i++) {
        timers[i].Start(length);
		timers[i].resetOnComplete = false;
		timers[i].resetOnStop = true;
    }

    for (int i = 0; i < length; i++) {
        timers[i].Stop();
		timers[i].resetOnComplete = false;
		timers[i].resetOnStop = false;
    }

    EXPECT_FALSE(TimerMock::HasDuplicateID());
    EXPECT_EQ(TimerMock::GetCount(), length);

	Timer::Loop();

    EXPECT_FALSE(TimerMock::HasDuplicateID());
    EXPECT_EQ(TimerMock::GetCount(), length);

    for (int i = 0; i < length; i++) {
        timers[i].Start(length);
    }

    // Timers will reset
    for (int i = length - 1; i >= 0; i--) {
		timers[i].resetOnStop = true;
        // Will stop and start new one
        timers[i].Stop();
		timers[i].resetOnStop = false;
        timers[i].Stop();
    }

    EXPECT_FALSE(TimerMock::HasDuplicateID());

    Timer::Loop();
    EXPECT_EQ(TimerMock::GetCount(), 0);


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
	const uint8_t length = 10;

    for (uint8_t i = 0; i < length; i++) {
        Timer::Start(onTimerComplete, (i + 1));
    }

    for (uint8_t i = 0; i < length; i++) {
        timeOffset += 1;
        TimerMock::Loop();
    }
}

TimerID timerToStop = 0;
void onTimerCompleteAndStop(TimerID timer, uint8_t data) {
    if (timerToStop != 0) {
        Timer::Stop(timerToStop);
        timerToStop = 0;
    }
}

TEST(TimerTest, TimerStopNextProcessed)
{
    timeOffset = 0;
	TimerMock::Reset();

    const int timerCount = 10;
    TimerID timers[timerCount];

    for (uint8_t i = 0; i < timerCount; i++) {
        timers[i] = Timer::Start(onTimerCompleteAndStop, (i + 1));
    }
    timerToStop = timers[1];
    timeOffset = 1;
    Timer::Loop();
}

void onTimerDoNothing(TimerID timer, uint8_t data) {
    
}
TEST(TimerTest, TimerStopInstance)
{
    timeOffset = 0;
	TimerMock::Reset();

    TimerID id = Timer::Start(&onTimerDoNothing, 1);
    EXPECT_TRUE(Timer::Stop(id));

    Timer::TimerCallback *cb = new Timer::TimerCallback(&onTimerDoNothing);
    TimerID id1 = Timer::Start(cb, 1);
    TimerID id2 = Timer::Start(cb, 1);

    delete cb;

    TimerMock::Loop();

    EXPECT_EQ(TimerMock::GetCount(), 0);
}

void onTimerStartNew(TimerID timer, uint8_t data) {
    Timer::Start(&onTimerDoNothing, 1);
}
TEST(TimerTest, TimerOrder)
{
    timeOffset = 0;
	TimerMock::Reset();

    TimerID id[3];

    id[0] = Timer::Start(&onTimerDoNothing, 10);
    id[1] = Timer::Start(&onTimerDoNothing, 1);
    id[2] = Timer::Start(&onTimerDoNothing, 10);

    EXPECT_EQ(TimerMock::GetCount(), 3);

    Timer::Stop(id[1]);

    EXPECT_EQ(TimerMock::GetCount(), 2);

    id[1] = Timer::Start(&onTimerDoNothing, 1, 0, &onTimerStartNew);
    Timer::Stop(id[1]);

    EXPECT_EQ(TimerMock::GetCount(), 3);

    id[1] = Timer::Start(&onTimerStartNew, 1);
    timeOffset += 1;
    Timer::Loop();

    EXPECT_EQ(TimerMock::GetCount(), 3);
}

TEST(TimerTest, TimerAddRemoveTest)
{
    timeOffset = 0;
	TimerMock::Reset();

    int const count = 5;
    TimerMock timers[count];

    timers[0].Start(10);
    timers[0].resetOnComplete = true;

    timers[1].Start(1);
    timers[1].resetOnComplete = true;

    timers[2].Start(1);
    timers[2].resetOnComplete = true;

    timers[3].Start(10);
    timers[3].resetOnComplete = true;

    timers[4].Start(10);
    timers[4].resetOnComplete = true;


    timeOffset = 1;
    Timer::Loop();

    ASSERT_EQ(TimerMock::GetCount(), count);

    for (int i = 0; i < count; i++) {
        timers[i].resetOnComplete = false;
        timers[i].Stop();
    }

    ASSERT_EQ(TimerMock::GetCount(), 0);

    timeOffset = 0;
	TimerMock::Reset();

    timers[0].Start(1);
    timers[0].resetOnComplete = true;

    timers[1].Start(1);
    timers[1].resetOnComplete = true;

    timers[2].Start(10);
    timers[2].resetOnComplete = true;

    timers[3].Start(10);
    timers[3].resetOnComplete = true;

    timers[4].Start(10);
    timers[4].resetOnComplete = true;

    timeOffset = 1;
    Timer::Loop();

    ASSERT_EQ(TimerMock::GetCount(), count);

    timeOffset = 0;
	TimerMock::Reset();

    ASSERT_EQ(TimerMock::GetCount(), 0);

    timers[0].Start(2);
    timers[0].resetOnComplete = false;

    timers[1].Start(2);
    timers[1].resetOnStop = true;

    timers[2].Start(1);
    timers[2].resetOnComplete = false;
    timers[2].stopTimerOnComplete = timers[1].timerId;

    timers[3].Start(2);
    timers[3].resetOnComplete = false;

    //timers[1].resetOnStop = false;

    timeOffset = 1;
    Timer::Loop();

    ASSERT_EQ(TimerMock::GetCount(), 3);

    timeOffset = 0;
	TimerMock::Reset();

    ASSERT_EQ(TimerMock::GetCount(), 0);

    timers[0].Start(2);
    timers[0].resetOnStop = true;

    timers[1].Start(1);
    timers[1].resetOnComplete = false;
    timers[1].stopTimerOnComplete = timers[0].timerId;

    timers[2].Start(2);
    timers[2].resetOnComplete = false;

    timers[3].Start(2);
    timers[3].resetOnComplete = false;

    timeOffset = 1;
    Timer::Loop();

    ASSERT_EQ(TimerMock::GetCount(), 3);

}