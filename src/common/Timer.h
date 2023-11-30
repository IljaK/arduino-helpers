#pragma once
#include <Arduino.h>
#include "TimeDefines.h"

enum TIMER_STATE: uint8_t {
    TIMER_STATE_NONE,
    TIMER_STATE_RUNNING,
    TIMER_STATE_COMPLETED,
};

class Timer;
class TimerNode;

class ITimerCallback
{
public:
	virtual ~ITimerCallback();
	virtual void OnTimerComplete(Timer *timer);
};

typedef void (*timerCallBack) (Timer *timer);

class Timer
{
private:
    TIMER_STATE state = TIMER_STATE_NONE;
    ITimerCallback *pCallback;
	uint64_t remainMicros = 0;
    uint32_t prevStamp = 0;
public:
    Timer(ITimerCallback *pCallback); 
    void StartMicros(uint64_t micros);
    void StartMillis(uint64_t millis);
    void StartSeconds(uint64_t seconds);

    void Stop();

    bool Update();

    uint64_t RemainMicros();

    uint64_t RemainMillis();
    uint64_t RemainSeconds();

    bool IsRunning();
    virtual ~Timer();

private:
#if defined(ESP32)
	static SemaphoreHandle_t xTimerSemaphore;
#endif
    static TimerNode *pFirst;
    
    static void AddTimer(Timer *node);
    static void RemoveTimer(Timer *node);
public:
    static void Loop();
    static void RemoveAll(ITimerCallback* pCaller);
};


class TimerNode 
{
public:
    Timer *pTimer;
    TimerNode *pNext = NULL;

    TimerNode(Timer *pTimer) {
        this->pTimer = pTimer;
    }
};
