#pragma once
#include "common/Timer.h"

class TimerMock: public Timer, public ITimerCallback
{
public:
	TimerID timerId = 0;
	unsigned long duration = 0;
	bool resetOnComplete = false;
	bool resetOnStop = false;
    TimerID stopTimerOnComplete = 0;

	TimerMock();
	~TimerMock();
	void OnTimerComplete(TimerID timerId, uint8_t data) override;
	void OnTimerStop(TimerID timerId, uint8_t data) override;
	bool IsCompleted();
	void Start(unsigned long duration);
    void Stop();
	uint8_t TimerId() { return timerId; }
	unsigned long Remain();

	static void Reset();
    static void StopAll();
    static void PrintAll();
    static uint32_t GetCount();
    static uint32_t GetAllCount();
    static bool HasDuplicateID();
};

