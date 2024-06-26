#include "Timer.h"

// TODO: definition for multi/single core
#if defined(ESP32)
SemaphoreHandle_t Timer::xTimerSemaphore = xSemaphoreCreateRecursiveMutex();

#define TIMER_MUTEX_LOCK()    while (xSemaphoreTakeRecursive( xTimerSemaphore, portMAX_DELAY ) != pdPASS) {};
#define TIMER_MUTEX_UNLOCK()  xSemaphoreGiveRecursive(xTimerSemaphore);

#else
    #define TIMER_MUTEX_LOCK()
    #define TIMER_MUTEX_UNLOCK()
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

	TIMER_MUTEX_LOCK();

    this->prevStamp = micros();
    this->remainMicros = durationMicros;
    this->state = TIMER_STATE_RUNNING;

    TIMER_MUTEX_UNLOCK();
}
void Timer::StartMillis(uint64_t durationMillis) {
    StartMicros((uint64_t)durationMillis * (uint64_t)MICROS_IN_MILLIS);
}
void Timer::StartSeconds(uint64_t durationSeconds) {
    StartMicros((uint64_t)durationSeconds * (uint64_t)MICROS_IN_SECOND);
}

void Timer::Stop() {

	TIMER_MUTEX_LOCK();

    if (!IsRunning()) {
        TIMER_MUTEX_UNLOCK();
        return;
    }
    state = TIMER_STATE_NONE;
    remainMicros = 0;

    TIMER_MUTEX_UNLOCK();

}

bool Timer::Update() {

	TIMER_MUTEX_LOCK();

    if (!IsRunning()) {
        TIMER_MUTEX_UNLOCK();
        return false;
    }

    uint32_t stamp = micros();
    uint64_t delta = 0;
    delta = stamp - prevStamp;

    if (delta >= remainMicros) {
        remainMicros = 0;
        state = TIMER_STATE_COMPLETED;
        TIMER_MUTEX_UNLOCK();
        return true;
    }
    prevStamp = stamp;
    remainMicros -= delta;
    TIMER_MUTEX_UNLOCK();
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
	TIMER_MUTEX_LOCK();
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

    TIMER_MUTEX_UNLOCK();
}

void Timer::RemoveAll(ITimerCallback* pCaller)
{
    if (pCaller == NULL) return;
	TIMER_MUTEX_LOCK();
    for (TimerNode *pNode = pFirst; pNode; pNode = pNode->pNext) {
        if (pNode->pTimer != NULL && pCaller == pNode->pTimer->pCallback) {
            pNode->pTimer->pCallback = NULL;
            pNode->pTimer = NULL;
        }
    }
    TIMER_MUTEX_UNLOCK();
}

void Timer::AddTimer(Timer *pTimer)
{
	TIMER_MUTEX_LOCK();
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
    TIMER_MUTEX_UNLOCK();
}
void Timer::RemoveTimer(Timer *pTimer)
{
    if (pTimer == NULL) return;
	TIMER_MUTEX_LOCK();
    for (TimerNode *pNode = pFirst; pNode; pNode = pNode->pNext) {
        if (pNode->pTimer == pTimer) {
            pNode->pTimer = NULL;
            break;
        }
    }
    TIMER_MUTEX_UNLOCK();
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
