#include "Timer.h"

#if defined(ESP32)
SemaphoreHandle_t Timer::xTimerSemaphore = xSemaphoreCreateRecursiveMutex();
#define TIMER_MUTEX_LOCK()    while (xSemaphoreTakeRecursive( xTimerSemaphore, portMAX_DELAY ) != pdPASS) {}
#define TIMER_MUTEX_UNLOCK()  xSemaphoreGiveRecursive(xTimerSemaphore)
#endif
TimerNode *Timer::pFirst = NULL;


ITimerCallback::~ITimerCallback()
{
	Timer::RemoveAll(this);
};

void ITimerCallback::OnTimerComplete(Timer *timer) {}

Timer::Timer(ITimerCallback *pCallback) {
    this->pCallback = pCallback;
    Timer::AddTimer(this);
}
Timer::~Timer() {
    RemoveTimer(this);
}

void Timer::StartMicros(uint64_t durationMicros) {
#if defined(ESP32)
	TIMER_MUTEX_LOCK();
#endif
    this->prevStamp = micros();
    this->remainMicros = durationMicros;
    this->state = TIMER_STATE_RUNNING;
#if defined(ESP32)
    TIMER_MUTEX_UNLOCK();
#endif
}
void Timer::StartMillis(uint64_t durationMillis) {
    StartMicros((uint64_t)durationMillis * (uint64_t)MICROS_IN_MILLIS);
}
void Timer::StartSeconds(uint64_t durationSeconds) {
    StartMicros((uint64_t)durationSeconds * (uint64_t)MICROS_IN_SECOND);
}

void Timer::Stop() {
#if defined(ESP32)
	TIMER_MUTEX_LOCK();
#endif
    if (!IsRunning()) {
#if defined(ESP32)
        TIMER_MUTEX_UNLOCK();
#endif
        return;
    }
    state = TIMER_STATE_NONE;
    remainMicros = 0;
#if defined(ESP32)
    TIMER_MUTEX_UNLOCK();
#endif
}

bool Timer::Update() {
#if defined(ESP32)
	TIMER_MUTEX_LOCK();
#endif
    if (!IsRunning()) {
#if defined(ESP32)
        TIMER_MUTEX_UNLOCK();
#endif
        return false;
    }

    uint32_t stamp = micros();
    uint64_t delta = 0;
    delta = stamp - prevStamp;

    if (delta >= remainMicros) {
        remainMicros = 0;
        state = TIMER_STATE_COMPLETED;
#if defined(ESP32)
        TIMER_MUTEX_UNLOCK();
#endif
        return true;
    }
    prevStamp = stamp;
    remainMicros -= delta;
#if defined(ESP32)
    TIMER_MUTEX_UNLOCK();
#endif
    return false;
}

uint64_t Timer::RemainMicros() {
    return remainMicros;
}
uint64_t Timer::RemainMillis() {
    return remainMicros / (uint64_t)MICROS_IN_MILLIS;
}
uint64_t Timer::RemainSeconds() {
    return remainMicros / (uint64_t)MICROS_IN_MILLIS;
}

bool Timer::IsRunning() {
    return state == TIMER_STATE_RUNNING;
}

void Timer::Loop()
{
#if defined(ESP32)
	TIMER_MUTEX_LOCK();
#endif
    // First pass Update timers
    for (TimerNode *pNode = pFirst; pNode; pNode = pNode->pNext) {
        if (pNode->pTimer != NULL && pNode->pTimer->Update()) {
            if (pNode->pTimer->pCallback != NULL) {
                pNode->pTimer->pCallback->OnTimerComplete(pNode->pTimer);
            }
        }
    }

    // Second pass delete empty nodes:
    TimerNode *pPrev = NULL;
    for (TimerNode *pNode = pFirst; pNode; ) {
        if (pNode->pTimer == NULL) {
            if (pNode == pFirst) {
                pFirst = pNode->pNext;
            }
            if (pPrev != NULL) {
                pPrev->pNext = pNode->pNext;
            }

            TimerNode *delNode = pNode;
            pNode = pNode->pNext;
            delete delNode;
            continue;
        }

        pPrev = pNode;
        pNode = pNode->pNext;
    }

#if defined(ESP32)
    TIMER_MUTEX_UNLOCK();
#endif
}

void Timer::RemoveAll(ITimerCallback* pCaller)
{
#if defined(ESP32)
	TIMER_MUTEX_LOCK();
#endif 
    for (TimerNode *pNode = pFirst; pNode; pNode = pNode->pNext) {
        if (pNode->pTimer != NULL && pCaller == pNode->pTimer->pCallback) {
            pNode->pTimer->pCallback = NULL;
            pNode->pTimer = NULL;
        }
    }
#if defined(ESP32)
    TIMER_MUTEX_UNLOCK();
#endif
}

void Timer::AddTimer(Timer *pTimer)
{
#if defined(ESP32)
	TIMER_MUTEX_LOCK();
#endif 
    if (pFirst == NULL) {
        pFirst = new TimerNode(pTimer);
    } else {
        for (TimerNode *pNode = pFirst; pNode; pNode = pNode->pNext) {
            if (pNode->pNext == NULL) {
                pNode->pNext = new TimerNode(pTimer);
                break;
            }
        }
    }
#if defined(ESP32)
    TIMER_MUTEX_UNLOCK();
#endif
}
void Timer::RemoveTimer(Timer *pTimer)
{
#if defined(ESP32)
	TIMER_MUTEX_LOCK();
#endif 
    for (TimerNode *pNode = pFirst; pNode; pNode = pNode->pNext) {
        if (pNode->pTimer == pTimer) {
            pNode->pTimer = NULL;
            break;
        }
    }
#if defined(ESP32)
    TIMER_MUTEX_UNLOCK();
#endif
}


uint32_t Timer::GetNodesCount()
{
    uint32_t count = 0;
    for (TimerNode *pNode = pFirst; pNode; pNode = pNode->pNext) {
        count++;
    }
    return count;
}
uint32_t Timer::GetActiveNodesCount()
{
    uint32_t count = 0;
    for (TimerNode *pNode = pFirst; pNode; pNode = pNode->pNext) {
        if (pNode->pTimer != NULL) {
            count++;
        }
    }
    return count;
}

uint32_t Timer::GetRunningNodesCount()
{
    uint32_t count = 0;
    for (TimerNode *pNode = pFirst; pNode; pNode = pNode->pNext) {
        if (pNode->pTimer != NULL) {
            if (pNode->pTimer->IsRunning()) {
                count++;
            }
        }
    }
    return count;
}
