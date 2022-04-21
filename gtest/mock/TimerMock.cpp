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
    }
}

bool TimerMock::IsCompleted() {
	return timerId == 0;
}
void TimerMock::Start(unsigned long duration) {
	if (timerId != 0) {
		Timer::Stop(timerId);
		timerId = 0;
	}
	this->duration = duration;
	timerId = Timer::Start(this, duration);
}
unsigned long TimerMock::Remain()
{
	if (IsCompleted()) return 0;
	return Timer::Remain(timerId);
}
void TimerMock::Reset()
{
	frameTS = 0;
}

void TimerMock::StopAll() {
    ITimerCallback *pCaller = NULL;
    TimerID id = 0;

    for (TimerNode *pNode = pFirst; pNode; pNode = pNode->pNext) {
        id = pNode->id;
        pNode->id = 0;
        pNode->remain = 0;
        if (pNode->pCaller != NULL) {
            pCaller = pNode->pCaller;
            pNode->pCaller = NULL;
            pCaller->OnTimerStop(id, pNode->data);
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