#include "Timer.h"

TimerNode *Timer::pFirst = NULL;
unsigned long Timer::frameTS = 0;

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
	TimerNode *newNode = new TimerNode(pCaller, duration, data);

	if (pFirst == NULL) {
		newNode->pNext = pFirst;
		pFirst = newNode;
		newNode->id = 1u;
	} else {
        TimerID maxID = -1;
        TimerID nextId = 1u;
        bool skipIteration = false;

        TimerNode *pPrev = NULL;
		for (TimerNode *pNode = pFirst; pNode; pPrev = pNode, pNode = pNode->pNext ) {

			if (nextId < pNode->id) {
                if (pPrev == NULL) {
                    pFirst = newNode;
                } else {
                    pPrev->pNext = newNode;
                }
				newNode->pNext = pNode;
				newNode->id = nextId;
				break;
			}

            if (pNode->id != 0) {
                nextId = pNode->id + 1u;
            }
            if (nextId == maxID) {
                newNode->id = 0;
                break;
            }

            if (pNode->pNext == NULL) {
                pNode->pNext = newNode;
				newNode->id = nextId;
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

    LoopCompleted();
#if defined(ESP32)
    TIMER_MUTEX_UNLOCK();
#endif
}

void Timer::LoopCompleted()
{
    if (pFirst == NULL) return;
    
    TimerNode *pPrev = NULL;
    TimerNode *delNode = NULL;

	for (TimerNode *pNode = pFirst; pNode;  ) {

        if (pNode->IsStopped() || pNode->IsCompleted()) {
            if (pPrev == NULL) {
                pFirst = pNode->pNext;
            } else {
                pPrev->pNext = pNode->pNext;
            }
            delNode = pNode;
            pNode = pNode->pNext;

            if (delNode->id != 0 && delNode->pCaller != NULL) {
                delNode->pCaller->OnTimerComplete(delNode->id, delNode->data);
            }
            delete delNode;
            continue;
        }
        pPrev = pNode;
        pNode = pNode->pNext;
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
        if (pNode->id == 0) {
            continue;
        }
		if (pNode->pCaller == pCaller) {
            timerId = pNode->id;
			pNode->id = 0;
            pNode->pCaller = NULL;
            if (pCaller != NULL) {
                pCaller->OnTimerStop(timerId, pNode->data);
            }
		}
	}
#if defined(ESP32)
    TIMER_MUTEX_UNLOCK();
#endif
}

unsigned long Timer::Remain(TimerID timerId)
{
	if (timerId == 0) return 0;
#if defined(ESP32)
	TIMER_MUTEX_LOCK();
#endif

	unsigned long remain = 0;
	for (TimerNode *pNode = pFirst; pNode; pNode = pNode->pNext) {
		if (pNode->id == timerId) {
			remain = pNode->Remain();
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
    if (pCaller == NULL) return false;
#if defined(ESP32)
	TIMER_MUTEX_LOCK();
#endif
	bool result = false;
	for (TimerNode *pNode = pFirst; pNode; pNode = pNode->pNext) {
        if (pNode->id == 0) {
            continue;
        }

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
    if (timerId == 0) return 0;

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