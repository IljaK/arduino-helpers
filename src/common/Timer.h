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
        this->remain = duration;
        this->data = data;
        this->updateStamp = micros();
    }
	ITimerCallback *pCaller;
	unsigned long remain;
	unsigned long updateStamp;
	uint8_t data;
	TimerID id = 0;
	TimerNode *pNext = NULL;

    bool IsCompleted() {
        if (remain == 0) return true;

        unsigned long delta = micros() - this->updateStamp;
        this->updateStamp += delta;

        if (remain < delta) {
            remain = 0;
        } else {
            remain -= delta;
        }
        return (remain == 0);
    }

    bool IsStopped() {
        return id == 0;
    }

    unsigned long Remain() {
        return remain;
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
    static bool SetData(TimerID timerId, uint8_t data);

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
