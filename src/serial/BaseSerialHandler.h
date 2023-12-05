#pragma once
#include "../common/Util.h"
#include "../common/Timer.h"
#include <Stream.h>

typedef void (*SerialResponseCallback)(uint8_t *data, size_t size, bool isTimeOut, bool isOverFlow);

class BaseSerialHandler: public ITimerCallback
{
protected:
    SerialResponseCallback responseCallback = NULL;

	Stream * serial = NULL;
	Timer responseTimeoutTimer;

	virtual void ResponseDetectedInternal(bool isTimeOut, bool isOverFlow = false);
	
public:
	BaseSerialHandler(Stream * serial);
	virtual ~BaseSerialHandler();

	void StartTimeoutTimer(unsigned long microSecTimeOut);
	void StopTimeoutTimer();
	void OnTimerComplete(Timer * timer) override;

	virtual bool IsBusy();

	virtual void OnResponseReceived(bool isTimeOut, bool isOverFlow = false);
	virtual void FlushData();
	virtual void Loop();

	virtual void SetResponseCallBack(SerialResponseCallback responseCallback);
};

