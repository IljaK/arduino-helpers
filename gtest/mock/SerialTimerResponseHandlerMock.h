#pragma once
#include "serial/SerialTimerResponseHandler.h"
#include "BaseSerialMock.h"

class SerialTimerResponseHandlerMock: public SerialTimerResponseHandler, public BaseSerialMock
{
public:
	uint8_t receivedCommand[SERIAL_RX_BUFFER_SIZE];
	uint8_t receivedLength = 0;

	SerialTimerResponseHandlerMock(Stream * serial);
	~SerialTimerResponseHandlerMock();

	void OnResponseReceived(bool IsTimeOut, bool isOverFlow = false) override;
	void Clear() override;
};

