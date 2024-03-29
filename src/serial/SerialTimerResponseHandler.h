#pragma once
#include "../common/Util.h"
#include "../common/Timer.h"
#include "BaseSerialHandler.h"

class SerialTimerResponseHandler: protected BaseSerialHandler
{
protected:
    size_t serialRxBufferSize = 0;
	Timer messageTimer;
	size_t registeredBytes = 0;

	void StartTimer();
	void StopTimer();
	void ResponseDetectedInternal(bool isTimeOut, bool isOverFlow = false) override;

public:
	SerialTimerResponseHandler(Stream * serial, size_t serialRxBufferSize);
	~SerialTimerResponseHandler();

	void Loop() override;
	bool IsBusy() override;

	virtual uint8_t GetDataBitsAmount();
	virtual uint8_t GetStopBitsAmount();
	virtual uint32_t GetBaudRate();

	void FlushData() override;
	void OnTimerComplete(Timer *timer) override;

	virtual unsigned long ResponseByteTimeOut();

	double SingleByteTransferDuration();
    
    size_t Available();
    size_t Write(const uint8_t data);
    size_t Write(const uint8_t *data, size_t size);

    size_t ReadBytes(uint8_t *data, size_t size);
    int Read();
};

