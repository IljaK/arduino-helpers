#pragma once
#include "../common/Util.h"
#include "../common/Timer.h"
#include <Stream.h>

typedef void (*SerialResponseCallback)(uint8_t *data, size_t size, bool IsTimeOut, bool isOverFlow);

class BaseSerialHandler: public ITimerCallback
{
protected:
    SerialResponseCallback responseCallback = NULL;

	Stream * serial = NULL;
	TimerID responseTimeoutTimer = 0;

	virtual void ResponseDetectedInternal(bool IsTimeOut, bool isOverFlow = false);
	
public:
	BaseSerialHandler(Stream * serial);
	virtual ~BaseSerialHandler();

	void StartTimeoutTimer(unsigned long microSecTimeOut);
	void StopTimeoutTimer();
	void OnTimerComplete(TimerID timerId, uint8_t data) override;
	void OnTimerStop(TimerID timerId, uint8_t data) override;

	virtual bool IsBusy();

	virtual void OnResponseReceived(bool IsTimeOut, bool isOverFlow = false);
	virtual void FlushData();
	virtual void Loop();

	virtual void SetResponseCallBack(SerialResponseCallback responseCallback);
};

