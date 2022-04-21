#include "Timer.h"

TimerNode *Timer::pFirst = NULL;
unsigned long Timer::frameTS = 0;
TimerNode *Timer::processingNode = NULL;

#if defined(ESP32)
SemaphoreHandle_t Timer::xTimerSemaphore = xSemaphoreCreateRecursiveMutex();
#define TIMER_MUTEX_LOCK()    while (xSemaphoreTakeRecursive( xTimerSemaphore, portMAX_DELAY ) != pdPASS) {}
#define TIMER_MUTEX_UNLOCK()  xSemaphoreGiveRecursive(xTimerSemaphore)
#endif


ITimerCallback::~ITimerCallback()
{
	Timer::StopAll(this);
};

TimerID Timer::Start(ITimerCallback *pCaller, unsigned long duration, uint8_t data)
{
#if defined(ESP32)
	TIMER_MUTEX_LOCK();
#endif

	TimerNode *newNode = new TimerNode();
	newNode->pCaller = pCaller;
	newNode->remain = duration;
    newNode->data = data;
    newNode->skipIteration = false;

	if (pFirst == NULL) {
		newNode->pNext = pFirst;
		pFirst = newNode;
		newNode->id = 1u;
	} else {
        TimerID maxID = -1;
        TimerID latestId = 0;
        bool skipIteration = false;
		for (TimerNode *pNode = pFirst; pNode; pNode = pNode->pNext ) {

            skipIteration = skipIteration || pNode == processingNode;
            newNode->skipIteration = skipIteration;

            if (pNode->id != 0) {
                latestId = pNode->id;
            }
            if (latestId == maxID) {
                newNode->id = 0;
                break;
            }
			if (pNode->pNext == NULL || latestId + 1u < pNode->pNext->id) {
				newNode->pNext = pNode->pNext;
				pNode->pNext = newNode;
				newNode->id = latestId + 1u;
				break;
			}
		}
	}

	TimerID id = newNode->id;
    if (id == 0) {
        delete newNode;
    }
	
#if defined(ESP32)
    TIMER_MUTEX_UNLOCK();
#endif
	return id;
}
TimerID Timer::Start(timerCallBack completeCB, unsigned long duration, uint8_t data, timerCallBack stopCB)
{
    if (completeCB == NULL) {
        return 0;
    }
    TimerCallback *tc = new TimerCallback(completeCB, stopCB);
    TimerID id = Start(tc, duration, data);
    if (id == 0) {
        delete tc;
    }
    return id;
}

void Timer::Loop()
{
#if defined(ESP32)
	TIMER_MUTEX_LOCK();
#endif

	unsigned long microsTS = micros();
	unsigned long delta = microsTS - frameTS;
    frameTS = microsTS;
    LoopCompleted(delta);
#if defined(ESP32)
    TIMER_MUTEX_UNLOCK();
#endif
}

void Timer::LoopCompleted(unsigned long delta)
{
    if (pFirst == NULL) return;
    
    TimerNode *pPrev = NULL;
    TimerNode *delNode = NULL;

	for (processingNode = pFirst; processingNode; ) {

        if (processingNode->skipIteration) {
            processingNode->skipIteration = false;
            pPrev = processingNode;
            processingNode = processingNode->pNext;
            continue;
        }
        if (processingNode->remain <= delta) {
            processingNode->remain = 0;
        } else {
            processingNode->remain -= delta;
        }

		if (processingNode->remain == 0 || processingNode->id == 0) {
			if (pPrev == NULL) {
				pFirst = processingNode->pNext;
			} else {
				pPrev->pNext = processingNode->pNext;
			}
            delNode = processingNode;
            processingNode = processingNode->pNext;
			delNode->pNext = NULL;
			delNode->remain = 0;
            
            if (delNode->id != 0 && delNode->pCaller != NULL) {
			    delNode->pCaller->OnTimerComplete(delNode->id, delNode->data);
            }
			delete delNode;
            continue;
		}
        pPrev = processingNode;
        processingNode = processingNode->pNext;
	}
}

bool Timer::Stop(TimerID timerId)
{
	if (timerId == 0) return false;

	bool result = false;
    ITimerCallback *pCaller = NULL;

#if defined(ESP32)
	TIMER_MUTEX_LOCK();
#endif
	for (TimerNode *pNode = pFirst; pNode; pNode = pNode->pNext) {
		if (pNode->id == timerId) {
            pNode->id = 0;
            pCaller = pNode->pCaller;
            pNode->pCaller = NULL;
            pNode->remain = 0;
            if (pCaller != NULL) {
                pCaller->OnTimerStop(timerId, pNode->data);
            }
            result = true;
			break;
		}
	}
#if defined(ESP32)
    TIMER_MUTEX_UNLOCK();
#endif
    return result;
}


void Timer::StopAll(ITimerCallback* pCaller)
{
    if (pCaller == NULL) return;
#if defined(ESP32)
	TIMER_MUTEX_LOCK();
#endif
    TimerID timerId = 0;
	for (TimerNode *pNode = pFirst; pNode; pNode = pNode->pNext) {
		if (pNode->pCaller == pCaller) {
            timerId = pNode->id;
			pNode->id = 0;
            pCaller = pNode->pCaller;
            pNode->pCaller = NULL;
            pNode->remain = 0;
            if (pCaller != NULL) {
                pCaller->OnTimerStop(timerId, pNode->data);
            }
			continue;
		}
	}
#if defined(ESP32)
    TIMER_MUTEX_UNLOCK();
#endif
}

unsigned long Timer::Remain(TimerID timerId)
{
#if defined(ESP32)
	TIMER_MUTEX_LOCK();
#endif

	unsigned long remain = 0;
	for (TimerNode *pNode = pFirst; pNode; pNode = pNode->pNext) {
		if (pNode->id == timerId) {
			remain = pNode->remain;
			break;
		}
	}

#if defined(ESP32)
    TIMER_MUTEX_UNLOCK();
#endif
	return remain;
}

bool Timer::Contains(ITimerCallback *pCaller, uint8_t data)
{
#if defined(ESP32)
	TIMER_MUTEX_LOCK();
#endif
	bool result = false;
	for (TimerNode *pNode = pFirst; pNode; pNode = pNode->pNext) {
		if (pNode->pCaller == pCaller && pNode->data == data) {
			result = true;
			break;
		}
	}

#if defined(ESP32)
    TIMER_MUTEX_UNLOCK();
#endif
	return result;
}

uint8_t Timer::GetData(TimerID timerId)
{
    uint8_t data = 0;
#if defined(ESP32)
	TIMER_MUTEX_LOCK();
#endif
	for (TimerNode *pNode = pFirst; pNode; pNode = pNode->pNext) {
		if (pNode->id == timerId) {
            data = pNode->data;
            break;
		}
	}
#if defined(ESP32)
    TIMER_MUTEX_UNLOCK();
#endif

    return data;
}