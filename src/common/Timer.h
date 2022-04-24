#pragma once
#include <Arduino.h>

typedef size_t TimerID;

#define MILLI_SECONDS 1000ul
#define MICRO_SECONDS 1000000ul

class ITimerCallback
{
public:
	virtual ~ITimerCallback();
	virtual void OnTimerComplete(TimerID timerId, uint8_t data) = 0;
	virtual void OnTimerStop(TimerID timerId, uint8_t data) {};
};

typedef void (*timerCallBack) (TimerID timerId, uint8_t data);

struct TimerNode
{
    TimerNode(ITimerCallback *pCaller, unsigned long duration, uint8_t data) {
        this->pCaller = pCaller;
        this->startStamp = micros();
        this->endStamp = micros() + duration;
        this->data = data;
    }
	ITimerCallback *pCaller;
	unsigned long startStamp;
	unsigned long endStamp;
	uint8_t data;
	TimerID id = 0;
	TimerNode *pNext = NULL;
    bool IsCompleted() {
        unsigned long stamp = micros();
        return micros() - startStamp >= endStamp - startStamp;
    }
    bool IsStopped() {
        return id == 0;
    }

    unsigned long Remain() {
        if (id == 0 || IsCompleted()) return 0;
        return endStamp - (micros() - startStamp);
    }

};

class Timer
{
protected:
#if defined(ESP32)
	static SemaphoreHandle_t xTimerSemaphore;
#endif
	static TimerNode *pFirst;
	static TimerNode *pProcessing;
	static TimerNode *pProcessingPrev;
    static void LoopCompleted();
    static TimerID AddNode(ITimerCallback *pCaller, unsigned long duration, uint8_t data);
public:
	static TimerID Start(ITimerCallback *pCaller, unsigned long duration, uint8_t data = 0);
	static TimerID Start(timerCallBack completeCB, unsigned long duration, uint8_t data = 0, timerCallBack stopCB = NULL);
	static void Loop();
	static bool Stop(TimerID timerId);
	static bool Contains(ITimerCallback *pCaller, uint8_t data);
	static unsigned long Remain(TimerID timerId);
	static void StopAll(ITimerCallback* pCaller);
    static uint8_t GetData(TimerID timerId);

    class TimerCallback: public ITimerCallback
    {
    private:
        timerCallBack completeCB = NULL;
        timerCallBack stopCB = NULL;
    public:
        TimerCallback(timerCallBack completeCB, timerCallBack stopCB = NULL) {
            this->completeCB = completeCB;
            this->stopCB = stopCB;
        };
        virtual ~TimerCallback() {
            this->completeCB = NULL;
            this->stopCB = NULL;
        };
        void OnTimerComplete(TimerID timerId, uint8_t data) override {
            if (completeCB != NULL) completeCB(timerId, data);
            delete this;
        }
        void OnTimerStop(TimerID timerId, uint8_t data) override {
            if (stopCB != NULL) stopCB(timerId, data);
            delete this;
        }
    };
};
