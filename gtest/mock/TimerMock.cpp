#include "TimerMock.h"

TimerMock::TimerMock()
{
    
}

TimerMock::~TimerMock()
{
	if (timerId != 0) {
		Timer::Stop(timerId);
		timerId = 0;
	}
}

void TimerMock::OnTimerComplete(TimerID timerId, uint8_t data) {
	if (timerId == this->timerId) {
		this->timerId = 0;
		if (resetOnComplete) {
			this->timerId = Timer::Start(this, duration, data);
		}
	}
}
void TimerMock::OnTimerStop(TimerID timerId, uint8_t data) {
    if (timerId == this->timerId) {
		this->timerId = 0;
        if (resetOnStop) {
			this->timerId = Timer::Start(this, duration, data);
		}
    }
}

bool TimerMock::IsCompleted() {
	return timerId == 0;
}
void TimerMock::Start(unsigned long duration) {
	if (timerId != 0) {
		Timer::Stop(timerId);
	}
	this->duration = duration;
	timerId = Timer::Start(this, duration);
}
void TimerMock::Stop() {
	if (timerId != 0) {
		Timer::Stop(timerId);
	}
}
unsigned long TimerMock::Remain()
{
	if (IsCompleted()) return 0;
	return Timer::Remain(timerId);
}
void TimerMock::Reset()
{
    StopAll();
    Loop();
}

void TimerMock::StopAll() {
    TimerID id = 0;

    for (TimerNode *pNode = pFirst; pNode; pNode = pNode->pNext) {
        if (pNode->id == 0) continue;
        id = pNode->id;
        pNode->id = 0;
        if (pNode->pCaller != NULL) {
            pNode->pCaller->OnTimerStop(id, pNode->data);
            pNode->pCaller = NULL;
        }
	}
}

void TimerMock::PrintAll() {
    for (TimerNode *pNode = pFirst; pNode; pNode = pNode->pNext) {
        if (pNode->pNext == NULL) {
            printf("%zu\n", pNode->id);
        } else {
            printf("%zu ", pNode->id);
        }
	}
}

uint32_t TimerMock::GetAllCount() {
    uint32_t count = 0;
    for (TimerNode *pNode = pFirst; pNode; pNode = pNode->pNext) {
        count++;
	}
    return count;
}
uint32_t TimerMock::GetCount() {
    uint32_t count = 0;
    for (TimerNode *pNode = pFirst; pNode; pNode = pNode->pNext) {
        if (pNode->id == 0) continue;
        count++;
	}
    return count;
}
bool TimerMock::HasDuplicateID()
{
    if (pFirst == NULL || pFirst->pNext == NULL) {
        return false;
    }
    for (TimerNode *pNode = pFirst; pNode; pNode = pNode->pNext) {
        if (pNode->id == 0) continue;
        for (TimerNode *pNode2 = pFirst; pNode2; pNode2 = pNode2->pNext) {
            if (pNode != pNode2 && pNode->id == pNode2->id) {
                return true;
            }
	    }
	}
    return false;
}