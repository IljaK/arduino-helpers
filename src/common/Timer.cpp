#include "Timer.h"

TimerNode *Timer::pFirst = NULL;
unsigned long Timer::frameTS = 0;

#if ESP32
SemaphoreHandle_t Timer::xTimerSemaphore = xSemaphoreCreateRecursiveMutex();
#endif

ITimerCallback::~ITimerCallback()
{
	Timer::StopAll(this);
};

TimerID Timer::Start(ITimerCallback *pCaller, unsigned long duration, uint8_t data)
{
#if ESP32
	xSemaphoreTakeRecursive( xTimerSemaphore, portMAX_DELAY );
#endif

	TimerNode *newNode = new TimerNode();
	newNode->pCaller = pCaller;
	newNode->remain = duration;

	if (pFirst == NULL || pFirst->id != 1u) {
		if (pFirst != NULL) newNode->pNext = pFirst;
		pFirst = newNode;
        newNode->data = data;
		newNode->id = 1u;
	} else {
		for (TimerNode *pNode = pFirst; pNode; pNode = pNode->pNext ) {
			if (pNode->pNext == NULL || pNode->pNext->id - pNode->id > 1) {
				newNode->pNext = pNode->pNext;
				pNode->pNext = newNode;
				newNode->id = pNode->id + 1u;
				newNode->data = data;
				break;
			}
		}
	}

	TimerID id = newNode->id;
	
#if ESP32
    xSemaphoreGiveRecursive(xTimerSemaphore);
#endif
	return id;
}
// TODO: Test this
TimerID Timer::Start(timerCompleteCallBack pCallBack, unsigned long duration, uint8_t data)
{
    if (pCallBack == NULL) {
        return 0;
    }
    TimerCallback *tc = new TimerCallback(pCallBack);
    if (tc == NULL) {
        // Could not allocate?
        return 0;
    }
    TimerID id = Start(tc, duration, data);
    if (id == 0) {
        delete tc;
    }
    return id;
}

void Timer::Loop()
{
#if ESP32
	xSemaphoreTakeRecursive( xTimerSemaphore, portMAX_DELAY );
#endif

	unsigned long microsTS = micros();
	unsigned long delta = microsTS - frameTS;
	frameTS = microsTS;


	TimerNode *pPrev = NULL;

	for (TimerNode* pNode = pFirst; pNode; ) {
		
		if (pNode->remain <= delta) {
			TimerNode *freeNode = pNode;
			pNode = pNode->pNext;

			if (freeNode == pFirst) {
				pFirst = pNode;
				pPrev = NULL;
			} else {
				pPrev->pNext = pNode;
			}

			freeNode->pCaller->OnTimerComplete(freeNode->id, freeNode->data);

			// pPrev node could be changed during callback
			if (pNode != NULL) {
				if ((pPrev == NULL && pFirst != pNode) || (pPrev != NULL && pPrev->pNext != pNode)) {
					for (TimerNode* pN = pFirst; pN; pN = pN->pNext) {
						if (pN->pNext && pN->pNext == pNode) {
							pPrev = pN;
							break;
						}
					}
				}
			}

			freeNode->remain = 0;
			freeNode->pNext = NULL;
			delete(freeNode);

			continue;
		}
		pNode->remain -= delta;
		pPrev = pNode;
		pNode = pNode->pNext;
	}
#if ESP32
    xSemaphoreGiveRecursive(xTimerSemaphore);
#endif
}
bool Timer::Stop(TimerID timerId)
{
	if (timerId == 0) return false;

#if ESP32
	xSemaphoreTakeRecursive( xTimerSemaphore, portMAX_DELAY );
#endif
	bool result = false;

	TimerNode *pPrev = NULL;
	for (TimerNode *pNode = pFirst; pNode; pNode = pNode->pNext) {

		if (pNode->id == timerId) {
			if (pNode == pFirst) {
				pFirst = pNode->pNext;
				pPrev = NULL;
			} else {
				pPrev->pNext = pNode->pNext;
			}
            if (pNode->pCaller) {
                pNode->pCaller->OnTimerStop(pNode->id, pNode->data);
            }
			delete(pNode);
			result = true;
			break;
		}
		pPrev = pNode;
	}

#if ESP32
    xSemaphoreGiveRecursive(xTimerSemaphore);
#endif
	return result;
}


void Timer::StopAll(ITimerCallback* pCaller)
{
#if ESP32
	xSemaphoreTakeRecursive( xTimerSemaphore, portMAX_DELAY );
#endif
	TimerNode* pPrev = NULL;

	for (TimerNode* pNode = pFirst; pNode; ) {

		if (pNode->pCaller == pCaller) {
			TimerNode* freeNode = pNode;
			pNode = pNode->pNext;

			if (freeNode == pFirst) {
				pFirst = freeNode->pNext;
				pPrev = NULL;
			}
			else {
				pPrev->pNext = freeNode->pNext;
			}

            if (freeNode != NULL && freeNode->pCaller) {
                freeNode->pCaller->OnTimerStop(freeNode->id, freeNode->data);
            }
			delete(freeNode);

			continue;
		}
		pPrev = pNode;
		pNode = pNode->pNext;
	}
#if ESP32
    xSemaphoreGiveRecursive(xTimerSemaphore);
#endif
}

unsigned long Timer::Remain(TimerID timerId)
{
#if ESP32
	xSemaphoreTakeRecursive( xTimerSemaphore, portMAX_DELAY );
#endif

	unsigned long remain = 0;
	for (TimerNode *pNode = pFirst; pNode; pNode = pNode->pNext) {
		if (pNode->id == timerId) {
			remain = pNode->remain;
			break;
		}
	}

#if ESP32
    xSemaphoreGiveRecursive(xTimerSemaphore);
#endif
	return remain;
}

bool Timer::Contains(ITimerCallback *pCaller, uint8_t data)
{
#if ESP32
	xSemaphoreTakeRecursive( xTimerSemaphore, portMAX_DELAY );
#endif
	bool result = false;
	for (TimerNode *pNode = pFirst; pNode; pNode = pNode->pNext) {
		if (pNode->pCaller == pCaller && pNode->data == data) {
			result = true;
			break;
		}
	}

#if ESP32
    xSemaphoreGiveRecursive(xTimerSemaphore);
#endif
	return result;
}

uint8_t Timer::GetData(TimerID timerId)
{
#if ESP32
	xSemaphoreTakeRecursive( xTimerSemaphore, portMAX_DELAY );
#endif
	for (TimerNode *pNode = pFirst; pNode; pNode = pNode->pNext) {
		if (pNode->id == timerId) {
        #if ESP32
            uint8_t data = pNode->data;
            xSemaphoreGiveRecursive(xTimerSemaphore);
            return data;
        #else
            return pNode->data;
        #endif
		}
	}
#if ESP32
    xSemaphoreGiveRecursive(xTimerSemaphore);
#endif

    return 0;
}